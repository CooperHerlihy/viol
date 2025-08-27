#include "utils.h"

[[nodiscard]] static bool is_numeral(int character) {
    return character >= '0' && character <= '9';
}

[[nodiscard]] static bool is_whitespace(int character) {
    return character == ' ' || character == '\t' || character == '\n' || character == EOF;
}

static constexpr usize MaxStringLength = 256;
static constexpr usize StackSize = 1024;

struct StringStack {
    char *data;
    char *head;
    usize size;
};

[[nodiscard]] static struct StringStack create_string_stack(usize size) {
    struct StringStack stack = {
        .data = halloc(sizeof(char) * size),
        .size = sizeof(char) * size,
        .head = stack.data,
    };
    return stack;
}

static void push_string(struct StringStack *stack, char value) {
    if (stack->head + 1 > stack->data + stack->size)
        ERROR("String stack overflow");
    *stack->head = value;
    ++stack->head;
}

[[nodiscard]] static char *pop_string(struct StringStack *stack) {
    if (stack->head <= stack->data)
        ERROR("No string to pop");
    push_string(stack, '\0');
    stack->head = stack->data;
    return stack->data;
}

struct ValueStack {
    i32 *data;
    i32 *head;
    usize size;
};

[[nodiscard]] static struct ValueStack create_value_stack(usize size) {
    struct ValueStack stack = {
        .data = halloc(sizeof(i32) * size),
        .size = sizeof(i32) * size,
        .head = stack.data,
    };
    return stack;
}

static void push_value(struct ValueStack *stack, i32 value) {
    if (stack->head + 1 > stack->data + stack->size)
        ERROR("Value stack overflow");
    *stack->head = value;
    ++stack->head;
}

[[nodiscard]] static i32 pop_value(struct ValueStack *stack) {
    if (stack->head <= stack->data)
        ERROR("Value stack underflow");
    --stack->head;
    return *stack->head;
}

enum Operation {
    OperationNone,
    OperationPush,
    OperationPop,
    OperationCall,
    OperationReturn,
};

[[nodiscard]] static enum Operation parse_operation(char *string) {
    if (strcmp(string, "push") == 0)
        return OperationPush;
    if (strcmp(string, "pop") == 0)
        return OperationPop;
    if (strcmp(string, "call") == 0)
        return OperationCall;
    if (strcmp(string, "return") == 0)
        return OperationReturn;
    ERRORF("Unknown operation '%s'", string);
}

[[nodiscard]] static int parse_literal(char *string) {
    if (!is_numeral(string[0]))
        ERRORF("Expected number literal, got '%s'", string);

    long value = atoi(string);

    if (value > INT_MAX)
        ERROR("Literal too large");
    return (int)value;
}

enum ParseState {
    ParseComment,
    ParseOperation,
    ParseArgument,
};

struct ParseData {
    struct StringStack string_stack;
    struct ValueStack stack;
    enum ParseState state;
    enum Operation operation;
};

static void call_function(struct ParseData *parse_data, char *name) {
    if (strcmp(name, "print") == 0) {
        int value = pop_value(&parse_data->stack);
        printf("%d\n", value);
    } else if (strcmp(name, "add") == 0) {
        int augend = pop_value(&parse_data->stack);
        int addend = pop_value(&parse_data->stack);
        push_value(&parse_data->stack, augend + addend);
    } else if (strcmp(name, "sub") == 0) {
        int subtrahend = pop_value(&parse_data->stack);
        int minuend = pop_value(&parse_data->stack);
        push_value(&parse_data->stack, minuend - subtrahend);
    } else if (strcmp(name, "mul") == 0) {
        int multiplier = pop_value(&parse_data->stack);
        int multiplicand = pop_value(&parse_data->stack);
        push_value(&parse_data->stack, multiplicand * multiplier);
    } else if (strcmp(name, "div") == 0) {
        int divisor = pop_value(&parse_data->stack);
        if (divisor == 0)
            ERROR("Division by zero");
        int numerator = pop_value(&parse_data->stack);
        push_value(&parse_data->stack, numerator / divisor);
    } else {
        ERRORF("Unknown function '%s'", name);
    }
}

static void execute_operation(struct ParseData *parse_data, char* arg) {
    switch (parse_data->operation) {
        case OperationPush: {
            push_value(&parse_data->stack, parse_literal(arg));
        } break;
        case OperationPop: {
            ASSERT(false);
        } break;
        case OperationCall: {
            call_function(parse_data, arg);
        } break;
        case OperationReturn: {
            ASSERT(false);
        } break;
        default: {
            ASSERT(false);
        } break;
    }
}

void parse(FILE *src) {
    struct ParseData parse_data = {
        .string_stack = create_string_stack(MaxStringLength),
        .stack = create_value_stack(StackSize),
        .state = ParseOperation,
        .operation = OperationNone,
    };
    int arg_count = 0;

    while (true) {
        int character = fgetc(src);

        if (character == '#')
            parse_data.state = ParseComment;

        switch (parse_data.state) {
            case ParseComment: {
                if (character == '\n')
                    parse_data.state = ParseOperation;
            } break;
            case ParseOperation: {
                if (parse_data.string_stack.head > parse_data.string_stack.data && is_whitespace(character)) {
                    parse_data.operation = parse_operation(pop_string(&parse_data.string_stack));
                    arg_count = 1;

                    parse_data.state = ParseArgument;
                }
                if (!is_whitespace(character))
                    push_string(&parse_data.string_stack, (char)character);
            } break;
            case ParseArgument: {
                if (parse_data.string_stack.head > parse_data.string_stack.data && is_whitespace(character)) {
                    --arg_count;
                    if (arg_count == 0) {
                        execute_operation(&parse_data, pop_string(&parse_data.string_stack));

                        parse_data.state = ParseOperation;
                    }
                }
                if (!is_whitespace(character))
                    push_string(&parse_data.string_stack, (char)character);
            } break;
        }

        if (character == EOF)
            return;
    }
}

int main(int argc, char **argv) {
    if (argc == 2) {
        constexpr usize GlobalHeapSize = 256 * 1024 * 1024;
        constexpr usize GlobalStackSize = 16 * 1024 * 1024;
        init_heap(GlobalHeapSize, GlobalStackSize);

        const char *source_name = argv[1];
        LOGF("Compiling %s", source_name);

        FILE *src_file = fopen(source_name, "r");
        if (src_file == nullptr) {
            ERRORF("Could not open %s\n", source_name);
            return EXIT_FAILURE;
        }
        parse(src_file);

        LOGF("Successfully compiled %s", source_name);
    } else {
        printf("Usage: %s source\n", argv[0]);
        return EXIT_FAILURE;
    }
}


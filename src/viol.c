#include "utils.h"

static constexpr usize HeapSize = 256 * 1024 * 1024;
static constexpr usize StackSize = 16 * 1024 * 1024;

static constexpr usize FunctionStackSize = 1024;
static constexpr usize VariableStackSize = 1024;
static constexpr usize MaxLiteralLength = 256;

enum BuiltinFunction : u8 {
    BuiltinFunctionPlus,
    BuiltinFunctionMinus,
    BuiltinFunctionMultiply,
    BuiltinFunctionDivide
};

struct FunctionStack {
    enum BuiltinFunction *data;
    enum BuiltinFunction *head;
    usize size;
};

[[nodiscard]] static struct FunctionStack create_function_stack(usize size) {
    struct FunctionStack stack = {
        .data = halloc(sizeof(enum BuiltinFunction) * size),
        .size = sizeof(enum BuiltinFunction) * size,
        .head = stack.data,
    };
    return stack;
}

static void push_function(struct FunctionStack *stack, enum BuiltinFunction value) {
    if (stack->head + 1 > stack->data + stack->size)
        ERROR("Function stack overflow");
    *stack->head = value;
    ++stack->head;
}

[[nodiscard]] static enum BuiltinFunction pop_function(struct FunctionStack *stack) {
    if (stack->head <= stack->data)
        ERROR("Function stack underflow");
    --stack->head;
    return *stack->head;
}

struct VariableStack {
    i32 *data;
    i32 *head;
    usize size;
};

[[nodiscard]] static struct VariableStack create_variable_stack(usize size) {
    struct VariableStack stack = {
        .data = halloc(sizeof(i32) * size),
        .size = sizeof(i32) * size,
        .head = stack.data,
    };
    return stack;
}

static void push_variable(struct VariableStack *stack, i32 value) {
    if (stack->head + 1 > stack->data + stack->size)
        ERROR("Variable stack overflow");
    *stack->head = value;
    ++stack->head;
}

[[nodiscard]] static i32 pop_variable(struct VariableStack *stack) {
    if (stack->head <= stack->data)
        ERROR("Variable stack underflow");
    --stack->head;
    return *stack->head;
}

struct LiteralStack {
    char *data;
    char *head;
    usize size;
};

[[nodiscard]] static struct LiteralStack create_literal_stack(usize size) {
    struct LiteralStack stack = {
        .data = halloc(sizeof(char) * size),
        .size = sizeof(char) * size,
        .head = stack.data,
    };
    return stack;
}

static void push_literal_char(struct LiteralStack *stack, char value) {
    if (stack->head + 1 > stack->data + stack->size)
        ERROR("Literal stack overflow");
    *stack->head = value;
    ++stack->head;
}

[[nodiscard]] static int pop_literal(struct LiteralStack *stack) {
    if (stack->head <= stack->data)
        ERROR("Literal stack underflow");
    *stack->head = '\0';
    long value = atoi(stack->data);
    if (value > INT_MAX)
        ERROR("Literal too large");
    stack->head = stack->data;
    return (int)value;
}

[[nodiscard]] static bool is_whitespace(int character) {
    return character == ' ' || character == '\t' || character == '\n';
}

static void parse_function(FILE* src, struct FunctionStack *function_stack) {
    int character;
    while (is_whitespace(character = fgetc(src))) {}
    switch (character) {
        case '+': {
            push_function(function_stack, BuiltinFunctionPlus);
        } break;
        case '-': {
            push_function(function_stack, BuiltinFunctionMinus);
        } break;
        case '*': {
            push_function(function_stack, BuiltinFunctionMultiply);
        } break;
        case '/': {
            push_function(function_stack, BuiltinFunctionDivide);
        } break;
        default: ERRORF("Unexpected character '%c' in function name", character);
    }
}

[[nodiscard]] static bool is_numeral(int character) {
    return character >= '0' && character <= '9';
}

[[nodiscard]] static i32 parse_literal(FILE* src, struct LiteralStack *literal_stack) {
    int character;
    while (is_numeral(character = fgetc(src))) {
        push_literal_char(literal_stack, (char)character);
    }
    fseek(src, -1, SEEK_CUR);
    return pop_literal(literal_stack);
}

static void execute_funtion(struct FunctionStack *function_stack, struct VariableStack *variable_stack) {
    enum BuiltinFunction function = pop_function(function_stack);
    switch (function) {
        case BuiltinFunctionPlus: {
            int augend = pop_variable(variable_stack);
            int addend = pop_variable(variable_stack);
            push_variable(variable_stack, augend + addend);
        } break;
        case BuiltinFunctionMinus: {
            int subtrahend = pop_variable(variable_stack);
            int minuend = pop_variable(variable_stack);
            push_variable(variable_stack, minuend - subtrahend);
        } break;
        case BuiltinFunctionMultiply: {
            int multiplier = pop_variable(variable_stack);
            int multiplicand = pop_variable(variable_stack);
            push_variable(variable_stack, multiplicand * multiplier);
        } break;
        case BuiltinFunctionDivide: {
            f32 divisor = (f32)pop_variable(variable_stack);
            if (divisor == 0)
                ERROR("Division by zero");
            f32 numerator = (f32)pop_variable(variable_stack);
            push_variable(variable_stack, (int)(numerator / divisor));
        } break;
        default: ERRORF("Unknown function %d", function);
    }
}

static void parse(FILE* src) {
    struct FunctionStack function_stack = create_function_stack(FunctionStackSize);
    struct VariableStack variable_stack = create_variable_stack(VariableStackSize);
    struct LiteralStack literal_stack = create_literal_stack(MaxLiteralLength);

    int character;
    while ((character = fgetc(src)) != EOF) {
        switch (character) {
            case ' ': [[fallthrough]];
            case '\t': [[fallthrough]];
            case '\n': break;
            case '#': {
                while ((character = fgetc(src)) != '\n') {
                    if (character == EOF)
                        goto COMPLETE_PARSING;
                }
            } break;
            case '(': {
                parse_function(src, &function_stack);
            } break;
            case '0': [[fallthrough]];
            case '1': [[fallthrough]];
            case '2': [[fallthrough]];
            case '3': [[fallthrough]];
            case '4': [[fallthrough]];
            case '5': [[fallthrough]];
            case '6': [[fallthrough]];
            case '7': [[fallthrough]];
            case '8': [[fallthrough]];
            case '9': {
                fseek(src, -1, SEEK_CUR);
                push_variable(&variable_stack, parse_literal(src, &literal_stack));
            } break;
            case ')': {
                execute_funtion(&function_stack, &variable_stack);
            } break;
            default:
                ERRORF("Unexpected character '%c' to begin argument", character);
        }
    }

COMPLETE_PARSING:
    printf("Result: %d\n", pop_variable(&variable_stack));
    return;
}

int main(int argc, char **argv) {
    if (argc == 2) {
        init_heap(HeapSize, StackSize);

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


#include "utils.h"

static constexpr usize HeapSize = 256 * 1024 * 1024;
static constexpr usize StackSize = 16 * 1024 * 1024;

int main(int argc, char **argv) {
    if (argc == 2) {
        init_heap(HeapSize, StackSize);


        const char *source_name = argv[1];
        LOGF("compiling %s", source_name);

        FILE *src_file = fopen(source_name, "r");
        if (src_file == nullptr) {
            ERRORF("Could not open %s\n", source_name);
            return EXIT_FAILURE;
        }

    } else {
        printf("Usage: %s source\n", argv[0]);
        return EXIT_FAILURE;
    }
}


#include "utils.h"

static byte *s_heap_data;
static byte *s_heap_head;
static usize s_heap_size;

static byte *s_stack_data;
static byte *s_stack_head;
static usize s_stack_size;

byte *align(byte *ptr, usize alignment) {
    return (byte *)(((usize)ptr + alignment - 1) & ~(alignment - 1));
}

byte *halloc(usize size) {
    ASSERT(align(s_heap_head, 16) + size <= s_heap_data + s_heap_size);
    s_heap_head = align(s_heap_head, 16);
    byte *ptr = s_heap_head;
    s_heap_head += size;
    return ptr;
}

byte *stalloc(usize size) {
    ASSERT(align(s_stack_head, 16) + size <= s_stack_data + s_stack_size);
    s_stack_head = align(s_stack_head, 16);
    byte *ptr = s_stack_head;
    s_stack_head += size;
    return ptr;
}

void destalloc(byte *ptr, usize size) {
    ASSERT(ptr + size == s_heap_head);
    s_heap_head = ptr;
}

void init_heap(usize heap_size, usize stack_size) {
    s_heap_data = malloc(heap_size);
    if (s_heap_data == nullptr)
        ERROR("Malloc returned null; could not allocate heap");
    s_heap_size = heap_size;
    s_heap_head = s_heap_data;
    s_stack_data = halloc(stack_size);
    s_stack_size = stack_size;
}


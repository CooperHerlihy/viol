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

void *halloc(usize size) {
    ASSERT(align(s_heap_head, 16) + size <= s_heap_data + s_heap_size);
    s_heap_head = align(s_heap_head, 16);
    byte *ptr = s_heap_head;
    s_heap_head += size;
    return ptr;
}

void *rehalloc(byte *ptr, usize old_size, usize new_size) {
    if (ptr + old_size == s_heap_head) {
        s_heap_head = ptr + new_size;
        return ptr;
    }
    byte *new_ptr = halloc(new_size);
    memcpy(new_ptr, ptr, old_size);
    return new_ptr;
}

void *stalloc(usize size) {
    ASSERT(align(s_stack_head, 16) + size <= s_stack_data + s_stack_size);
    s_stack_head = align(s_stack_head, 16);
    byte *ptr = s_stack_head;
    s_stack_head += size;
    return ptr;
}

void *restalloc(byte *ptr, usize old_size, usize new_size) {
    ASSERT(ptr + old_size == s_heap_head);
    s_heap_head = ptr + new_size;
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


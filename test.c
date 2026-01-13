#define CORE_IMPLEMENTATION
#define CORE_DEBUG_ASSERT
#define CORE_MEM_DEBUG
//#define CORE_FLUSH_IO
#include "core.h"

typedef struct ArenaEntry {
    Vec(char) buffer;
    char *base;
    size_t capacity;
}ArenaEntry;

typedef struct NewArena {
    Vec(ArenaEntry) entries;
    size_t current_index;
}NewArena;

NewArena newarena_new(size_t capacity) {
    return (NewArena) {
        .entries = vec_new(),
        .current_index = 0,
    };
}

void *newarena_alloc(NewArena *arena, size_t size) {
    if(vec_len(arena->entries) == 0) {
        Vec(char) buffer = vec_with_size(char, CORE_KB(4));
        ArenaEntry entry = {
            .capacity = CORE_KB(4),
            .buffer = buffer,
            .base = buffer,
        };
        vec_push(arena->entries, entry);
    }
    ArenaEntry *entry = &arena->entries[arena->current_index];
    /*if(vec_len(entry->buffer) + size > vec_cap(entry->buffer)) {
        Vec(char) buffer = vec_with_size(char, CORE_KB(4));
        ArenaEntry entry = {
            .capacity = CORE_KB(4),
            .buffer = buffer,
            .base = buffer,
        };
        vec_push(arena->entries, entry);
        arena->current_index++;
        entry = &arena->entries[arena->current_index];
    }*/

    return NULL;
}

/*#define LIST_VECTOR_TYPES(suffix, typ)\
    typedef struct CORE_CONCAT(Vector2, suffix) { typ x; typ y; } CORE_CONCAT(Vector2, suffix);\
    CORE_CONCAT(Vector2, suffix) CORE_CONCAT(Vector2, suffix) ##_new(typ x, typ y);*/

static void test(void);

int main(void) {
    test();

    ringbuffer_print_stats(&core_context.ring_buffer);
    arena_print_stats(&core_context.temp_arena);
#ifdef CORE_MEM_DEBUG
    vec_foreach(core_context.memory_stats.allocations, alloc) {
        println("%s:%zu: addr = %p, size = %zu", alloc->file.data, alloc->line, alloc->addr, alloc->size);
    }
#endif
    return 0;
}

static void test(void) {
    Arena arena = arena_new(CORE_KB(1));
    Vec(uint32_t) vec = vec_new(.allocator = arena_allocator(&arena));
    for(size_t i = 0; i < 100; i++) {
        vec_push(vec, i);
    }

    vec_remove(vec, 4);

    vec_foreach(vec, item) {
        println("%d", *item);
    }
    vec_dump(vec);

    arena_print_stats(&arena);
}

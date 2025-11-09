#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef void *(*AllocFn)(void *self, size_t size);
typedef void *(*ReallocFn)(void *self, void *mem, size_t size);
typedef void (*FreeFn)(void *self, void *_block);
typedef struct Allocator{
    void *self;
    AllocFn alloc;
    ReallocFn realloc;
    FreeFn free;
}Allocator;

void *alloc(void *self, size_t size) {
    (void)self;
    return malloc(size);
}

void *realloc_fn(void *self, void *mem, size_t size) {
    (void)self;
    return realloc(mem, size);
}

void dealloc(void *self, void *_block) {
    (void)self;
    free(_block);
}

Allocator global_allocator = { NULL, alloc, realloc_fn, dealloc };

typedef struct ArenaAllocator ArenaAllocator;

struct ArenaAllocator {
    char* buffer;
    char *current_alloc;
    size_t alloc_size;
    ArenaAllocator *next;
};

ArenaAllocator arena_new(size_t size) {
    if(size == 0) {
        size = 4 * 1024;
    }
    char* buffer = NULL;
    ArenaAllocator self = {
        .buffer = buffer,
        .current_alloc = buffer,
        .alloc_size = 0,
        .next = NULL,
    };
    return self;
}

void *arena_alloc(ArenaAllocator *alloc, size_t size) {
    return NULL;
}

void *arena_realloc(ArenaAllocator *alloc, void *src, size_t new_size) {
    (void)src;
    void *tmp = arena_alloc(alloc, new_size);
    return tmp;
}

void free_noop(void *self, void *_block) {
    (void)self;
    (void)_block;
}

Allocator arena_allocator(ArenaAllocator *self) {
    return (Allocator) {
        .self = self,
        .alloc = (AllocFn)arena_alloc,
        .realloc = (ReallocFn)arena_realloc,
        .free = free_noop,
    };
}

typedef struct String {
    Allocator *alloc;
    enum {
        STRING_SHORT,
        STRING_LONG,
    }type;
    union {
        struct {
            char data[24];
        }s;
        struct {
            char *ptr;
            size_t cap;
            size_t len;
        }l;
    }data;
}String;

struct StringNewArgs { Allocator *alloc; };

String string_new_impl(struct StringNewArgs args);
#define string_new(...) string_new_impl((struct StringNewArgs){__VA_ARGS__});

String string_new_impl(struct StringNewArgs args) {
    Allocator *alloc = args.alloc ? args.alloc : &global_allocator;
}

void foo(void) {
    Allocator allocator = { .alloc = alloc, .realloc = realloc_fn, .free = dealloc };
    String str = string_new(.alloc = &allocator);
}

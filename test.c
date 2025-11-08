#define CORE_IMPLEMENTATION
#define CORE_DEBUG_ASSERT
#define CORE_MEM_DEBUG
//#define CORE_FLUSH_IO
#include "core.h"
#include <aimline.h>

int start(size_t i) {
    println("Start = %zu", i);
    return i + 1;
}

void end(size_t i) {
    println("End = %zu", i);
}

//#include "gfx.h"

/*void window_event_callback(WindowHandle window, WindowEvent event, void *user_data) {
    CORE_UNUSED(window);
    CORE_UNUSED(user_data);
    if(event.kind != WINDOW_EVENT_MOUSE_MOVE) {
        window_event_print(&event);
    }
}*/

static void test(void);
//static void window_test(void);
static void test_strings(void);

int main(void) {
    CORE_UNUSED(test);
    CORE_UNUSED(test_strings);
    test();
    //test_strings();
    ringbuffer_print_stats(&core_context.ring_buffer);
    arena_print_stats(&core_context.temp_arena);
#ifdef CORE_MEM_DEBUG
    vec_foreach(core_context.memory_stats.allocations, alloc) {
        println("%s:%zu: addr = %p, size = %zu", alloc->file.data, alloc->line, alloc->addr, alloc->size);
    }
#endif
    return 0;
}

/*static void window_test(void) {
    WindowHandle window = window_create(.name = sv("Test"), .width = 400, .height = 400, .event_callback = window_event_callback);
    while(!window_should_close(window)) {
        window_wait_events();
        window_poll_events(window);
    }
    window_destroy(window);
}*/

/*static bool find(int *item, int *pred) {
    return *item == *pred;
}*/

static void test(void) {
    Arena arena = arena_new(64);
    Vec(uint32_t) vec = vec_new(.allocator = arena_allocator(&arena));
    for(size_t i = 0; i < 10; i++) {
        vec_push(vec, i);
    }

    vec_remove(vec, 4);

    vec_foreach(vec, item) {
        println("%d", *item);
    }
    vec_dump(vec);

    arena_print_stats(&arena);

    /*int pred = 10;
    size_t index = vec_find(vec, pred, (EqCallback)find);
    println("index = %zu", index);
    println("%d", vec[index]);*/

    /*auto alloc = arena_allocator(&core_context.temp_arena);
    core_defer_var(file, file_open("test.txt", FILE_READ, .allocator = alloc), file_close(file), {
        core_defer_var(content, file_read(file), string_destroy(&content), {
            println("content = %s", string_cstr(&content));
            string_dump(&content);
        });
    });*/

    //auto allocator = ringbuffer_allocator(&core_context.ring_buffer);
    /*for(size_t i = 0; i < 15; i++) {
        u32 *vec = vec_with_size(u32, 64);
        CORE_UNUSED(vec);
    }*/
}

static void test_strings(void) {
    {
        String s = string_new();
        assert(string_len(&s) == 0 && string_cap(&s) == 24);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_new_size(10);
        assert(string_len(&s) == 0 && string_cap(&s) == 24);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_new_size(100);
        assert(string_len(&s) == 0 && string_cap(&s) == 101);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_from("Hello World");
        assert(string_len(&s) == 11);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_from("fsdgkdflnfsodnfsidbfskdifbsdif");
        assert(string_len(&s) == 30 && string_cap(&s) > 30);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_from("fdsgdfgdfgdfgdfgfdfgffg");
        //assert(string_len(&s) == 23);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_format("fdsgdfgdfgd%s", "fgdfgfdfgfff");
        assert(string_len(&s) == 23);
        println("s = %s", string_cstr(&s));
    }
    {
        String s = string_format("fsdgkdflnfsodn%s", "fsidbfskdifbsdif");
        assert(string_len(&s) == 30 && string_cap(&s) > 30);
        println("s = %s", string_cstr(&s));
    }
    {
        String str = tmp_printf("Hello, %s", "Worldfgdmfgindlgkndo");
        println("s = %s", string_cstr(&str));
    }
    {
        String str = string_from("Hello, gdfgdorldfgdmfgindlgkndo", .allocator = arena_allocator(&core_context.temp_arena));
        println("s = %s", string_cstr(&str));
    }
}

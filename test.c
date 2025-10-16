#define CORE_IMPLEMENTATION
#define CORE_DEBUG_ASSERT
#include "core.h"

/*int start(size_t i) {
    println("Start = %zu", i);
    return i + 1;
}

void end(size_t i) {
    println("End = %zu", i);
}*/

#include "gfx.h"

void window_event_callback(WindowHandle window, WindowEvent event, void *user_data) {
    CORE_UNUSED(window);
    CORE_UNUSED(user_data);
    if(event.kind != WINDOW_EVENT_MOUSE_MOVE) {
        window_event_print(&event);
    }
}

static void test(void);
static void window_test(void);
static void test_strings(void);

int main(void) {
    CORE_UNUSED(test);
    CORE_UNUSED(test_strings);
    window_test();
    return 0;
}

static void window_test(void) {
    WindowHandle window = window_create(.name = sv("Test"), .width = 400, .height = 400, .event_callback = window_event_callback);
    while(!window_should_close(window)) {
        window_wait_events();
        window_poll_events(window);
    }
    window_destroy(window);
}

static void test(void) {
    /*ArenaAllocator arena = arena_new(64);
    Vec(uint32_t) vec = vec_new();
    for(size_t i = 0; i < 100; i++) {
        vec_push(vec, i);
    }

    vec_foreach(vec, item) {
        println("%d", *item);
    }

    vec_dump(vec);*/
    /*Slice(uint32_t) test = slice_from_vec(vec);
    println("ptr = %p, len = %zu", test.data, test.len);

    Vec(uint32_t) copy = slice_to_vec(test);
    vec_dump(copy);

    avec_foreach(copy, item) {
        println("%d", *item);
    }

    Slice(uint32_t) foo = test;
    bar(foo);*/

    /*AString tmp = tmp_printf("Hello, %s!dfgdfgdhdfhdhdhdhdg", "World");
    AString tmp1 = tmp_printf("Hello, %s!dfgdfgdhdfhdhdhdhdg", "World");
    astring_dump(&tmp);
    astring_dump(&tmp1);

    println("string: %s", tmp.ptr);
    println("context: %lld", (char*)core_context.temp_arena.current_alloc - (char*)core_context.temp_arena.buffer);

    String tmp2 = string_format("Hello, dfgdfgdhdfhdhdWogfdg");
    string_dump(&tmp2);

    core_defer(start(0), end(1)) {
        println("Middle");
    }*/

    /*core_defer_var(FileHandle, file, file_open("test.txt", FILE_READ), file_close(file), {
        core_defer_var(String, content, file_read(file), string_destroy(&content), {
            println("content = %s", string_cstr(&content));
            string_dump(&content);
        });
    });*/

    /*FileHandle file = file_open("test.txt", FILE_READ);
    String content = file_read(file);
    println("content = %s", string_cstr(&content));
    string_dump(&content);
    string_destroy(&content);
    file_close(file);*/
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
}

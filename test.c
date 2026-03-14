#define CORE_IMPLEMENTATION
#define CORE_DEBUG_ASSERT
#define CORE_MEM_DEBUG
//#define CORE_FLUSH_IO
#include "core.h"

int core_main() {
    vec_foreach(args(), arg) {
        println("arg = "SV_FMT, SV_ARG(*arg));
    }

    StringView view = sv("Hello, World");

    println("str = "SV_FMT, SV_ARG(view));

    String str = string_from("Foo Bar");
    println("string = "STRING_FMT, STRING_ARG(&str));

    ringbuffer_print_stats(&core_context.ring_buffer);
    arena_print_stats(&core_context.temp_arena);
    return 0;
}

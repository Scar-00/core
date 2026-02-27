#define CORE_IMPLEMENTATION
#define CORE_DEBUG_ASSERT
#define CORE_MEM_DEBUG
//#define CORE_FLUSH_IO
#include "core.h"

int main(void) {
    ringbuffer_print_stats(&core_context.ring_buffer);
    arena_print_stats(&core_context.temp_arena);
    return 0;
}

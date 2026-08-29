#define CORE_IMPLEMENTATION
#include <core.h>

int core_main(void) {
    vec_foreach(core_context.program_args.args, arg) {
        println(SV_FMT, SV_ARG(*arg));
    }
    return 0;
}

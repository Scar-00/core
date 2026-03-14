#define CORE_NO_ENTRY
#include "../core.h"

int main(void) {
    StringView path = sv("examples/demo-output.txt");
    StringView data = sv("core file example\n");

    if(!file_write_string(path, data)) {
        log(CORE_ERROR, "failed to write example file");
        return 1;
    }

    String read_back = file_read_to_string(path);
    println("wrote %zu bytes to %.*s", string_len(&read_back), (int)path.len, path.data);
    println("content = "STRING_FMT, STRING_ARG(&read_back));

    string_destroy(&read_back);
    return 0;
}

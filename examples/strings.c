#define CORE_NO_ENTRY
#define CORE_IMPLEMENTATION
#include "../core.h"

int main(void) {
    String title = string_from("core");
    string_pushf(&title, " %s", "strings");

    StringView prefix = sv("hello");
    String message = string_view_into_string(prefix);
    string_pushf(&message, ", %s", string_cstr(&title));

    println("title   = "STRING_FMT, STRING_ARG(title));
    println("message = "STRING_FMT, STRING_ARG(message));
    println("starts  = %s", string_view_starts_with(string_into_view(&message), sv("hello")) ? "true" : "false");

    string_destroy(&message);
    string_destroy(&title);
    return 0;
}

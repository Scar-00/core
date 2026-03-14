#define CORE_NO_ENTRY
#include "../core.h"

int main(void) {
    JSON json = json_parse(sv("{\"project\":\"core\",\"stable\":true,\"items\":[1,2,3]}"));
    String pretty = json_to_string(&json, .pretty_print = 2);

    println("json = "STRING_FMT, STRING_ARG(&pretty));

    string_destroy(&pretty);
    json_free(json);
    return 0;
}

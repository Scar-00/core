#define CORE_NO_ENTRY
#include "../core.h"

static bool int_eq(void *lhs, void *rhs) {
    return *(int *)lhs == *(int *)rhs;
}

int main(void) {
    Vec(int) values = vec_new();
    vec_push(values, 3);
    vec_push(values, 5);
    vec_push(values, 8);

    int needle = 5;
    size_t index = vec_find(values, needle, int_eq);

    println("len   = %zu", vec_len(values));
    println("index = %zu", index);

    vec_remove(values, 1);
    println("after remove = [%d, %d]", values[0], values[1]);

    vec_destroy(values);
    return 0;
}

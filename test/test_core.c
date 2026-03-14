#define CORE_NO_ENTRY
#define CORE_DEBUG_ASSERT
#include "../core.h"

#include <stdio.h>
#include <string.h>

typedef bool (*TestFn)(void);

static int tests_run = 0;
static int tests_failed = 0;

static bool expect_true_impl(bool cond, const char *expr, const char *file, int line) {
    if(cond) {
        return true;
    }

    fprintf(stderr, "[FAIL] %s:%d: %s\n", file, line, expr);
    return false;
}

static bool expect_size_eq_impl(size_t lhs, size_t rhs, const char *lhs_expr, const char *rhs_expr, const char *file, int line) {
    if(lhs == rhs) {
        return true;
    }

    fprintf(stderr, "[FAIL] %s:%d: %s (%zu) != %s (%zu)\n", file, line, lhs_expr, lhs, rhs_expr, rhs);
    return false;
}

static bool expect_int_eq_impl(int lhs, int rhs, const char *lhs_expr, const char *rhs_expr, const char *file, int line) {
    if(lhs == rhs) {
        return true;
    }

    fprintf(stderr, "[FAIL] %s:%d: %s (%d) != %s (%d)\n", file, line, lhs_expr, lhs, rhs_expr, rhs);
    return false;
}

static bool expect_mem_eq_impl(const void *lhs, const void *rhs, size_t len, const char *lhs_expr, const char *rhs_expr, const char *file, int line) {
    if(memcmp(lhs, rhs, len) == 0) {
        return true;
    }

    fprintf(stderr, "[FAIL] %s:%d: %s != %s for %zu bytes\n", file, line, lhs_expr, rhs_expr, len);
    return false;
}

static bool string_equals_literal(String const *self, const char *literal) {
    size_t len = strlen(literal);
    return string_len(self) == len && memcmp(string_cstr(self), literal, len) == 0;
}

static bool expect_string_eq_impl(String const *self, const char *literal, const char *expr, const char *file, int line) {
    if(string_equals_literal(self, literal)) {
        return true;
    }

    fprintf(stderr, "[FAIL] %s:%d: %s != \"%s\" (actual: \"%s\")\n", file, line, expr, literal, string_cstr(self));
    return false;
}

#define EXPECT_TRUE(expr) do { if(!expect_true_impl((expr), #expr, __FILE__, __LINE__)) return false; } while(0)
#define EXPECT_SIZE_EQ(lhs, rhs) do { if(!expect_size_eq_impl((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) return false; } while(0)
#define EXPECT_INT_EQ(lhs, rhs) do { if(!expect_int_eq_impl((lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)) return false; } while(0)
#define EXPECT_MEM_EQ(lhs, rhs, len) do { if(!expect_mem_eq_impl((lhs), (rhs), (len), #lhs, #rhs, __FILE__, __LINE__)) return false; } while(0)
#define EXPECT_STRING_EQ(str, literal) do { if(!expect_string_eq_impl((str), (literal), #str, __FILE__, __LINE__)) return false; } while(0)

static bool int_eq(void *lhs, void *rhs) {
    return *(int *)lhs == *(int *)rhs;
}

static JsonValue *json_find(JSON *json, const char *key) {
    vec_foreach(json->root.fields, field) {
        if(string_cmp_sv(&field->key, string_view_from(key))) {
            return &field->value;
        }
    }
    return NULL;
}

static bool run_test(const char *name, TestFn fn) {
    bool ok = fn();
    tests_run++;

    if(ok) {
        fprintf(stderr, "[PASS] %s\n", name);
        return true;
    }

    tests_failed++;
    fprintf(stderr, "[FAIL] %s\n", name);
    return false;
}

static bool test_string_empty_and_views(void) {
    String empty = string_new();
    EXPECT_SIZE_EQ(string_len(&empty), 0);
    EXPECT_SIZE_EQ(string_cap(&empty), SHORT_STRING_CAPACITY);
    EXPECT_STRING_EQ(&empty, "");
    EXPECT_INT_EQ(empty.type, STRING_SHORT);

    StringView view = string_into_view(&empty);
    EXPECT_SIZE_EQ(view.len, 0);
    EXPECT_TRUE(string_view_cmp(view, sv("")));

    String from_empty_view = string_view_into_string(view);
    EXPECT_SIZE_EQ(string_len(&from_empty_view), 0);
    EXPECT_STRING_EQ(&from_empty_view, "");

    string_destroy(&from_empty_view);
    string_destroy(&empty);
    return true;
}

static bool test_string_from_parts_without_null(void) {
    const char raw[] = { 'h', 'e', 'l', 'l', 'o' };
    String from_parts = string_from_parts(raw, 5, 5);
    EXPECT_SIZE_EQ(string_len(&from_parts), 5);
    EXPECT_SIZE_EQ(string_cap(&from_parts), SHORT_STRING_CAPACITY);
    EXPECT_MEM_EQ(string_cstr(&from_parts), "hello", 5);
    EXPECT_INT_EQ(string_cstr(&from_parts)[5], '\0');

    String long_from_parts = string_from_parts(raw, 5, 32);
    EXPECT_SIZE_EQ(string_len(&long_from_parts), 5);
    EXPECT_SIZE_EQ(string_cap(&long_from_parts), 32);
    EXPECT_MEM_EQ(string_cstr(&long_from_parts), "hello", 5);
    EXPECT_INT_EQ(string_cstr(&long_from_parts)[5], '\0');

    StringView raw_view = string_view_new(raw, 5);
    String from_view = string_view_into_string(raw_view);
    EXPECT_SIZE_EQ(string_len(&from_view), 5);
    EXPECT_MEM_EQ(string_cstr(&from_view), "hello", 5);
    EXPECT_INT_EQ(string_cstr(&from_view)[5], '\0');

    string_destroy(&from_view);
    string_destroy(&long_from_parts);
    string_destroy(&from_parts);
    return true;
}

static bool test_string_growth_and_mutation(void) {
    String self = string_from("abcdefghijklmnopqrstuv");
    EXPECT_INT_EQ(self.type, STRING_SHORT);
    EXPECT_SIZE_EQ(string_len(&self), SHORT_STRING_CAPACITY);
    EXPECT_SIZE_EQ(string_cap(&self), SHORT_STRING_CAPACITY);

    string_push(&self, 'w');
    EXPECT_INT_EQ(self.type, STRING_LONG);
    EXPECT_SIZE_EQ(string_len(&self), SHORT_STRING_CAPACITY + 1);
    EXPECT_SIZE_EQ(string_cap(&self), SHORT_STRING_LENGTH);
    EXPECT_STRING_EQ(&self, "abcdefghijklmnopqrstuvw");

    string_push(&self, 'x');
    string_push(&self, 'y');
    EXPECT_SIZE_EQ(string_len(&self), 25);
    EXPECT_TRUE(string_cap(&self) >= 25);
    EXPECT_STRING_EQ(&self, "abcdefghijklmnopqrstuvwxy");

    string_pop(&self);
    string_pop(&self);
    EXPECT_SIZE_EQ(string_len(&self), 23);
    EXPECT_INT_EQ(self.type, STRING_LONG);

    string_pop(&self);
    EXPECT_SIZE_EQ(string_len(&self), SHORT_STRING_CAPACITY);
    EXPECT_INT_EQ(self.type, STRING_SHORT);
    EXPECT_STRING_EQ(&self, "abcdefghijklmnopqrstuv");

    string_destroy(&self);
    return true;
}

static bool test_string_copy_compare_and_search(void) {
    String self = string_from("alpha-beta-gamma");
    String copy = string_copy(&self);
    StringView view = string_into_view(&self);
    StringView copied_view = string_view_copy(view);

    EXPECT_TRUE(string_cmp(&self, &copy));
    EXPECT_TRUE(string_cmp_sv(&self, sv("alpha-beta-gamma")));
    EXPECT_TRUE(string_view_cmp(copied_view, sv("alpha-beta-gamma")));
    EXPECT_TRUE(string_view_cmp_str(copied_view, &copy));
    EXPECT_TRUE(string_contains(&self, sv("beta")));
    EXPECT_TRUE(string_view_contains(view, sv("beta")));
    EXPECT_TRUE(string_view_starts_with(view, sv("alpha")));
    EXPECT_TRUE(string_view_ends_with(view, sv("gamma")));
    EXPECT_TRUE(!string_contains(&self, sv("delta")));
    EXPECT_TRUE(!string_view_contains(view, sv("")));

    string_destroy(&copy);
    string_destroy(&self);
    return true;
}

static bool test_string_format_and_temporary_helpers(void) {
    String formatted = string_format("%s:%d", "item", 7);
    EXPECT_STRING_EQ(&formatted, "item:7");

    string_pushf(&formatted, "/%s", "done");
    EXPECT_STRING_EQ(&formatted, "item:7/done");

    String tmp = tmp_printf("%s-%d", "scratch", 42);
    EXPECT_STRING_EQ(&tmp, "scratch-42");

    const char raw[] = { 'a', 'b', 'c' };
    StringView tmp_view = tmp_copy(string_view_new(raw, 3));
    EXPECT_SIZE_EQ(tmp_view.len, 3);
    EXPECT_MEM_EQ(tmp_view.data, "abc", 3);
    EXPECT_INT_EQ(tmp_view.data[3], '\0');

    StringView tmp_str = tmp_copy_str(&formatted);
    EXPECT_SIZE_EQ(tmp_str.len, string_len(&formatted));
    EXPECT_MEM_EQ(tmp_str.data, string_cstr(&formatted), tmp_str.len);
    EXPECT_INT_EQ(tmp_str.data[tmp_str.len], '\0');

    string_destroy(&formatted);
    return true;
}

static bool test_vector_helpers(void) {
    Vec(int) values = vec_new();
    vec_push(values, 10);
    vec_push(values, 20);
    vec_push(values, 30);

    EXPECT_SIZE_EQ(vec_len(values), 3);
    EXPECT_INT_EQ(values[0], 10);
    EXPECT_INT_EQ(values[2], 30);

    int needle = 20;
    EXPECT_SIZE_EQ(vec_find(values, needle, int_eq), 1);

    Vec(int) clone = vec_copy(values);
    clone[1] = 99;
    EXPECT_INT_EQ(values[1], 20);
    EXPECT_INT_EQ(clone[1], 99);

    EXPECT_TRUE(vec_remove(values, 1));
    EXPECT_SIZE_EQ(vec_len(values), 2);
    EXPECT_INT_EQ(values[0], 10);
    EXPECT_INT_EQ(values[1], 30);

    vec_put(values, 4, 77);
    EXPECT_SIZE_EQ(vec_len(values), 5);
    EXPECT_INT_EQ(values[4], 77);

    EXPECT_INT_EQ(vec_pop(values), 77);
    EXPECT_SIZE_EQ(vec_len(values), 4);

    vec_destroy(clone);
    vec_destroy(values);
    return true;
}

static bool test_file_roundtrip(void) {
    const char *text_path = "test/tmp_core_roundtrip.txt";
    const char *binary_path = "test/tmp_core_roundtrip.bin";
    StringView text_path_view = string_view_from(text_path);
    StringView binary_path_view = string_view_from(binary_path);
    remove(text_path);
    remove(binary_path);

    EXPECT_TRUE(file_write_string(text_path_view, sv("alpha\nbeta")));
    EXPECT_TRUE(file_exists(text_path_view));

    String content = file_read_to_string(text_path_view);
    EXPECT_STRING_EQ(&content, "alpha\nbeta");

    const char raw[] = { 0x00, 0x01, 'A', '\n' };
    Vec(char) expected = vec_from_parts(char, (char *)raw, sizeof(raw));
    EXPECT_TRUE(file_write_vec(binary_path_view, expected));
    EXPECT_TRUE(file_exists(binary_path_view));

    Vec(char) bytes = file_read_to_vec(binary_path_view);
    EXPECT_SIZE_EQ(vec_len(bytes), sizeof(raw));
    EXPECT_MEM_EQ(bytes, raw, sizeof(raw));

    vec_destroy(expected);
    vec_destroy(bytes);
    string_destroy(&content);
    remove(text_path);
    remove(binary_path);
    return true;
}

static bool test_json_roundtrip(void) {
    JSON json = json_parse(sv("{\"name\":\"core\",\"count\":3,\"enabled\":true,\"items\":[1,2],\"nested\":{\"ok\":false},\"nothing\":null}"));

    EXPECT_SIZE_EQ(vec_len(json.root.fields), 6);

    JsonValue *name = json_find(&json, "name");
    EXPECT_TRUE(name != NULL);
    EXPECT_TRUE(json_is_string(name));
    EXPECT_STRING_EQ(json_as_string(name), "core");

    JsonValue *count = json_find(&json, "count");
    EXPECT_TRUE(count != NULL);
    EXPECT_TRUE(json_is_number(count));
    EXPECT_TRUE(json_as_number(count) != NULL);
    EXPECT_TRUE(*json_as_number(count) == 3.0);

    JsonValue *enabled = json_find(&json, "enabled");
    EXPECT_TRUE(enabled != NULL);
    EXPECT_TRUE(json_is_bool(enabled));
    EXPECT_INT_EQ(enabled->kind, JSON_VALUE_TRUE);

    JsonValue *items = json_find(&json, "items");
    EXPECT_TRUE(items != NULL);
    EXPECT_TRUE(json_is_array(items));
    EXPECT_SIZE_EQ(vec_len(*json_as_array(items)), 2);

    JsonValue *nested = json_find(&json, "nested");
    EXPECT_TRUE(nested != NULL);
    EXPECT_TRUE(json_is_obj(nested));
    EXPECT_SIZE_EQ(vec_len(nested->obj->fields), 1);
    EXPECT_TRUE(nested->obj->fields[0].value.kind == JSON_VALUE_FALSE);

    JsonValue *nothing = json_find(&json, "nothing");
    EXPECT_TRUE(nothing != NULL);
    EXPECT_TRUE(json_is_null(nothing));
    EXPECT_INT_EQ(nothing->kind, JSON_VALUE_NULL);

    String pretty = json_to_string(&json, .pretty_print = 2);
    EXPECT_TRUE(string_contains(&pretty, sv("\"name\": \"core\"")));
    EXPECT_TRUE(string_contains(&pretty, sv("\n  \"items\": ")));

    string_destroy(&pretty);
    json_free(json);
    return true;
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
    run_test("string.empty_and_views", test_string_empty_and_views);
    run_test("string.from_parts_without_null", test_string_from_parts_without_null);
    run_test("string.growth_and_mutation", test_string_growth_and_mutation);
    run_test("string.copy_compare_and_search", test_string_copy_compare_and_search);
    run_test("string.format_and_temporary_helpers", test_string_format_and_temporary_helpers);
    run_test("vec.helpers", test_vector_helpers);
    run_test("file.roundtrip", test_file_roundtrip);
    run_test("json.roundtrip", test_json_roundtrip);

    fprintf(stderr, "\n%d/%d tests passed\n", tests_run - tests_failed, tests_run);
    return tests_failed == 0 ? 0 : 1;
}

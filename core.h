#ifndef _CORE_H_
#define _CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <threads.h>

#ifdef  _WIN32
    #define  PLATFORM_WIN32
#else
    #define PLATFORM_POSIX
#endif

#ifndef STRING_GROW_FACTOR
#define STRING_GROW_FACTOR 1.5
#endif

#ifdef CORE_DEBUG_ASSERT
#define CORE_ASSERT(e) assert(e)
#else
#define CORE_ASSERT(e)
#endif

#define CORE_UNUSED(value) (void)(value)
#define CORE_TODO(message) do { fprintf(stderr, "%s:%d: TODO: %s\n", __FILE__, __LINE__, message); abort(); } while(0)
#define CORE_UNREACHABLE(message) do { fprintf(stderr, "%s:%d: UNREACHABLE: %s\n", __FILE__, __LINE__, message); abort(); } while(0)

#define CORE_BIT(x) 1 << (x)
#define FLAG_SET(v, flag) ((v) |= (flag))
#define FLAG_CLEAR(v, flag) ((v) &= ~(flag))
#define FLAG_HAS(v, flag) ((v) & (flag))

#define CORE_CONCAT(a, b) a##b
#define CORE_MACRO_VAR(var) CORE_CONCAT(var, __LINE__)
#define CORE_ARRLEN(arr) (sizeof((arr))/sizeof((arr)[0]))
#define core_defer(begin, end) \
    for(size_t CORE_MACRO_VAR(i) = (begin, 0); !CORE_MACRO_VAR(i); (CORE_MACRO_VAR(i)++), end)
#define core_defer_var(type, var, begin, end, body) \
    { type var; for(size_t CORE_MACRO_VAR(i) = (var = begin, 0); !CORE_MACRO_VAR(i); (CORE_MACRO_VAR(i)++), end) body }
#define SHORT_STRING_LENGTH 24
#define ARENA_DEFAULT_ALLOC_SIZE 1024 * 4
#define RINGBUFFER_SIZE 1024 * 4

#if defined(__GNUC__) || defined(__clang__)
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
//   TODO: implement NOB_PRINTF_FORMAT for MSVC
#    define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef wchar_t wchar;

//  ----------------------------------- //
//             forward-decls            //
//  ----------------------------------- //
//  the caller needs to garuentee that both pointers are valid and cannot go oob
bool partial_cmp_ptr(const char *self, const char *predicate, size_t len);

//  ----------------------------------- //
//                string                //
//  ----------------------------------- //
typedef struct StringView {
    size_t len;
    const char *data;
}StringView;

StringView string_view_from(const char *data);
StringView string_view_new(const char *data, size_t len);
StringView string_view_copy(StringView self);
#define string_view_new_const(s) ((StringView){ .len = sizeof((s)) - 1, .data = (s) })

bool string_view_contains(StringView self, StringView predicate);

#define sv string_view_new_const
#define sv_from string_view_from
#define sv_new string_view_new
#define sv_into_string string_view_into_string
#define sv_contains stringstring_view_contains

typedef struct String {
    enum {
        STRING_SHORT,
        STRING_LONG,
    }type;
    union {
        struct {
            char data[24];
        }s;
        struct {
            char *ptr;
            size_t cap;
            size_t len;
        }l;
    }data;
}String;

typedef struct StringView StringView;

String string_new(void);
String string_new_size(size_t size);
String string_from(const char *ptr);
String string_from_parts(const char *ptr, size_t len, size_t cap);
String string_format(const char *format, ...);
String string_vformat(const char *fmt, va_list args);
void string_destroy(String *self);

const char *string_cstr(String const *self);
size_t string_cap(String const *self);
size_t string_len(String const *self);

void string_push(String *self, char c);
void string_pushf(String *self, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);
void string_push_str(String *self, String other);
void string_push_ptr(String *self, const char *ptr);
void string_pop(String *self);
bool string_cmp(String const *self, String const *other);
bool string_cmp_sv(String const *self, StringView other);
bool string_contains(String *self, StringView predicate);

void string_dump(String const *self);

String string_copy(String const *self);

StringView string_into_view(String const *self);
String string_view_into_string(StringView self);

typedef struct StringView StringView;
typedef struct String String;

//  ----------------------------------- //
//              allocator               //
//  ----------------------------------- //
typedef void *(*AllocFn)(size_t size);
typedef void *(*ReallocFn)(void *mem, size_t size);
typedef void (*FreeFn)(void *_block);
typedef struct Allocator{
    AllocFn alloc;
    ReallocFn realloc;
    FreeFn free;
}Allocator;

extern Allocator global_allocator;

void global_allocator_set(Allocator alloc);
void global_allocator_clear(void);

typedef struct RingBuffer {
    void *base;
    size_t size;
    size_t write_pos;
}RingBuffer, ScratchBuffer;

void *ringbuffer_alloc(RingBuffer *self, size_t size);

//  ----------------------------------- //
//                 file                 //
//  ----------------------------------- //
typedef unsigned int FileMode_;
typedef enum FileMode {
    FILE_READ       = CORE_BIT(0),
    FILE_WRITE      = CORE_BIT(1),
    FILE_APPEND     = CORE_BIT(2),
    FILE_PLUS       = CORE_BIT(3),
    FILE_BIN        = CORE_BIT(4),
}FileMode;
typedef struct File {
    String path;
    FileMode_ mode;
    FILE *fd;
}File, *FileHandle;

FileHandle file_open(const char *path, FileMode_ mode);
void file_close(FileHandle self);
void *file_raw(FileHandle self);
String file_read(FileHandle self);
char *file_read_binary(FileHandle self);
bool file_write_raw(FileHandle self, const char *data, size_t len);
bool file_write(FileHandle self, const StringView data);
bool file_exists(const StringView path);

String file_read_to_string(const char *path);
char *file_read_to_vec(const char *path);

FileHandle stdio_get(void);
FileHandle stderr_get(void);
FileHandle stdin_get(void);

//  ----------------------------------- //
//               vector                 //
//  ----------------------------------- //
#define DEFAULT_INITIAL_VECTOR_SIZE 8

typedef struct ArrayHeader {
    size_t len;
    size_t cap;
} ArrayHeader;

void *core_vec_create_internal(size_t capacity, size_t elem_size);
void *core_vec_maygrow_internal(void *arr, size_t elem_size);
void core_vec_destroy_internal(void *arr);
void *core_vec_create_empty_internal(void);
void *core_vec_copy(void *arr, size_t elem_size);
void *core_vec_create_from_parts_internal(void *ptr, size_t size, size_t elem_size);

#define vec_header(v) ((struct ArrayHeader *)(v) - 1)
#define vec_len(v) (vec_header((v))->len)
#define vec_cap(v) (vec_header((v))->cap)

#define Vec(type) type *
#define vec_new() core_vec_create_empty_internal()
#define vec_new_with(items) core_vec_create_from_parts_internal(items, CORE_ARRLEN(items), sizeof(items[0]))
#define vec_clear(v) (vec_header((v))->len = 0)
#define vec_with_size(ty, size) core_vec_create_internal((size), sizeof(ty))
#define vec_from_parts(ty, ptr, size) core_vec_create_from_parts_internal((ptr), (size), sizeof(ty))
#define vec_destroy(arr) (core_vec_destroy_internal(vec_header(arr)), arr = NULL)
#define vec_push(arr, value) ((arr) = core_vec_maygrow_internal((arr), sizeof(*(arr))), (arr)[vec_header((arr))->len++] = (value))
#define vec_pop(arr) (vec_header((arr))->len--)
#define vec_put(arr, index, value) {\
        if(!arr) arr = core_vec_maygrow_internal((arr), (size_t)sizeof(*(arr)));\
        if((index) >= vec_len((arr))) {\
            vec_len(arr) = index + 1;\
        }\
        (arr) = core_vec_maygrow_internal((arr), (size_t)sizeof(*(arr)));\
        (arr)[(index)] = (value);\
}

#define vec_move(arr) (arr)
#define vec_copy(arr) core_vec_copy((arr), sizeof((*arr)))
#define vec_at(arr, index) (CORE_ASSERT(index < vec_len(arr)), (arr)[(index)])
#define vec_iter(arr, iter) for(size_t (iter) = 0; (iter) < vec_len((arr)); (iter)++)
#define vec_foreach(arr, item) for(__typeof__(*(arr)) *item = (arr); item != (arr) + vec_len((arr)); item++)

void vec_dump(void *vec);

//  ----------------------------------- //
//                slice                 //
//  ----------------------------------- //
typedef struct _Slice {
    void *data;
    size_t len;
}_Slice;

#define Slice(ty) _Slice
#define slice_copy(slice) (_Slice){ .data = (slice).data, .len = (slice).len }
#define slice_from_vec(vec) (_Slice){ .data = vec, .len = vec_len(vec) }
#define slice_from_avec(vec) (_Slice){ .data = vec, .len = avec_len(vec) }
#define slice_to_vec(slice) core_vec_create_from_parts_internal((slice).data, (slice).len, sizeof(*(slice).data))

//  ----------------------------------- //
//                arena                 //
//  ----------------------------------- //
typedef struct ArenaAllocator ArenaAllocator;

struct ArenaAllocator {
    Vec(char) buffer;
    char *current_alloc;
    size_t alloc_size;
    ArenaAllocator *next;
};

ArenaAllocator arena_new(size_t size);
void arena_dealloc(ArenaAllocator *arena);

void *arena_alloc(ArenaAllocator *alloc, size_t size);
void *arena_realloc(ArenaAllocator *alloc, void *src, size_t size);
void arena_clear(ArenaAllocator *alloc);

//  ----------------------------------- //
//                astring               //
//  ----------------------------------- //
typedef struct AString {
    ArenaAllocator *arena;
    char *ptr;
    size_t cap;
    size_t len;
}AString;

AString astring_new(ArenaAllocator *arena);
AString astring_new_size(ArenaAllocator *arena, size_t size);
AString astring_from(ArenaAllocator *arena, const char *ptr);
AString astring_from_parts(ArenaAllocator *arena, const char *ptr, size_t len, size_t cap);
AString astring_format(ArenaAllocator *arena, const char *format, ...);
AString astring_vformat(ArenaAllocator *arena, const char *fmt, va_list args);
AString astring_from_string(ArenaAllocator *arena, String const *string);

const char *astring_cstr(AString const *self);
size_t astring_cap(AString const *self);
size_t astring_len(AString const *self);

void astring_push(AString *self, char c);
void astring_push_str(AString *self, AString other);
void astring_push_ptr(AString *self, const char *ptr);
void astring_pop(AString *self);
bool astring_cmp(AString const *self, AString const *other);
bool astring_contains(AString const *self, AString const *predicate);
bool astring_contains_sv(AString const *self, StringView predicate);

void astring_dump(AString const *self);

AString astring_copy(AString const *self);
String astring_to_string(AString const *self);

void *core_avec_create_internal(ArenaAllocator *arena, size_t capacity, size_t elem_size);
void *core_avec_maygrow_internal(ArenaAllocator *arena, void *arr, size_t elem_size);
void *core_avec_create_empty_internal(ArenaAllocator *arena);
void *core_avec_copy(ArenaAllocator *arena, void *arr, size_t elem_size);
void *core_avec_to_vec_int(void *src, size_t elem_size);

#define avec_header(v) ((struct ArrayHeader *)(v) - 1)
#define avec_len(v) (avec_header((v))->len)
#define avec_cap(v) (avec_header((v))->cap)

#define AVec(type) type *
#define avec_new(arena) core_avec_create_empty_internal((arena))
#define avec_clear(v) (avec_header((v))->len = 0)
#define avec_with_size(arena, ty, size) core_avec_create_internal((arena), (size), sizeof(ty))
#define avec_destroy(arr) (arr = NULL)
#define avec_push(arr, arena, value) ((arr) = core_avec_maygrow_internal((arena), (arr), sizeof(*(arr))), (arr)[avec_header((arr))->len++] = (value))
#define avec_pop(arr) (avec_header((arr))->len--)
#define avec_put(arr, arena, index, value) {\
        if(!arr) arr = core_avec_maygrow_internal((arena), (arr), (size_t)sizeof(*(arr)));\
        if((index) >= avec_len((arr))) {\
            avec_len(arr) = index + 1;\
        }\
        (arr) = core_avec_maygrow_internal((arena), (arr), (size_t)sizeof(*(arr)));\
        (arr)[(index)] = (value);\
}

#define avec_copy(arr, arena) core_avec_copy((arena), (arr), sizeof((*arr)))
#define avec_at(arr, index) (arr)[(index)]
#define avec_iter(arr, iter) for(size_t (iter) = 0; (iter) < avec_len((arr)); (iter)++)
#define avec_foreach(arr, item) for(__typeof__(*(arr)) *item = (arr); item != (arr) + avec_len((arr)); item++)

//  ----------------------------------- //
//                 print                //
//  ----------------------------------- //
int print(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
int println(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
int fprint(FileHandle stream, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);
int fprintln(FileHandle stream, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);

typedef enum LogLevel {
    CORE_TRACE,
    CORE_DEBUG,
    CORE_INFO,
    CORE_WARNING,
    CORE_ERROR,
} LogLevel;

void core_log(LogLevel level, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);
void __core_log_file(LogLevel level, const char *file, const char *fmt, ...) CORE_PRINTF_FORMAT(3, 4);
#define log core_log
#define log_file(level, fmt, ...) __core_log_file(level, __FILE__, fmt, __VA_ARGS__)
/*
//  ----------------------------------- //
//                flags                 //
//  ----------------------------------- //
typedef struct FlagContext {
    int argc;
    const char *const *argv;
    const char *program_name;
}FlagContext;
const char *const *flag_str(const char *lo, char sh, const char *const *def);

void flags_parse(int argc, const char *const *argv);
*/
//  ----------------------------------- //
//                context               //
//  ----------------------------------- //
typedef struct Context {
    ArenaAllocator temp_arena;
    RingBuffer ring_buffer;
    //FlagContext *flag_context;
}Context;

extern thread_local Context core_context;
AString tmp_printf(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
StringView tmp_copy(StringView self);
StringView tmp_copy_str(String *self);

#ifdef CORE_IMPLEMENTATION
//  ----------------------------------- //
//             string-impl              //
//  ----------------------------------- //
StringView string_view_from(const char *data) {
    return (StringView){ strlen(data), data };
}

StringView string_view_new(const char *data, size_t len) {
    return (StringView){ len, data };
}

String string_view_into_string(StringView self) {
    return string_from_parts(self.data, self.len, self.len);
}

StringView string_view_copy(StringView self) {
    return (StringView) {
        .len = self.len,
        .data = self.data,
    };
}

bool string_view_contains(StringView self, StringView predicate) {
    if(predicate.len == 0) {
        return false;
    }

    for(size_t i = 0; i < self.len; i++) {
        if(partial_cmp_ptr(&self.data[i], predicate.data, predicate.len)) {
            return true;
        }
    }

    return false;
}

//  the caller needs to garuentee that both pointers are valid and cannot go oob
bool partial_cmp_ptr(const char *self, const char *predicate, size_t len) {
    for(size_t i = 0; i < len; i ++) {
        if(self[i] != predicate[i]) {
            return false;
        }
    }
    return true;
}

static char string_cpy_buffer[SHORT_STRING_LENGTH] = {0};
static void string_transform(String *self) {
    if(self->type == STRING_LONG) {
        size_t len = self->data.l.len;
        memcpy(string_cpy_buffer, self->data.l.ptr, len * sizeof(char));
        global_allocator.free(self->data.l.ptr);
        memmove(self->data.s.data, string_cpy_buffer, len * sizeof(char));
        self->type = STRING_SHORT;
    }else {
        memcpy(string_cpy_buffer, self->data.s.data, SHORT_STRING_LENGTH * sizeof(char));
        self->data.l.ptr = global_allocator.alloc((SHORT_STRING_LENGTH + 1) * sizeof(char));
        memcpy(self->data.l.ptr, string_cpy_buffer, SHORT_STRING_LENGTH * sizeof(char));
        self->data.l.len = SHORT_STRING_LENGTH;
        self->data.l.cap = SHORT_STRING_LENGTH + 1;
        self->type = STRING_LONG;
    }
    memset(string_cpy_buffer, 0, sizeof(*string_cpy_buffer));
    //printf("[info]: transformed string\n");
}

static void string_grow(String *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_grow`");
    CORE_ASSERT(self->type == STRING_LONG && "error: cannot grow small `String`");
    size_t new_size = (self->data.l.cap * STRING_GROW_FACTOR);
    self->data.l.ptr = global_allocator.realloc(self->data.l.ptr, new_size * sizeof(char));
    self->data.l.cap = new_size;
    //printf("[info]: grew string\n");
}

String string_new(void) {
    return (String){
        .type = STRING_SHORT,
        .data = {0},
    };
}

String string_new_size(size_t size) {
    String self = {0};
    self = (String){
        .type = size > SHORT_STRING_LENGTH ? STRING_LONG : STRING_SHORT,
    };
    if(self.type == STRING_LONG) {
        self.data.l.cap = size;
        self.data.l.ptr = global_allocator.alloc((size + 1) * sizeof(char));
        CORE_ASSERT(self.data.l.ptr && "error: failed to allocate `String`");
    }
    return self;
}

String string_from(const char *ptr) {
    CORE_ASSERT(ptr && "error: cannot pass nullptr to `string_from`");
    //FIXME: do not put the fucking nulltermination char in length ????
    size_t len = strlen(ptr);
    String self = string_new_size(len);
    if(self.type == STRING_LONG) {
        self.data.l.len = len;
        strcpy(self.data.l.ptr, ptr);
    }else {
        self.data.s.data[23] = SHORT_STRING_LENGTH - len;
        strcpy(self.data.s.data, ptr);
    }
    return self;
}

String string_from_parts(const char *ptr, size_t len, size_t cap) {
    if(cap > SHORT_STRING_LENGTH) {
        String str = {
            .type = STRING_LONG,
            .data.l.ptr = global_allocator.alloc(cap * sizeof(char)),
            .data.l.len = len,
            .data.l.cap = cap,
        };
        strcpy(str.data.l.ptr, ptr);
        return str;
    }
    String str = { .type = STRING_SHORT };
    str.data.s.data[23] = SHORT_STRING_LENGTH - len;
    strcpy(str.data.s.data, ptr);
    return str;
}

String string_format(const char *format, ...) {
    va_list args;
    va_start(args, format);
    String self = string_vformat(format, args);
    va_end(args);
    return self;
}

String string_vformat(const char *fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    String self = string_new_size(size);
    if(self.type == STRING_SHORT) {
        vsnprintf(self.data.s.data, size + 1, fmt, args_copy);
        println("size = %zu", size);
        self.data.s.data[size] = '\0';
        self.data.s.data[23] = SHORT_STRING_LENGTH - size;
    }else {
        vsnprintf(self.data.l.ptr, size + 1, fmt, args_copy);
        self.data.l.len = size;
    }

    return self;
}

void string_destroy(String *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_destroy`");
    if(self->type == STRING_SHORT)
        return;
    global_allocator.free(self->data.l.ptr);
}

const char *string_cstr(String const *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_cstr`");
    if(self->type == STRING_LONG) {
        return self->data.l.ptr;
    }
    return self->data.s.data;
}

size_t string_cap(String const *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_cap`");
    if(self->type == STRING_SHORT) {
        return SHORT_STRING_LENGTH;
    }
    return self->data.l.cap;
}

size_t string_len(String const *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_len`");
    if(self->type == STRING_SHORT) {
        return SHORT_STRING_LENGTH - self->data.s.data[23];
    }
    return self->data.l.len;
}

void string_push(String *self, char c) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_push`");
    if(self->type == STRING_SHORT) {
        if((SHORT_STRING_LENGTH - self->data.s.data[23]) >= SHORT_STRING_LENGTH) {
            string_transform(self);
            self->data.l.ptr[self->data.l.len - 1] = c;
            self->data.l.ptr[self->data.l.len++] = '\0';
            return;
        }
        self->data.s.data[SHORT_STRING_LENGTH - self->data.s.data[23] - 1] = c;
        self->data.s.data[SHORT_STRING_LENGTH - self->data.s.data[23]--] = '\0';
        return;
    }
    if(self->data.l.len == self->data.l.cap) {
        string_grow(self);
    }
    self->data.l.ptr[self->data.l.len - 1] = c;
    self->data.l.ptr[self->data.l.len++] = '\0';
}

void string_pushf(String *self, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    char *buffer = ringbuffer_alloc(&core_context.ring_buffer, size + 1);
    vsnprintf(buffer, size + 1, fmt, args);
    va_end(args);
    string_push_ptr(self, buffer);
}

void string_push_str(String *self, String other) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_push_str`");
    for(size_t i = 0; i < string_len(&other) - 1; i++) {
        string_push(self, string_cstr(&other)[i]);
    }
}

void string_push_ptr(String *self, const char *ptr) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_push_ptr`");
    while(*ptr != '\0') {
        string_push(self, *ptr++);
    }
}

void string_pop(String *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_pop`");
    CORE_ASSERT(string_len(self) > 0 && "error: cannot pop from empty `String`");
    if(self->type == STRING_SHORT) {
        self->data.s.data[(SHORT_STRING_LENGTH - ++self->data.s.data[23]) - 1] = '\0';
    }else {
        self->data.l.ptr[(--self->data.l.len) - 1] = '\0';
        if((self->data.l.len) <= 24) {
            string_transform(self);
        }
    }
}

bool string_cmp(String const *self, String const *other) {
    size_t self_len = string_len(self);
    if(self_len != string_len(other)) {
        return false;
    }

    const char *self_ptr = string_cstr(self);
    const char *other_ptr = string_cstr(other);
    for(size_t i = 0; i < self_len; i++) {
        if(self_ptr[i] != other_ptr[i]) {
            return false;
        }
    }
    return true;
}

bool string_cmp_sv(String const *self, StringView other) {
    size_t self_len = string_len(self);
    if(self_len != other.len) {
        return false;
    }

    const char *self_ptr = string_cstr(self);
    for(size_t i = 0; i < self_len; i++) {
        if(self_ptr[i] != other.data[i]) {
            return false;
        }
    }
    return true;
}

bool string_contains(String *self, StringView predicate) {
    if(predicate.len == 0) {
        return false;
    }

    const char *self_ptr = string_cstr(self);
    for(size_t i = 0; i < string_len(self); i++) {
        if(partial_cmp_ptr(&self_ptr[i], predicate.data, predicate.len)) {
            return true;
        }
    }

    return false;
}

void string_dump(String const *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_dump`");
    const char *data = NULL;
    if(string_len(self) > 40) {
        data = "[..]";
    }else {
        data = string_cstr(self);
    }

    fprintf(stdout, "String { data: \"%s\", len: %zu, cap: %zu, type: %s }\n",
            data,
            string_len(self),
            string_cap(self),
            self->type == STRING_SHORT ? "`short`" : "`long`");
}

String string_copy(String const *self) {
    return string_from_parts(string_cstr(self), string_len(self), string_cap(self));
}

StringView string_into_view(String const *self) {
    return (StringView){
        .len = string_len(self),
        .data = string_cstr(self),
    };
}


//  ----------------------------------- //
//           allocator-impl             //
//  ----------------------------------- //
Allocator global_allocator;

#ifdef CORE_MEM_DEBUG
static void *debug_alloc(size_t size) {
    void *alloc = malloc(size);
    fprintf(stderr, "[INFO]: allocated `%zu` bytes at address `%p`\n", size, alloc);
    fflush(stderr);
    return alloc;
}

static void *debug_realloc(void *ptr, size_t new_size) {
    void *new = realloc(ptr, new_size);
    fprintf(stderr, "[INFO]: reallocated `%p` to `%p` location with size `%zu`\n", ptr, new, new_size);
    fflush(stderr);
    return new;
}

static void debug_free(void *block) {
    free(block);
    fprintf(stderr, "[INFO]: freed allocation at `%p`\n", block);
    fflush(stderr);
}

Allocator default_allocator = {
    .alloc = debug_alloc,
    .realloc = debug_realloc,
    .free = debug_free,
};

Allocator global_allocator = {
    .alloc = debug_alloc,
    .realloc = debug_realloc,
    .free = debug_free,
};
#else

Allocator default_allocator = {
    .alloc = malloc,
    .realloc = realloc,
    .free = free,
};

Allocator global_allocator = {
    .alloc = malloc,
    .realloc = realloc,
    .free = free,
};

#endif //CORE_MEM_DEBUG

void set_global_allocator(Allocator alloc) {
    global_allocator = alloc;
}

void clear_global_allocator(void) {
    global_allocator = default_allocator;
}

static RingBuffer ring_buffer_init(void) {
    return (RingBuffer) {
        .base = global_allocator.alloc(RINGBUFFER_SIZE),
        .size = RINGBUFFER_SIZE,
        .write_pos = 0,
    };
}

void *ringbuffer_alloc(RingBuffer *self, size_t size) {
    if(!self) {
        *self = ring_buffer_init();
    }

    if((char*)self->base + self->write_pos + size > (char*)self->base + self->size) {
        self->write_pos = 0;
    }

    void *alloc = (char*)self->base + self->write_pos;
    self->write_pos += size;
    return alloc;
}

//  ----------------------------------- //
//               file-impl              //
//  ----------------------------------- //
static String mode_to_string(FileMode_ mode) {
    char mode_buf[64] = "";
    switch (mode) {
        case FILE_READ: {
            snprintf(mode_buf, 64, "%s", "r");
        } break;
        case FILE_APPEND: {
            snprintf(mode_buf, 64, "%s", "a");
        } break;
        case FILE_WRITE: {
            snprintf(mode_buf, 64, "%s", "w");
        } break;
        case FILE_PLUS: {
            snprintf(mode_buf, 64, "%s", "+");
        } break;
        case FILE_BIN: {
            snprintf(mode_buf, 64, "%s", "b");
        } break;
    }
    return string_from(mode_buf);
}

FileHandle file_open(const char *path, FileMode_ mode) {
    String mode_str = mode_to_string(mode);
    const char *mode_cstr = string_cstr(&mode_str);
    CORE_ASSERT(mode_cstr && "invalid file mode");
    FileHandle self = global_allocator.alloc(sizeof(File));
    if(!self) return NULL;
    *self = (File){
        .path = string_from(path),
        .mode = mode,
        .fd = fopen(path, mode_cstr)
    };
    string_destroy(&mode_str);
    if(!self->fd) {
        return NULL;
    }
    return self;
}

void file_close(FileHandle self) {
    fclose(self->fd);
    global_allocator.free(self);
    self = NULL;
}

void *file_raw(FileHandle self) {
    return self->fd;
}

String file_read(FileHandle self) {
    size_t size = 0;
    fseek(self->fd, 0, SEEK_END);
    size = ftell(self->fd);
    rewind(self->fd);
    char *content = global_allocator.alloc((size + 1) * sizeof(char));
    if(!content)
        return string_new();
    fread(content, sizeof(char), size, self->fd);
    content[size] = '\0';
    String str = string_from_parts(content, size, size);
    global_allocator.free(content);
    return str;
}

Vec(char) file_read_binary(FileHandle self) {
    size_t size = 0;
    fseek(self->fd, 0, SEEK_END);
    size = ftell(self->fd);
    rewind(self->fd);
    char *content = global_allocator.alloc((size) * sizeof(char));
    if(!content)
        return vec_new();
    fread(content, sizeof(char), size, self->fd);
    Vec(char) vec = vec_from_parts(char, content, size);
    global_allocator.free(content);
    return vec;
}

bool file_write_raw(FileHandle self, const char *data, size_t len) {
    fwrite(data, sizeof(char), len, self->fd);
    return ferror(self->fd) != 0;
}

bool file_write(FileHandle self, const StringView data) {
    return file_write_raw(self, data.data, data.len);
}

bool file_exists(const StringView path) {
    FILE *fd = fopen(path.data, "r");
    if(fd) {
        fclose(fd);
        return true;
    }
    return false;
}


String file_read_to_string(const char *path) {
    FileHandle file = file_open(path, FILE_READ);
#ifndef CORE_DEBUG
    if(!file) {
        fprintf(stderr, "[INFO]: failed to open file `%s`", path);
    }
#endif
    String content = file_read(file);
    file_close(file);
    return content;
}

Vec(char) file_read_to_vec(const char *path) {
    FileHandle file = file_open(path, FILE_READ | FILE_BIN);
#ifndef CORE_DEBUG
    if(!file) {
        fprintf(stderr, "[INFO]: failed to open file `%s`", path);
    }
#endif
    Vec(char) content = file_read_binary(file);
    file_close(file);
    return content;
}

static bool std_file_handles_init = false;
static File out = {0};
static File err = {0};
static File in = {0};

static void init_file_handles(void) {
    out = (File) {
        .fd = stdout,
        .mode = FILE_WRITE,
        .path = string_from("STDOUT"),
    };
    err = (File) {
        .fd = stderr,
        .mode = FILE_WRITE,
        .path = string_from("STDERR"),
    };
    in = (File) {
        .fd = stdin,
        .mode = FILE_READ,
        .path = string_from("STDIN"),
    };
    std_file_handles_init = true;
}

FileHandle stdio_get(void) {
    if(!std_file_handles_init) {
        init_file_handles();
    }
    return &out;
}

FileHandle stderr_get(void) {
    if(!std_file_handles_init) {
        init_file_handles();
    }
    return &err;
}

FileHandle stdin_get(void) {
    if(!std_file_handles_init) {
        init_file_handles();
    }
    return &in;
}

//  ----------------------------------- //
//             vector-impl              //
//  ----------------------------------- //
void *core_vec_create_internal(size_t capacity, size_t elem_size) {
    ArrayHeader *arr = global_allocator.alloc((capacity * elem_size) + sizeof(ArrayHeader));
    memset(arr, 0, (capacity * elem_size) + sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = capacity;
    arr++;
    return arr;
}

/*static void *core_vec_create_header_present_internal(void *arr, size_t capacity, size_t elem_size) {
    ArrayHeader *tmp = global_allocator.realloc(vec_header(arr), sizeof(ArrayHeader) + capacity * elem_size);
    tmp->cap = capacity;
    tmp++;
    return tmp;
}*/


void *core_vec_maygrow_internal(void *arr, size_t elem_size) {
    /*if(!arr) {
        if((arr - sizeof(ArrayHeader)) != NULL) {
            arr = core_vec_create_header_present_internal(arr, DEFAULT_INITIAL_VECTOR_SIZE, elem_size);
        }else {
            arr = core_vec_create_internal(DEFAULT_INITIAL_VECTOR_SIZE, elem_size);
        }
    }*/

    if(vec_len(arr) >= vec_cap(arr)) {
        vec_cap(arr) = vec_cap(arr) == 0 ? DEFAULT_INITIAL_VECTOR_SIZE : vec_cap(arr) * 2;
        ArrayHeader *tmp = global_allocator.realloc(vec_header(arr), (vec_cap(arr) * elem_size) + sizeof(ArrayHeader));
        tmp++;
        vec_len(tmp) = vec_len(arr);
        vec_cap(tmp) = vec_cap(arr);
        memcpy(tmp, arr, vec_len(arr) * elem_size);
        return tmp;
    }
    return arr;
}

void core_vec_destroy_internal(void *arr) {
    global_allocator.free(arr);
}

void *core_vec_create_empty_internal(void) {
    ArrayHeader *arr = global_allocator.alloc(sizeof(ArrayHeader));
    memset(arr, 0, sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = 0;
    arr++;
    return arr;
}

void *core_vec_copy(void *arr, size_t elem_size) {
    ArrayHeader *other = arr;
    size_t other_cap = other[-1].cap;
    ArrayHeader *new = core_vec_create_internal(other_cap, elem_size);
    new[-1].len = other[-1].len;
    memcpy((void*)new, arr, new[-1].len * elem_size);
    return new;
}

void *core_vec_create_from_parts_internal(void *ptr, size_t size, size_t elem_size) {
    void *vec = core_vec_create_internal(size, elem_size);
    vec_len(vec) = size;
    memcpy(vec, ptr, size * elem_size);
    return vec;
}

void vec_dump(void *vec) {
    println("Vec { data: [..], len: %zu, cap: %zu }", vec_len(vec), vec_cap(vec));
}

//  ----------------------------------- //
//             arena-impl               //
//  ----------------------------------- //
ArenaAllocator arena_new(size_t size) {
    if(size == 0) {
        size = ARENA_DEFAULT_ALLOC_SIZE;
    }
    Vec(char) buffer = vec_with_size(char, size);
    ArenaAllocator self = {
        .buffer = buffer,
        .current_alloc = buffer,
        .alloc_size = 0,
        .next = NULL,
    };
    return self;
}

void arena_dealloc(ArenaAllocator *self) {
    if(self->next) {
        arena_dealloc(self->next);
    }
    vec_destroy(self->buffer);
}

static void arena_clear_int(ArenaAllocator *self) {
    self->current_alloc = self->buffer;
    if(self->next)
        arena_clear_int(self->next);
}

static void* arena_alloc_internal(ArenaAllocator *self, size_t size) {
    if(self->buffer == NULL || self->current_alloc == NULL) {
        *self = arena_new(0);
    }

    if(self->next) {
        return arena_alloc_internal(self->next, size);
    }
    if(self->current_alloc + size > self->buffer + vec_cap(self->buffer)) {
        self->next = default_allocator.alloc(sizeof(ArenaAllocator));
        *self->next = arena_new(ARENA_DEFAULT_ALLOC_SIZE);
        return arena_alloc_internal(self->next, size);
    }
    void *alloc = self->current_alloc;
    self->current_alloc += size;
    return alloc;
}

void *arena_alloc(ArenaAllocator *alloc, size_t size) {
    return arena_alloc_internal(alloc, size);
}

void *arena_realloc(ArenaAllocator *alloc, void *src, size_t new_size) {
    (void)src;
    void *tmp = arena_alloc(alloc, new_size);
    return tmp;
}

void arena_clear(ArenaAllocator *alloc) {
    arena_clear_int(alloc);
}

//  ----------------------------------- //
//             astring-impl             //
//  ----------------------------------- //
static void astring_grow(AString *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `string_grow`");
    size_t new_size = (self->cap * STRING_GROW_FACTOR);
    self->ptr = arena_realloc(self->arena, self->ptr, new_size * sizeof(char));
    self->cap = new_size;
}

AString astring_new(ArenaAllocator *arena) {
    return (AString){
        .arena = arena,
        .ptr = NULL,
        .cap = 0,
        .len = 0,
    };
}

AString astring_new_size(ArenaAllocator *arena, size_t size) {
    char *ptr = arena_alloc(arena, size);
    CORE_ASSERT(ptr && "error: failed to allocate `AString`");
    return (AString){
        .arena = arena,
        .ptr = ptr,
        .cap = size,
        .len = 0,
    };
}

AString astring_from(ArenaAllocator *arena, const char *ptr) {
    CORE_ASSERT(ptr && "error: cannot pass nullptr to `astring_from`");
    size_t size = strlen(ptr);
    AString self = astring_new_size(arena, size);
    strcpy_s(self.ptr, size + 1, ptr);
    self.len = size;
    return self;
}

AString astring_from_parts(ArenaAllocator *arena, const char *ptr, size_t len, size_t cap) {
    AString self = {
        .arena = arena,
        .ptr = arena_alloc(arena, cap),
        .len = len,
        .cap = cap
    };
    strcpy_s(self.ptr, self.cap, ptr);
    return self;
}

AString astring_format(ArenaAllocator *arena, const char *format, ...) {
    va_list args;
    va_start(args, format);
    AString self = astring_vformat(arena, format, args);
    va_end(args);
    return self;
}

AString astring_vformat(ArenaAllocator *arena, const char *fmt, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    AString self = astring_new_size(arena, size + 1);
    vsnprintf(self.ptr, size + 1, fmt, args_copy);
    self.len = size;
    return self;
}

AString astring_from_string(ArenaAllocator *arena, String const *string) {
    return astring_from_parts(arena, string_cstr(string), string_len(string), string_cap(string));
}

const char *astring_cstr(AString const *self) {
    return self->ptr;
}

size_t astring_cap(AString const *self) {
    return self->cap;
}

size_t astring_len(AString const *self) {
    return self->len;
}

void astring_push(AString *self, char c) {
    if(self->len == self->cap) {
        astring_grow(self);
    }
    self->ptr[self->len - 1] = c;
    self->ptr[self->len++] = '\0';
}

void astring_push_str(AString *self, AString other) {
    for(size_t i = 0; i < astring_len(self); i++) {
        astring_push(self, astring_cstr(&other)[i]);
    }
}

void astring_push_ptr(AString *self, const char *ptr) {
    while(*ptr) {
        astring_push(self, *ptr++);
    }
}

void astring_pop(AString *self) {
    CORE_ASSERT(self->len > 0 && "error: cannot pop of an empty `AString`");
    self->ptr[(--self->len) - 1] = '\0';
}

bool astring_cmp(AString const *self, AString const *other) {
    size_t self_len = astring_len(self);
    if(self_len != astring_len(other)) {
        return false;
    }

    const char *self_ptr = astring_cstr(self);
    const char *other_ptr = astring_cstr(other);
    for(size_t i = 0; i < self_len; i++) {
        if(self_ptr[i] != other_ptr[i]) {
            return false;
        }
    }
    return true;
}

bool astring_contains(AString const *self, AString const *predicate) {
    if(astring_len(predicate) == 0) {
        return false;
    }

    const char *self_ptr = astring_cstr(self);
    const char *predicate_ptr = astring_cstr(predicate);
    for(size_t i = 0; i < astring_len(self); i++) {
        if(partial_cmp_ptr(&self_ptr[i], predicate_ptr, astring_len(predicate))) {
            return true;
        }
    }

    return false;
}

bool astring_contains_sv(AString const *self, StringView predicate) {
    if(predicate.len == 0) {
        return false;
    }

    const char *self_ptr = astring_cstr(self);
    for(size_t i = 0; i < astring_len(self); i++) {
        if(partial_cmp_ptr(&self_ptr[i], predicate.data, predicate.len)) {
            return true;
        }
    }

    return false;
}

void astring_dump(AString const *self) {
    CORE_ASSERT(self && "error: cannot pass nullptr to `astring_dump`");
    const char *data = NULL;
    if(astring_len(self) > 40) {
        data = "[..]";
    }else {
        data = astring_cstr(self);
    }
    fprintf(stdout, "AString { ptr: \"%s\", len: %zu, cap: %zu }\n", data, self->len, self->cap);
}

AString astring_copy(AString const *self) {
    return astring_from_parts(self->arena, astring_cstr(self), astring_len(self), astring_cap(self));
}

String astring_to_string(AString const *self) {
    return string_from_parts(self->ptr, self->len, self->cap);
}

//  ----------------------------------- //
//             avector-impl             //
//  ----------------------------------- //
void *core_avec_create_internal(ArenaAllocator *arena, size_t capacity, size_t elem_size) {
    ArrayHeader *arr = arena_alloc(arena, (capacity * elem_size) + sizeof(ArrayHeader));
    memset(arr, 0, (capacity * elem_size) + sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = capacity;
    arr++;
    return arr;
}

void *core_avec_maygrow_internal(ArenaAllocator *arena, void *arr, size_t elem_size) {
    /*if(!arr) {
        if((arr - sizeof(ArrayHeader)) != NULL) {
            arr = core_vec_create_header_present_internal(arr, DEFAULT_INITIAL_VECTOR_SIZE, elem_size);
        }else {
            arr = core_vec_create_internal(DEFAULT_INITIAL_VECTOR_SIZE, elem_size);
        }
    }*/

    if(avec_len(arr) >= avec_cap(arr)) {
        avec_cap(arr) = avec_cap(arr) == 0 ? DEFAULT_INITIAL_VECTOR_SIZE : avec_cap(arr) * 2;
        ArrayHeader *tmp = arena_realloc(arena, avec_header(arr), (avec_cap(arr) * elem_size) + sizeof(ArrayHeader));
        tmp++;
        avec_len(tmp) = avec_len(arr);
        avec_cap(tmp) = avec_cap(arr);
        memcpy(tmp, arr, avec_len(arr) * elem_size);
        return tmp;
    }
    return arr;
}

void *core_avec_create_empty_internal(ArenaAllocator *arena) {
    ArrayHeader *arr = arena_alloc(arena, sizeof(ArrayHeader));
    CORE_ASSERT(arr && "failed to allocate array in arena");
    memset(arr, 0, sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = 0;
    arr++;
    return arr;
}

void *core_avec_copy(ArenaAllocator *arena, void *arr, size_t elem_size) {
    ArrayHeader *other = arr;
    size_t other_cap = other[-1].cap;
    ArrayHeader *new = core_avec_create_internal(arena, other_cap, elem_size);
    new[-1].len = other[-1].len;
    memcpy((void*)new, arr, new[-1].len * elem_size);
    return new;
}

void *core_avec_to_vec_int(void *src, size_t elem_size) {
    void *dest = core_vec_create_internal(avec_cap(src), elem_size);
    memcpy(dest, src, avec_len(src) * elem_size);
    vec_len(dest) = avec_len(src);
    return dest;
}

//  ----------------------------------- //
//               print-impl             //
//  ----------------------------------- //
int print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(stdout, fmt, args);
    va_end(args);
    return ret;
}

int println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(stdout, fmt, args);
    va_end(args);
    printf("\n");
    return ret;
}

int fprint(FileHandle stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(file_raw(stream), fmt, args);
    va_end(args);
    return ret;
}

int fprintln(FileHandle stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int ret = vfprintf(file_raw(stream), fmt, args);
    va_end(args);
    fprint(file_raw(stream), "\n");
    return ret;
}

void core_log(LogLevel level, const char *fmt, ...)
{
    switch (level) {
    case CORE_TRACE:
        fprintf(stderr, "[TRACE] ");
        break;
    case CORE_DEBUG:
        fprintf(stderr, "[DEBUG] ");
        break;
    case CORE_INFO:
        fprintf(stderr, "[INFO] ");
        break;
    case CORE_WARNING:
        fprintf(stderr, "[WARNING] ");
        break;
    case CORE_ERROR:
        fprintf(stderr, "[ERROR] ");
        break;
    default:
        CORE_UNREACHABLE("core_log()");
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void __core_log_file(LogLevel level, const char *file, const char *fmt, ...)
{
    switch (level) {
    case CORE_TRACE:
        fprintf(stderr, "[TRACE]:%s: ", file);
        break;
    case CORE_DEBUG:
        fprintf(stderr, "[DEBUG]:%s: ", file);
        break;
    case CORE_INFO:
        fprintf(stderr, "[INFO]:%s: ", file);
        break;
    case CORE_WARNING:
        fprintf(stderr, "[WARNING]:%s: ", file);
        break;
    case CORE_ERROR:
        fprintf(stderr, "[ERROR]:%s: ", file);
        break;
    default:
        CORE_UNREACHABLE("core_log()");
    }

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

//  ----------------------------------- //
//              flags-impl              //
//  ----------------------------------- //
const char *const *flag_str(const char *lo, char sh, const char *const *def) {
    (void)lo;
    (void)sh;
    (void)def;
    return NULL;
}

//  ----------------------------------- //
//             context-impl             //
//  ----------------------------------- //
thread_local Context core_context = {0};

AString tmp_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    AString self = astring_vformat(&core_context.temp_arena, fmt, args);
    va_end(args);
    return self;
}

StringView tmp_copy(StringView self) {
    char *new = ringbuffer_alloc(&core_context.ring_buffer, (self.len + 1) * sizeof(*self.data));
    memcpy(new, self.data, self.len + 1);
    return string_view_new(new, self.len);
}

StringView tmp_copy_str(String *self) {
    size_t len = string_len(self);
    char *new = ringbuffer_alloc(&core_context.ring_buffer, (len + 1) * sizeof(char));
    memcpy(new, string_cstr(self), len + 1);
    return string_view_new(new, len);
}

#endif //CORE_IMPLEMENTATION
#ifdef __cplusplus
}
#endif
#endif //_CORE_H_

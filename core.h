#ifndef _CORE_H_
#define _CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

//#define CORE_MEM_DEBUG

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

//float types
typedef double              f64;
typedef float               f32;
//signed types
typedef int64_t             i64;
typedef int32_t             i32;
typedef int16_t             i16;
typedef int8_t              i8;

//unsigned types
typedef uint64_t            u64;
typedef uint32_t            u32;
typedef uint16_t            u16;
typedef uint8_t             u8;
typedef uintptr_t           ptr_t;

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
#if __STDC_VERSION__ >= 202000
#define core_defer_var(var, begin, end, body) \
    { auto var = begin; for(size_t CORE_MACRO_VAR(i) = (0); !CORE_MACRO_VAR(i); (CORE_MACRO_VAR(i)++), end) body }
#else
#define core_defer_var(type, var, begin, end, body) \
    { type var; for(size_t CORE_MACRO_VAR(i) = (var = begin, 0); !CORE_MACRO_VAR(i); (CORE_MACRO_VAR(i)++), end) body }
#endif
#define SHORT_STRING_LENGTH 24
#define ARENA_DEFAULT_ALLOC_SIZE 1024 * 4
#ifndef RINGBUFFER_SIZE
#define RINGBUFFER_SIZE 1024 * 4
#endif

#if defined(__GNUC__) || defined(__clang__)
//stolen from //   TODO: implement NOB_PRINTF_FORMAT for MSVC
//   https://gcc.gnu.org/onlinedocs/gcc-4.7.2/gcc/Function-Attributes.html
#    ifdef __MINGW_PRINTF_FORMAT
#        define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (__MINGW_PRINTF_FORMAT, STRING_INDEX, FIRST_TO_CHECK)))
#    else
#        define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK) __attribute__ ((format (printf, STRING_INDEX, FIRST_TO_CHECK)))
#    endif // __MINGW_PRINTF_FORMAT
#else
#    define CORE_PRINTF_FORMAT(STRING_INDEX, FIRST_TO_CHECK)
#endif

typedef wchar_t wchar;

//  ----------------------------------- //
//             forward-decls            //
//  ----------------------------------- //
//  the caller needs to garuentee that both pointers are valid and cannot go oob
bool partial_cmp_ptr(const char *self, const char *predicate, size_t len);
typedef void *(*AllocFn)(void *self, size_t size);
typedef void *(*ReallocFn)(void *self, void *mem, size_t size);
typedef void (*FreeFn)(void *self, void *_block);
void _core_noop_free(void *self, void *_block);
typedef struct Allocator{
    void *self;
    AllocFn alloc;
    ReallocFn realloc;
    FreeFn free;
}Allocator;
typedef struct OptAllocArg { Allocator allocator; } OptAllocArg;
extern Allocator default_allocator;

void *allocate_in_impl(void *item, size_t item_size, OptAllocArg arg);
#define allocate_in(item, ...) memcpy(\
    allocator_alloc(\
        (OptAllocArg){__VA_ARGS__}.allocator.alloc ?\
        &(OptAllocArg){__VA_ARGS__}.allocator :\
        &default_allocator, sizeof((item))\
    ),\
    &(item),\
    (sizeof((item)))\
)

//allocate_in_impl(&(item), sizeof((item)), (OptAllocArg){__VA_ARGS__})
#define to_heap(item) allocate_in((item), .allocator = default_allocator)

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
    Allocator alloc;
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

String string_new_impl(OptAllocArg arg);
#define string_new(...) string_new_impl((OptAllocArg){__VA_ARGS__})
String string_new_size_impl(size_t size, OptAllocArg arg);
#define string_new_size(size, ...) string_new_size_impl((size), (OptAllocArg){__VA_ARGS__})
String string_from_impl(const char *ptr, OptAllocArg arg);
#define string_from(ptr, ...) string_from_impl((ptr), (OptAllocArg){__VA_ARGS__})
String string_from_parts_impl(const char *ptr, size_t len, size_t cap, OptAllocArg arg);
#define string_from_parts(ptr, len, cap, ...) string_from_parts_impl((ptr), (len), (cap), (OptAllocArg){__VA_ARGS__})
String string_format(const char *format, ...);
String string_format_opt(OptAllocArg arg, const char *format, ...);
String string_vformat(const char *fmt, va_list args);
String string_vformat_opt(OptAllocArg arg, const char *fmt, va_list args);
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

String string_copy_impl(String const *self, OptAllocArg arg);
#define string_copy(self, ...) string_copy_impl((self), (OptAllocArg){__VA_ARGS__})

StringView string_into_view(String const *self);
String string_view_into_string_impl(StringView self, OptAllocArg arg);
#define string_view_into_string(self, ...) string_view_into_string_impl((self), (OptAllocArg){__VA_ARGS__})

typedef struct StringView StringView;
typedef struct String String;

//  ----------------------------------- //
//              allocator               //
//  ----------------------------------- //
Allocator default_allocator;

#ifdef CORE_MEM_DEBUG
void *allocator_alloc_debug(Allocator *self, size_t size, size_t line, const char *file);
#define allocator_alloc(self, size) allocator_alloc_debug((self), (size), __LINE__, __FILE__)
void *allocator_realloc_debug(Allocator *self, void *mem, size_t size, size_t line, const char *file);
#define allocator_realloc(self, mem, size) allocator_realloc_debug((self), (mem), (size), __LINE__, __FILE__)
void allocator_free_debug(Allocator *self, void *_block, size_t line, const char *file);
#define allocator_free(self, _block) allocator_free_debug((self), (_block), __LINE__, __FILE__)
#else
void *allocator_alloc(Allocator *self, size_t size);
void *allocator_realloc(Allocator *self, void *mem, size_t size);
void allocator_free(Allocator *self, void *_block);
#endif

typedef struct RingBuffer {
    void *base;
    size_t size;
    size_t write_pos;
    Allocator alloc;
}RingBuffer, ScratchBuffer;

RingBuffer ringbuffer_init_impl(OptAllocArg args);
#define ringbuffer_init(...) ringbuffer_init_impl((OptAllocArg){__VA_ARGS__})
#define scratch_init ringbuffer_init
void *ringbuffer_alloc(RingBuffer *self, size_t size);
#define scratch_alloc ringbuffer_alloc

Allocator ringbuffer_allocator(RingBuffer *self);
#define scratch_allocator ringbuffer_allocator

void ringbuffer_print_stats(RingBuffer *self);

//  ----------------------------------- //
//                 file                 //
//  ----------------------------------- //
typedef u32 FileMode_;
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
    Allocator alloc;
}File, *FileHandle;

FileHandle file_open_impl(const char *path, FileMode_ mode, OptAllocArg arg);
#define file_open(path, mode, ...) file_open_impl((path), (mode), (OptAllocArg){__VA_ARGS__})
void file_close(FileHandle self);
void *file_raw(FileHandle self);
String file_read_impl(FileHandle self, OptAllocArg arg);
#define file_read(self, ...) file_read_impl((self), (OptAllocArg){__VA_ARGS__})
char *file_read_binary_impl(FileHandle self, OptAllocArg arg);
#define file_read_binary(self, ...) file_read_binary_impl((self), (OptAllocArg){__VA_ARGS__})
bool file_write_raw(FileHandle self, const char *data, size_t len);
bool file_write(FileHandle self, const StringView data);
bool file_exists(const StringView path);

String file_read_to_string_impl(const char *path, OptAllocArg arg);
#define file_read_to_string(path, ...) file_read_to_string((path), (OptAllocArg){__VA_ARGS__})
char *file_read_to_vec_impl(const char *path, OptAllocArg arg);
#define file_read_to_vec(path, ...) file_read_to_vec((path), (OptAllocArg){__VA_ARGS__})

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
    Allocator alloc;
} ArrayHeader;

void *core_vec_create_internal_impl(size_t capacity, size_t elem_size, OptAllocArg arg);
void *core_vec_maygrow_internal(void *arr, size_t elem_size);
void core_vec_destroy_internal(void *arr);
void *core_vec_create_empty_internal(OptAllocArg arg);
void *core_vec_copy(void *arr, size_t elem_size, OptAllocArg arg);
void *core_vec_create_from_parts_internal(void *ptr, size_t size, size_t elem_size, OptAllocArg arg);
typedef bool (*EqCallback)(void *lhs, void *rhs);
#define FIND_NO_ELEM (-1)
size_t core_vec_find(void *vec, size_t elem_size, void *pred, size_t pred_size, EqCallback callback);
bool core_vec_remove(void *vec, size_t elem_size, size_t index);

#define vec_header(v) ((struct ArrayHeader *)(v) - 1)
#define vec_len(v) (vec_header((v))->len)
#define vec_cap(v) (vec_header((v))->cap)

#define Vec(type) type *
#define vec_new(...) core_vec_create_empty_internal((OptAllocArg){__VA_ARGS__})
#define vec_new_with(items, ...) core_vec_create_from_parts_internal(items, CORE_ARRLEN(items), sizeof(items[0]), (OptAllocArg){__VA_ARGS__})
#define vec_clear(v) (vec_header((v))->len = 0)
#define vec_with_size(ty, size, ...) core_vec_create_internal((size), sizeof(ty), (OptAllocArg){__VA_ARGS__})
#define vec_from_parts(ty, ptr, size, ...) core_vec_create_from_parts_internal((ptr), (size), sizeof(ty), (OptAllocArg){__VA_ARGS__})
#define vec_destroy(arr) (core_vec_destroy_internal(vec_header(arr)), arr = NULL)
#define vec_push(arr, value) ((arr) = core_vec_maygrow_internal((arr), sizeof(*(arr))), (arr)[vec_header((arr))->len++] = (value))
#define vec_pop(arr) (vec_header((arr))->len--, (arr)[vec_len((arr))])
#define vec_put(arr, index, value) do{\
    if(!arr) arr = core_vec_maygrow_internal((arr), (size_t)sizeof(*(arr)));\
    if((index) >= vec_len((arr))) {\
        vec_len(arr) = index + 1;\
    }\
    (arr) = core_vec_maygrow_internal((arr), (size_t)sizeof(*(arr)));\
    (arr)[(index)] = (value);\
}while(0)
#define vec_remove(vec, idx) core_vec_remove((vec), sizeof(*(vec)), (idx))

#define vec_move(arr) (arr)
#define vec_copy(arr, ...) core_vec_copy((arr), sizeof((*arr)), (OptAllocArg){__VA_ARGS__})
#define vec_at(arr, index) (CORE_ASSERT(index < vec_len(arr)), (arr)[(index)])
#define vec_iter(arr, iter) for(size_t (iter) = 0; (iter) < vec_len((arr)); (iter)++)
#define vec_foreach(arr, item) for(__typeof__(*(arr)) *item = (arr); item != (arr) + vec_len((arr)); item++)
#define vec_find(arr, pred, callback) core_vec_find((arr), sizeof(*(arr)), &(pred), sizeof((pred)), (callback))

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
#define slice_to_vec(slice, ...) core_vec_create_from_parts_internal((slice).data, (slice).len, sizeof(*(slice).data), (OptAllocArg){__VA_ARGS__})

//  ----------------------------------- //
//                arena                 //
//  ----------------------------------- //
typedef struct Arena Arena;

struct Arena {
    Vec(char) buffer;
    char *current_alloc;
    Arena *next;
};

Arena arena_new(size_t size);
void arena_dealloc(Arena *arena);

void *arena_alloc(Arena *alloc, size_t size);
void *arena_realloc(Arena *alloc, void *src, size_t size);
void arena_clear(Arena *alloc);

Allocator arena_allocator(Arena *self);

void arena_print_stats(Arena *self);

//  ----------------------------------- //
//              static-arena            //
//  ----------------------------------- //
typedef struct StaticArena {
    void *buffer;
    void *current_alloc;
    size_t size;
}StaticArena;

StaticArena static_arena_new_impl(size_t size, char buffer[size]);
#define static_arena_new(buffer) static_arena_new_impl(CORE_ARRLEN(buffer), buffer)

void *static_arena_alloc(StaticArena *alloc, size_t size);
void *static_arena_realloc(StaticArena *alloc, void *src, size_t size);
void static_arena_clear(StaticArena *alloc);

Allocator static_arena_allocator(StaticArena *self);

void static_arena_print_stats(StaticArena *self);

//  ----------------------------------- //
//                 print                //
//  ----------------------------------- //
i32 print(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
i32 println(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
i32 fprint(FileHandle stream, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);
i32 fprintln(FileHandle stream, const char *fmt, ...) CORE_PRINTF_FORMAT(2, 3);

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
    i32 argc;
    const char *const *argv;
    const char *program_name;
}FlagContext;
const char *const *flag_str(const char *lo, char sh, const char *const *def);

void flags_parse(i32 argc, const char *const *argv);
*/
//  ----------------------------------- //
//                context               //
//  ----------------------------------- //
#ifdef CORE_MEM_DEBUG
typedef struct Allocation {
    void *addr;
    size_t size;
    size_t line;
    StringView file;
    struct { StringView file; size_t line; } freed_at;
}Allocation;

typedef struct MemoryStats {
    Vec(Allocation) allocations;
}MemoryStats;
#endif

typedef struct Context {
    Arena temp_arena;
    RingBuffer ring_buffer;
    //FlagContext *flag_context;
    #ifdef CORE_MEM_DEBUG
    MemoryStats memory_stats;
    #endif
}Context;

extern thread_local Context core_context;
String tmp_printf(const char *fmt, ...) CORE_PRINTF_FORMAT(1, 2);
StringView tmp_copy(StringView self);
StringView tmp_copy_str(String *self);

typedef struct Bitmap {
    char *data;
    size_t width;
    size_t height;
    size_t stride;
    Allocator alloc;
}Bitmap;

Bitmap bitmap_new_impl(size_t width, size_t height, size_t stride, OptAllocArg arg);
#define bitmap_new(width, height, stride, ...) bitmap_new_impl((width), (height), (stride), (OptAllocArg){__VA_ARGS__})
Bitmap bitmap_from_impl(const char *data, size_t width, size_t height, size_t stride, OptAllocArg arg);
#define bitmap_from(data, width, height, stride, ...) bitmap_from_impl((data), (width), (height), (stride), (OptAllocArg){__VA_ARGS__})

const void *bitmap_at(Bitmap *self, size_t x, size_t y);
void bitmap_put(Bitmap *self, size_t x, size_t y, void *data);

#ifdef CORE_IMPLEMENTATION
#define ALLOC_ARG_OR_DEF(arg) (arg.allocator.alloc ? (CORE_ASSERT(arg.allocator.alloc&&arg.allocator.realloc&&arg.allocator.free), arg.allocator) : default_allocator)
//  ----------------------------------- //
//             string-impl              //
//  ----------------------------------- //
StringView string_view_from(const char *data) {
    return (StringView){ strlen(data), data };
}

StringView string_view_new(const char *data, size_t len) {
    return (StringView){ len, data };
}

String string_view_into_string_impl(StringView self, OptAllocArg arg) {
    return string_from_parts_impl(self.data, self.len, self.len, arg);
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
        allocator_free(&self->alloc, self->data.l.ptr);
        memmove(self->data.s.data, string_cpy_buffer, len * sizeof(char));
        self->type = STRING_SHORT;
    }else {
        memcpy(string_cpy_buffer, self->data.s.data, SHORT_STRING_LENGTH * sizeof(char));
        self->data.l.ptr = allocator_alloc(&self->alloc, (SHORT_STRING_LENGTH + 1) * sizeof(char));
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
    self->data.l.ptr = allocator_realloc(&self->alloc, self->data.l.ptr, new_size * sizeof(char));
    self->data.l.cap = new_size;
    //printf("[info]: grew string\n");
}

String string_new_impl(OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    return (String){
        .alloc = alloc,
        .type = STRING_SHORT,
        .data = {
            .s = {
                .data = {
                    [23] = SHORT_STRING_LENGTH,
                },
            },
        },
    };
}

String string_new_size_impl(size_t size, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    String self = {
        .alloc = alloc,
        .type = size + 1 > SHORT_STRING_LENGTH ? STRING_LONG : STRING_SHORT,
    };
    if(self.type == STRING_LONG) {
        self.data.l.cap = size + 1;
        self.data.l.ptr = allocator_alloc(&alloc, (size + 1) * sizeof(char));
        memset(self.data.l.ptr, 0, (size + 1) * sizeof(char));
        CORE_ASSERT(self.data.l.ptr && "error: failed to allocate `String`");
    }else {
        self.data.s.data[23] = SHORT_STRING_LENGTH;
    }
    return self;
}

String string_from_impl(const char *ptr, OptAllocArg arg) {
    CORE_ASSERT(ptr && "error: cannot pass nullptr to `string_from`");
    //FIXME: do not put the fucking nulltermination char in length ????
    size_t len = strlen(ptr);
    String self = string_new_size(len, .allocator = arg.allocator);
    if(self.type == STRING_LONG) {
        self.data.l.len = len;
        strcpy(self.data.l.ptr, ptr);
    }else {
        self.data.s.data[23] = SHORT_STRING_LENGTH - len;
        strcpy(self.data.s.data, ptr);
    }
    return self;
}

String string_from_parts_impl(const char *ptr, size_t len, size_t cap, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    if(cap > SHORT_STRING_LENGTH) {
        String str = {
            .alloc = alloc,
            .type = STRING_LONG,
            .data.l.ptr = allocator_alloc(&alloc, cap * sizeof(char)),
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

String string_format_opt(OptAllocArg arg, const char *format, ...) {
    va_list args;
    va_start(args, format);
    String self = string_vformat_opt(arg, format, args);
    va_end(args);
    return self;
}

String string_vformat(const char *fmt, va_list args) {
    return string_vformat_opt((OptAllocArg){.allocator = default_allocator}, fmt, args);
}

String string_vformat_opt(OptAllocArg arg, const char *fmt, va_list args) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    va_list args_copy;
    va_copy(args_copy, args);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    String self = string_new_size(size, .allocator = alloc);
    if(self.type == STRING_SHORT) {
        vsnprintf(self.data.s.data, size + 1, fmt, args_copy);
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
    allocator_free(&self->alloc, self->data.l.ptr);
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
        self->data.s.data[SHORT_STRING_LENGTH - self->data.s.data[23]] = c;
        self->data.s.data[(SHORT_STRING_LENGTH - (self->data.s.data[23]--)) + 1] = '\0';
        return;
    }
    if(self->data.l.len == self->data.l.cap) {
        string_grow(self);
    }
    self->data.l.ptr[self->data.l.len - 1] = c;
    self->data.l.ptr[self->data.l.len++] = '\0';
}

void string_pushf(String *self, const char *fmt, ...) {
    va_list args, args_copy;
    va_copy(args_copy, args);
    va_start(args, fmt);
    size_t size = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char *buffer = ringbuffer_alloc(&core_context.ring_buffer, size + 1);
    va_start(args_copy, fmt);
    vsnprintf(buffer, size + 1, fmt, args_copy);
    va_end(args_copy);
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

String string_copy_impl(String const *self, OptAllocArg arg) {
    return string_from_parts_impl(string_cstr(self), string_len(self), string_cap(self), arg);
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
#ifdef CORE_MEM_DEBUG
#define CORE_DEBUG_ALLOCATOR_MARKER (void*)0xFFFFFFFFFFFFFFFF
void *_std_alloc(void *self, size_t size);
void *_std_realloc(void *self, void *mem, size_t size);
void _std_free(void *self, void *_block);
Allocator std_alloc = {
    .self = NULL,
    .alloc = _std_alloc,
    .realloc = _std_realloc,
    .free = _std_free,
};

/*static bool _core_allocation_find(Allocation *elem, Allocation *pred) {
    return elem->addr == pred->addr;
}*/

void *allocator_alloc_debug(Allocator *self, size_t size, size_t line, const char *file) {
    if(self->self == CORE_DEBUG_ALLOCATOR_MARKER) {
        if(!core_context.memory_stats.allocations) {
            core_context.memory_stats.allocations = vec_new(.allocator = std_alloc);
        }
        auto alloc = self->alloc(self->self, size);
        Allocation a = {alloc, size, line, sv(file), .freed_at = {}};
        vec_push(core_context.memory_stats.allocations, a);
        return alloc;
    }
    return self->alloc(self->self, size);
}

void *allocator_realloc_debug(Allocator *self, void *mem, size_t size, size_t line, const char *file) {
    CORE_UNUSED(line);
    CORE_UNUSED(file);
    if(self->self == CORE_DEBUG_ALLOCATOR_MARKER) {
        if(!core_context.memory_stats.allocations) {
            core_context.memory_stats.allocations = vec_new(.allocator = std_alloc);
        }
        return self->realloc(self->self, mem, size);
    }
    return self->realloc(self->self, mem, size);
}

void allocator_free_debug(Allocator *self, void *_block, size_t line, const char *file) {
    CORE_UNUSED(line);
    CORE_UNUSED(file);
    if(self->self == CORE_DEBUG_ALLOCATOR_MARKER) {
        if(!core_context.memory_stats.allocations) {
            core_context.memory_stats.allocations = vec_new(.allocator = std_alloc);
        }
    }
    self->free(self->self, _block);
}
#else
void *allocator_alloc(Allocator *self, size_t size) {
    return self->alloc(self->self, size);
}
void *allocator_realloc(Allocator *self, void *mem, size_t size) {
    return self->realloc(self->self, mem, size);
}

void allocator_free(Allocator *self, void *_block) {
    self->free(self->self, _block);
}
#endif

void *_std_alloc(void *self, size_t size) {
    CORE_UNUSED(self);
    return malloc(size);
}

void *_std_realloc(void *self, void *mem, size_t size) {
    CORE_UNUSED(self);
    return realloc(mem, size);
}

void _std_free(void *self, void *_block) {
    CORE_UNUSED(self);
    free(_block);
}

void *_core_noop_alloc(void *self, size_t size) {
    CORE_UNUSED(self);
    CORE_UNUSED(size);
    return NULL;
}

void *_core_noop_realloc(void *self, void *mem, size_t size) {
    CORE_UNUSED(self);
    CORE_UNUSED(mem);
    CORE_UNUSED(size);
    return NULL;
}

void _core_noop_free(void *self, void *_block) {
    CORE_UNUSED(self);
    CORE_UNUSED(_block);
}

Allocator default_allocator = {
#ifdef CORE_MEM_DEBUG
    .self = CORE_DEBUG_ALLOCATOR_MARKER,
#endif
    .alloc = _std_alloc,
    .realloc = _std_realloc,
    .free = _std_free,
};

/*void *allocate_in_impl(void *item, size_t item_size, OptAllocArg arg) {
    return ;
}*/

RingBuffer ringbuffer_init_impl(OptAllocArg args) {
    Allocator alloc = ALLOC_ARG_OR_DEF(args);
    return (RingBuffer) {
        .base = allocator_alloc(&alloc, RINGBUFFER_SIZE),
        .size = RINGBUFFER_SIZE,
        .write_pos = 0,
        .alloc = alloc,
    };
}

void *ringbuffer_alloc(RingBuffer *self, size_t size) {
    if(!self || (self && self->base == NULL)) {
        *self = ringbuffer_init(.allocator = self->alloc);
    }

    if(size > self->size) {
        log(CORE_ERROR, "size = %zu, self->size = %zu", size, self->size);
        CORE_ASSERT(false && "ringbuffer tried to allocate more then self->size");
        return NULL;
    }

    if((char*)self->base + self->write_pos + size > (char*)self->base + self->size) {
        log(CORE_DEBUG, "reset ringbuffer");
        self->write_pos = 0;
    }

    void *alloc = (char*)self->base + self->write_pos;
    self->write_pos += size;
    return alloc;
}

static void *_ringbuffer_realloc(RingBuffer *self, void *mem, size_t size) {
    CORE_UNUSED(mem);
    return ringbuffer_alloc(self, size);
}

Allocator ringbuffer_allocator(RingBuffer *self) {
    return (Allocator) {
        .self = self,
        .alloc = (AllocFn)ringbuffer_alloc,
        .realloc = (ReallocFn)_ringbuffer_realloc,
        .free = _core_noop_free,
    };
}

void ringbuffer_print_stats(RingBuffer *self) {
    println("Ringbuffer { base: %p, size: %zu, write_pos: %zu }", self->base, self->size, self->write_pos);
}

//  ----------------------------------- //
//               file-impl              //
//  ----------------------------------- //
static String mode_to_string(FileMode_ mode) {
    String buffer = string_new_size(20, .allocator = scratch_allocator(&core_context.ring_buffer));
    switch (mode) {
        case FILE_READ: {
            string_pushf(&buffer, "%s", "r");
        } break;
        case FILE_APPEND: {
            string_pushf(&buffer, "%s", "a");
        } break;
        case FILE_WRITE: {
            string_pushf(&buffer, "%s", "w");
        } break;
        case FILE_PLUS: {
            string_pushf(&buffer, "%s", "+");
        } break;
        case FILE_BIN: {
            string_pushf(&buffer, "%s", "b");
        } break;
    }
    return buffer;
}

FileHandle file_open_impl(const char *path, FileMode_ mode, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    String mode_str = mode_to_string(mode);
    const char *mode_cstr = string_cstr(&mode_str);
    CORE_ASSERT(mode_cstr && "invalid file mode");
    FileHandle self = allocator_alloc(&alloc, sizeof(File));
    if(!self) return NULL;
    *self = (File){
        .path = string_from(path, .allocator = alloc),
        .mode = mode,
        .fd = fopen(path, mode_cstr),
        .alloc = alloc,
    };
    if(!self->fd) {
        return NULL;
    }
    return self;
}

void file_close(FileHandle self) {
    fclose(self->fd);
    allocator_free(&self->alloc, self);
    self = NULL;
}

void *file_raw(FileHandle self) {
    return self->fd;
}

String file_read_impl(FileHandle self, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    fseek(self->fd, 0, SEEK_END);
    size_t size = ftell(self->fd);
    rewind(self->fd);
    char *content = allocator_alloc(&alloc, (size + 1) * sizeof(char));
    if(!content)
        return string_new(.allocator = alloc);
    fread(content, sizeof(char), size, self->fd);
    content[size] = '\0';
    String str = string_from_parts(content, size, size + 1, .allocator = alloc);
    allocator_free(&alloc, content);
    return str;
}

Vec(char) file_read_binary_impl(FileHandle self, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    fseek(self->fd, 0, SEEK_END);
    size_t size = ftell(self->fd);
    rewind(self->fd);
    char *content = allocator_alloc(&alloc, (size + 1) * sizeof(char));
    if(!content)
        return vec_new(.allocator = alloc);
    fread(content, sizeof(char), size, self->fd);
    Vec(char) vec = vec_from_parts(char, content, size, .allocator = alloc);
    allocator_free(&alloc, content);
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


String file_read_to_string_impl(const char *path, OptAllocArg arg) {
    FileHandle file = file_open(path, FILE_READ, .allocator = arg.allocator);
#ifndef CORE_DEBUG
    if(!file) {
        fprintf(stderr, "[INFO]: failed to open file `%s`", path);
    }
#endif
    String content = file_read(file, .allocator = arg.allocator);
    file_close(file);
    return content;
}

Vec(char) file_read_to_vec_impl(const char *path, OptAllocArg arg) {
    FileHandle file = file_open(path, FILE_READ | FILE_BIN, .allocator = arg.allocator);
#ifndef CORE_DEBUG
    if(!file) {
        fprintf(stderr, "[INFO]: failed to open file `%s`", path);
    }
#endif
    Vec(char) content = file_read_binary(file, .allocator = arg.allocator);
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
        .alloc = default_allocator
    };
    err = (File) {
        .fd = stderr,
        .mode = FILE_WRITE,
        .path = string_from("STDERR"),
        .alloc = default_allocator
    };
    in = (File) {
        .fd = stdin,
        .mode = FILE_READ,
        .path = string_from("STDIN"),
        .alloc = default_allocator
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
void *core_vec_create_internal(size_t capacity, size_t elem_size, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    ArrayHeader *arr = allocator_alloc(&alloc, (capacity * elem_size) + sizeof(ArrayHeader));
    memset(arr, 0, (capacity * elem_size) + sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = capacity;
    arr[0].alloc = alloc;
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
        ArrayHeader *tmp = allocator_alloc(&vec_header(arr)->alloc, vec_cap(arr) * elem_size + sizeof(ArrayHeader));
        tmp++;
        vec_len(tmp) = vec_len(arr);
        vec_cap(tmp) = vec_cap(arr);
        vec_header(tmp)->alloc = vec_header(arr)->alloc;
        memcpy(tmp, arr, vec_len(arr) * elem_size);
        allocator_free(&vec_header(arr)->alloc, vec_header(arr));
        return tmp;
    }
    return arr;
}

void core_vec_destroy_internal(void *arr) {
    ArrayHeader *header = arr;
    allocator_free(&header->alloc, header);
}

void *core_vec_create_empty_internal(OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    ArrayHeader *arr = allocator_alloc(&alloc, sizeof(ArrayHeader));
    memset(arr, 0, sizeof(ArrayHeader));
    arr[0].len = 0;
    arr[0].cap = 0;
    arr[0].alloc = alloc;
    arr++;
    return arr;
}

void *core_vec_copy(void *arr, size_t elem_size, OptAllocArg arg) {
    ArrayHeader *other = arr;
    size_t other_cap = other[-1].cap;
    ArrayHeader *new = core_vec_create_internal(other_cap, elem_size, arg);
    new[-1].len = other[-1].len;
    memcpy((void*)new, arr, new[-1].len * elem_size);
    return new;
}

void *core_vec_create_from_parts_internal(void *ptr, size_t size, size_t elem_size, OptAllocArg arg) {
    void *vec = core_vec_create_internal(size, elem_size, arg);
    vec_len(vec) = size;
    memcpy(vec, ptr, size * elem_size);
    return vec;
}

size_t core_vec_find(void *vec, size_t elem_size, void *pred, size_t pred_size, EqCallback callback) {
    if(elem_size != pred_size) {
        return FIND_NO_ELEM;
    }
    vec_iter(vec, i) {
        if(callback((char*)vec + (i * elem_size), pred)) {
            return i;
        }
    }
    return FIND_NO_ELEM;
}

bool core_vec_remove(void *vec, size_t elem_size, size_t index) {
    if(vec_len(vec) < index) {
        return false;
    }
    for(size_t i = index; i < vec_len(vec); i++) {
        memcpy((char*)vec + i * elem_size, ((char*)vec + (i + 1) * elem_size), elem_size);
    }
    vec_len(vec)--;
    return true;
}

void vec_dump(void *vec) {
    println("Vec { data: [..], len: %zu, cap: %zu }", vec_len(vec), vec_cap(vec));
}

//  ----------------------------------- //
//             arena-impl               //
//  ----------------------------------- //
Arena arena_new(size_t size) {
    if(size == 0) {
        size = ARENA_DEFAULT_ALLOC_SIZE;
    }
    Vec(char) buffer = vec_with_size(char, size);
    Arena self = {
        .buffer = buffer,
        .current_alloc = buffer,
        .next = NULL,
    };
    return self;
}

void arena_dealloc(Arena *self) {
    if(self->next) {
        arena_dealloc(self->next);
    }
    vec_destroy(self->buffer);
}

static void arena_clear_int(Arena *self) {
    self->current_alloc = self->buffer;
    if(self->next)
        arena_clear_int(self->next);
}

static void* arena_alloc_internal(Arena *self, size_t size) {
    if(self->buffer == NULL || self->current_alloc == NULL) {
        *self = arena_new(0);
    }

    if(self->next) {
        return arena_alloc_internal(self->next, size);
    }
    if(self->current_alloc + size > self->buffer + vec_cap(self->buffer)) {
        self->next = allocator_alloc(&default_allocator, sizeof(Arena));
        *self->next = arena_new(ARENA_DEFAULT_ALLOC_SIZE);
        return arena_alloc_internal(self->next, size);
    }
    void *alloc = self->current_alloc;
    self->current_alloc += size;
    return alloc;
}

void *arena_alloc(Arena *alloc, size_t size) {
    return arena_alloc_internal(alloc, size);
}

void *arena_realloc(Arena *alloc, void *src, size_t new_size) {
    (void)src;
    void *tmp = arena_alloc(alloc, new_size);
    return tmp;
}

void arena_clear(Arena *alloc) {
    arena_clear_int(alloc);
}

Allocator arena_allocator(Arena *self) {
    return (Allocator) {
        .self = self,
        .alloc = (AllocFn)arena_alloc,
        .realloc = (ReallocFn)arena_realloc,
        .free = _core_noop_free,
    };
}

void arena_print_stats(Arena *self) {
    println("Arena { buffer: %p, curent: %p, size: 0x%zx, next: %p }",
        self->buffer,
        self->current_alloc,
        (ptr_t)self->current_alloc - (ptr_t)self->buffer,
        (void*)self->next
    );
}

//  ----------------------------------- //
//            static-arena-impl         //
//  ----------------------------------- //
StaticArena static_arena_new_impl(size_t size, char buffer[size]) {
    return (StaticArena) {
        .buffer = buffer,
        .current_alloc = buffer,
        .size = size,
    };
}

void *static_arena_alloc(StaticArena *self, size_t size) {
    if(self->buffer == NULL || self->current_alloc == NULL) {
        CORE_ASSERT(self->buffer == NULL || self->current_alloc == NULL && "static_arena_alloc() cannot alloc in ");
        return NULL;
    }

    void *alloc = self->current_alloc;
    self->current_alloc = (char*)self->current_alloc + size;
    return alloc;
}

void *static_arena_realloc(StaticArena *self, void *src, size_t size) {
    CORE_UNUSED(src);
    //grow allocation, if its the last one done
    return static_arena_alloc(self, size);
}

void static_arena_clear(StaticArena *self) {
    self->current_alloc = self->buffer;
}

Allocator static_arena_allocator(StaticArena *self) {
    return (Allocator) {
        .self = self,
        .alloc = (AllocFn)static_arena_alloc,
        .realloc = (ReallocFn)static_arena_realloc,
        .free = _core_noop_free,
    };
}

void static_arena_print_stats(StaticArena *self) {
    println("StaticArena { buffer: %p, curent: %p, size: 0x%zx, buf-size: %zu }",
        self->buffer,
        self->current_alloc,
        (ptr_t)self->current_alloc - (ptr_t)self->buffer,
        self->size
    );
}

//  ----------------------------------- //
//               print-impl             //
//  ----------------------------------- //
i32 print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    i32 ret = vfprintf(stdout, fmt, args);
    va_end(args);
#ifdef CORE_FLUSH_IO
    fflush(stdout);
#endif
    return ret;
}

i32 println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    i32 ret = vfprintf(stdout, fmt, args);
    va_end(args);
    printf("\n");
#ifdef CORE_FLUSH_IO
    fflush(stdout);
#endif
    return ret;
}

i32 fprint(FileHandle stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    i32 ret = vfprintf(file_raw(stream), fmt, args);
    va_end(args);
    return ret;
}

i32 fprintln(FileHandle stream, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    i32 ret = vfprintf(file_raw(stream), fmt, args);
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
#ifdef CORE_FLUSH_IO
    fflush(stdout);
#endif
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
#ifdef CORE_FLUSH_IO
    fflush(stdout);
#endif
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

String tmp_printf(const char *fmt, ...) {
    CORE_UNUSED(fmt);
    va_list args;
    va_start(args, fmt);
    String self = string_vformat_opt((OptAllocArg){.allocator = scratch_allocator(&core_context.ring_buffer) }, fmt, args);
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

//  ----------------------------------- //
//              bitmap-impl             //
//  ----------------------------------- //
Bitmap bitmap_new_impl(size_t width, size_t height, size_t stride, OptAllocArg arg) {
    Allocator alloc = ALLOC_ARG_OR_DEF(arg);
    return (Bitmap){
        .data = allocator_alloc(&alloc, width * height * stride),
        .width = width,
        .height = height,
        .stride = stride,
        .alloc = alloc,
    };
}

Bitmap bitmap_from_impl(const char *data, size_t width, size_t height, size_t stride, OptAllocArg arg) {
    return (Bitmap){
        .data = (char *)data,
        .width = width,
        .height = height,
        .stride = stride,
        .alloc = ALLOC_ARG_OR_DEF(arg),
    };
}

/*const void *bitmap_at(Bitmap *self, size_t x, size_t y) {
    return &self->data[(y * self->stride) * self->height + (x * self->stride)];
}

void bitmap_put(Bitmap *self, size_t x, size_t y, void *data) {
    for(size_t i = 0; i < self->stride; i++) {
        self->data[(x * i) * self->width + (y * i)] = ((char*)data)[i];
    }
}*/

#endif //CORE_IMPLEMENTATION
#ifdef __cplusplus
}
#endif
#endif //_CORE_H_

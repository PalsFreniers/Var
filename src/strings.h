#ifndef STRINGS_H
#define STRINGS_H

#include <stdbool.h>
#include "types.h"
#include "array.h"

#define DEFAULT_STRING_CAPACITY 1024
#define String_zero() (struct String) {0}
#define String_len(s) (s).len
#define String_lenP(s) (s)->len
#define STRING_PRINTF_FORMAT "%.*s"
#define STRING_PRINTF_ARGUMENTS(s) (int)(s)->len, (s)->ptr

typedef enum StringError {
        // strings
        STRING_SUB_STRING_TOO_LONG,
        STRING_NO_DESTROY_UNOWNED,
        STRING_FILE_NOT_FOUNDED,
        STRING_NULL_FUNCTION,
        STRING_MODIFY_UNOWED,
        STRING_MALLOC_FAILED,
        STRING_BAD_INDEX,
        STRING_EMPTY,

        // strings iterator
        STRING_ITERATOR_INDEX_OVERFLOW,
        STRING_ITERATOR_INDEX_UNDERFLOW,
        STRING_ITERATOR_NULL_STRING,
        
        // both
        STRING_SUCCESS,
} StringError;

extern enum StringError stringError;

typedef enum StringMap {
        STRING_MAP_MALLOC,
        STRING_MAP_MMAP,
} StringMap;

typedef struct String {
        str ptr;
        u64 len;
        u64 capacity;
        bool owned;
        enum StringMap mapping;
} String;

typedef void (*String_map_f)(char *);
typedef char (*String_iter_f)(char);

typedef struct StringIterator {
        struct String *ptr;
        u64 index;
} StringIterator;

// constructors
struct String String_newFromCstr(str s);
struct String String_new(str s, u64 len);
struct String String_newFromFile(str path);
struct String String_newCstrUnowned(str s);
struct String String_newUnowned(str s, u64 len);
struct String String_newWithCapacity(u64 capacity);

// copy
struct String String_copy(struct String self);
str String_toCstr(struct String self);

// destructor
void String_destruct(struct String *self);

// method
struct String String_substr(struct String self, u64 start, u64 end);
void String_insert(struct String *self, struct String s, u64 index);
struct String String_iter(struct String *self, String_iter_f func);
void String_erase(struct String *self, u64 start, u64 lenght);
void String_append(struct String *self, struct String s);
void String_map(struct String *self, String_map_f func);
struct StringIterator String_begin(struct String *self);
struct StringIterator String_end(struct String *self);
struct String *String_relocate(struct String *self);
void String_push(struct String *self, char c);

// strings iterators
bool StringIterator_equal(struct StringIterator self, struct StringIterator other);
char StringIterator_next(struct StringIterator *self);
char StringIterator_peek(struct StringIterator *self);
void StringIterator_inc(struct StringIterator *self);
void StringIterator_dec(struct StringIterator *self);

// string errors
void StringError_show();
str StringError_toCStr();

#endif // STRING_H

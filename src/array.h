#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

#include "types.h"
#include <stdbool.h>

#define DEFAULT_DYNAMIC_ARRAY_CAPACITY 1024
#define DynamicArray_zero() (struct DynamicArray) {0}
#define DynamicArray_len(s) (s).len
#define DynamicArray_lenP(s) (s)->len

typedef enum DynamicArrayError {
        // strings
        DYNAMIC_ARRAY_NULL_FUNCTION,
        DYNAMIC_ARRAY_MALLOC_FAILED,
        DYNAMIC_ARRAY_BAD_INDEX,
        DYNAMIC_ARRAY_EMPTY,

        // strings iterator
        DYNAMIC_ARRAY_ITERATOR_INDEX_OVERFLOW,
        DYNAMIC_ARRAY_ITERATOR_INDEX_UNDERFLOW,
        DYNAMIC_ARRAY_ITERATOR_NULL_DYNAMIC_ARRAY,
        
        // both
        DYNAMIC_ARRAY_SUCCESS,
} DynamicArrayError;

extern enum DynamicArrayError dynamicArrayError;

typedef struct DynamicArray {
        void **data;
        u64 len;
        u64 capacity;
} DynamicArray;

typedef void (*DynamicArray_map_f)(void *);
typedef void *(*DynamicArray_iter_f)(void *);
typedef void *(*DynamicArray_copy_f)(void *);
typedef void (*DynamicArray_delete_f)(void *);

typedef struct DynamicArrayIterator {
        struct DynamicArray *ptr;
        u64 index;
} DynamicArrayIterator;

// contructors
struct DynamicArray DynamicArray_new(u64 capacity);
struct DynamicArray DynamicArray_newFromArray(void **ptr, u64 len, DynamicArray_copy_f func);

// copy
struct DynamicArray DynamicArray_copy(struct DynamicArray self, DynamicArray_copy_f func);

// destructor
void DynamicArray_destruct(struct DynamicArray *self, DynamicArray_delete_f func);

// method
void DynamicArray_insert(struct DynamicArray *self, struct DynamicArray a, u64 index, DynamicArray_copy_f func);
void DynamicArray_erase(struct DynamicArray *self, u64 start, u64 lenght, DynamicArray_delete_f func);
void DynamicArray_append(struct DynamicArray *self, struct DynamicArray a, DynamicArray_copy_f func);
struct DynamicArray DynamicArray_iter(struct DynamicArray *self, DynamicArray_iter_f func);
void DynamicArray_map(struct DynamicArray *self, DynamicArray_map_f func);
struct DynamicArrayIterator DynamicArray_begin(struct DynamicArray *self);
struct DynamicArrayIterator DynamicArray_end(struct DynamicArray *self);

// dynamic array iterators
bool DynamicArrayIterator_equal(struct DynamicArrayIterator self, struct DynamicArrayIterator other);
void *DynamicArrayIterator_next(struct DynamicArrayIterator *self);
void *DynamicArrayIterator_peek(struct DynamicArrayIterator *self);
void DynamicArrayIterator_inc(struct DynamicArrayIterator *self);
void DynamicArrayIterator_dec(struct DynamicArrayIterator *self);

#endif // DYNAMIC_ARRAY_H

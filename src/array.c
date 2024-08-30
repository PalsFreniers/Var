#include "array.h"
#include <stdlib.h>
#include <string.h>

enum DynamicArrayError dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
void DynamicArray_grow(struct DynamicArray *self);

// contructors
struct DynamicArray DynamicArray_new(u64 capacity) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        void **ptr = malloc(capacity * sizeof(void *));
        if(ptr == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_MALLOC_FAILED;
                return (struct DynamicArray) {0};
        }
        return (struct DynamicArray){
                .data = ptr,
                .capacity = capacity,
                .len = 0,
        };
}

struct DynamicArray DynamicArray_newFromArray(void **ptr, u64 len, DynamicArray_copy_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return (struct DynamicArray) {0};
        }
        if(ptr == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return (struct DynamicArray) {0};
        }
        void **pr = malloc(len * sizeof(void *));
        if(pr == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_MALLOC_FAILED;
                return (struct DynamicArray) {0};
        }
        for(u64 i = 0; i < len; i++) pr[i] = func(ptr[i]);
        return (struct DynamicArray){
                .data = ptr,
                .capacity = len,
                .len = len,
        };
}

// copy
struct DynamicArray DynamicArray_copy(struct DynamicArray self, DynamicArray_copy_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return (struct DynamicArray) {0};
        }
        if(self.data == NULL || self.len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return (struct DynamicArray) {0};
        }
        struct DynamicArray arr = DynamicArray_newFromArray(self.data, self.len, func);
        return arr;
}

// destructor
void DynamicArray_destruct(struct DynamicArray *self, DynamicArray_delete_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return;
        }
        if(self->data == NULL) return;
        for(u64 i = 0; i < self->len; i++) func(self->data[i]);
        free(self->data);
        self->capacity = 0;
        self->len = 0;
}

// method
void DynamicArray_insert(struct DynamicArray *self, struct DynamicArray a, u64 index, DynamicArray_copy_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return;
        }
        if(self->data == NULL || self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return;
        }
        if(index >= self->len) {
                dynamicArrayError = DYNAMIC_ARRAY_BAD_INDEX;
                return;
        }
        while(self->capacity < self->len + a.len && dynamicArrayError == DYNAMIC_ARRAY_SUCCESS) DynamicArray_grow(self);
        if(dynamicArrayError != DYNAMIC_ARRAY_SUCCESS) return;
        memmove(self->data + index + a.len, self->data + index, (self->len - index) * sizeof(void *));
        for(u64 i = 0; i < a.len; i++) self->data[self->len + i] = func(a.data[i]);
}

void DynamicArray_erase(struct DynamicArray *self, u64 start, u64 lenght, DynamicArray_delete_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return;
        }
        if(self->data == NULL || self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return;
        }
        if(start >= self->len) {
                dynamicArrayError = DYNAMIC_ARRAY_BAD_INDEX;
                return;
        }
        if(start + lenght >= self->len) lenght = self->len - start;
        for(u64 i = 0; i < lenght; i++) func(self->data[start + i]);
        self->len -= lenght;
}

void DynamicArray_append(struct DynamicArray *self, struct DynamicArray a, DynamicArray_copy_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return;
        }
        if(self->data == NULL || self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return;
        }
        while(self->capacity < self->len + a.len && dynamicArrayError == DYNAMIC_ARRAY_SUCCESS) DynamicArray_grow(self);
        if(dynamicArrayError != DYNAMIC_ARRAY_SUCCESS) return;
        for(u64 i = 0; i < a.len; i++) self->data[self->len + i] = func(a.data[i]);
        self->len += a.len;
}

struct DynamicArray DynamicArray_iter(struct DynamicArray *self, DynamicArray_iter_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return (struct DynamicArray) {0};
        }
        if(self->data == NULL || self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return (struct DynamicArray) {0};
        }
        struct DynamicArray arr = DynamicArray_newFromArray(self->data, self->len, func);
        return arr;
}

void DynamicArray_map(struct DynamicArray *self, DynamicArray_map_f func) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(func == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_NULL_FUNCTION;
                return;
        }
        if(self->data == NULL || self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return;
        }
        for(u64 i = 0; i < self->len; i++) func(self->data[i]);
}

struct DynamicArrayIterator DynamicArray_begin(struct DynamicArray *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(self->len == 0) dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
        return (struct DynamicArrayIterator) {
                .ptr = self,
                .index = 0,
        };
}

struct DynamicArrayIterator DynamicArray_end(struct DynamicArray *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(self->len == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_EMPTY;
                return DynamicArray_begin(self);
        }
        return (struct DynamicArrayIterator) {
                .ptr = self,
                .index = self->len - 1,
        };
}

void DynamicArray_grow(struct DynamicArray *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(self->capacity == 0) self->capacity = DEFAULT_DYNAMIC_ARRAY_CAPACITY;
        else self->capacity *= 2;
        self->data = realloc(self->data, self->capacity * sizeof(void *));
        if(self->data == NULL) {
                dynamicArrayError = DYNAMIC_ARRAY_MALLOC_FAILED;
                return;
        }
}

// dynamic array iterators
bool DynamicArrayIterator_equal(struct DynamicArrayIterator self, struct DynamicArrayIterator other) {
        return self.ptr == other.ptr && self.index == other.index;
}

void *DynamicArrayIterator_next(struct DynamicArrayIterator *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(!self->ptr) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_NULL_DYNAMIC_ARRAY;
                return 0;
        }
        DynamicArrayIterator_inc(self);
        return dynamicArrayError == DYNAMIC_ARRAY_SUCCESS ? self->ptr->data[self->index] : 0;
}

void *DynamicArrayIterator_peek(struct DynamicArrayIterator *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(!self->ptr) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_NULL_DYNAMIC_ARRAY;
                return 0;
        }
        return self->ptr->data[self->index];
}

void DynamicArrayIterator_inc(struct DynamicArrayIterator *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(!self->ptr) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_NULL_DYNAMIC_ARRAY;
                return;
        }
        if(self->index + 1 >= self->ptr->len) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_INDEX_OVERFLOW;
                return;
        }
        self->index++;
}

void DynamicArrayIterator_dec(struct DynamicArrayIterator *self) {
        dynamicArrayError = DYNAMIC_ARRAY_SUCCESS;
        if(!self->ptr) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_NULL_DYNAMIC_ARRAY;
                return;
        }
        if(self->index == 0) {
                dynamicArrayError = DYNAMIC_ARRAY_ITERATOR_INDEX_UNDERFLOW;
                return;
        }
        self->index--;
}

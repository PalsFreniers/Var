#include "strings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>

enum StringError stringError = STRING_SUCCESS;
void String_grow(struct String *self);

// constructors
struct String String_newFromCstr(str s) {
        stringError = STRING_SUCCESS;
        if(s == NULL) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        register u64 len = strlen(s);
        return (struct String) {
                .ptr = s,
                .capacity = len,
                .len = len,
                .owned = true,
                .mapping = STRING_MAP_MALLOC,
        };
}

struct String String_new(str s, u64 len) {
        stringError = STRING_SUCCESS;
        if(s == NULL) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        return (struct String) {
                .ptr = s,
                .capacity = len,
                .len = len,
                .owned = true,
                .mapping = STRING_MAP_MALLOC,
        };
}

struct String String_newCstrUnowned(str s) {
        stringError = STRING_SUCCESS;
        if(s == NULL) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        return (struct String) {
                .ptr = s,
                .capacity = 0,
                .len = strlen(s),
                .owned = false,
                .mapping = STRING_MAP_MALLOC,
        };
}

struct String String_newUnowned(str s, u64 len) {
        stringError = STRING_SUCCESS;
        if(s == NULL || len == 0) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        return (struct String) {
                .ptr = s,
                .capacity = 0,
                .len = len,
                .owned = false,
                .mapping = STRING_MAP_MALLOC,
        };
}

struct String String_newWithCapacity(u64 capacity) {
        stringError = STRING_SUCCESS;
        if(capacity == 0) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        str c = malloc(capacity * sizeof(char));
        if(!c) {
                stringError = STRING_MALLOC_FAILED;
                return (struct String) {0};
        }
        return (struct String) {
                .ptr = c,
                .capacity = capacity,
                .len = 0,
                .owned = true,
                .mapping = STRING_MAP_MALLOC,
        };
}

struct String String_newFromFile(str path) {
        int fd = open(path, O_RDONLY);
        if(fd < 0) {
                stringError = STRING_FILE_NOT_FOUNDED;
                return String_zero();
        }
        struct stat buf;
        fstat(fd, &buf);
        void *ptr = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        if(ptr == NULL) return String_zero();
        struct String ret = String_new(ptr, buf.st_size);
        ret.mapping = STRING_MAP_MMAP;
        return ret;
}

struct String String_copy(struct String self) {
        stringError = STRING_SUCCESS;
        if(self.ptr == NULL || self.len == 0) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        str c = (str)malloc(self.len * sizeof(char));
        if(!c) {
                stringError = STRING_MALLOC_FAILED;
                return (struct String) {0};
        }
        strncpy(c, self.ptr, self.len);
        return (struct String) {
                .ptr = c,
                .capacity = self.len,
                .len = self.len,
                .owned = true,
                .mapping = STRING_MAP_MALLOC,
        };
}

str String_toCstr(struct String self) {
        stringError = STRING_SUCCESS;
        if(!self.ptr || self.len == 0) {
                stringError = STRING_EMPTY;
                return NULL;
        }
        struct String ret = String_newWithCapacity(self.len + 1);
        if(stringError != STRING_SUCCESS) return NULL;
        memcpy(ret.ptr, self.ptr, self.len);
        ret.ptr[self.len] = 0;
        return ret.ptr;
}

// destructor
void String_destruct(struct String *self) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_NO_DESTROY_UNOWNED;
                return;
        }
        if(self->mapping == STRING_MAP_MALLOC) free(self->ptr);
        else if(self->mapping == STRING_MAP_MMAP) munmap(self->ptr, self->len);
        self->owned = false;
        self->len = 0;
        self->capacity = 0;
}

// methods
struct String String_substr(struct String self, u64 start, u64 lenght) {
        stringError = STRING_SUCCESS;
        if(self.len == 0) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        if(start >= self.len) {
                stringError = STRING_BAD_INDEX;
                return (struct String) {0};
        }
        if(start + lenght > self.len) {
                stringError = STRING_SUB_STRING_TOO_LONG;
                return (struct String) {0};
        }
        return String_newUnowned(self.ptr + start, lenght);
}

void String_insert(struct String *self, struct String s, u64 index) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        if(!self->ptr || self->len == 0) {
                stringError = STRING_EMPTY;
                return;
        }
        if(index >= self->len) {
                stringError = STRING_BAD_INDEX;
                return;
        }
        while(self->capacity < self->len + s.len && stringError == STRING_SUCCESS) String_grow(self);
        if(stringError != STRING_SUCCESS) return;
        memmove(self->ptr + index + s.len, self->ptr + index, (self->len - index) * sizeof(char));
        strncpy(self->ptr + index, s.ptr, s.len);
        self->len += s.len;
}

struct String String_iter(struct String *self, String_iter_f func) {
        stringError = STRING_SUCCESS;
        if(func == NULL) {
                stringError = STRING_NULL_FUNCTION;
                return (struct String) {0};
        }
        if(self->ptr == NULL || self->len == 0) {
                stringError = STRING_EMPTY;
                return (struct String) {0};
        }
        struct String s = String_newWithCapacity(self->len + 1);
        if(stringError != STRING_SUCCESS) return (struct String) {0};
        for(u64 i = 0; i < self->len; i++) s.ptr[i] = func(self->ptr[i]);
        s.len = self->len;
        return s;
}

void String_erase(struct String *self, u64 start, u64 lenght) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        if(!self->ptr || self->len == 0) {
                stringError = STRING_EMPTY;
                return;
        }
        if(start >= self->len) {
                stringError = STRING_BAD_INDEX;
                return;
        }
        if(start + lenght > self->len) lenght = self->len - start;
        strncpy(self->ptr + start, self->ptr + start + lenght, self->len - lenght);
        self->len -= lenght;
}

void String_append(struct String *self, struct String s) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        while(self->capacity < self->len + s.len && stringError == STRING_SUCCESS) String_grow(self);
        if(stringError != STRING_SUCCESS) return;
        strncpy(self->ptr + self->len, s.ptr, s.len);
        self->len += s.len;
}

void String_map(struct String *self, String_map_f func) {
        stringError = STRING_SUCCESS;
        if(func == NULL) {
                stringError = STRING_NULL_FUNCTION;
                return;
        }
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        if(self->ptr == NULL || self->len == 0) {
                stringError = STRING_EMPTY;
                return;
        }
        for(u64 i = 0; i < self->len; i++) func(self->ptr + i);
}

struct StringIterator String_begin(struct String *self) {
        stringError = STRING_SUCCESS;
        if(self->len == 0) stringError = STRING_EMPTY;
        return (struct StringIterator) {
                .ptr = self,
                .index = 0,
        };
}

struct StringIterator String_end(struct String *self) {
        stringError = STRING_SUCCESS;
        if(self->len == 0) {
                stringError = STRING_EMPTY;
                return String_begin(self);
        }
        return (struct StringIterator) {
                .ptr = self,
                .index = self->len - 1,
        };
}

void String_push(struct String *self, char c) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        if(self->len == self->capacity) String_grow(self);
        if(stringError != STRING_SUCCESS) return;
        self->ptr[self->len++] = c;
}

void String_grow(struct String *self) {
        stringError = STRING_SUCCESS;
        if(!self->owned) {
                stringError = STRING_MODIFY_UNOWED;
                return;
        }
        if(self->capacity == 0) self->capacity = DEFAULT_STRING_CAPACITY;
        else self->capacity *= 2;
        self->ptr = realloc(self->ptr, self->capacity * sizeof(char));
        if(self->ptr == NULL) {
                stringError = STRING_MALLOC_FAILED;
                return;
        }
}

// strings iterators
bool StringIterator_equal(struct StringIterator self, struct StringIterator other) {
        return self.ptr == other.ptr && self.index == other.index;
}

char StringIterator_next(struct StringIterator *self) {
        stringError = STRING_SUCCESS;
        if(!self->ptr) {
                stringError = STRING_ITERATOR_NULL_STRING;
                return 0;
        }
        StringIterator_inc(self);
        return stringError == STRING_SUCCESS ? self->ptr->ptr[self->index] : 0;
}

char StringIterator_peek(struct StringIterator *self) {
        stringError = STRING_SUCCESS;
        if(!self->ptr) {
                stringError = STRING_ITERATOR_NULL_STRING;
                return 0;
        }
        return self->ptr->ptr[self->index];
}

void StringIterator_inc(struct StringIterator *self) {
        stringError = STRING_SUCCESS;
        if(!self->ptr) {
                stringError = STRING_ITERATOR_NULL_STRING;
                return;
        }
        if(self->index + 1 >= self->ptr->len) {
                stringError = STRING_ITERATOR_INDEX_OVERFLOW;
                return;
        }
        self->index++;
}

void StringIterator_dec(struct StringIterator *self) {
        stringError = STRING_SUCCESS;
        if(!self->ptr) {
                stringError = STRING_ITERATOR_NULL_STRING;
                return;
        }
        if(self->index == 0) {
                stringError = STRING_ITERATOR_INDEX_UNDERFLOW;
                return;
        }
        self->index--;
}

str StringError_toCStr() {
        switch(stringError) {
                case STRING_SUB_STRING_TOO_LONG: return "Sub-String Too Long";
                case STRING_NO_DESTROY_UNOWNED: return "Unable to destroy unowned String";
                case STRING_FILE_NOT_FOUNDED: return "File not founded";
                case STRING_NULL_FUNCTION: return "Passed function pointer is null";
                case STRING_MODIFY_UNOWED: return "Unable to modify an unowed String";
                case STRING_MALLOC_FAILED: return "Malloc failure";
                case STRING_BAD_INDEX: return "Index out of bounds";
                case STRING_EMPTY: return "Empty String";

                case STRING_ITERATOR_INDEX_OVERFLOW: return "String Iterator overflowed";
                case STRING_ITERATOR_INDEX_UNDERFLOW: return "String Iterator underflowed";
                case STRING_ITERATOR_NULL_STRING: return "String Iterator does not point to any String";

                case STRING_SUCCESS: return "Success";
                default: return "Unknown error";
        }
}

void StringError_show() {
        if(stringError == STRING_SUCCESS) return;
        fprintf(stderr, "[ERROR] => %s\n", StringError_toCStr());
}

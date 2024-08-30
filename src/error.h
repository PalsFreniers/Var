#ifndef ERROR_H
#define ERROR_H

typedef void (*Panic_out_f)(void *);

void panic(Panic_out_f func, void *data);

#endif // ERROR_H

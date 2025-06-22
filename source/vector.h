#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stddef.h>

typedef struct vector
{
    char *data;
    size_t size;
    size_t capacity;
} vector;

int vector_init(vector *vector, size_t size);
void vector_term(vector *vector);

void vector_push(vector *vector, const char *data, size_t size);

void vector_clear(vector *vector);

#endif /* _VECTOR_H_ */

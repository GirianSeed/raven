#ifndef _VECTOR_H_
#define _VECTOR_H_

#include <stddef.h>

typedef struct vector
{
    char *data;
    size_t size;
    size_t capacity;
} vector;

int vector_init(vector *vec, size_t size);
void vector_term(vector *vec);

void vector_push(vector *vec, const void *data, size_t size);

void vector_clear(vector *vec);

#endif /* _VECTOR_H_ */

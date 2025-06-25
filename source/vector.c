#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int vector_init(vector *vec, size_t size)
{
    vec->size = 0;
    vec->capacity = size;
    vec->data = malloc(size);

    return vec->data == NULL;
}

void vector_term(vector *vec)
{
    free(vec->data);
}

void vector_push(vector *vec, const void *data, size_t size)
{
    if ((vec->size + size) > vec->capacity)
    {
        vec->capacity = vec->size + size;
        vec->data = realloc(vec->data, vec->capacity);
    }

    memcpy(vec->data + vec->size, data, size);
    vec->size += size;
}

void vector_clear(vector *vec)
{
    vec->size = 0;
}

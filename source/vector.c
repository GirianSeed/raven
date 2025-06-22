#include "vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int vector_init(vector *vector, size_t size)
{
    vector->size = 0;
    vector->capacity = size;
    vector->data = malloc(size);

    return vector->data == NULL;
}

void vector_term(vector *vector)
{
    free(vector->data);
}

void vector_push(vector *vector, const char *data, size_t size)
{
    if ((vector->size + size) > vector->capacity)
    {
        vector->capacity = vector->size + size;
        vector->data = realloc(vector->data, vector->capacity);
    }

    memcpy(vector->data + vector->size, data, size);
    vector->size += size;
}

void vector_clear(vector *vector)
{
    vector->size = 0;
}

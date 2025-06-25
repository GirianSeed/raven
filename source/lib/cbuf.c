#include "cbuf.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int cbuf_init(cbuf *buf, size_t log2size)
{
    buf->rd = buf->wr = 0;
    buf->size = 1 << log2size;
    buf->data = malloc(buf->size);

    return buf->data == NULL;
}

void cbuf_term(cbuf *buf)
{
    free(buf->data);
}

void cbuf_read(cbuf *buf, char *data, size_t size)
{
    assert(size <= cbuf_size(buf));

    // read will wrap the buffer so split in two
    if ((buf->rd + size) > buf->size)
    {
        size_t left = buf->size - buf->rd;

        // read from our current index to the buffer end
        memcpy(data, buf->data + buf->rd, left);

        // read remaining data from the start of the buffer
        memcpy(data + left, buf->data, size - left);
    }
    else
    {
        memcpy(data, buf->data + buf->rd, size);
    }

    buf->rd = (buf->rd + size) & (buf->size - 1);
}

void cbuf_write(cbuf *buf, const char *data, size_t size)
{
    assert(size <= cbuf_avail(buf));

    // write will wrap the buffer so split in two
    if ((buf->wr + size) > buf->size)
    {
        size_t left = buf->size - buf->wr;

        // write from our current index to the buffer end
        memcpy(buf->data + buf->wr, data, left);

        // write remaining data to the start of the buffer
        memcpy(buf->data, data + left, size - left);
    }
    else
    {
        memcpy(buf->data + buf->wr, data, size);
    }

    buf->wr = (buf->wr + size) & (buf->size - 1);
}

size_t cbuf_size(cbuf *buf)
{
    // rd == wr means empty, size must be pow2
    return (buf->wr - buf->rd) & (buf->size - 1);
}

size_t cbuf_avail(cbuf *buf)
{
    // rd == wr means empty, so max is size - 1
    return buf->size - cbuf_size(buf) - 1;
}

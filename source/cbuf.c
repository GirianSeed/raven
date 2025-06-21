#include "cbuf.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

int cbuf_init(cbuf *cbuf, size_t log2size)
{
    cbuf->rd = cbuf->wr = 0;
    cbuf->size = 1 << log2size;
    cbuf->buf = malloc(cbuf->size);

    return cbuf->buf == NULL;
}

void cbuf_term(cbuf *cbuf)
{
    free(cbuf->buf);
}

void cbuf_read(cbuf *cbuf, char *data, size_t size)
{
    assert(size <= cbuf_size(cbuf));

    // read will wrap the buffer so split in two
    if ((cbuf->rd + size) > cbuf->size)
    {
        size_t left = cbuf->size - cbuf->rd;

        // read from our current index to the buffer end
        memcpy(data, cbuf->buf + cbuf->rd, left);

        // read remaining data from the start of the buffer
        memcpy(data + left, cbuf->buf, size - left);
    }
    else
    {
        memcpy(data, cbuf->buf + cbuf->rd, size);
    }

    cbuf->rd = (cbuf->rd + size) & (cbuf->size - 1);
}

void cbuf_write(cbuf *cbuf, const char *data, size_t size)
{
    assert(size <= cbuf_avail(cbuf));

    // write will wrap the buffer so split in two
    if ((cbuf->wr + size) > cbuf->size)
    {
        size_t left = cbuf->size - cbuf->wr;

        // write from our current index to the buffer end
        memcpy(cbuf->buf + cbuf->wr, data, left);

        // write remaining data to the start of the buffer
        memcpy(cbuf->buf, data + left, size - left);
    }
    else
    {
        memcpy(cbuf->buf + cbuf->wr, data, size);
    }

    cbuf->wr = (cbuf->wr + size) & (cbuf->size - 1);
}

size_t cbuf_size(cbuf *cbuf)
{
    // rd == wr means empty, size must be pow2
    return (cbuf->wr - cbuf->rd) & (cbuf->size - 1);
}

size_t cbuf_avail(cbuf *cbuf)
{
    // rd == wr means empty, so max is size - 1
    return cbuf->size - cbuf_size(cbuf) - 1;
}

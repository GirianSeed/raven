#ifndef _CBUF_H_
#define _CBUF_H_

#include <stddef.h>

typedef struct cbuf
{
    char *buf;
    size_t rd, wr;
    size_t size;
} cbuf;

int cbuf_init(cbuf *buf, size_t log2size);
void cbuf_term(cbuf *buf);

void cbuf_read(cbuf *buf, char *data, size_t size);
void cbuf_write(cbuf *buf, const char *data, size_t size);

size_t cbuf_size(cbuf *buf);
size_t cbuf_avail(cbuf *cbuf);

#endif /* _CBUF_H_ */

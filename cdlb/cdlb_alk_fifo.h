#ifndef _CDLB_ALK_FIFO_H
#define _CDLB_ALK_FIFO_H

#ifdef _cplusplus
	extern "c" {
#endif

/*
application level kfifo
form kernel/kfifo.c
*/
#if 0
typedef struct alk_fifo {
	unsigned char *buffer; /* the buffer holding the data*/
	unsigned int size; /* the size of the allocated buffer*/
	unsigned int in; /* data is added at offset (in % size)*/
	unsigned int out; /* data is extracted from off. (out % size)*/
} alk_fifo_t;
#endif

typedef struct alk_fifo {
	unsigned char *buffer;
	unsigned int size;
	unsigned int in;
	unsigned int out;
} alk_fifo_t;

static inline void alk_fifo_reset(alk_fifo_t *fifo)
{
	fifo->in = fifo->out = 0;
}

static inline unsigned int alk_fifo_len(alk_fifo_t *fifo)
{
	return fifo->in - fifo->out;
}


#ifdef _cplusplus
	}
#endif


#endif

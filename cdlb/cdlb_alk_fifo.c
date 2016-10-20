#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cdlb_alk_fifo.h"


#define min(a,b) ((a) < (b) ? (a):(b))


alk_fifo_t *alk_fifo_init(unsigned char *buffer, unsigned int size)
{
	alk_fifo_t *fifo;

	fifo = malloc(sizeof(alk_fifo_t));
	if (!fifo) {
		return NULL;
	}

	fifo->buffer = buffer;
	fifo->size = size;
	fifo->in = fifo->out = 0;

	return fifo;
}

int alk_fifo_destory(alk_fifo_t *fifo)
{
	free(fifo);
	
	return 0;
}

alk_fifo_t *alk_fifo_alloc(unsigned int size)
{
	unsigned char *buffer;
	alk_fifo_t *ret;
	/*
	* round up to the next power of 2, since our 'let the indices
	* wrap' tachnique works only in this case.
	*/

	buffer = malloc(size);
	if (!buffer) {
		return NULL;
	}
	
	ret = alk_fifo_init(buffer, size);
	if (!ret) {
		free(buffer);
	}
	
	return ret;
}

int alk_fifo_free(alk_fifo_t *fifo)
{
	free(fifo->buffer);
	alk_fifo_destory(fifo);

	return 0;
}

unsigned int alk_fifo_put(alk_fifo_t *fifo,unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = min(len, fifo->size - fifo->in + fifo->out);/*可能是缓冲区的空闲长度或者要写长度*/

	/* first put the data starting from fifo->in to buffer end*/
	l = min(len, fifo->size - (fifo->in & (fifo->size -1)));
	memcpy(fifo->buffer + (fifo->in & (fifo->size -1)), buffer, l);

	/* then put the rest (if any) at the beginning of the buffer*/
	memcpy(fifo->buffer, buffer + l, len - l);

	fifo->in += len;

	return len;
}

unsigned int alk_fifo_get(alk_fifo_t *fifo,unsigned char *buffer, unsigned int len)
{
	unsigned int l;

	len = min(len, fifo->in - fifo->out); /*可读数据*/

	/* first get the data from fifo->out until the end of the buffer*/
	l = min(len, fifo->size - (fifo->out & (fifo->size -1)));
	memcpy(buffer, fifo->buffer + (fifo->out & (fifo->size -1)), l);

	/* then get the rest (if any) from the beginning of the buffer*/
	memcpy(buffer + l, fifo->buffer, len - l);

	fifo->out += len;
	
	return len;
}



#include "ringbuf.h"
void fifo8_init(struct FIFO8 *fifo,int size, unsigned char *buf)
{
	fifo->buf=buf;
	fifo->flags=0;
	fifo->free=size;
	fifo->size=size;
	fifo->putP=0;
	fifo->getP=0;

	return;
}

int fifo8_putPut(struct FIFO8 *fifo,unsigned char data)
{
	if(fifo->putP == fifo->getP)
	{
		fifo->putP = 0;
		fifo->getP = 0;
	}
	if(fifo->putP == fifo->size)
	{
		return -1;
	}
	fifo->buf[fifo->putP] = data;
	fifo->putP++;
	return 0;
}

unsigned char fifo8_get(struct FIFO8 *fifo)
{
	int data;
	data = fifo->buf[fifo->getP];
	fifo->getP++;
	return data;
}

int fifo8_status(struct FIFO8 *fifo)
{
	return fifo->putP-fifo->getP;
}

int fifo8_free(struct FIFO8 *fifo)
{
	return fifo->free;
}


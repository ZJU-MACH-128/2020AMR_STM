#ifndef	_RINGBUF_H_
#define	RINGBUF
#define FLAGS_OVERRUN 0x0001
struct FIFO8{
         unsigned char *buf;
         int putP,getP,size,free,flags;
};
extern struct FIFO8 serialRcvBuf;
void fifo8_init(struct FIFO8 *fifo,int size, unsigned char *buf);
int fifo8_put(struct FIFO8 *fifo,unsigned char data);
unsigned char fifo8_get(struct FIFO8 *fifo);
int fifo8_status(struct FIFO8 *fifo);
int fifo8_free(struct FIFO8 *fifo);
int fifo8_putPut(struct FIFO8 *fifo,unsigned char data);
#endif

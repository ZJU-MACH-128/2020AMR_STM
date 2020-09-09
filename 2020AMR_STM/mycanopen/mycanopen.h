#ifndef	__MYCANOPEN_H__
#define	__MYCANOPEN_H__
/* The 8 bytes data of the SDO */
typedef struct 
{
	unsigned char data[8];
}BODY;

/* The SDO structure ...*/
typedef struct  
{
	unsigned char nodeId;		/*in any case, Node ID of the server (case sender or receiver).*/
	BODY body;
}struct_s_SDO;

/** The PDO structure */
typedef struct  
{
	unsigned int cobId;	  /* COB-ID */
	unsigned char           len;	  /* Number of data transmitted (in data[]) */
	unsigned char           data[8]; /* Contain the data */
}struct_s_PDO;

void canOpenSendNMT(unsigned char id,unsigned char param);
//void canOpenWritePDO(unsigned char id,int op1,int op2,unsigned int op3);
void canOpenReadSOD(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len);
void canOpenWriteSOD(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len,unsigned int param);
void canOpenReadSODAanser(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len);
unsigned char canOpenWriteSODAnser(unsigned char id,unsigned short index,unsigned char subindex,unsigned int len,unsigned int param,unsigned int *getParam);
#endif
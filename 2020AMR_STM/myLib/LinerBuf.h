#ifndef	__CANBUFFER_H
#define	__CANBUFFER_H

// typedef struct
// {
// 	unsigned char  CanId[4];
// 	unsigned char  buffer[8];
// }STRUCT_strdata;

// typedef struct
// {
// 	STRUCT_strdata *strdata;
// 	unsigned int StackOutIndex;
// 	unsigned int StackInIndex;
// 	unsigned int StackLen;
// 	unsigned char isFull;
// }STRUCT_Stack;


// typedef struct
// {
// 	unsigned char *Strdata;
// 	unsigned int DataLen;
// 	unsigned int StackOutIndex;
// 	unsigned int StackInIndex;
// 	unsigned int StackLen;
// 	unsigned char isFull;
// }STRUCT_AllocStack;

#define	STATUS_CAN_CMD_START_SEND		1
#define STATUS_CAN_CMD_START_WAIT		2
//#define STATUS_CAN_CMD_START_ERR		3
#define STATUS_CAN_CMD_START_SUCCESS		4
#define STATUS_CAN_CMD_START_NULL	0

//#define DELAY_CAN_CMD_MS			(10/5)


typedef	struct
{
	unsigned char cmd;
	unsigned char addr;
	unsigned char fl0;
	unsigned char fl1;
	unsigned char sf;
	unsigned char type;
	unsigned short offset;
	unsigned short value;
	unsigned char crc;
	unsigned char ef;
}STRUCT_Cmd_Can;

typedef	struct
{	
	unsigned int waitms;
	unsigned char status;
	unsigned char type;
	unsigned short offset;
	unsigned short p_value;
}STRUCT_Cmd_Set_Carrige;


typedef struct
{
	unsigned char *Strdata;
	unsigned int DataLen;
	unsigned int StackOutIndex;
	unsigned int StackInIndex;
	unsigned int StackLen;
	unsigned char isFull;
}STRUCT_AllocStack;

extern STRUCT_AllocStack Snd_Can_Stack;
extern STRUCT_AllocStack Rcv_SDO_Stack;
extern STRUCT_AllocStack Rcv_PDO14_Stack;
extern STRUCT_AllocStack Rcv_PDO58_Stack;

void Init_Can_Stack(void);
void Cmd_Set_Carrige_Cyc(void);
unsigned char Send_Can_Cmd_Wait(unsigned char cmd,unsigned char type, unsigned short offset,unsigned short *value);

void Init_Can_Stack(void);
void CleanStackAlloc( STRUCT_AllocStack *whichstack);
void InintStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *addr,unsigned int len,unsigned int datalen);
unsigned char NmlGetDataFromStackAlloc( STRUCT_AllocStack*	,unsigned char* );
unsigned char NmlAddDataToStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *databuf);
unsigned char IntAddDataToStackAlloc( STRUCT_AllocStack *whichstack, unsigned char *databuf);
unsigned char NmlGetDataFromStackAlloc( STRUCT_AllocStack *whichstack,unsigned char  * data);
unsigned char IntGetDataFromStackAlloc( STRUCT_AllocStack *whichstack,  unsigned char  *data);

#define	TOTAL_BUF		16
#define	BTYE_PER_BUF	16
#endif





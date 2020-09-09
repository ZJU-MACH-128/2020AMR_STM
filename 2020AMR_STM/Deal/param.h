#ifndef	__PARAM_H__
#define	__PARAM_H__
//�����ļ������ַ��
#define	PARAM_ADDR_OFFSET	(0x8020000+0x80000)
typedef struct
{
	unsigned int wheelDiameter;	//����ֱ����
	unsigned int wheelEncoder;	//����һ�ܵı�������
	unsigned short offsetRotate[4];
}Struct_SysParam;
extern Struct_SysParam SysParam;
typedef struct
{
	unsigned char head[8];
	unsigned char check[8];
	Struct_SysParam param;
}Struct_SysFile;

typedef struct
{
	double data_r_e1;
	double data_r_e2;
}Struct_RunParam;
extern Struct_RunParam RunParam;

unsigned char InitSysParam(void);
unsigned char readSysParam(void);
unsigned char writeSysParam(void);
#endif
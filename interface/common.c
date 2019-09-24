/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : common.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <malloc.h>
#include <sys/time.h>
#include <stdlib.h>
#include "uartExtern.h"
//#include "../protocal/ProtocolDef.h"
#include "../include/ConstDef.h"
#include "../include/DataTypeDef.h"
#include "../protocal/ToolExtern.h"
#include "../ThreadPool/SPThreadExtern.h"
#include "../ThreadPool/SPThreadPoolExtern.h"
#include "../TPMsgDef/TPMsgDef.h"
#include "../EnetPortPrs/Port_FunExtern.h"
//#include "../Alarm/InitAlarmExtern.h"
#include "../interface/interfaceExtern.h"
#include "../interface/stringhelper.h"
#include "../Module/Module.h"
#include "../Ethernet/EthernetExtern.h"
#include "../SGCC/store.h"
#include "../XML/xml.h"
#include "../alarm/alarmExtern.h"

#define PLCALARMPATH  "/mnt/flash/OAM/xml/info.xml"
int PlcAlarmfd;
//Config *PlcCnf=NULL;

sDeviceInfo sPlcInfoBuf[2];//PLC

UINT8 gMBQueryDeviceCountBuf=0;         /*������ѯʱ�豸�ļ���*/
UINT8 gMBRecvBuf[256];                  /*������ѯʱ�������ݻ���*/
UINT8 gYXChangeBuf[256];                /*������ѯң��ͻ���ϱ�����*/
UINT8 gYCChangeBuf[256];                /*������ѯң��ͻ���ϱ�����*/
UINT8 gGJChangeBuf[256];                /*������ѯ�澯ͻ���ϱ�����*/
UINT8 gYXChangeCount=0;                 /*������ѯң��ͻ���ϱ�����*/
UINT8 gYCChangeCount=0;                 /*������ѯң��ͻ���ϱ�����*/
UINT8 gGJChangeCount=0;                 /*������ѯ�澯ͻ���ϱ�����*/

UINT8 gLocationInfo[30];                /*��λ��Ϣ����*/

union uIEEE754 uIE;                     /*������ѯң��ֵ����תIEEE754*/

struct sSouthernAlarm *gSouthernAlarm;  //�������ݸ澯��ֵ��������Դ��Ŀ
struct sDt1000Update gDt1000Update;





UINT8 gTypePointClearFlag=0;            //��������־λ
UINT32 gPointTablePossessFlag=0;        //���ռ�ñ�־��ÿһλ����һ������ռ��
UINT8 gUploadHWDeviceFlag = 0;

extern UINT8 gDeviceIPSetFlag;
extern UINT8 gMainDeviceSN[20];
extern UINT8 gMainDeviceStatus;
extern UINT8 gMainDeviceModel[20];
extern UINT16 gRecvCount;
extern UINT16 gSendCount;
extern UINT16 gMainDeviceID;
extern UINT8 gDeviceTypeNum;
extern UINT8 gServerIP[4];
extern UINT8 gMainDeviceIP[4];
extern int g_nRemotesockfd;
extern UINT8 gDeviceIPSetTempFlag;
extern UINT8 gDeviceAlarmBuf[10];
extern struct sAlarmGroup *gAlarmHead;
extern struct sAlarmGroup *gAlarmPoint;
extern UINT8 gSocketMode;
extern UINT8 gModemConnectFlag;
extern UINT8 gModemNetStatus;
extern UINT8 gSocketHeartChannel0Count;
extern UINT8 gUpdataModeFlag;
extern UINT8 gDeviceStationBuild;
extern UINT16 aAlarmInfo[40][20];
extern UINT8 gTimeCount;
extern UINT8 gLogFileBufferFrame[SYS_FRAME_LEN];
extern UINT8 gConnectDeviceMaxNum;
extern UINT16 gDeviceAlarm;
extern UINT8 gDLinkDeviceInfoCount;
extern struct sTypeGroup *gTypeHeadBuf;

extern UINT8 gIVLicenseStatus;
extern UINT32 gIVLicenseCount;                //0:normal  FF:report all device license info
extern UINT8  gIVCRCHi;
extern UINT8  gIVCRCLo;
extern UINT8  gIVLicenseDevice;
extern sIVStruct gDeviceIVInfo[MAXDEVICE];
extern UINT8 gPlatformDisConnect;

extern UINT8 gDeviceTypeNumBuf;
extern UINT16 gDeviceTypeBuf[40];
extern UINT8 gConnectDeviceNumBuf;
extern UINT16 gYXPointNum;                        //the total number of 104 YX point
extern UINT16 gYXPointNumBuf;                     //the total number of 104 YX point
extern UINT16 gYCPointNum;                        //the total number of 104 YC point
extern UINT16 gYCPointNumBuf;                     //the total number of 104 YC point
extern UINT16 gYKPointNum;                        //the total number of 104 YK point
extern UINT16 gYKPointNumBuf;                     //the total number of 104 YK point
extern UINT16 gSDPointNum;                        //the total number of 104 SD point
extern UINT16 gSDPointNumBuf;                     //the total number of 104 SD point
extern UINT16 gDDPointNum;                        //the total number of 104 DD point
extern UINT16 gDDPointNumBuf;                     //the total number of 104 DD point


/*CRC�����*/
unsigned char auchCRCHi[] = {
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
} ;

unsigned char auchCRCLo[] = {
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,   0x40
};

extern sDeviceInfo gDeviceInfo[MAXDEVICE];
extern sDeviceInfo gDeviceInfoBuf[MAXDEVICE];
extern s104Point aPointBuf[0x6601];
extern UINT32 aPointTimeBuf[0x6601];
extern UINT8 gTimeCallBackTime[7];
extern UINT8 gConnectDeviceNum;
struct sTypeGroup *gTypeGroupPointMB=NULL;
struct sTypeParam *gTypeParamPointMB=NULL;
extern struct sTypeGroup *gTypeHead;
extern UINT8 gMainDeviceConfig;
extern UINT8 load_point_table_flag;
extern int ComRead(UINT8 nCom,UINT8 *pDataBuf,UINT8 nLen,UINT8 *pRecvBuf);

int NumLocalthread=11;//ͳ������ɨ�����
int nRecordChangeFlag=0;
struct sTypeGroup *psPointTable=NULL;
UINT8 njjjPointType=0;
UINT8 PlcC7Buff0[84]={0};
UINT8 PlcC7Buff1[84]={0};
UINT8 PLC0ConnectiveStatus=0;//COM1��
UINT8 PLC1ConnectiveStatus=0;//COM2 ��

UINT8 GetOffset(UINT16 nPointMbAddr,UINT8 nDeviceID,UINT8 *nRecordPointType,UINT8 *nDataType);
UINT8 HandleSubCallBack(int nPointTimeRecordFd,UINT8 *aTimeBuf);
UINT8 CheckRecord(UINT8 *aReadTemp,UINT8 *aTimeBuf,int nPointTimeRecordFd,UINT8 nStartFlag,UINT8 nGetDataFlag );
void HandleRecordData(int nPointTimeRecordFd,UINT8 *aReadTemp);
void SetPointType(UINT8 gVal,UINT8 Type);


/*void pn_delay(unsigned int nMillisecond)
{
    struct timeval tpstart,tpend;
    unsigned long lTime;

    gettimeofday(&tpstart,NULL);
    do
    {
        gettimeofday(&tpend,NULL);
        lTime = 1000 * (tpend.tv_sec - tpstart.tv_sec);
        lTime += (tpend.tv_usec - tpstart.tv_usec)/1000;
    }while(lTime < nMillisecond);
}*/
void hs_udelay(unsigned int udelayTime)
{
    static struct timeval _tstart,_tend;
    double t,t1,t2;
    int i;
    gettimeofday(&_tstart,NULL);
    t1 = (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
    for(i =0;;i++)
    {
      gettimeofday(&_tend,NULL);
      t2 = (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
      t = t2-t1;
      if(t> (double)udelayTime/(1000*1000))
      {
          return ;
      }
    }
}

/*****************************************************************************
* Description:      CRC����
* Parameters:          puchMsg:��Ҫ���������
                     usDataLen:���ݳ���
* Returns:           ���ɵ�CRC16
*****************************************************************************/
unsigned short CRC16 (unsigned char * puchMsg,unsigned short usDataLen ) /* The function returns the CRC as a unsigned short type */
{
    unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
    unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
    unsigned uIndex ; /* will index into CRC lookup table */
    while (usDataLen--) /* pass through message buffer */
    {
        uIndex = uchCRCLo ^ *puchMsg++ ; /* calculate the CRC */
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
        uchCRCHi = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

/*****************************************************************************
* Description:      CRC����
* Parameters:          puchMsg:��Ҫ���������
                     usDataLen:���ݳ���
* Returns:           ���ɵ�CRC16
*****************************************************************************/
unsigned short FileCRC16 (unsigned char * puchMsg,unsigned short usDataLen ) /* The function returns the CRC as a unsigned short type */
{
    unsigned char uchCRCHi = gIVCRCHi ; /* high byte of CRC initialized */
    unsigned char uchCRCLo = gIVCRCLo ; /* low byte of CRC initialized */
    unsigned uIndex ; /* will index into CRC lookup table */
    while (usDataLen--) /* pass through message buffer */
    {
        uIndex = uchCRCLo ^ *puchMsg++ ; /* calculate the CRC */
        uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
        uchCRCHi = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

/*****************************************************************************
* Description:      дE2PROM
* Parameters:         pValue:д��Ĳ���
                    addrValue:д��ĵ�ַ
                    count:д������ݳ���
* Returns:          1: д��ʧ��
                       0: д��ɹ�
*****************************************************************************/
INT32 E2promWrite(UINT8 * pValue,UINT16 addrValue,UINT8 count)
{
    int res;
    unsigned char nPageAddr,nStartNum,nEndNum,nPageLen,nLoopCount;
    unsigned short nPageNum;

    pthread_mutex_lock(&e2promSem);/*��ȡE2PROM�ź���*/
    nPageNum = addrValue>>5;//����ҳ��
    nPageAddr = (unsigned char)addrValue & 0x1F;/*����ҳ�ڵ�ַ*/
    nStartNum = E2PROM_PAGE_SIZE - nPageAddr;/*���㱾ҳ��ʣ��ռ�*/
    nEndNum = count >= nStartNum ? ((count - nStartNum)&0x1F) : count;/*д����������ʣ��ռ������βҳҳ�л���Ҫд������,����д��������Ϊ��ҳд������*/
    nPageLen = (count-nStartNum)>>5;/*���������ҳ��βҳ����Ҫд�����ҳ*/
    if((nPageAddr+count)>=E2PROM_PAGE_SIZE)/*д�����ݳ��ȳ���ҳ����,��Ҫ��ҳд*/
    {
        /*д�뱾ҳ����*/
        res = ioctl(nI2Cfd,I2C_CHIP_ID,E2PROM_CHIP_ADDR);
        res = ioctl(nI2Cfd,I2C_ADDR,addrValue);
        res = ioctl(nI2Cfd,I2C_ADDR_LEN,3);
        res = ioctl(nI2Cfd,I2C_DATA_LEN,(nStartNum));
        res = ioctl(nI2Cfd,I2C_FPGA_SEL,0x0200);
        res = write(nI2Cfd,pValue,nStartNum);
        //DbgPrintf("INTERFACE-----e2prom write res = %d Addr = %d Num = %d\n",res,addrValue,nStartNum);
        usleep(5000);
        /*д������ҳ*/
        for(nLoopCount = 0;nLoopCount < nPageLen;nLoopCount++)
        {
            res = ioctl(nI2Cfd,I2C_ADDR,(nPageNum+nLoopCount+1)<<5);
            res = ioctl(nI2Cfd,I2C_DATA_LEN,E2PROM_PAGE_SIZE);
            res = write(nI2Cfd,&pValue[nStartNum+(nLoopCount<<5)],E2PROM_PAGE_SIZE);
            //DbgPrintf("INTERFACE-----e2prom write res = %d Addr = %d Num = %d\n",res,(nPageNum+nLoopCount+1)<<5,E2PROM_PAGE_SIZE);
            usleep(5000);
        }
        /*д��βҳ*/
        res = ioctl(nI2Cfd,I2C_ADDR,(nPageNum+nPageLen+1)<<5);
        res = ioctl(nI2Cfd,I2C_DATA_LEN,nEndNum);
        res = write(nI2Cfd,&pValue[nStartNum+(nPageLen<<5)],nEndNum);
        //DbgPrintf("INTERFACE-----e2prom write res = %d Addr = %d Num = %d\n",res,(nPageNum+nPageLen+1)<<5,nEndNum);
    }
    else/*δ����ҳ����,����*/
    {
        res = ioctl(nI2Cfd,I2C_CHIP_ID,E2PROM_CHIP_ADDR);
        res = ioctl(nI2Cfd,I2C_ADDR,addrValue);
        res = ioctl(nI2Cfd,I2C_ADDR_LEN,3);
        res = ioctl(nI2Cfd,I2C_DATA_LEN,count);
        res = ioctl(nI2Cfd,I2C_FPGA_SEL,0x0200);
        res = write(nI2Cfd,pValue,count);
        //DbgPrintf("INTERFACE-----e2prom write res = %d Num = %d\n",res,count);
    }
    usleep(5000);
    pthread_mutex_unlock(&e2promSem);
    return res;
}

/*****************************************************************************
* Description:      ��E2PROM
* Parameters:         pValue:��ȡ��ֵ
                    addrValue:��ȡ����ʼ��ַ
                    count:��ȡ�ĳ���
* Returns:          1: ��ȡʧ��
                    0: ��ȡ�ɹ�
*****************************************************************************/
INT32 E2promRead(UINT8 * pValue,UINT16 addrValue,UINT8 count)
{
    int res;

    pthread_mutex_lock(&e2promSem);
    res = ioctl(nI2Cfd,I2C_CHIP_ID,E2PROM_CHIP_ADDR);
    res = ioctl(nI2Cfd,I2C_ADDR,addrValue);
    res = ioctl(nI2Cfd,I2C_ADDR_LEN,3);
    res = ioctl(nI2Cfd,I2C_DATA_LEN,count);
    res = ioctl(nI2Cfd,I2C_FPGA_SEL,0x0200);
    res = read(nI2Cfd,pValue,count);
    usleep(5000);
    pthread_mutex_unlock(&e2promSem);
    return res;
}

/*****************************************************************************
* Description:      дRTC�Ĵ���
* Parameters:          nAddr:�Ĵ�����ַ
                     nValue:д��ֵ
* Returns:
*****************************************************************************/
void RTCWrite(unsigned char nAddr,unsigned char nValue)
{
    int res;

    pthread_mutex_lock(&e2promSem);
    res = ioctl(nI2Cfd,0x01,0xD0);
    res = ioctl(nI2Cfd,0x02,nAddr);
    res = ioctl(nI2Cfd,0x03,2);
    res = ioctl(nI2Cfd,0x04,1);
    res = ioctl(nI2Cfd,0x05,0x0200);
    res = write(nI2Cfd,&nValue,1);
    pthread_mutex_unlock(&e2promSem);
    return;
}

/*****************************************************************************
* Description:      ��RTC�Ĵ���
* Parameters:          nAddr:�Ĵ�����ַ
* Returns:           nValue:��ȡ�Ĵ���ֵ
*****************************************************************************/
unsigned char RTCRead(unsigned char nAddr)
{
    int res;
    unsigned char value;

    pthread_mutex_lock(&e2promSem);
    res = ioctl(nI2Cfd,0x01,0xD0);
    res = ioctl(nI2Cfd,0x02,nAddr);
    res = ioctl(nI2Cfd,0x03,2);
    res = ioctl(nI2Cfd,0x04,1);
    res = ioctl(nI2Cfd,0x05,0x0200);
    res = read(nI2Cfd,&value,1);
    pthread_mutex_unlock(&e2promSem);
    return value;
}

/*****************************************************************************
* Description:      ����RTCʱ��
* Parameters:          pParaValue:��Ҫ���õ�ʱ�仺��
                     year mouth day hour min sec
* Returns:
*****************************************************************************/
UINT8 RTCSet(UINT8 *pParaValue)
{
    unsigned char temp[6];

    temp[0] = (pParaValue[6]&0x7f);
    temp[1] = (pParaValue[5]&0x7f);
    temp[2] = (pParaValue[4]&0x3F);
    temp[3] = (pParaValue[3]&0x3f);
    temp[4] = (pParaValue[2]&0x1f);
    temp[5] = pParaValue[1];
    if(pParaValue[0] == 0x20)
        temp[2] &= 0xbf;
    RTCWrite(0x00,temp[0]);
    RTCWrite(0x01,temp[1]);
    RTCWrite(0x02,temp[2]);
    RTCWrite(0x04,temp[3]);
    RTCWrite(0x05,temp[4]);
    RTCWrite(0x06,temp[5]);
    return 0;
}

/*****************************************************************************
* Description:      ��ȡRTC�е�ʱ��
* Parameters:
* Returns:           year mouth day hour min sec
*****************************************************************************/
UINT8 RTCGet()
{
    unsigned char temp[6];
    char *aCmd="date -s %02d%02d%02d%02d20%02d.%02d";
    char aSetcmd[50];

    //temp[6] = RTCRead(0x00);
    temp[5] = RTCRead(0x01);
    temp[4] = RTCRead(0x02);
    temp[3] = RTCRead(0x04);
    temp[2] = RTCRead(0x05);
    temp[1] = RTCRead(0x06);
    temp[0] = 0x20;
    DbgPrintf("%x%x-%x-%x %x:%x:%x\n",temp[0],temp[1],temp[2],temp[3],(temp[4]&0x3f),temp[5]&0x7F,temp[6]);
    sprintf(aSetcmd,aCmd,temp[2]/16*10+temp[2]%16,temp[3]/16*10+temp[3]%16,temp[4]/16*10+temp[4]%16,(temp[5]&0x7F)/16*10+(temp[5]&0x7F)%16,temp[1]/16*10+temp[1]%16,temp[6]/16*10+temp[6]%16);
    system(aSetcmd);
    return 0;
}

/*****************************************************************************
* Description:      дCPLD�Ĵ���
* Parameters:        nAddr:reg addr
                     nValue:reg value
* Returns:
*****************************************************************************/
void FpgaWrite(UINT16 nAddr, UINT8 nValue)
{
    UINT8 aBuf[6];

    //pthread_mutex_lock(&fpgaSem);
    aBuf[0] = (UINT8)(nAddr);
    aBuf[1] = (UINT8)(nAddr>>8);
    aBuf[2] = (UINT8)(nAddr>>16);
    aBuf[3] = (UINT8)(nAddr>>24);
    aBuf[4] = nValue;
    write(nFPGAfd,aBuf,5);
    usleep(5000);
    //pthread_mutex_unlock(&fpgaSem);
    return;
}

/*****************************************************************************
* Description:      ��CPLD�Ĵ���
* Parameters:        nAddr:reg addr
* Returns:           reg value
*****************************************************************************/
UINT8 FpgaRead(UINT16 nAddr)
{
    UINT8 aBuf[4];
    UINT8 nValue;
    //pthread_mutex_lock(&fpgaSem);

    aBuf[0] = (UINT8)(nAddr);
    aBuf[1] = (UINT8)(nAddr>>8);
    aBuf[2] = (UINT8)(nAddr>>16);
    aBuf[3] = (UINT8)(nAddr>>24);
    nValue = read(nFPGAfd,aBuf,4);
    //pthread_mutex_unlock(&fpgaSem);
    usleep(5000);
    return nValue;
}

/*****************************************************************************
* Description:      ����IO���
* Parameters:        nLine:0:GP_A 1:GP_B 2:GP_C every line include 32 pins
                     nPin:0-31
                     nValue:0:Low level 1:High Level
* Returns:
*****************************************************************************/
void GPIOSet(UINT8 nLine,UINT8 nPin,UINT8 nValue)
{
    UINT8 aBuf[3];

    nGPIOfd = open("/dev/hsgpio",O_RDWR);
    if(nGPIOfd<0)
    {
        DbgPrintf("open gpio error\r\n");
        close(nGPIOfd);
    }
    aBuf[0]=nLine;
    aBuf[1]=nPin;
    aBuf[2]=nValue;
    write(nGPIOfd,aBuf,3);
    close(nGPIOfd);
}

/*****************************************************************************
* Description:      ��ȡIO�����Ϣ
* Parameters:        nLine:0:GP_A 1:GP_B 2:GP_C every line include 32 pins
                     nPin:0-31
* Returns:           0:Low level 1:High Level
*****************************************************************************/
UINT8 GPIOGet(UINT8 nLine,UINT8 nPin)
{
    UINT8 aBuf[2],res;

    nGPIOfd = open("/dev/hsgpio",O_RDWR);
    if(nGPIOfd<0)
    {
        DbgPrintf("open gpio error\r\n");
        close(nGPIOfd);
    }
    aBuf[0]=nLine;
    aBuf[1]=nPin;
    res = read(nGPIOfd,aBuf,2);
    close(nGPIOfd);
    return res;
}

/*****************************************************************************
* Description:      ����LED����
* Parameters:        nBus:0-5 Power Link Modem COM1 COM2 Alarm
                     nStatus:0:�� 1:��
* Returns:
*****************************************************************************/
void LedSet(UINT8 nBus,UINT16 nStatus)
{
    struct sAlarmGroup *p;
    int i;

    switch(nBus)
    {
        case 0://Power
            GPIOSet(2,29,nStatus);
            break;
        case 1://Link
            GPIOSet(2,30,nStatus);
            break;
        case 2://Modem
            GPIOSet(2,28,nStatus);
            break;
        case 3://COM1
            if(nStatus == 1)
            {
                for(i = 0;i < MAXDEVICE;i++)
                {
                    if(gDeviceInfo[i].nDownlinkPort == 1)
                    {
                        //DbgPrintf("gDeviceInfo[%d].nDownlinkPort == %d\n\n\n\n\n",i,gDeviceInfo[i].nDownlinkPort);
                        if(gDeviceInfo[i].nInUse == _YES)
                        {
                            //DbgPrintf("gDeviceInfo[%d].nInUse == %d\n\n\n\n\n",i,gDeviceInfo[i].nInUse);
                            p = gAlarmHead;
                            if(p == NULL)
                            {
                                //printf("p is empty!!\n\n\n\n\n");
                                GPIOSet(2,27,1);
                                break;
                            }
                            else
                            {
                                //printf("ELSE!!\n\n\n\n\n");
                                while(p != NULL)
                                {
                                    if(p->nDeviceID != i)
                                    {
                                        //printf("p->nDeviceID == %d\n\n\n\n\n",p->nDeviceID);
                                        //printf("p->nAlarmID == %d\n\n\n\n\n",p->nAlarmID);
                                        p = p->pNext;
                                        continue;
                                    }
                                    if(p->nAlarmID != Alarm_Link_Error)
                                    {
                                        p = p->pNext;
                                        continue;
                                    }
                                    break;
                                }
                                if(p == NULL)
                                {
                                    //printf("judge finished!!\n\n\n\n\n");
                                    GPIOSet(2,27,1);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if(nStatus == 0)
                GPIOSet(2,27,0);
            break;
        case 4://COM2
            if(nStatus == 1)
            {
                for(i=0;i<MAXDEVICE;i++)
                {
                    if(gDeviceInfo[i].nDownlinkPort == 2)
                    {
                        if(gDeviceInfo[i].nInUse == _YES)
                        {
                            p = gAlarmHead;
                            if(p == NULL)
                            {
                                GPIOSet(2,22,1);
                                break;
                            }
                            else
                            {
                                while(p != NULL)
                                {
                                    if(p->nDeviceID != i)
                                    {
                                        p = p->pNext;
                                        continue;
                                    }
                                    if(p->nAlarmID != Alarm_Link_Error)
                                    {
                                        p = p->pNext;
                                        continue;
                                    }
                                    break;
                                }
                                if(p == NULL)
                                {
                                    //printf("p is empty!!\n\n\n\n\n");
                                    GPIOSet(2,22,1);
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            else if(nStatus == 0)
                GPIOSet(2,22,0);
            break;
        case 5://Alarm
            GPIOSet(2,16,nStatus);
            break;
    }
}

/*****************************************************************************
* Description:      ����APģ��
* Parameters:        nStatus 1:work mode 0:shut down
* Returns:
*****************************************************************************/
void WIFIEnable(UINT16 nStatus)
{
    GPIOSet(0,29,0);
    //GPIOSet(1,31,0);
    sleep(1);
    GPIOSet(0,29,nStatus);
    //GPIOSet(1,31,nStatus);
    GPIOSet(0,27,1);
    sleep(1);
    GPIOSet(0,27,0);
}

/*****************************************************************************
* Description:      ����ϵͳʱ��
* Parameters:
* Returns:
*****************************************************************************/
void SysTimeInit()
{
    UINT8 aTime[7],aTimeBuf[14],aTimeStr[15],nCount;
    int res;
    UINT8 aCmd[50]="date ";

    res = RTCGet(aTime);/*��ȡRTCʱ��*/
    usleep(5000);
    for(nCount=0;nCount<7;nCount++)
    {
        aTimeBuf[nCount*2] = 0x30 + aTime[nCount]/16;
        aTimeBuf[nCount*2+1] = 0x30 + aTime[nCount]%16;
    }
    for(nCount=0;nCount<8;nCount++)
    {
        aTimeStr[nCount] = aTimeBuf[nCount+4];
    }
    for(nCount=0;nCount<4;nCount++)
    {
        aTimeStr[nCount+8] = aTimeBuf[nCount];
    }
    aTimeStr[12] = '.';
    aTimeStr[13] = aTimeBuf[12];
    aTimeStr[14] = aTimeBuf[13];
    strncat((void *)aCmd,(void *)aTimeStr,15);
    system((void *)aCmd);/*����ϵͳʱ��*/
}

/*****************************************************************************
* Description:      Open log file with right day number
* @basepath          the base directory of log file
* Returns: @fd       0: open failed  >0: open success
*****************************************************************************/
int openLogFile(char *basepath)
{
    // Get day number from current day time
    time_t timedelay;
    struct tm *pTime;
    int fd = 0;
    unsigned char day,month;

    time(&timedelay);
    pTime=gmtime(&timedelay);
    day = pTime->tm_mday;//RTCRead(0x04);
    month = pTime->tm_mon;

    //printf("XXX Get day: %d XXX\n", day);
    if (day >= 1 && day <= 31)
    {
        // construct file name
        char path[256] = {0x00};
        char filename[10] = {0x00};
        my_itoa(day, filename);
        strcat(filename, ".bin");
        strcat(path, basepath);
        strcat(path, filename);

        // do not need change writing file
        if (gCurrentDayNum == day || gCurrentDayNum == 0)
        {
            gCurrentDayNum = day;
            if(access(path,F_OK)==0)
            {
                if(GetFileSize(path)>(1000*1000*1))
                {
                        fd = (int)fopen(path,"w+");
                }
                else
                {
                       fd = (int)fopen(path,"a+");
                }
            }
            else
            {
                fd = (int)fopen(path,"w+");
            }
        }
        else // change another file (clear old data)
        {
            fd = (int)fopen(path,"w+");
            gCurrentDayNum = day;
        }

        return fd;
    }

    return 0;//failed
}

/*****************************************************************************
* Description:      ���汱���շ���־
* Parameters:          nStatus:1-6 Socket Recv/Send,AP Recv/Send,Modem Recv/Send
                     aRecvBuf: �շ����ݻ���
                     nLen:���ݳ���
* Returns:
*****************************************************************************/
void LogRecordFileWrite(UINT8 nStatus,UINT8 *aRecvBuf,UINT8 nLen)
{
    time_t timep;
    struct tm *pTime;
    UINT8 aBuf[300],i;

    pthread_mutex_lock(&logsem);
    time(&timep);
    pTime=gmtime(&timep);
    /*
    if(GetFileSize(LogRecordFile)>(1000*1000*10))//����10M�������־
        nLogRecordFd=fopen(LogRecordFile,"w+");
    else
        nLogRecordFd=fopen(LogRecordFile,"a+");
    */
    nLogRecordFd = openLogFile(LogRecodeFilePath);
    if(nLogRecordFd==0)
    {
        DbgPrintf("[Log]Open log file fail!!!\r\n");
        pthread_mutex_unlock(&logsem);
    }
    else
    {
        memset(aBuf,0,sizeof(aBuf));
        //printf("%d-%d-%d %d:%d:%d\n",pTime->tm_year+1900,pTime->tm_mon,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
        //sprintf(aBuf,LogRecordFileRegex,pTime->tm_year+1900-2000,pTime->tm_mon+1,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);/*����ʱ��*/
        aBuf[0] = (pTime->tm_year+1900-2000) / 10 * 16 + (pTime->tm_year+1900-2000) % 10;
        aBuf[1] = (pTime->tm_mon+1) / 10 * 16 + (pTime->tm_mon+1) % 10;
        aBuf[2] = (pTime->tm_mday) / 10 * 16 + (pTime->tm_mday) % 10;
        aBuf[3] = (pTime->tm_hour) / 10 * 16 + (pTime->tm_hour) % 10;
        aBuf[4] = (pTime->tm_min) / 10 * 16 + (pTime->tm_min) % 10;
        aBuf[5] = (pTime->tm_sec) / 10 * 16 + (pTime->tm_sec) % 10;
        switch(nStatus)
        {
            case 1:
            {
                //strcat(aBuf,"0x01");
                aBuf[6] = 0x01;    //Socket Recv
                break;
            }
            case 2:
            {
                //strcat(aBuf,"0x02");
                aBuf[6] = 0x02;    //Socket Send
                break;
            }
            case 3:
            {
                //strcat(aBuf,"0x03");
                aBuf[6] = 0x03;    //AP Recv
                break;
            }
            case 4:
            {
                //strcat(aBuf,"0x04");
                aBuf[6] = 0x04;    //AP Send
                break;
            }
            case 5:
            {
                //strcat(aBuf,"0x05");
                aBuf[6] = 0x05;    //Modem Recv
                break;
            }
            case 6:
            {
                //strcat(aBuf,"0x06");
                aBuf[6] = 0x06;    //Modem Send
                break;
            }
        }
        for(i=0;i<nLen;i++)
        {
            aBuf[7+i] = aRecvBuf[i];
            //sprintf(aBuf,"0x%d",aRecvBuf[i]);
        }
        aBuf[(nLen+7)] = 0x0A;
        fwrite(aBuf,(nLen+8),1,(void *)nLogRecordFd);
        fclose((void *)nLogRecordFd);
        pthread_mutex_unlock(&logsem);
    }
}

/*****************************************************************************
* Description:      ���������շ���־
* Parameters:          nStatus:1-2 Recv/Send
                     aRecvBuf: Recv/Send Data
                     nLen:Data Length
* Returns:
*****************************************************************************/
void LogComRecordFileWrite(UINT8 nStatus,UINT8 *aRecvBuf,UINT8 nLen)
{
    /*time_t timep;
    struct tm *pTime;
    UINT8 aBuf[300],i,nTemp;

    time(&timep);
    pTime=gmtime(&timep);


    if(GetFileSize(LogComRecordFile)>(1000*1000*50))
        nComLogRecordFd=fopen(LogComRecordFile,"w+");
    else
        nComLogRecordFd=fopen(LogComRecordFile,"a+");


    memset(aBuf,0,sizeof(aBuf));
    //printf("%d-%d-%d %d:%d:%d\n",pTime->tm_year+1900,pTime->tm_mon,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
    sprintf(aBuf,LogRecordFileRegex,pTime->tm_year+1900,pTime->tm_mon+1,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
    switch(nStatus)
    {
        case 1:
        {
            strcat(aBuf,"COM Recv:");
            break;
        }
        case 2:
        {
            strcat(aBuf,"COM Send:");
            break;
        }
    }
    fwrite(aBuf,strlen(aBuf),1,nComLogRecordFd);
    for(i=0;i<nLen;i++)
    {
        sprintf(aBuf,"%02X ",aRecvBuf[i]);
        fwrite(aBuf,strlen(aBuf),1,nComLogRecordFd);
    }
    nTemp=0x0A;
    fwrite(&nTemp,1,1,nComLogRecordFd);
    fclose(nComLogRecordFd);*/
}

/*****************************************************************************
* Description:      ���������־,��Ҫ�����������
* Parameters:          aRecvBuf: Recv/Send Data
                     nLen:Data Length
* Returns:
*****************************************************************************/
void TestLogStringFileWrite(UINT8 *aRecvBuf,UINT8 nLen)
{
    /*pthread_mutex_lock(&logsem);
    nTestLogRecordFd=fopen(TestLogRecordFile,"a+");
    fwrite(aRecvBuf,nLen,1,nTestLogRecordFd);
    fclose(nTestLogRecordFd);
    pthread_mutex_unlock(&logsem);*/
}

/*****************************************************************************
* Description:      write time into Testlog file
* Parameters:          aRecvBuf: Recv/Send Data
                     nLen:Data Length
* Returns:
*****************************************************************************/
void TestLogTimeFileWrite()
{
    /*time_t timep;
    struct tm *pTime;
    UINT8 aBuf[300],i;

    pthread_mutex_lock(&logsem);
    time(&timep);
    pTime=gmtime(&timep);
    memset(aBuf,0,sizeof(aBuf));
    sprintf(aBuf,LogRecordFileRegex,pTime->tm_year+1900,pTime->tm_mon,pTime->tm_mday,pTime->tm_hour,pTime->tm_min,pTime->tm_sec);
    nTestLogRecordFd=fopen(TestLogRecordFile,"a+");
    fwrite(aBuf,strlen(aBuf),1,nTestLogRecordFd);
    fclose(nTestLogRecordFd);
    pthread_mutex_unlock(&logsem);*/
}

/*****************************************************************************
* Description:      write operation platform into Testlog file
* Parameters:          nStatus: 1,2 platform 3,4 app 5,6 modem
* Returns:
*****************************************************************************/
void TestLogTypeFileWrite(UINT8 nStatus)
{
    /*UINT8 aBuf[300];

    memset(aBuf,0,sizeof(aBuf));
    pthread_mutex_lock(&logsem);
    switch(nStatus)
    {
        case 1:
        case 2:
        {
            strcat(aBuf,"Platform:");
            break;
        }
        case 3:
        case 4:
        {
            strcat(aBuf,"APP:");
            break;
        }
        case 5:
        case 6:
        {
            strcat(aBuf,"Modem:");
            break;
        }
    }
    nTestLogRecordFd=fopen(TestLogRecordFile,"a+");
    fwrite(aBuf,strlen(aBuf),1,nTestLogRecordFd);
    fclose(nTestLogRecordFd);
    pthread_mutex_unlock(&logsem);*/
}

/*****************************************************************************
* Description:      ����ͻ��������ƽ̨
* Parameters:       nFlag: 1: YC  2:YX
                    nChannel: channel id
* Returns:
*****************************************************************************/
void SendChangeBuf(UINT8 nFlag)
{
    UINT8 aSendTemp[256];
    UINT16 nPacketValue;


    switch(nFlag)
    {
        case Type_104_YC:/*����ң������*/
        {
            aSendTemp[0]=0x68;
            aSendTemp[1]=gYCChangeCount*8+10;
            nPacketValue=gSendCount*2;
            memcpy((UINT8 *)&aSendTemp[2],(UINT8 *)&nPacketValue,2);
            nPacketValue=gRecvCount*2;
            memcpy((UINT8 *)&aSendTemp[4],(UINT8 *)&nPacketValue,2);
            aSendTemp[6]=0x0D;
            aSendTemp[7]=gYCChangeCount;
            aSendTemp[8]=0x03;
            aSendTemp[9]=0x00;
            memcpy((UINT8 *)&aSendTemp[10],(UINT8 *)&gMainDeviceID,2);
            memcpy((UINT8 *)&aSendTemp[12],gYCChangeBuf,gYCChangeCount*8);
            if(gMasterReportSwitch!=1)
            {
                Socket_Send(aSendTemp,gYCChangeCount*8+12);
            }
            if(g_ethernet_connect_status == 1)
            {
                AddMsgToSGCCSendBuff(aSendTemp,gYCChangeCount*8+12);
            }
            gSendCount++;
            memset(gYCChangeBuf,0,sizeof(gYCChangeBuf));
            gYCChangeCount=0;
            break;
        }
        case Type_104_YX:/*����ң������*/
        {
            aSendTemp[0]=0x68;
            aSendTemp[1]=gYXChangeCount*4+10;
            nPacketValue=gSendCount*2;
            memcpy((UINT8 *)&aSendTemp[2],(UINT8 *)&nPacketValue,2);
            nPacketValue=gRecvCount*2;
            memcpy((UINT8 *)&aSendTemp[4],(UINT8 *)&nPacketValue,2);
            aSendTemp[6]=0x01;
            aSendTemp[7]=gYXChangeCount;
            aSendTemp[8]=0x03;
            aSendTemp[9]=0x00;
            memcpy((UINT8 *)&aSendTemp[10],(UINT8 *)&gMainDeviceID,2);
            memcpy((UINT8 *)&aSendTemp[12],gYXChangeBuf,gYXChangeCount*7);
            if(gMasterReportSwitch!=1)
                Socket_Send(aSendTemp,gYXChangeCount*4+12);
            if(g_ethernet_connect_status == 1)
                AddMsgToSGCCSendBuff(aSendTemp,gYXChangeCount*4+12);
            gSendCount++;
            memset(gYXChangeBuf,0,sizeof(gYXChangeBuf));
            gYXChangeCount=0;
            break;
        }
    }
}

/*****************************************************************************
* Description:      ���ͻ�����������ݻ���
* Parameters:          nFlag: 1: YC  2:YX
                     nDataType: 4:signed short 5:signed int
                     nAddr:104 address
                     nValue:device reg value
* Returns:
*****************************************************************************/
void AddChangeBuf(UINT8 nFlag,UINT8 nDataType,UINT16 nAddr,UINT32 nValue)
{
    switch(nFlag)
    {
        case Type_104_YC://*����ң������--�źŵ�����*/
        {
            INT32 nDataTemp;
            if(gTypeParamPointMB->nDataType==Type_Data_FLOAT)
            {
                memcpy((UINT8 *)&gYCChangeBuf[gYCChangeCount*8],(UINT8 *)&nAddr,2);
                memcpy((UINT8 *)&gYCChangeBuf[gYCChangeCount*8+3],(UINT8 *)&nValue,4);
                gYCChangeCount++;
                if((gYCChangeCount*8)>230)
                    SendChangeBuf(nFlag);
                    break;
            }
            else if(gTypeParamPointMB->nDataType==Type_Data_INT16)/*INT16 ���и����ж�*/
            {
                if((nValue&0x8000)!=0)
                {
                    nDataTemp = nValue - pow(2,16);
                    uIE.nIndex=nDataTemp;
                }
                else
                    uIE.nIndex=nValue;
            }
            else if(gTypeParamPointMB->nDataType==Type_Data_INT32)/*INT32 ���и����ж�*/
            {
                if((nValue&0x80000000)!=0)
                {
                    nDataTemp = nValue - pow(2,32);
                    uIE.nIndex=nDataTemp;
                }
                else
                    uIE.nIndex=nValue;
            }
            else
                uIE.nIndex=nValue;
            memcpy((UINT8 *)&gYCChangeBuf[gYCChangeCount*8],(UINT8 *)&nAddr,2);
            memcpy((UINT8 *)&gYCChangeBuf[gYCChangeCount*8+3],uIE.nChar,4);
            gYCChangeCount++;
            if((gYCChangeCount*8)>230)
                SendChangeBuf(nFlag);
            break;
        }
        case Type_104_YX://*����ң������--�źŵ�����*/
        {
            memcpy((UINT8 *)&gYXChangeBuf[gYXChangeCount*4],(UINT8 *)&nAddr,2);
            gYXChangeBuf[gYXChangeCount*4+3]=nValue;
            gYXChangeCount++;
            if((gYXChangeCount*4)>230)
                SendChangeBuf(nFlag);
            break;
        }
    }
}

/*****************************************************************************
* Description:      �����������ݼ�¼�ļ�
* Parameters:
* Returns:
*****************************************************************************/
void AddPointRecordFile(UINT8 nOpraMode)
{
    UINT8 aBuf[50];
    time_t timep;
    struct tm *pTime;

    time(&timep);
    pTime=gmtime(&timep);
    if(pTime->tm_year==1970)
        return;
    sprintf((void *)aBuf,PointRecordFileRegex,pTime->tm_mday);/*������Ϊ�ļ���*/
    if(nOpraMode==1)
    {
        nPointRecordFd = (int)fopen((void *)aBuf,"wb+");
    }
    else
    {
        /*if(GetFileSize(aBuf)>(1000*1000*5))
        {
            fclose(nPointRecordFd);
            nPointRecordFd=0;
            return;
        }
        else*/
        nPointRecordFd = (int)fopen((void *)aBuf,"ab+");
    }

}

/*****************************************************************************
* Description:      д�������������ݻ�ȡ��ʱ�䵽��¼�ļ���
* Parameters:
* Returns:
*****************************************************************************/
void AddPointRecordTime(void)
{
    //UINT8 aBuf[50];
    UINT8 aBuf[9];
    time_t timep;
    struct tm *pTime;

    time(&timep);
    pTime=gmtime(&timep);
    if((pTime->tm_year==1970)||(nPointRecordFd==0))
        return;
    /*��Ӽ�¼ͷ*/
    aBuf[0]=0x7E;
    aBuf[1]=0xAA;
    aBuf[2]=0x55;
    aBuf[3]=0x6E;
    /*���ʱ��*/
    aBuf[4]=pTime->tm_mon;
    aBuf[5]=pTime->tm_mday;
    aBuf[6]=pTime->tm_hour;
    aBuf[7]=pTime->tm_min;
    aBuf[8]=pTime->tm_sec;
    fwrite(aBuf,sizeof(aBuf),1,(void *)nPointRecordFd);
    NumLocalthread++;
    if(NumLocalthread>12)
    {
        DbgPrintf("XXX NEW ROUND");
        NumLocalthread=1;
    }
}

/*****************************************************************************
* Description:      д�������������ݵ���¼�ļ���
* Parameters:        n104Addr:104��ַ
                     nDevice:�����豸��ַ
                     nMBAddr:MODBUS��ַ
                     nValue:��������
                     nDataType:��������
* Returns:
*****************************************************************************/
void AddPointRecord(UINT16 n104Addr,UINT8 nDeviceID,UINT16 nMBAddr,UINT32 nValue,UINT8 nDataType)
{
    //UINT8 aBuf[100];
    time_t timep;
    struct tm *pTime;

    time(&timep);
    pTime=gmtime(&timep);
    if((pTime->tm_year==1970)||(nPointRecordFd==0))
        return;
    /*sprintf(aBuf,PointRecordRegex,n104Addr,nDeviceID,nMBAddr,nValue);
    //printf("%s\r\n",aBuf);
    fwrite(aBuf,strlen(aBuf),1,nPointRecordFd);*/
    //fwrite((UINT8 *)&n104Addr,sizeof(n104Addr),1,nPointRecordFd);
    fwrite((UINT8 *)&nDeviceID,sizeof(nDeviceID),1,(void *)nPointRecordFd);
    fwrite((UINT8 *)&nMBAddr,sizeof(nMBAddr),1,(void *)nPointRecordFd);
    fwrite((UINT8 *)&nValue,sizeof(nValue),1,(void *)nPointRecordFd);
    //fwrite((UINT8 *)&nDataType,sizeof(nDataType),1,nPointRecordFd);
}

/*****************************************************************************
* Description:      �ж��Ƿ���ڶ�Ӧʱ������������ݼ�¼
* Parameters:
* Returns:           0: ������ 1:������
*****************************************************************************/
UINT8 GetPointRecordStatus(void)
{
    int nPointTimeRecordFd;
    UINT8 aFilePath[50];
    UINT8 aReadTemp[10],nStartFlag=0;/*nStartFlag:��¼ͷ�ļ���ʶ ֱ����ȡ��7E AA 55 7E*/

    sprintf((void *)aFilePath,PointRecordFileRegex,gTimeCallBackTime[4]);/*�򿪶�Ӧ���ڵļ�¼�ļ�*/
    nPointTimeRecordFd = open((void *)aFilePath,O_RDONLY);
    if(nPointTimeRecordFd<0)
        return 0;
    while(read(nPointTimeRecordFd,aReadTemp,1)!=0)/*ÿ�ζ�ȡһ���ֽ�ֱ���ļ���ȡ���*/
    {
        switch(nStartFlag)
        {
            case 0:
            {
                if(aReadTemp[0]==0x7E)
                    nStartFlag++;
                break;
            }
            case 1:
            {
                if(aReadTemp[0]==0xAA)
                    nStartFlag++;
                else
                    nStartFlag=0;
                break;
            }
            case 2:
            {
                if(aReadTemp[0]==0x55)
                    nStartFlag++;
                else
                    nStartFlag=0;
                break;
            }
            case 3:
            {
                if(aReadTemp[0]==0x7E)
                {
                    if(read(nPointTimeRecordFd,aReadTemp,5)!=0)
                    {
                        if(((gTimeCallBackTime[2]+gTimeCallBackTime[3]*60)-(aReadTemp[3]+aReadTemp[2]*60)>=15)&&
                            ((gTimeCallBackTime[2]+gTimeCallBackTime[3]*60)-(aReadTemp[3]+aReadTemp[2]*60)<=30))
                        {
                            nStartFlag=0;
                            close(nPointTimeRecordFd);
                            return 1;
                        }
                    }
                }
                nStartFlag=0;
                break;
            }
        }
    }
    close(nPointTimeRecordFd);
    return 0;
}

/*****************************************************************************
* Description:      ����ʱ���,��ȡ��Ӧ����ʷ����
* Parameters:        aTimeBuf:sec min hour day mouth year
* Returns:
*****************************************************************************/
UINT8 GetPointRecord(UINT8 *aTimeBuf)
{
    int nPointTimeRecordFd;
    UINT8 aFilePath[50];//��־·��
    UINT8 aReadTemp[14],nStartFlag=0,nGetDataFlag=0;
    UINT8 nTimeFlag=0;

    memset(aPointTimeBuf,0xFFFFFFFF,sizeof(aPointTimeBuf));//��ʼ��104���
    sprintf((void *)aFilePath,PointRecordFileRegex,aTimeBuf[4]);
    nPointTimeRecordFd = open((void *)aFilePath,O_RDONLY);/*�򿪶�Ӧ���ڵ��ļ�*/
    if(nPointTimeRecordFd<0)
        return 0;
    memcpy(gTimeCallBackTime,aTimeBuf,7);
    //��whileֻ�����ҵ���һ�����ϱ�׼��ʱ��
    while(read(nPointTimeRecordFd,aReadTemp,1)!=0)
    {
        //printf("%02X ",aReadTemp[0]);
        switch(nStartFlag)
        {
            case 0:
            {
                if(aReadTemp[0]==0x7E)
                    nStartFlag++;
                break;
            }
            case 1:
            {
                if(aReadTemp[0]==0xAA)
                    nStartFlag++;
                else
                    nStartFlag=0;
                break;
            }
            case 2:
            {
                if(aReadTemp[0]==0x55)
                    nStartFlag++;
                else
                    nStartFlag=0;
                break;
            }
            case 3:/*����ÿһ��ʱ���*/
            {
                nStartFlag=0;
                if(aReadTemp[0]==0x7E)
                {
                    if(read(nPointTimeRecordFd,aReadTemp,5)!=0)
                    {
                        if(abs((aTimeBuf[2]+aTimeBuf[3]*60-5)-(aReadTemp[3]+aReadTemp[2]*60))<=2)/*ƥ���¼��ʱ���ϣ����ȡ��ʱ��*/
                        {
                            UINT8 nReadLen=0;

                            DbgPrintf("CallBack Time = %02d-%02d %02d:%02d:00\r\n",aTimeBuf[5]+1,aTimeBuf[4],aTimeBuf[3],aTimeBuf[2]);
                            DbgPrintf("File     Time = %02d-%02d %02d:%02d:00\r\n",aReadTemp[0]+1,aReadTemp[1],aReadTemp[2],aReadTemp[3]);

                            nReadLen=read(nPointTimeRecordFd,aReadTemp,14);
                            do{
                                UINT16 nUpAddr;
                                UINT32 nValue;
                                INT32 nValueTemp;
                                UINT8 nFlag;
                                union uIEEE754 uIETemp;
                                UINT8 i;

                                for(i=0;i<10;i++)
                                {
                                    if((aReadTemp[i]==0x7E)&&(aReadTemp[i+1]==0xAA)&&(aReadTemp[i+2]==0x55)&&((aReadTemp[i+3]==0x7E) ||(aReadTemp[i+3]==0x6E)))
                                    {
                                        close(nPointTimeRecordFd);
                                        return nGetDataFlag;
                                    }
                                }
                                memcpy((UINT8 *)&nUpAddr,aReadTemp,2);
                                DbgPrintf("Addr = 0x%04X\r\n",nUpAddr);
                                memcpy((UINT8 *)&nValue,(UINT8 *)&aReadTemp[5],4);
                                if(nUpAddr>0x6600)
                                {
                                    close(nPointTimeRecordFd);
                                    return nGetDataFlag;
                                }
                                nGetDataFlag=1;
                                nFlag = aReadTemp[9];
                                if(nUpAddr>0x4000)/*ң�����͵�����*/
                                {
                                    if(nFlag==Type_Data_FLOAT)
                                        {
                                             memcpy((UINT8 *)&aPointTimeBuf[nUpAddr],(void *)nValue,4);
                                             continue;
                                        }
                                    if(nFlag==Type_Data_INT16)
                                    {
                                        if((nValue&0x8000)!=0)
                                        {
                                            nValueTemp = nValue - pow(2,16);
                                            uIETemp.nIndex=nValueTemp;
                                        }
                                        else
                                            uIETemp.nIndex=nValue;
                                    }
                                    else if(nFlag==Type_Data_INT32)
                                    {
                                        if((nValue&0x80000000)!=0)
                                        {
                                            nValueTemp = nValue - pow(2,32);
                                            uIETemp.nIndex=nValueTemp;
                                        }
                                        else
                                            uIETemp.nIndex=nValue;
                                    }
                                    else
                                        uIETemp.nIndex=nValue;
                                    memcpy((UINT8 *)&aPointTimeBuf[nUpAddr],uIETemp.nChar,4);
                                }
                                else/*ң������*/
                                    aPointTimeBuf[nUpAddr]=nValue;
                                //printf("ADDR = 0x%04X Value = %08X Char = %02X %02X %02X %02X %08X Flag = %d \r\n",
                                //       nUpAddr,nValue,uIETemp.nChar[0],uIETemp.nChar[1],uIETemp.nChar[2],uIETemp.nChar[3],aPointTimeBuf[nUpAddr],nFlag);
                                for(i=0;i<4;i++)
                                        aReadTemp[i]=aReadTemp[10+i];
                                nReadLen=read(nPointTimeRecordFd,(UINT8 *)&aReadTemp[4],10);
                                if(nReadLen<6)
                                {
                                    close(nPointTimeRecordFd);
                                    return nGetDataFlag;
                                }
                            }while(nReadLen>=6);//no case
                        }
                    }
                }
            /**--------New-----------**/
                else if(aReadTemp[0]==0x6E)
                {
                    if(read(nPointTimeRecordFd,aReadTemp,5)!=0)
                    {
                        if((aTimeBuf[5] != aReadTemp[0])
                        ||(aTimeBuf[4] != aReadTemp[1]))
                        {
                            nStartFlag=0;
                            break;
                        }

                        if(((aTimeBuf[2]+aTimeBuf[3]*60)>=(aReadTemp[3]+aReadTemp[2]*60))/*ƥ���¼��ʱ���ϣ����ȡ��ʱ��*/
                        &&(((aTimeBuf[2]+aTimeBuf[3]*60-5)-(aReadTemp[3]+aReadTemp[2]*60))<=75))/*ƥ���¼��ʱ���ϣ����ȡ��ʱ��*/
                        {
                            DbgPrintf("CallBack Time = %02d-%02d %02d:%02d:00\r\n",aTimeBuf[5]+1,aTimeBuf[4],aTimeBuf[3],aTimeBuf[2]);
                            DbgPrintf("File     Time = %02d-%02d %02d:%02d:00\r\n",aReadTemp[0]+1,aReadTemp[1],aReadTemp[2],aReadTemp[3]);

                            //if((aTimeBuf[5] == aReadTemp[0]) && (aTimeBuf[4] == aReadTemp[1]))
                            //{
                                nGetDataFlag=HandleSubCallBack(nPointTimeRecordFd,aTimeBuf);
                                //���β��ɳɹ�����
                                close(nPointTimeRecordFd);
                                return nGetDataFlag;
                            //}
                            /*else
                            {
                                printf("CallBack Date and File Date are not match!\r\n");
                                close(nPointTimeRecordFd);
                                return 0;
                            }*/
                        }
                        else if((aTimeBuf[2]+aTimeBuf[3]*60)<(aReadTemp[3]+aReadTemp[2]*60))//TimeLabel��������ʱ��,���β��ɽ���
                        {
                            close(nPointTimeRecordFd);
                            return nGetDataFlag;
                        }
                        else if(nTimeFlag==0)//ʱ��̫��Զ��, ��������һ��ʱ��
                        {
                            nStartFlag=0;
                            break;
                        }
                    }
                }
                else
                {
                    nStartFlag=0;
                    break;
                }
            }
        }
    }
    close(nPointTimeRecordFd);
    return 0;
}

/*****************************************************************************
* Description:      �߳�-�����������ݻ�ȡ
* Parameters:
* Returns:
*****************************************************************************/
void *LocalThread()
{
    unsigned short nCRC;
    int nPreRate=9600;
    time_t timep,timedelay;
    struct tm *pTime;

    DbgPrintf("THREAD-----LocalThread Init OK!\r\n");

    int nDay=0;
    UINT8 nCurrentLoopFlag=0;
    UINT8 nMinDelay=5,nOldMinDelay=0;
    UINT16 nFPGAValue=0;
    UINT8 crc_error_count=0;
    UINT8 DeviceCountBuf=0;

    nUartFd = UartOper(2,9600);

    while(1)
    {
        UINT8 i,aSendBuf[256];
        int nRecvLen=0;
        UINT8 nCount=0,nLen=0;
        UINT16 nMBAddr;
        struct sTypeParam *pTypeParaTemp = NULL;

        while(gMainDeviceStatus != DEVSTATUS_NORMAL_WORKMODE)/*�����ڵȴ���վ��״̬��,�ȴ���վ���*/
        {
            sleep(1);
            continue;
        }
        while(gUpdataModeFlag==1)/*��������ģʽ��ʱ��,�ȴ��������*/
        {
            sleep(1);
            continue;
        }
        if((gConnectDeviceNum>40)||(gConnectDeviceNum==0))/*�����豸����40����0ʱ,����ѭ���ȴ��豸��������*/
        {
            sleep(1);
            continue;
        }
        /*MODEMδ��ʼ����������ģʽ��,�ȴ�*/
        /*if((gModuleChannel0InitFlag!=2)&&(gSocketMode==1)&&(gModemConnectFlag==0)&&(gPlatformDisConnect==0))
        {
            sleep(1);
            continue;
        }*/
        memset(gMBRecvBuf,0,sizeof(gMBRecvBuf));/*����������ݻ���*/
        if((gTypeGroupPointMB==NULL)||(gDeviceInfo[gMBQueryDeviceCountBuf].nInUse==0))
        {
            gMBQueryDeviceCountBuf++;
            if(gYXChangeCount!=0)
                SendChangeBuf(2);
            if(gYCChangeCount!=0)
                SendChangeBuf(1);
            if((gMBQueryDeviceCountBuf>40))/*������ѯ,���һ̨�豸��ѯ���*/
            {
                gMBQueryDeviceCountBuf=0;
                nCurrentLoopFlag=0;
                gPointTablePossessFlag&=~(1<<2);
                do{
                    sleep(1);
                    time(&timedelay);
                    pTime=gmtime(&timedelay);
                    nMinDelay=pTime->tm_min;
                }while((abs(nMinDelay-nOldMinDelay))<5);
                gPointTablePossessFlag|=(1<<2);
                ReloadAlarmFile();
                    //}while(nMinDelay==nOldMinDelay);
                nOldMinDelay = nMinDelay;
                continue;
            }
            gTypeGroupPointMB=gTypeHead;
            while(gTypeGroupPointMB!=NULL)
            {
                //DbgPrintf("Group Type ID = %d Device Type ID = %d\r\n",gTypeGroupPointMB->nTypeID,gDeviceInfo[gMBQueryDeviceCountBuf].nType);

                //LogApartWriteLog(TEXT,"Group Type ID = %d Device Type ID = %d\r\n",gTypeGroupPointMB->nTypeID,gDeviceInfo[gMBQueryDeviceCountBuf].nType);

                if(gTypeGroupPointMB->nTypeID != gDeviceInfo[gMBQueryDeviceCountBuf].nType)//*�����������豸������Ͳ�һ��*/
                    gTypeGroupPointMB = gTypeGroupPointMB->pNext;
                else
                    break;
            }
            /*δ�ҵ��豸��Ӧ�ĵ��*/
            if(gTypeGroupPointMB==NULL)
            {
                if(gMBQueryDeviceCountBuf<40)
                {
                    for(i=(gMBQueryDeviceCountBuf+1);i<MAXDEVICE;i++)
                    {
                        if(gDeviceInfo[i].nInUse==1)
                        {
                            break;
                        }
                    }
                    gMBQueryDeviceCountBuf=i-1;
                }
                continue;
            }
            gTypeParamPointMB=gTypeGroupPointMB->pParamNext;/*��ȡ���ĵ�һ���źŵ�*/
            /*����Ǳ�����ѯ��һ̨�豸��ʱ��,��¼ʱ��*/
            if((gMBQueryDeviceCountBuf==1)&&(nCurrentLoopFlag==0))
            {
                time(&timep);
                pTime=gmtime(&timep);
                //*���δ�򿪹��ļ�*/
               if(nPointRecordFd==0)
                {
                    nDay = pTime->tm_mday;
                    AddPointRecordFile(0);
                }
                  //���Ϊ�µ�һ�����δ�򿪹��ļ�
                if(pTime->tm_mday!=nDay)
                {
                    nDay = pTime->tm_mday;
                    if(nPointRecordFd!=0)//*�Ѵ��ļ���,�ر��ļ�
                        fclose((void *)nPointRecordFd);
                    AddPointRecordFile(1);
                }
                AddPointRecordTime();
            }
            nCurrentLoopFlag=1;
        }
        /*���������豸���Ӷ˿�*/
        pthread_mutex_lock(&Uartsem);
        do
        {
            if(gDeviceInfo[gMBQueryDeviceCountBuf].nDownlinkPort!=1)
            {
                nFPGAValue = 0;
            }
            else
            {
                nFPGAValue = 1;
            }
            FpgaWrite(3, nFPGAValue);
        }while(FpgaRead(0x03)!=nFPGAValue);
        /*���������豸������*/
        if(gDeviceInfo[gMBQueryDeviceCountBuf].nRate!=nPreRate)
        {
            closeDev(nUartFd);
            usleep(500);
            nUartFd = UartOper(2,gDeviceInfo[gMBQueryDeviceCountBuf].nRate);
            nPreRate = gDeviceInfo[gMBQueryDeviceCountBuf].nRate;
        }
        /*��ȡ���β�ѯ���׵�ַ*/
        pTypeParaTemp=gTypeParamPointMB;
        aSendBuf[0]=gMBQueryDeviceCountBuf;
        if(gTypeParamPointMB == NULL)
        {
            pthread_mutex_unlock(&Uartsem);
            gTypeGroupPointMB=NULL;
            gTypeParamPointMB=NULL;
            continue;
        }
        nMBAddr=gTypeParamPointMB->nMBAddr;
        do{
            if(pTypeParaTemp==NULL)
                break;
            /*�Ĵ������ȳ���2���Ĵ�����������Ϊң�ص���Ҫ�������źŵ�*/
            if(((pTypeParaTemp->nLen)>2)||(pTypeParaTemp->nType==Type_104_YK))
            {
                /*�����Ϊ������һ���źŵ�*/
                if(pTypeParaTemp->pNext!=NULL)
                {
                    if(nLen==0)//*���Ϊ���β�ѯ�ĵ�һ���źŵ�*/
                    {
                        pTypeParaTemp=pTypeParaTemp->pNext;
                        nMBAddr=pTypeParaTemp->nMBAddr;
                        continue;
                    }
                    else//*����ǵ�һ����,�򱾴β�ѯ�ķ��ͱ����齨����*/
                        break;
                }
                else//*Ϊ�������һ���źŵ�*/
                {
                    if(nLen==0)/*Ϊ���β�ѯ�ĵ�һ���źŵ�*/
                    {
                        gTypeGroupPointMB=NULL;
                        gTypeParamPointMB=NULL;
                        if((gDeviceAlarmBuf[(gMBQueryDeviceCountBuf-1)/8]>>((gMBQueryDeviceCountBuf-1)%8))&0x01)
                            AlarmDelete(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);
                    }
                    break;
                }
            }
            /*���ݳ��ȳ���*/
            if((pTypeParaTemp->nLen*2+nLen*2+4)>256)
                break;
            /*���㱾�β�ѯ�ļĴ�������*/
            nLen=nLen+pTypeParaTemp->nLen;
            if(pTypeParaTemp->pNext!=NULL)
            {
                if(((pTypeParaTemp->nMBAddr+pTypeParaTemp->nLen)!=(pTypeParaTemp->pNext->nMBAddr))
                   ||((pTypeParaTemp->pNext->nLen)>2)||((pTypeParaTemp->pNext->nType)==4))
                {
                    pTypeParaTemp=pTypeParaTemp->pNext;
                    break;
                }
            }
            pTypeParaTemp=pTypeParaTemp->pNext;
        }while((nLen*2+4)<256);
        if(nLen==0)
        {
            pthread_mutex_unlock(&Uartsem);
            continue;
        }
        //printf("4!!\n\n\n");
        if(gTypeGroupPointMB->nProtocalTypeID==Type_Huawei_Modbus)//*��Ϊmodbus*/
            aSendBuf[1]=0x03;
        else if(gTypeGroupPointMB->nProtocalTypeID==Type_Standard_Modbus)//*��׼modbus*/
        {
            aSendBuf[1]=0x04;
            //*4XXXX��ͷ�ĵ�ַ,�����ʶʹ��3*/
            if((nMBAddr/10000)==4)
            {
                aSendBuf[1]=0x03;
                nMBAddr = nMBAddr-40000;
            }
            else if((nMBAddr/10000)==3)//*43XXXX��ͷ�ĵ�ַ,�����ʶʹ��4*/
            {
                aSendBuf[1]=0x04;
                nMBAddr = nMBAddr-30000;
            }
        }
        aSendBuf[2]=nMBAddr>>8;
        aSendBuf[3]=nMBAddr;
        aSendBuf[4]=nLen/256;
        aSendBuf[5]=nLen%256;
        //aSendBuf[0]=aSendBuf[0]+40*(gMainDeviceID-1);
        //DEBUG("[80-Local]MainDeviceID is %d \r\n",gMainDeviceID);
        /*����CRC*/
        nCRC = CRC16(aSendBuf,6);
        //nCRC = 0;
        if(gDeviceInfo[gMBQueryDeviceCountBuf].nEndian==1)
            memcpy((UINT8 *)&aSendBuf[6],(UINT8 *)&nCRC,2);
        else
        {
            aSendBuf[7]=(unsigned char)(nCRC>>8);
            aSendBuf[6]=(unsigned char)nCRC;
        }
        GPIOSet(2,19,1);//*����COM���ڷ���״̬*/
        DbgPrintf("COM Send:");
        for(i=0;i<8;i++)
        {
            DbgPrintf("%02X ",aSendBuf[i]);
        }
        DbgPrintf("\r\n");
        //usleep(1000);
        writeDev(nUartFd,aSendBuf,8);//*����������COM*/
        //usleep(100);
        GPIOSet(2,19,0);//*�����·Ϊ����ģʽ*/
        nRecvLen=readDev(nUartFd,gMBRecvBuf);//*��COM��������*/
        usleep(1000);
        pthread_mutex_unlock(&Uartsem);
        //�жϲ�������Ƿ����Ҫ��
        if(/*((gMBRecvBuf[1]&0x80)!=0)||*/((gMBRecvBuf[1] != 0x83) && (gMBRecvBuf[1] != 0x84)
           && (gMBRecvBuf[1]!=3)&&(gMBRecvBuf[1]!=4))||(gMBRecvBuf[0]!=gMBQueryDeviceCountBuf))
        {
            nRecvLen = -1;
        }
        DbgPrintf("nRecvLen = %d\n\n\n",nRecvLen);
        /*δ���յ�����*/
        if(nRecvLen==-1)
        {
            UINT8 nErrorCount;
            nErrorCount = AlarmAdd(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);//*����澯�б�*/
            DbgPrintf("Alarm----------Downlink Device No.%02d Error Count = %d\r\n",gMBQueryDeviceCountBuf,nErrorCount);
            /*����쳣�����ﵽ��������*/
            if(nErrorCount>ALARM_SCANF_MAXNUMBER)
            {
                gTypeGroupPointMB=NULL;
                gTypeParamPointMB=NULL;
            }
            usleep(1000);
            continue;
        }
        else if(nRecvLen>=5)
        {
            if(DeviceCountBuf!=gMBQueryDeviceCountBuf)
            {
                AlarmDelete(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);
                DeviceCountBuf=gMBQueryDeviceCountBuf;
            }
        }

        /*��������,��65534��֮��*/
        if(gTypeParamPointMB->nMBAddr!=65534)
        {
            /*�豸��������*/
            if(((gMBRecvBuf[1]&0x80)!=0)||((gMBRecvBuf[1]!=3)&&(gMBRecvBuf[1]!=4))||(gMBRecvBuf[0]!=gMBQueryDeviceCountBuf))
            {
                //printf("485 received data error!\n\n\n\n\n");
                /*����Ѿ�Ϊ���һ���źŵ�*/
                if(gTypeParamPointMB->pNext==NULL)
                {
                    if((gDeviceAlarmBuf[(gMBQueryDeviceCountBuf-1)/8]>>((gMBQueryDeviceCountBuf-1)%8))&0x01)
                        AlarmDelete(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);
                    gTypeGroupPointMB=NULL;
                    gTypeParamPointMB=NULL;
                }
                else
                {
                    gTypeParamPointMB=gTypeParamPointMB->pNext;
                }
                continue;
            }
            /*�����ݽ���CRCУ��*/
            nCRC = CRC16(gMBRecvBuf,nRecvLen-2);
            if(((nCRC&0x00FF)!=gMBRecvBuf[nRecvLen-2]) || (((nCRC>>8)&0x00FF)!=gMBRecvBuf[nRecvLen-1]))
            {
                crc_error_count++;
                //printf("485 received data crc correction error!\n\n\n\n");
                if(gTypeParamPointMB->pNext==NULL)
                {
                    if((gDeviceAlarmBuf[(gMBQueryDeviceCountBuf-1)/8]>>((gMBQueryDeviceCountBuf-1)%8))&0x01)
                        AlarmDelete(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);
                }
                else
                {
                    gTypeParamPointMB=gTypeParamPointMB->pNext;
                }
                if(crc_error_count > 2)
                {
                    crc_error_count = 0;
                    gTypeGroupPointMB=NULL;
                    gTypeParamPointMB=NULL;
                }
                continue;
            }
        }
        if((gTypeParamPointMB->nMBAddr == 65534 ) && ((gMBRecvBuf[1] & 0x80) != 0) && (nRecvLen == 5))
        {
            nLen=2;
        }
        else
            nLen=gMBRecvBuf[2];
        /*���ݴ���*/
        while((nLen/2)!=0)
        {
            UINT32 nValueTemp=0;
            UINT8 nPointCount=0,nAlarmPointCount=0;

            //printf("Len = %d   len=%d addr=0x%X Type=%d\r\n",nRecvLen,nLen,gTypeParamPointMB->nMBAddr,gTypeParamPointMB->nType);
            if(((gTypeParamPointMB->nLen)>2)||(gTypeParamPointMB->nType==Type_104_YK))/*/�źŵ�Ĵ�������2�����Ȼ����źŵ�����Ϊң��*/
            {
                gTypeParamPointMB=gTypeParamPointMB->pNext;
                continue;
            }
            nLen=nLen-gTypeParamPointMB->nLen*2;
            /*С��ģʽ����*/
            if(gDeviceInfo[gMBQueryDeviceCountBuf].nEndian==1)
            {
                for(i=0;i<gTypeParamPointMB->nLen;i++)
                {
                    nValueTemp=nValueTemp+(gMBRecvBuf[3+nCount]<<(8*((i+1)*2-1)))
                                         +(gMBRecvBuf[3+nCount+1]<<(8*((i+1)*2-1-1)));
                    nCount=nCount+2;
                }
            }
            else/*���ģʽ����*/
            {
                for(i=0;i<gTypeParamPointMB->nLen;i++)
                {
                    nValueTemp=nValueTemp+(gMBRecvBuf[3+nCount]<<(8*((gTypeParamPointMB->nLen-i)*2-1)))
                                         +(gMBRecvBuf[3+nCount+1]<<(8*((gTypeParamPointMB->nLen-i)*2-1-1)));
                    /*nValueTemp=nValueTemp+(gMBRecvBuf[3+nCount]<<(8*((i+1)*2-1-1)))
                                         +(gMBRecvBuf[3+nCount+1]<<(8*((i+1)*2-1)));*/
                    nCount=nCount+2;
                }
            }
            /*ͨ���豸����104��ʼ��ַ��������ƫ�������㱾�źŵ���104����е�λ��*/
            pTypeParaTemp=gTypeGroupPointMB->pParamNext;
            while(pTypeParaTemp!=gTypeParamPointMB)
            {
                if((pTypeParaTemp->nType==gTypeParamPointMB->nType)
                    ||((gTypeParamPointMB->nType==9)&&(pTypeParaTemp->nType==1)))
                {
                    nAlarmPointCount++;
                    nPointCount++;
                }
                pTypeParaTemp=pTypeParaTemp->pNext;
            }
            switch(gTypeParamPointMB->nType)
            {
                usleep(200);
                case Type_104_YX:
                {
                    UINT32 nDataAddr;
                    if(nValueTemp==0xFFFF)
                    {
                        DbgPrintf("[Alarm]0xFFFF Do Nothing\r\n");
                        break;
                    }
                    nValueTemp = nValueTemp&0x0001;
                    if(gTypeParamPointMB->nMBAddr==65534)/*����źŵ�Ϊ65534����1*/
                    {
                        nValueTemp = 1;
                    }
                    nDataAddr=gDeviceInfo[gMBQueryDeviceCountBuf].nYXAddr+nPointCount;//*����ó�104��ַ*/
                    if(nValueTemp!=aPointBuf[nDataAddr].nPreValue)/*������ϴ�ֵ��һ���򱣴���ͻ��������*/
                    {
                        nRecordChangeFlag=1;
                        //printf("YX add MB addr =%d    prevalue = %d  value = %d\r\n",gTypeParamPointMB->nMBAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                        AddChangeBuf(2,gTypeParamPointMB->nDataType,nDataAddr,nValueTemp);
                    }
                    aPointBuf[nDataAddr].nPreValue=nValueTemp;
                    aPointBuf[nDataAddr].nLen=1;
                    aPointBuf[nDataAddr].nValue=nValueTemp;
                    aPointBuf[nDataAddr].nStatus=0;
                    if((NumLocalthread==12)||(nRecordChangeFlag==1))
                    {
                        //printf("XXX ADDRECORD %d XXX\r\n",NumLocalthread);
                        AddPointRecord(nDataAddr,gMBQueryDeviceCountBuf,gTypeParamPointMB->nMBAddr,nValueTemp,gTypeParamPointMB->nDataType);
                    }
                    else
                    {
                        //printf("XXX Reason  %d --%d --%dXXX\r\n",NumLocalthread,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                    }
                    nRecordChangeFlag=0;
                    //printf("YXXXXX  addr =   %d   value = %d\r\n",nDataAddr,nValueTemp);
                    //printf("YXXXXX  MBaddr = %d     104addr =   %d   value = %d\r\n",gTypeParamPointMB->nMBAddr,nDataAddr,nValueTemp);
                    break;
                }
                case Type_104_YC:
                {
                    UINT32 nDataAddr;
                    INT32 nDataTemp;
                    if(nValueTemp==0xFFFF)
                    {
                        DbgPrintf("[Alarm]0xFFFF Do Nothing\r\n");
                        break;
                    }

                    nDataAddr=(gDeviceInfo[gMBQueryDeviceCountBuf].nYCAddr+nPointCount);
                    /*���з����������������⴦��*/
                    /*if(gTypeParamPointMB->nDataType==Type_Data_FLOAT)
                        {
                            nDataTemp=nValueTemp;
                        }
                    else
                        {
                            uIE.nIndex=nValueTemp;
                        }*/
                    if(gTypeParamPointMB->nDataType==Type_Data_INT16)
                    {
                        if((nValueTemp&0x8000)!=0)
                        {
                            nDataTemp = nValueTemp - pow(2,16);
                            uIE.nIndex=nDataTemp;
                        }
                        else
                            uIE.nIndex=nValueTemp;
                    }
                    else if(gTypeParamPointMB->nDataType==Type_Data_INT32)
                    {
                        if((nValueTemp&0x80000000)!=0)
                        {
                            nDataTemp = nValueTemp - pow(2,32);
                            uIE.nIndex=nDataTemp;
                        }
                        else
                            uIE.nIndex=nValueTemp;
                    }
                    else
                        {
                            uIE.nIndex=nValueTemp;
                        }
                    /*������ϴ�ֵ��һ���򱣴���ͻ��������*/
                    if(nValueTemp!=aPointBuf[nDataAddr].nPreValue)
                    {
                        nRecordChangeFlag=1;
                        if((gTypeParamPointMB->nMBAddr!=32200)&&(gTypeParamPointMB->nMBAddr!=40000))
                        {
                            //printf("YC add MB addr =%d    prevalue = %d  value = %d\r\n",gTypeParamPointMB->nMBAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                            AddChangeBuf(1,gTypeParamPointMB->nDataType,nDataAddr,nValueTemp);
                        }
                    }
                    aPointBuf[nDataAddr].nPreValue=nValueTemp;
                    aPointBuf[nDataAddr].nLen=5;
                    if(gTypeParamPointMB->nDataType == Type_Data_FLOAT)
                        aPointBuf[nDataAddr].nValue = nValueTemp;
                    else
                        memcpy((UINT8 *)&aPointBuf[nDataAddr].nValue,uIE.nChar,4);

                    aPointBuf[nDataAddr].nStatus=0;
                     if((NumLocalthread==12)||(nRecordChangeFlag==1))
                     {
                        //printf("XXX ADDRECORD %d XXX\r\n",NumLocalthread);
                        AddPointRecord(nDataAddr,gMBQueryDeviceCountBuf,gTypeParamPointMB->nMBAddr,nValueTemp,gTypeParamPointMB->nDataType);
                     }
                    else
                    {
                        //printf("XXX Reason  %d --%d --%dXXX\r\n",NumLocalthread,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                    }

                    nRecordChangeFlag=0;
                            //printf("YCCCCC  MBaddr = %d     104addr =   %d   value = %d\r\n",gTypeParamPointMB->nMBAddr,nDataAddr,nValueTemp);
                    break;
                }
                case Type_104_DD:
                {
                    UINT32 nDataAddr;
                    if(nValueTemp==0xFFFF)
                    {
                        DbgPrintf("[Alarm]0xFFFF Do Nothing\r\n");
                        break;
                    }
                    nDataAddr=gDeviceInfo[gMBQueryDeviceCountBuf].nDDAddr+nPointCount;
                    /*if(nValueTemp!=aPointBuf[nDataAddr].nPreValue)
                    {
                        //printf("YX add MB addr =%d    prevalue = %d  value = %d\r\n",gTypeParamPointMB->nMBAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                        AddChangeBuf(2,nDataAddr,nValueTemp);
                    }*/
                    if(gTypeParamPointMB->nDataType == Type_Data_FLOAT)
                    {
                        //memcpy(uIE.nChar,(UINT8 *)&nValueTemp,4);
                        //nValueTemp = (UINT32)uIE.nIndex;
                        uIE.nIndex= nValueTemp;               //2019.1.28 Andre
                    }
                    aPointBuf[nDataAddr].nPreValue=nValueTemp;
                    aPointBuf[nDataAddr].nLen=1;
                    aPointBuf[nDataAddr].nValue=nValueTemp;
                    aPointBuf[nDataAddr].nStatus=0;
                    //printf("XXX ADDRECORD XXX\r\n");
                    AddPointRecord(nDataAddr,gMBQueryDeviceCountBuf,gTypeParamPointMB->nMBAddr,nValueTemp,gTypeParamPointMB->nDataType);
                    //printf("DDDDDDD  addr =   %d   value = %d\r\n",nDataAddr,nValueTemp);
                    break;
                }
                case Type_104_Alarm:
                {
                    UINT32 nDataAddr=0;
                    UINT8 nAlarmValueCount;

                    if(nValueTemp==0xFFFF)
                    {
                        DbgPrintf("[Alarm]0xFFFF Do Nothing\r\n");
                        break;
                    }
                    nValueTemp = nValueTemp&0xFFFF;
                    nAlarmPointCount=gTypeParamPointMB->nMBAddr%10000;

                    AddPointRecord(nDataAddr,gMBQueryDeviceCountBuf,gTypeParamPointMB->nMBAddr,nValueTemp,gTypeParamPointMB->nDataType);

                    for(nAlarmValueCount=0;nAlarmValueCount<16;nAlarmValueCount++)
                    {
                        if(((nValueTemp>>nAlarmValueCount)&0x0001)==1)
                        {
                            AlarmAdd(gMBQueryDeviceCountBuf,Alarm_DownLinkDevice_Error,gTypeParamPointMB->nMBAddr,nAlarmValueCount);
                            aAlarmInfo[gMBQueryDeviceCountBuf-1][nAlarmPointCount] |= (0x0001<<nAlarmValueCount);
                        }
                        else
                        {
                            AlarmDelete(gMBQueryDeviceCountBuf,Alarm_DownLinkDevice_Error,gTypeParamPointMB->nMBAddr,nAlarmValueCount);
                        }
                        E2promWrite((UINT8 *)&aAlarmInfo[gMBQueryDeviceCountBuf-1][nAlarmPointCount],AlarmInfoAddr+((gMBQueryDeviceCountBuf-1)*20+nAlarmPointCount)*2,2);
                    }
                        //AlarmDLDevice(gMBQueryDeviceCountBuf+1,gTypeParamPointMB->nMBAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                        //printf("YX add MB addr =%d    prevalue = %d  value = %d\r\n",gTypeParamPointMB->nMBAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                        //AddChangeBuf(3,nDataAddr,aPointBuf[nDataAddr].nPreValue,nValueTemp);
                    break;
                }
            }
            /*�����Ϊ��*/
            if((gTypeParamPointMB->pNext==NULL)||(gTypeParamPointMB==NULL))
            {
                gTypeGroupPointMB=NULL;
                gTypeParamPointMB=NULL;
                /*���յ�����,�����Ƿ������·�澯,�������ָ�*/
                if((gDeviceAlarmBuf[(gMBQueryDeviceCountBuf-1)/8]>>((gMBQueryDeviceCountBuf-1)%8))&0x01)
                    AlarmDelete(gMBQueryDeviceCountBuf,Alarm_Link_Error,0,0);
                break;
            }
            /*ָ����һ����*/
            else
                gTypeParamPointMB=gTypeParamPointMB->pNext;
        }
        usleep(1000);
    }
}
/*****************************************************************************
* Description:      �߳�-��������
* Parameters:
* Returns:
*****************************************************************************/
void *SouthUpdatethread()
{
    DbgPrintf("THREAD-----SouthUpdatethread Init OK!\r\n");


    while(1)
    {
		while(gDt1000UpdataFlag==0)
		{
            sleep(1);
			continue;
		}
       if(gDt1000UpdataFlag==1)
       {
		   DbgPrintf("SOUTH UPDATA START!!!\n");

		   SouthBroadcastUpdata();
		   sleep(30);
		   SouthDiscovery();//�Է��ֲ�ѯ���жϰ汾���Ƿ�����
		   SouthSingleUpdata();//ִ�е�������
		   sleep(30);
		   gDt1000UpdataFlag=0;
		   DbgPrintf("SOUTH UPDATA END!!!\n");
	   }

       sleep(1);
    }
}

/****************************************************************************
* Description��SouthBroadcastUpdata()
* Function��SouthBroadcastUpdata
* Parameters��
****************************************************************************/
void SouthBroadcastUpdata(void)
{
	UINT8  uTmpData[256];
	UINT16 check_crc;
	UINT16 uTmpCrc;
	UINT16 s_uSendseq;//s_uSendAll=0; // �������
	UINT8 uReadDataLen=0;   //��ȡ���ݳ���
	UINT8 uFrameLen = 240;
	UINT8 uLastFrameLen;
	union U32_F_Char_Convert uTemp;
	UINT8 aRecvBuf[256];

	//====================================================================================
	//�ȹ㲥�����ļ���Ϣ
	//====================================================================================
	DbgPrintf("UPDATA BROST MESSAGE!!!\n");

	memset(uTmpData,0,256);

	uTmpData[0] = 0xFF;
	uTmpData[1] = 0x1C;
	uTmpData[2] = 0x01; 	 //�ӹ���������0x01
	uTmpData[3] = 0x16; 	 //�ļ���Ϣ����

	memcpy(&uTmpData[4],gDt1000Update.vertion,17);  //������ư汾��

	uTemp.u = gDt1000Update.nDataLen;
	uTmpData[21] = uTemp.c[3];
	uTmpData[22] = uTemp.c[2];
	uTmpData[23] = uTemp.c[1];
	uTmpData[24] = uTemp.c[0];

	uTmpData[25] = uFrameLen; //ÿ֡���ݳ���

	uTmpCrc = CRC16(uTmpData,26);
	uTmpData[26] = uTmpCrc&0XFF;
	uTmpData[27] = uTmpCrc>>8;

	DbgPrintf("Upgrade Vertion��%17s File Len��%d\n",&uTmpData[4],gDt1000Update.nDataLen);
	SouthCmdTask(uTmpData,28,aRecvBuf,0);
	SouthCmdTask(uTmpData,28,aRecvBuf,1);
	sleep(1);

	//====================================================================================
	//�������ݴ���
	//====================================================================================
	DbgPrintf("UPDATA DATA MESSAGE!!!\n");
	UINT8 aFilePath[50];
	strcpy((char *)aFilePath,"/mnt/flash/OAM/DT1000.bin");
	for(s_uSendseq=0;(s_uSendseq*uFrameLen) < gDt1000Update.nDataLen;s_uSendseq++)
	{
		memset(uTmpData,0,256);

		uTmpData[0] = 0xFF;
		uTmpData[1] = 0x2C;
		uTmpData[2] = 0x01; 	 //�ӹ���������0x01
		uTmpData[3] = s_uSendseq>>8;
		uTmpData[4] = s_uSendseq&0XFF;

		uReadDataLen=GetFileData((char *)aFilePath, &uTmpData[6], s_uSendseq, uFrameLen);  //���ļ���ȡ������֡����֡�ֽ�

		if((gDt1000Update.nDataLen-s_uSendseq*uFrameLen)<uFrameLen)
		{
			uReadDataLen = gDt1000Update.nDataLen-s_uSendseq*uFrameLen;  //���һ֡�ֽڳ��Ȼ�ȡ
		}

        if(uReadDataLen>0)
        {
			uTmpData[5] = uReadDataLen;  //���ݳ���
			check_crc = CalculateCRC(&uTmpData[6],uReadDataLen);
			uTmpCrc = CRC16(uTmpData,uReadDataLen+6);
			uTmpData[uReadDataLen+6]=uTmpCrc&0XFF;
			uTmpData[uReadDataLen+7]=uTmpCrc>>8;
			SouthCmdTask(uTmpData,uReadDataLen+8,aRecvBuf,0);
			SouthCmdTask(uTmpData,uReadDataLen+8,aRecvBuf,1);
			DbgPrintf("\r\nSouth Upgrade %d Frame  DataLen:%d\r\n",s_uSendseq,uReadDataLen);
		}
	}

	//====================================================================================
	//�㲥����������Ϣ!!!
	//====================================================================================
	memset(uTmpData,0,256);

	uTmpData[0]=0xFF;
	uTmpData[1]=0x3C;
	uTmpData[2]=0x01;  //�ӹ���������0x01
	uTmpData[3]=0x04;  //����
	uTmpData[4]=s_uSendseq>>8;
	uTmpData[5]=s_uSendseq&0XFF;	//����֡����
	uTmpData[6]=check_crc>>8;
	uTmpData[7]=check_crc&0XFF;
	uTmpCrc = CRC16(uTmpData,8);
	uTmpData[8]=uTmpCrc&0XFF;
	uTmpData[9]=uTmpCrc>>8;

	//UartWrite(SLAVE_UART_USE,uTmpData,10);
	SouthCmdTask(uTmpData,10,aRecvBuf,0);
    SouthCmdTask(uTmpData,10,aRecvBuf,1);
	DbgPrintf("Broadcast Upgrade End!!!\n");
	//====================================================================================
}
/****************************************************************************
* Description��SouthDiscovery()
* Function��SouthDiscovery
* Parameters��
****************************************************************************/

void SouthDiscovery(void)
{
	UINT8 nConnectStatus=0,nNewDeviceFlag=0;
	sDeviceInfo aDeviceInfoBuf[MAXDEVICE];
	UINT8 uDeviceCharacter1[100];//�豸ESN��+�̼���
	UINT8 uDeviceCharacter2[100];//�豸ESN��+�̼���
	UINT8 uDownLinkDeviceLen=0;


	memset(uDeviceCharacter1,0,sizeof(uDeviceCharacter1));
	memset(uDeviceCharacter2,0,sizeof(uDeviceCharacter2));


	/** ɨ���豸֮ǰ�ж��Ƿ��Ѿ��ɹ����ӵ�ƽ̨ */
	if(((gSocketMode == SOCKETMODE_LAN) && (g_nRemotesockfd > 0))
		   ||((gSocketMode == SOCKETMODE_3G) && (gModuleChannel0InitFlag == MODEMFLAG_CONNECT_SERVER)))
	{
		UINT8 i;
		UINT8 aDownLinkDevice[MAXDEVICE][256];
		UINT8 nExistsDevice=0,nErrorCount=0,nAddDeviceCount=1;

		/****PLC Detect Start 2017.12.29 lj*****/
		// FPGA Ϊ 1 ��ӦCOM1   ��1 ��ӦCOM2
		//DEBUG("[PLC]Start PLC Device Detect \r\n");

		//PlcScanDev(0,uDeviceCharacter1);//COM2
		//PlcScanDev(1,uDeviceCharacter2);//COM1
	   // DEBUG("[PLC]End PLC Device Detect \r\n");
		/****PLC Detect Endt 2017.12.29 lj*****/

		memset(aDeviceInfoBuf,0,sizeof(aDeviceInfoBuf));
		memset(aDownLinkDevice,0,sizeof(aDownLinkDevice));
		nConnectStatus=1;
		nNewDeviceFlag=0;
		for(i=1;i<MAXDEVICE;i++)
		{
			UINT8 nComBegin,nComEnd,j;
			int nRecvLen=0;

			//printf("\r\n[DeviceScanf]nNewDeviceFlag=%d,nExistsDevice=%d,gConnectDeviceMaxNum=%d\r\n",nNewDeviceFlag,nExistsDevice,gConnectDeviceMaxNum);
			if((nNewDeviceFlag+nExistsDevice)>=gConnectDeviceMaxNum)
				break;
			if(gDeviceInfo[i].nInUse)
			{
				memcpy((UINT8 *)&aDeviceInfoBuf[i],(UINT8 *)&gDeviceInfo[i],sizeof(sDeviceInfo));
				nComBegin = 2-aDeviceInfoBuf[i].nDownlinkPort;
				nComEnd = 2-aDeviceInfoBuf[i].nDownlinkPort;
				nExistsDevice++;
				for(j=nComBegin;j<=nComEnd;j++)
				{
					UINT8 aSendBuf[8]={0x00,0x3B,0x03,0x0E,0x03,0x88,0x00,0x00};//DT1000 Function code is 3b
					UINT16 nCRC,nFPGAValue,k;
					UINT8 aEsnTemp[25],aRecvBuf[255];
					UINT8 *pEsnPoint,*pEsnEnd;
					UINT8 *pSoftVersion,*pSoftVersionEnd;

					nRecvLen = -1;
					/** �л���COM1�������� */
					pthread_mutex_lock(&Uartsem);
					do
					{
						nFPGAValue = j;
						FpgaWrite(3,nFPGAValue);
					}while((FpgaRead(0x03))!=nFPGAValue);

					aSendBuf[0] = i; /** ������ַ */
					//aSendBuf[0]=aSendBuf[0]+40*(gMainDeviceID-1);
					//DEBUG("[80-2B]MainDeviceID is %d \r\n",gMainDeviceID);
					nCRC=CRC16(aSendBuf,6);
					aSendBuf[6] = (UINT8)nCRC;
					aSendBuf[7] = (UINT8)(nCRC>>8);

					memset(aRecvBuf,0,sizeof(aRecvBuf));
					GPIOSet(2,19,1);
					usleep(1000);
					DbgPrintf("COM.%d Send:",2-j);
					for(k=0;k<8;k++)
						DbgPrintf("%02X ",aSendBuf[k]);
					DbgPrintf("\r\n");
					writeDev(nUartFd,aSendBuf,8);
					//usleep(100);
					GPIOSet(2,19,0);
					nRecvLen=readDevEsn(nUartFd,aRecvBuf);

					usleep(1000);
					pthread_mutex_unlock(&Uartsem);
					if((nRecvLen==-1)||(nRecvLen<20)
					   ||(aRecvBuf[0]!=i))
					{
						usleep(10);
						nRecvLen = -1;
						continue;
					}
					nCRC = CRC16(aRecvBuf,nRecvLen-2);
					if(((nCRC>>8)!=aRecvBuf[nRecvLen-1])||((nCRC&0xFF)!=aRecvBuf[nRecvLen-2]))
					{
						usleep(10);
						nRecvLen = -1;
						continue;
					}
					aDeviceInfoBuf[i].nDownlinkPort=2-j;
					memset((UINT8 *)&aDownLinkDevice[i],0,256);
				   /* if(aRecvBuf[6]==0x87)
					{
						memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[13],aRecvBuf[12]);
					}
					else
					{
						memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[10],aRecvBuf[9]);
						uDownLinkDeviceLen=aRecvBuf[9];
					}*/

					memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[11],aRecvBuf[10]);
					uDownLinkDeviceLen=aRecvBuf[10];   //discory len

					aDeviceInfoBuf[i].nInUse = _YES;
					aDeviceInfoBuf[i].nProtocolType = PTYPE_HUAWEI_MODBUS;
					aDeviceInfoBuf[i].nEndian = _BIG_ENDIAN;
					aDeviceInfoBuf[i].nRate = 9600;
					memset(aEsnTemp,0,sizeof(aEsnTemp));
					pEsnPoint = (void *)strstr((void *)&aDownLinkDevice[i],"4=");
					pEsnPoint += strlen("4=");
					pEsnEnd = (void *)strstr((void *)pEsnPoint,";");
					memset(aDeviceInfoBuf[i].aESN,0,sizeof(aDeviceInfoBuf[i].aESN));
					if(pEsnEnd - pEsnPoint>20)
						memcpy(aDeviceInfoBuf[i].aESN, pEsnPoint,20);
					else
						memcpy(aDeviceInfoBuf[i].aESN, pEsnPoint,pEsnEnd - pEsnPoint);
					DbgPrintf("[ESN]=%s   ",aDeviceInfoBuf[i].aESN);
					pSoftVersion = (void *)strstr((void *)&aDownLinkDevice[i],"2=");
					pSoftVersion += strlen("2=");
					pSoftVersionEnd = (void *)strstr((void *)pSoftVersion,";");
					memset(aDeviceInfoBuf[i].aSofeVersion,0,sizeof(aDeviceInfoBuf[i].aSofeVersion));
					if(pSoftVersionEnd - pSoftVersion>20)
						memcpy(aDeviceInfoBuf[i].aSofeVersion, pSoftVersion,20);
					else
						memcpy(aDeviceInfoBuf[i].aSofeVersion, pSoftVersion,pSoftVersionEnd - pSoftVersion);
					DbgPrintf("[SOFTVERSION]SoftVersion=%s\r\n",aDeviceInfoBuf[i].aSofeVersion);
					break;
				}

				if((nRecvLen==-1)||(nRecvLen<20))
				{
					usleep(10);
					continue;
				}
				if(gDeviceInfo[i].nInUse==1)
				{
					if (memcmp(aDeviceInfoBuf[i].aESN, gDeviceInfo[i].aESN,20) != 0)
					{
						DbgPrintf("Old ESN: %s\r\n", gDeviceInfo[i].aESN);
						DbgPrintf("New ESN: %s\r\n", aDeviceInfoBuf[i].aESN);
						SendFrameToPlatform(i,DEVICE_MODIFY_CMD,S_R_Set,aDeviceInfoBuf[i].aESN,sizeof(aDeviceInfoBuf[i].aESN));
						SendFrameToThirdPartyPlatform(i,DEVICE_MODIFY_CMD,S_R_Set,aDeviceInfoBuf[i].aESN,sizeof(aDeviceInfoBuf[i].aESN),1);
					}
					if (memcmp(aDeviceInfoBuf[i].aSofeVersion, gDeviceInfo[i].aSofeVersion,20) != 0)
					{
						DbgPrintf("[SOFTVERSION]Old SoftVersion: %s\r\n", gDeviceInfo[i].aSofeVersion);
						DbgPrintf("[SOFTVERSION]New SoftVersion: %s\r\n", aDeviceInfoBuf[i].aSofeVersion);
					   // SendFrameToPlatform(i,UPLOAD_HW_DEVICE_CMD,S_R_InfoReport,(UINT8 *)&aDownLinkDevice[i],uDownLinkDeviceLen);
						SendSofeVersionToPlatform(i,UPLOAD_HW_DEVICE_CMD,S_R_InfoReport,(UINT8 *)&aDownLinkDevice[i],uDownLinkDeviceLen);
						memcpy(gDeviceInfo[i].aSofeVersion,aDeviceInfoBuf[i].aSofeVersion,sizeof(aDeviceInfoBuf[i].aSofeVersion));
						TagBaseFileWrite();
					}

				}
				usleep(1000);
			}
		}

	}

}
/****************************************************************************
* ��    �ƣ�South_UpdataTask()
* ��    �ܣ�������������
* ��ڲ�����

* ���ڲ�������
* ��    ��: ��
****************************************************************************/
void SouthSingleUpdata(void)
{
	INT16  iResult[MAXDEVICE];			// �����ѯ�������
    UINT16 s_uSendseq; // �������
    UINT16 uCrc;
    UINT8  uReadDataLen=0;   //��ȡ���ݳ���
	UINT8  uTmpData[256];
    UINT8  i;
	UINT8  uFrameLen = 240;
	UINT8  uRecBuffer[256];

	union U32_F_Char_Convert uTemp;
	static UINT16 check_crc=0xFFFF;

	for(i=1;i<MAXDEVICE;i++)
	{
		uReadDataLen=0;
    	if(gDeviceInfo[i].nInUse)
		{
		    if(0!=memcmp(gDeviceInfo[i].aSofeVersion,gDt1000Update.vertion,17))
		    {
				DbgPrintf("Sigle Upgrade Device Addr��%d\n",i);
				memset(uTmpData,0,256);
				uTmpData[0] = i;
				uTmpData[1] = 0x1C;
				uTmpData[2] = 0x01;
				uTmpData[3] = 0x16;
				memcpy(&uTmpData[4],&gDt1000Update.vertion,17);
			    uTemp.u = gDt1000Update.nDataLen;
		        uTmpData[21] = uTemp.c[3];
		        uTmpData[22] = uTemp.c[2];
		        uTmpData[23] = uTemp.c[1];
		        uTmpData[24] = uTemp.c[0];
				uTmpData[25] = uFrameLen;  //֡����
			    uCrc = CRC16(uTmpData,26);
				uTmpData[26] = uCrc&0XFF;
				uTmpData[27] = uCrc>>8;

				memset(uRecBuffer,0,256);
				SouthCmdTask(uTmpData,28,uRecBuffer,gDeviceInfo[i].nDownlinkPort);
		        sleep(3);

				UINT8 aFilePath[50];
		        strcpy((char *)aFilePath,"/mnt/flash/OAM/DT1000.bin");
				for(s_uSendseq=0;(s_uSendseq*uFrameLen) < gDt1000Update.nDataLen;s_uSendseq++)
				{
					memset(uTmpData,0,256);

					uTmpData[0] = i;
					uTmpData[1] = 0x2C;
					uTmpData[2] = 0x01; 	 //�ӹ���������0x01
					uTmpData[3] = s_uSendseq>>8;
					uTmpData[4] = s_uSendseq&0XFF;

					uReadDataLen=GetFileData((char *)aFilePath, &uTmpData[6], s_uSendseq, uFrameLen);  //���ļ���ȡ������֡����֡�ֽ�

					if((gDt1000Update.nDataLen-s_uSendseq*uFrameLen)<uFrameLen)
					{
						uReadDataLen = gDt1000Update.nDataLen-s_uSendseq*uFrameLen;  //���һ֡�ֽڳ��Ȼ�ȡ
					}

			        if(uReadDataLen>0)
			        {
						uTmpData[5] = uReadDataLen;  //���ݳ���
						check_crc = CalculateCRC(&uTmpData[6],uReadDataLen);
						uCrc = CRC16(uTmpData,uReadDataLen+6);
						uTmpData[uReadDataLen+6]=uCrc&0XFF;
						uTmpData[uReadDataLen+7]=uCrc>>8;

						if(SouthCmdTask(uTmpData,uReadDataLen+8,uRecBuffer,gDeviceInfo[i].nDownlinkPort))
						{
						    printf("sigle upgrade error\r\n");
						    return;
						}

						DbgPrintf("\r\nSigle Upgrade %d Frame  DataLen:%d\r\n",s_uSendseq,uReadDataLen);
					}
					//sleep(1);
				}

				sleep(1);
				memset(uTmpData,0,256);
				uTmpData[0] = i;
		        uTmpData[1] = 0x3C;
		        uTmpData[2]=0x01;  //�ӹ���������0x01
		        uTmpData[3]=0x04;  //����
				uTmpData[4] = s_uSendseq>>8;
		        uTmpData[5] = s_uSendseq&0XFF;
			    uTmpData[6] = check_crc>>8;
		        uTmpData[7] = check_crc&0XFF;
			    uCrc = CRC16(uTmpData,8);
		        uTmpData[8] = uCrc&0xFF;
		        uTmpData[9] = uCrc>>8;
				check_crc=0xFFFF;
				memset(uRecBuffer,0,256);
				SouthCmdTask(uTmpData,10,uRecBuffer,gDeviceInfo[i].nDownlinkPort);
				DbgPrintf("\nSigle Upgrade End!!!\r\n");
				s_uSendseq=0;
		    }
		}
	}
}
/*****************************************************************************
* Description:      exchange ascii to int
* Parameters:        nValue:ascii code
* Returns:           integer value
*****************************************************************************/
UINT8 HsCharToInt(UINT8 nValue)
{
    if((nValue>=0x30)&&(nValue<=0x39))//0..9
        return nValue-0x30;
    else if((nValue>=0x41)&&(nValue<=0x46))//A..F
        return nValue-0x41+10;
    else if((nValue>=0x61)&&(nValue<=0x66))//a..f
        return nValue-0x61+10;
    else
        return 0;
}

void SaveThreadInfo(UINT8 *pThreadName)
{
   /* FILE *fThread;
    UINT8 aBuf[100];

    pthread_mutex_lock(&threadFileSem);
    fThread = fopen("/mnt/flash/OAM/thread.txt","a");
    memset(aBuf,0,sizeof(aBuf));
    sprintf(aBuf,"PID=%04d THREAD=",getpid());
    strcat(aBuf,pThreadName);
    strcat(aBuf,"\r\n");
    fwrite(aBuf,26,1,fThread);
    fclose(fThread);
    pthread_mutex_unlock(&threadFileSem);*/
}

/*****************************************************************************
* Description:      Get south device boudrate and endian
* Parameters:
* Returns:
*****************************************************************************/
void DLinkParamGet()
{
    UINT8 i;

    for(i=1;i<=80;i++)
    {
        if(gDeviceInfo[i].nInUse==1)
        {
            E2promRead((UINT8 *)&gDeviceInfo[i].nRate,DeviceTypeInfoAddr+(i-1)*5,4);
            if(gDeviceInfo[i].nRate>115200)
            {
                gDeviceInfo[i].nRate=9600;
            }
            E2promRead((UINT8 *)&gDeviceInfo[i].nEndian,DeviceTypeInfoAddr+(i-1)*5+4,1);
            if(gDeviceInfo[i].nEndian>1)
            {
                gDeviceInfo[i].nEndian=0;
            }
            DbgPrintf("Device No.%02d ESN=%s Table=%03d Type=%d Port=%d Rate=%05d Endian=%d YX=0x%04X YC=0x%04X YK=0x%04X SD=0x%04X DD=0x%04X\r\n",
                   i,gDeviceInfo[i].aESN,
                   gDeviceInfo[i].nPointTableNo,
                   gDeviceInfo[i].nType,
                   gDeviceInfo[i].nDownlinkPort,
                   gDeviceInfo[i].nRate,
                   gDeviceInfo[i].nEndian,
                   gDeviceInfo[i].nYXAddr,
                   gDeviceInfo[i].nYCAddr,
                   gDeviceInfo[i].nYKAddr,
                   gDeviceInfo[i].nSDAddr,
                   gDeviceInfo[i].nDDAddr);
        }
    }
}

/*****************************************************************************
* Description:      Create update file temp
* Parameters:
* Returns:
*****************************************************************************/
void UpdataFileOpen()
{
    nUpdataFd=(int)fopen(Up_Data_File_Path,"w+b");
}
/*****************************************************************************
* Description:open license file by device id
* Parameters:nDeviceID       device id
* Returns: none
* Staff&Date: Liujing 2018.5.24
*****************************************************************************/

void LicenseFileOpen(UINT8 nDeviceID)
{
    //sprintf(aBuf,"/mnt/flash/OAM/%02d.dat",nDeviceID);
    nLicenseFd=(int)fopen("/mnt/flash/OAM/license.tar.gz","w+b");
}
/*****************************************************************************
* Description:open license scanf file by device id
* Parameters:nDeviceID       device id
* Returns: none
* Staff&Date: Liujing 2018.5.24
*****************************************************************************/

void LVScanfFileOpen(UINT8 nDeviceID)
{
    UINT8 aBuf[100];

    sprintf((void *)aBuf,"/mnt/flash/OAM/IV_%02d.bin",nDeviceID);
    nIVScanfFd=(int)fopen((void *)aBuf,"w+b");
}

/*****************************************************************************
* Description:      Create import file temp
* Parameters:
* Returns:
*****************************************************************************/
void FileOpen()
{
    char cmd_line[50]={0};

    if(access(IMPORT_FOLDER_PATH, F_OK) == -1)
    {
        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "mkdir %s", IMPORT_FOLDER_PATH);
        DbgPrintf("[File Open] cmd_line = %s\n", cmd_line);
        system(cmd_line);
    }
    n_file_fd=(int)fopen(IMPORT_FILE_PATH,"w+b");
    DbgPrintf("[File Open] n_file_fd = %d\n", n_file_fd);
}

/*****************************************************************************
* Description:      save data to file
* Parameters:        aBuf:data point
                     nlen:data length
* Returns:
*****************************************************************************/
void FileWrite(int fd,UINT8 *aBuf,UINT8 nLen)
{
    fwrite(aBuf,nLen,1,(void *)fd);
}

/*****************************************************************************
* Description:      save update data to file
* Parameters:        aBuf:update data point
                     nlen:data length
* Returns:
*****************************************************************************/
void LicenseFileWrite(UINT8 *aBuf,UINT8 nLen)
{
    fwrite(aBuf,nLen,1,(void *)nLicenseFd);
}

/*****************************************************************************
* Description:      update date recv end
* Parameters:
* Returns:
*****************************************************************************/
void UpdataFileSave()
{
    fclose((void *)nUpdataFd);
    //close(nUpdataFd);
}

/*****************************************************************************
* Description:      file data recv finished
* Parameters:
* Returns:
*****************************************************************************/
void FileSave()
{
    fclose((void *)n_file_fd);
}

/*****************************************************************************
* Description:      update date recv end
* Parameters:
* Returns:
*****************************************************************************/
void LicenseFileSave()
{
    fclose((void *)nLicenseFd);
    //close(nUpdataFd);
}

/*****************************************************************************
* Description:      Thread-Get data from AP module
* Parameters:
* Returns:
*****************************************************************************/
void *APThread()
{
    UINT8 aRecvBuf[256];

    DbgPrintf("THREAD-----APThread Init OK!\r\n");

    while(1)
    {
        int nLen,i;

        memset(aRecvBuf,sizeof(aRecvBuf),0);
        nLen=readAP(nAPFd, aRecvBuf);
        if(nLen<0)
        {

        }
        else
        {
            DbgPrintf("AP recv:\r\n");
            for(i=0;i<nLen;i++)
            {
                DbgPrintf("%02X ",aRecvBuf[i]);
            }
            DbgPrintf("\r\n");
            APPackMainFunction(0,aRecvBuf,nLen);
        }
        //writeDev(nAPFd,atemp,8);
        usleep(100);
    }
}

/*****************************************************************************
* Description:      init wifi module
* Parameters:
* Returns:
*****************************************************************************/
void WifiInit(void)
{
    UINT8 aRecvTemp[50];
    UINT8 nErrorCount=0,nLen=0;;
    UINT8 aAPSSIDSetCmd[100]="AT+WAP=11BGN,PNLogger_";
    UINT8 aAPPWDSetCmd[50]="AT+WAKEY=WPA2PSK,AES,Pinnettech\r\n";

    WIFIEnable(1);
    sleep(3);
    nAPFd = UartOper(3,115200);
    do
    {
        nErrorCount++;
        if(nErrorCount>10)
        {
            DbgPrintf("AP INIT ERROR -1\r\n");
            return;
        }
        pthread_mutex_lock(&APsem);
        write(nAPFd, "+++", 3);
        pthread_mutex_unlock(&APsem);
        memset(aRecvTemp,0,sizeof(aRecvTemp));
        pthread_mutex_lock(&APsem);
        usleep(10000);
        nLen = read(nAPFd,aRecvTemp,50);
        pthread_mutex_unlock(&APsem);
    }while(strcmp((void *)aRecvTemp,"a")!=0);
    nErrorCount=0;
    do
    {
        nErrorCount++;
        if(nErrorCount>10)
        {
            DbgPrintf("AP INIT ERROR -2\r\n");
            return;
        }
        pthread_mutex_lock(&APsem);
        write(nAPFd, "a", 1);
        pthread_mutex_unlock(&APsem);
        memset(aRecvTemp,0,sizeof(aRecvTemp));
        pthread_mutex_lock(&APsem);
        usleep(1000);
        nLen = read(nAPFd,aRecvTemp,50);
        pthread_mutex_unlock(&APsem);
    }while(strncmp((void *)aRecvTemp,"+ok",3)!=0);
    if(gMainDeviceSN[0]!=0xFF)
    {
        UINT8 aEsnTemp[25];

        nErrorCount=0;
        do
        {
            nErrorCount++;
            if(nErrorCount>10)
            {
                DbgPrintf("AP INIT ERROR -3\r\n");
                return;
            }
            memset(aEsnTemp,0,sizeof(aEsnTemp));
            memcpy(aEsnTemp,gMainDeviceSN,20);
            strcat((void *)aAPSSIDSetCmd,(void *)aEsnTemp);
            strcat((void *)aAPSSIDSetCmd,",CH2\r\n");
            pthread_mutex_lock(&APsem);
            write(nAPFd, aAPSSIDSetCmd,strlen((void *)aAPSSIDSetCmd));
            pthread_mutex_unlock(&APsem);
            memset(aRecvTemp,0,sizeof(aRecvTemp));
            pthread_mutex_lock(&APsem);
            sleep(1);
            nLen = read(nAPFd,aRecvTemp,50);
            pthread_mutex_unlock(&APsem);
        }while(strstr((void *)aRecvTemp,"ok")==NULL);
    }
    nErrorCount=0;
    do
    {
        nErrorCount++;
        if(nErrorCount>10)
        {
            DbgPrintf("AP INIT ERROR -4\r\n");
            return;
        }
        pthread_mutex_lock(&APsem);
        write(nAPFd, aAPPWDSetCmd,strlen((void *)aAPPWDSetCmd));
        //pthread_mutex_unlock(&APsem);
        memset(aRecvTemp,0,sizeof(aRecvTemp));
        //pthread_mutex_lock(&APsem);
        sleep(1);
        nLen = read(nAPFd,aRecvTemp,50);
        pthread_mutex_unlock(&APsem);
    }while(strstr((void *)aRecvTemp,"ok")==NULL);
    WIFIEnable(1);
}

void *ScanfDeviceThread()
{
    UINT8 nConnectStatus=0,nNewDeviceFlag=0;
    sDeviceInfo aDeviceInfoBuf[MAXDEVICE];
    UINT8 uDeviceCharacter1[100];//�豸ESN��+�̼���
    UINT8 uDeviceCharacter2[100];//�豸ESN��+�̼���
    UINT8 uDownLinkDeviceLen=0;

    //PlcIni();
    //memset(sPlcInfoBuf,0,sizeof(sPlcInfoBuf));
    memset(uDeviceCharacter1,0,sizeof(uDeviceCharacter1));
    memset(uDeviceCharacter2,0,sizeof(uDeviceCharacter2));
    /*sPlcInfoBuf[0].nDownlinkPort=1;//249
    sPlcInfoBuf[1].nDownlinkPort=2;//250

    while(1)
        {
            if(((gSocketMode == SOCKETMODE_LAN) && (g_nRemotesockfd > 0))
               ||((gSocketMode == SOCKETMODE_3G) && (gModuleChannel0InitFlag == MODEMFLAG_CONNECT_SERVER)))
            {
                PlcAlarmEraseSend(0);
                PlcAlarmEraseSend(1);
                break;
            }
            else
                {
                    DEBUG("[PLC]NetWork is not Ready\r\n");
                    sleep(5);
                }
        }
        */
    while(1)
    {
        while(gDt1000UpdataFlag==1)/*��������ģʽ��ʱ��,�ȴ��������*/
        {
            sleep(1);
            continue;
        }

	    /** ɨ���豸֮ǰ�ж��Ƿ��Ѿ��ɹ����ӵ�ƽ̨ */
        if(((gSocketMode == SOCKETMODE_LAN) && (g_nRemotesockfd > 0))
               ||((gSocketMode == SOCKETMODE_3G) && (gModuleChannel0InitFlag == MODEMFLAG_CONNECT_SERVER)))
        {
            UINT8 i;
            UINT8 aDownLinkDevice[MAXDEVICE][256];
            UINT8 nExistsDevice=0,nErrorCount=0,nAddDeviceCount=1;

            /****PLC Detect Start 2017.12.29 lj*****/
            // FPGA Ϊ 1 ��ӦCOM1   ��1 ��ӦCOM2
            //DEBUG("[PLC]Start PLC Device Detect \r\n");

            //PlcScanDev(0,uDeviceCharacter1);//COM2
            //PlcScanDev(1,uDeviceCharacter2);//COM1
           // DEBUG("[PLC]End PLC Device Detect \r\n");
            /****PLC Detect Endt 2017.12.29 lj*****/

            memset(aDeviceInfoBuf,0,sizeof(aDeviceInfoBuf));
            memset(aDownLinkDevice,0,sizeof(aDownLinkDevice));
            nConnectStatus=1;
            nNewDeviceFlag=0;
			gUploadHWDeviceFlag = 0;
            for(i=1;i<MAXDEVICE;i++)
            {
                UINT8 nComBegin,nComEnd,j;
                int nRecvLen=0;
				if(gDt1000UpdataFlag==1)
					break;

                //printf("\r\n[DeviceScanf]nNewDeviceFlag=%d,nExistsDevice=%d,gConnectDeviceMaxNum=%d\r\n",nNewDeviceFlag,nExistsDevice,gConnectDeviceMaxNum);
                if((nNewDeviceFlag+nExistsDevice)>=gConnectDeviceMaxNum)
                    break;
                if(gDeviceInfo[i].nInUse)
                {
                    memcpy((UINT8 *)&aDeviceInfoBuf[i],(UINT8 *)&gDeviceInfo[i],sizeof(sDeviceInfo));
                    nComBegin = 2-aDeviceInfoBuf[i].nDownlinkPort;
                    nComEnd = 2-aDeviceInfoBuf[i].nDownlinkPort;
                    nExistsDevice++;
                }
                else
                {
                    nComBegin = 0;
                    nComEnd = 1;
                }
                for(j=nComBegin;j<=nComEnd;j++)
                {
                    UINT8 aSendBuf[8]={0x00,0x3B,0x03,0x0E,0x03,0x88,0x00,0x00};//DT1000 Function code is 3b
                    UINT16 nCRC,nFPGAValue,k;
                    UINT8 aEsnTemp[25],aRecvBuf[255];
                    UINT8 *pEsnPoint,*pEsnEnd;
                    UINT8 *pSoftVersion,*pSoftVersionEnd;

                    nRecvLen = -1;
                    /** �л���COM1�������� */
                    pthread_mutex_lock(&Uartsem);
                    do
                    {
                        nFPGAValue = j;
                        FpgaWrite(3,nFPGAValue);
                    }while((FpgaRead(0x03))!=nFPGAValue);

                    aSendBuf[0] = i; /** ������ַ */
                    //aSendBuf[0]=aSendBuf[0]+40*(gMainDeviceID-1);
                    //DEBUG("[80-2B]MainDeviceID is %d \r\n",gMainDeviceID);
                    nCRC=CRC16(aSendBuf,6);
                    aSendBuf[6] = (UINT8)nCRC;
                    aSendBuf[7] = (UINT8)(nCRC>>8);

                    memset(aRecvBuf,0,sizeof(aRecvBuf));
                    GPIOSet(2,19,1);
                    usleep(1000);
                    DbgPrintf("COM.%d Send:",2-j);
                    for(k=0;k<8;k++)
                        DbgPrintf("%02X ",aSendBuf[k]);
                    DbgPrintf("\r\n");
                    writeDev(nUartFd,aSendBuf,8);
                    //usleep(100);
                    GPIOSet(2,19,0);
                    nRecvLen=readDevEsn(nUartFd,aRecvBuf);

                    usleep(1000);
                    pthread_mutex_unlock(&Uartsem);
                    if((nRecvLen==-1)||(nRecvLen<20)
                       ||(aRecvBuf[0]!=i))
                    {
                        usleep(10);
                        nRecvLen = -1;
                        continue;
                    }
                    nCRC = CRC16(aRecvBuf,nRecvLen-2);
                    if(((nCRC>>8)!=aRecvBuf[nRecvLen-1])||((nCRC&0xFF)!=aRecvBuf[nRecvLen-2]))
                    {
                        usleep(10);
                        nRecvLen = -1;
                        continue;
                    }
                    aDeviceInfoBuf[i].nDownlinkPort=2-j;
                    memset((UINT8 *)&aDownLinkDevice[i],0,256);
                   /* if(aRecvBuf[6]==0x87)
                    {
                        memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[13],aRecvBuf[12]);
                    }
                    else
                    {
                        memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[10],aRecvBuf[9]);
                        uDownLinkDeviceLen=aRecvBuf[9];
                    }*/

				    memcpy((UINT8 *)&aDownLinkDevice[i],(UINT8 *)&aRecvBuf[11],aRecvBuf[10]);
					uDownLinkDeviceLen=aRecvBuf[10];   //discory len

                    aDeviceInfoBuf[i].nInUse = _YES;
                    aDeviceInfoBuf[i].nProtocolType = PTYPE_HUAWEI_MODBUS;
                    aDeviceInfoBuf[i].nEndian = _BIG_ENDIAN;
                    aDeviceInfoBuf[i].nRate = 9600;
                    memset(aEsnTemp,0,sizeof(aEsnTemp));
                    pEsnPoint = (void *)strstr((void *)&aDownLinkDevice[i],"4=");
                    pEsnPoint += strlen("4=");
                    pEsnEnd = (void *)strstr((void *)pEsnPoint,";");
                    memset(aDeviceInfoBuf[i].aESN,0,sizeof(aDeviceInfoBuf[i].aESN));
                    if(pEsnEnd - pEsnPoint>20)
                        memcpy(aDeviceInfoBuf[i].aESN, pEsnPoint,20);
                    else
                        memcpy(aDeviceInfoBuf[i].aESN, pEsnPoint,pEsnEnd - pEsnPoint);
                    DbgPrintf("[ESN]=%s   ",aDeviceInfoBuf[i].aESN);
                    pSoftVersion = (void *)strstr((void *)&aDownLinkDevice[i],"2=");
                    pSoftVersion += strlen("2=");
                    pSoftVersionEnd = (void *)strstr((void *)pSoftVersion,";");
                    memset(aDeviceInfoBuf[i].aSofeVersion,0,sizeof(aDeviceInfoBuf[i].aSofeVersion));
                    if(pSoftVersionEnd - pSoftVersion>20)
                        memcpy(aDeviceInfoBuf[i].aSofeVersion, pSoftVersion,20);
                    else
                        memcpy(aDeviceInfoBuf[i].aSofeVersion, pSoftVersion,pSoftVersionEnd - pSoftVersion);
                    DbgPrintf("[SOFTVERSION]SoftVersion=%s\r\n",aDeviceInfoBuf[i].aSofeVersion);
                    break;
                }
                if((nRecvLen==-1)||(nRecvLen<20))
                {
                    usleep(10);
                    continue;
                }
                if(gDeviceInfo[i].nInUse==1)
                {
                    if (memcmp(aDeviceInfoBuf[i].aESN, gDeviceInfo[i].aESN,20) != 0)
                    {
                        DbgPrintf("Old ESN: %s\r\n", gDeviceInfo[i].aESN);
                        DbgPrintf("New ESN: %s\r\n", aDeviceInfoBuf[i].aESN);
                        SendFrameToPlatform(i,DEVICE_MODIFY_CMD,S_R_Set,aDeviceInfoBuf[i].aESN,sizeof(aDeviceInfoBuf[i].aESN));
                        SendFrameToThirdPartyPlatform(i,DEVICE_MODIFY_CMD,S_R_Set,aDeviceInfoBuf[i].aESN,sizeof(aDeviceInfoBuf[i].aESN),1);
                    }
                    if (memcmp(aDeviceInfoBuf[i].aSofeVersion, gDeviceInfo[i].aSofeVersion,20) != 0)
                    {
                        DbgPrintf("[SOFTVERSION]Old SoftVersion: %s\r\n", gDeviceInfo[i].aSofeVersion);
                        DbgPrintf("[SOFTVERSION]New SoftVersion: %s\r\n", aDeviceInfoBuf[i].aSofeVersion);
                       // SendFrameToPlatform(i,UPLOAD_HW_DEVICE_CMD,S_R_InfoReport,(UINT8 *)&aDownLinkDevice[i],uDownLinkDeviceLen);
                        SendSofeVersionToPlatform(i,UPLOAD_HW_DEVICE_CMD,S_R_InfoReport,(UINT8 *)&aDownLinkDevice[i],uDownLinkDeviceLen);
                        memcpy(gDeviceInfo[i].aSofeVersion,aDeviceInfoBuf[i].aSofeVersion,sizeof(aDeviceInfoBuf[i].aSofeVersion));
                        TagBaseFileWrite();
                    }

                }
                else
                {
                    nNewDeviceFlag++;
                }
                usleep(10);
            }
            if(nNewDeviceFlag)
            {
                UINT8 aSendBuf[255],nSendLen=0;

                if(nExistsDevice)
                {
                    UINT8 aTemp[255],nRes=0,nLenTemp=0;

                    gDLinkDeviceInfoCount=0;
                    gMainDeviceStatus=DEVSTATUS_DEVINFO_RESPONSE;
                    nRes=GetDownLinkDeviceInfo(1,aTemp,&nLenTemp);
                    SendFrameToPlatform(gMainDeviceID,DOWNLINK_DEVICE_INFO_CMD,S_R_Set,aTemp,nLenTemp);
                    while((gMainDeviceStatus!=8) && (nErrorCount<90))
                    {
                        nErrorCount++;
                        sleep(1);
                    }
                    if(nErrorCount>=90)
                    {
                        gMainDeviceStatus=DEVSTATUS_NORMAL_WORKMODE;
                        continue;
                    }
                }

                memset(aSendBuf,0,sizeof(aSendBuf));
                nSendLen=0;
                do{
                    if((aDownLinkDevice[nAddDeviceCount][0]!=0) && (gDeviceInfo[nAddDeviceCount].nInUse==0))
                    {
                        aSendBuf[nSendLen]=nAddDeviceCount;
                        aSendBuf[nSendLen+1]=aDeviceInfoBuf[nAddDeviceCount].nDownlinkPort;
                        aSendBuf[nSendLen+2]=strlen((void *)aDownLinkDevice[nAddDeviceCount]);
                        memcpy((UINT8 *)&aSendBuf[nSendLen+3],(UINT8 *)&aDownLinkDevice[nAddDeviceCount],strlen((void *)aDownLinkDevice[nAddDeviceCount]));
                        nSendLen = nSendLen+strlen((void *)aDownLinkDevice[nAddDeviceCount])+3;
                    }
                    if(((nSendLen+strlen((void *)aDownLinkDevice[nAddDeviceCount+1])+3)>240)||(nAddDeviceCount==40))
                    {
                        nErrorCount=0;
                        gMainDeviceStatus=DEVSTATUS_DEVINFO_UPLOAD;
                        SendFrameToPlatform(gMainDeviceID,UPLOAD_HW_DEVICE_CMD,S_R_Set,aSendBuf,nSendLen);
						gUploadHWDeviceFlag = 1;
						while((gMainDeviceStatus==DEVSTATUS_DEVINFO_UPLOAD)&&(nErrorCount<90))
                        {
                            nErrorCount++;
                            sleep(1);
                        }
                        if(nErrorCount>=90)
                        {
                            break;
                        }
                        memset(aSendBuf,0,sizeof(aSendBuf));
                        nSendLen=0;
                    }
                    nAddDeviceCount++;
                }while(nAddDeviceCount<MAXDEVICE);
                if(nErrorCount<90)
                {
                    memcpy(gDeviceInfoBuf,gDeviceInfo,sizeof(gDeviceInfo));
                    for(i=1;i<MAXDEVICE;i++)
                    {
                        if(aDeviceInfoBuf[i].nInUse==1)
                            memcpy((UINT8 *)&gDeviceInfoBuf[i],(UINT8 *)&aDeviceInfoBuf[i],sizeof(sDeviceInfo));
                        gDeviceInfoBuf[i].nInUse=0;
                    }
                    //PlcC7SendtoPlatform();
                    //DEBUG("[PLC]Send PLC C7 to Platform , then C5 after\r\n");
                    SendSingleFrameToPlatform(0xC5,0x008C,0x01);
                    load_point_table_flag = 1;
                }
            }
        }
        else if(nConnectStatus==1)
        {/** �������� ���ͱ��ĸ�APP */

            /**
            * �������⣺
            * 1. ����ж��Ƿ�������APP���ӵ����豸
            * 2. ������豸���ӣ�Ӧ��ֻ֪ͨһ�Σ������Ƿ���֪ͨ��ǰ�������ܹ�֪����APP���룩*/

            if((gMainDeviceStatus==DEVSTATUS_DEVINFO_GET)||(gMainDeviceStatus==DEVSTATUS_DEVINFO_SET))
            {
                UINT8 SendBuf[] = {0x68, 0x0E, 0x00, 0x00, 0x00, 0x00, 0xC5, 0x00, 0x8C, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x06};

                SendBuf[2] = (gSendCount * 2) & 0xFF;
                SendBuf[3] = ((gSendCount * 2) >> 8) & 0xFF;
                SendBuf[4] = (gRecvCount * 2) & 0xFF;
                SendBuf[5] = ((gRecvCount * 2) >> 8) & 0xFF;

                AP_Send(SendBuf, sizeof(SendBuf));
                nConnectStatus=0;
            }
        }
        /** ��ӡ�豸״̬������̨�� ����������ֻ��������Ŷ */
        DbgPrintf("Status = %d\r\n",gMainDeviceStatus);
        sleep(100);
    }
}

/*****************************************************************************
* Function     : PowerAlarmThread()
* Description  : Report power alarm and recover
* Input        : None
* Output       : None
* Return       :
* Note(s)      :
* Contributor  : 2018��12��12��        Andre
*****************************************************************************/
void *PowerAlarmThread()
{
    UINT8 nErrorCount=0,alarmcount=10;
    UINT16 nPacketValue;
    UINT8 aReportPacket[21]={0x68,0x13,0x00,0x00,0x00,0x00,0xBF,0x01,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x03,0x01,0x00,0x00,0x00};

    while(1)
    {
        if(gIsReportInfo == _NO)
        {
            //DbgPrintf("[Power Alarm]wait!!!\r\n");
            sleep(5);
            continue;
        }

        //printf("[Power Alarm]GPIOGet(2,15) =%d !!!\r\n",GPIOGet(2,15));
        if(((gMainDeviceModel[13]=='A')||(gMainDeviceModel[13]=='a'))&&(gMainDeviceModel[14]=='1'))
        {
            if(((gWirelessNetMode==Net_Mode_3G)&&(GPIOGet(2,15) == 0)) || ((gWirelessNetMode==Net_Mode_4G)&&(GPIOGet(2,15) == 1)))
            {
                if(nErrorCount<3)
                {
                    nErrorCount++;
                    if(nErrorCount == 3)
                    {
                        nPacketValue=gSendCount*2;
                        memcpy((UINT8 *)&aReportPacket[2],(UINT8 *)&nPacketValue,2);
                        nPacketValue=gRecvCount*2;
                        memcpy((UINT8 *)&aReportPacket[4],(UINT8 *)&nPacketValue,2);
                        aReportPacket[20]=1;
                        AlarmAdd(0,Alarm_Device_Error,0x0001,0);
                        AlarmFileWrite();
                        Socket_Send(aReportPacket,21);
                        gDeviceAlarm = gDeviceAlarm |0x0001;
                        E2promWrite((UINT8 *)&gDeviceAlarm,AlarmLoggerAddr,2);
                        DbgPrintf("[Power Alarm]Report Power off alarm !!!\r\n");
                    }
                }
            }
            else
            {
                if(gDeviceAlarm&0x0001)
                {
                    if(gIsReportInfo == _YES)
                    {
                        DbgPrintf("[Power Alarm]Power off alarm recover\r\n");
                        nPacketValue=gSendCount*2;
                        memcpy((UINT8 *)&aReportPacket[2],(UINT8 *)&nPacketValue,2);
                        nPacketValue=gRecvCount*2;
                        memcpy((UINT8 *)&aReportPacket[4],(UINT8 *)&nPacketValue,2);
                        aReportPacket[20]=0;
                        //Socket_Send(aReportPacket,21);
                        if(alarmcount>=10)
                        {
                            AlarmDelete(0,Alarm_Device_Error,0x0001,0);
                            alarmcount=0;
                        }
                        alarmcount++;
                        nErrorCount=0;
                    }
                }
            }
        }
        sleep(2);
    }
}

void *ModemScanfThread()
{
    UINT8 nTimeCount=0;

    while(1)
    {
        /** ��֤�����Ѿ�����C0ָ���ƽ̨ */
        if (gIsReportInfo == _NO)
        {
                //printf("XXX Device do not report C0 command to server XXX\r\n");
            sleep(1);
            continue;
        }
        if(gSocketMode==0)
        {
            sleep(1);
            continue;
        }
        //printf("XXX Device have already reported C0 command to server XXX\r\n");
        /** �ϱ�SIM������Ϣ */
        if (hasReportSIM == _NO && sim_card_id[0] != 0)
        {
            /*UINT16 nValue, idx_buf, idx_str;

            memset(ModemBuf,0,sizeof(ModemBuf));
            ModemBuf[0] = 0x68;
            ModemBuf[1] = 0x21;
            nValue = gSendCount * 2;
            memcpy(&ModemBuf[2], (UINT8 *)&nValue, 2);
            nValue = gRecvCount * 2;
            memcpy(&ModemBuf[4], (UINT8 *)&nValue, 2);
            ModemBuf[6] = 0xD6;
            ModemBuf[7] = 0x01;
            ModemBuf[8] = S_R_InfoReport;
            ModemBuf[10] = 0x01;

            memcpy(ModemBuf + 15, sim_card_id, sizeof(sim_card_id));
            printf("MMM Report SIM Card CIMI : %s MMM\r\n", sim_card_id);
            Socket_Send(ModemBuf, 35);*/
            SendFrameToPlatform(gMainDeviceID,SIM_INFO_REPORT_CMD,S_R_InfoReport,sim_card_id,sizeof(sim_card_id));
            hasReportSIM = _YES;
        }
        if(nTimeCount>=30)
        {
            /*UINT8 aBuf[50],i,nPower=0,rnum;
            UINT8 *pCSQPoint,*pCSQEnd;
            UINT16 nValue;

            memset(aBuf,0,sizeof(aBuf));
            memset(ModemBuf,0,sizeof(ModemBuf));
            pthread_mutex_lock(&modemsem);
            sleep(1);
            do{
                writeModem("AT+CSQ\r\n",8);
                rnum = readModem(aBuf);
                sleep(1);
            }while(strstr(aBuf,"OK")==NULL);

            pCSQPoint=strstr(aBuf,"+CSQ: ");
            pCSQPoint+=strlen("+CSQ: ");
            pCSQEnd=strstr(pCSQPoint,",");
            for(i=0;i<pCSQEnd-pCSQPoint;i++)
            {
                nPower = nPower+(pCSQPoint[i]-0x30)*pow(10,pCSQEnd-pCSQPoint-i-1);
            }
            memset(ModemBuf,0,sizeof(ModemBuf));
            ModemBuf[0] = 0x68;
            ModemBuf[1] = 0x21;
            nValue = gSendCount * 2;
            memcpy(&ModemBuf[2], (UINT8 *)&nValue, 2);
            nValue = gRecvCount * 2;
            memcpy(&ModemBuf[4], (UINT8 *)&nValue, 2);
            ModemBuf[6] = 0xD7;
            ModemBuf[7] = 0x01;
            ModemBuf[8] = S_R_InfoReport;
            ModemBuf[10] = 0x01;
            ModemBuf[15] = nPower;

            ModemSend(ModemBuf,16);
            //Socket_Send(ModemBuf, 35);
            //SendSingleFrameToPlatform(MODEM_SIGNAL_PWR_CMD,S_R_InfoReport,nPower);
            nTimeCount=0;
             pthread_mutex_unlock(&modemsem);*/
        }
        nTimeCount++;
        sleep(1);
    }
}

/*****************************************************************************
* Description:      ����ʱ��ȡYC��YX��ʼ��ַ�����ƫ��
* Parameters:       �õ��modbus��ַ�������豸ID���õ��ȡ��ʽ
* Returns:          ���ƫ����
*****************************************************************************/
UINT8 GetOffset(UINT16 nPointMbAddr,UINT8 nDeviceID,UINT8 *nRecordPointType,UINT8 *nDataType)
{
    UINT8 NumYX=0;
    UINT8 NumYC=0;
    UINT8 NumDD=0;
    struct sTypeGroup *psDevice=NULL;
    struct sTypeParam *psPoint=NULL;

    psDevice=gTypeHead;
    if(nDeviceID>40)
    {
        DbgPrintf("Time Call Back Device Error = %02d\r\n",nDeviceID);
        return 0xff;
    }
    *nRecordPointType=99;
    if(gTypeHead==NULL)
    {
        return 0xff;
    }
    while(psDevice!=NULL)
    {
        if(psDevice->nTypeID==gDeviceInfo[nDeviceID].nPointTableNo)//�ҵ���Ӧ���
        {
            psPoint=psDevice->pParamNext;//��ʼ���ض�Ӧ������ĸ���
            while(psPoint!=NULL)
            {
                if(psPoint->nMBAddr==nPointMbAddr)
                {
                    *nRecordPointType=psPoint->nType;
                    *nDataType=psPoint->nDataType;
                    switch(psPoint->nType)
                    {
                        case Type_104_YX:   return NumYX;
                        case Type_104_YC:   return NumYC;
                        case Type_104_DD:   return NumDD;
                        default: return 0xFF;
                    }
                }
                else
                {
                    switch(psPoint->nType)
                    {
                        case Type_104_YX:   NumYX++;break;
                        case Type_104_YC:   NumYC++;break;
                        case Type_104_DD:   NumDD++;break;
                    }
                    psPoint=psPoint->pNext;
                }
            }
            return 0xFF;
        }
        else
            psDevice=psDevice->pNext;
    }
    return 0xFF;
}


//����ʱ����ָʱ���ǲ����ڲ��ɷ�Χ��
UINT8 HandleSubCallBack(int nPointTimeRecordFd,UINT8 *aTimeBuf)
{
    UINT8 aReadTemp[20],nGetDataFlag=0,i,j;
    UINT8 callback_time_flag = 0;//����ʱ����Ƿ����,1:���� 0:������
    /* �������ݿ��ܸ�ʱ�꣬ʱ������λ������ÿ�ζ����ݰ�4+3����5������������
       ����ǰ4���ֶεĽ����ٿ��������λ */
    int nLen=0;

    if(read(nPointTimeRecordFd,aReadTemp,11)<7)
        return nGetDataFlag;
    do{
        // ���ǰ4���ǲ���ʱ�꣬����ǣ�������ʱ�䳬��û��
        UINT8 nGotoFlag=0;

        for(i = 0; i < 7; i++)
        {
            if((aReadTemp[i]==0x7E)&&(aReadTemp[i+1]==0xAA)&&(aReadTemp[i+2]==0x55)&&(aReadTemp[i+3]==0x6E))
            {
                if(i > 0)
                {
                    for(j = 0; j < 11-i; j++)
                    {
                        aReadTemp[j] = aReadTemp[i+j];
                    }
                    nLen = read(nPointTimeRecordFd,(UINT8 *)&aReadTemp[11-i],i);
                    if(nLen < i)
                        return 0;
                }
                if(((aTimeBuf[2]+aTimeBuf[3]*60)>=(aReadTemp[7]+aReadTemp[6]*60))//*ƥ���¼��ʱ���ϣ����ȡ��ʱ��*/
                    &&(((aTimeBuf[2]+aTimeBuf[3]*60-5)-(aReadTemp[7]+aReadTemp[6]*60))<=75))//*ƥ���¼��ʱ���ϣ����ȡ��ʱ��*/
                {
                    DbgPrintf("get %02d-%02d %02d:%02d:%02d\r\n",aReadTemp[4]+1,aReadTemp[5],aReadTemp[6],aReadTemp[7],aReadTemp[8]);
                    if((aTimeBuf[2]+aTimeBuf[3]*60)-(aReadTemp[7]+aReadTemp[6]*60) < 5)
                        callback_time_flag = 1;
                    for(i = 0;i < 2; i++)
                    {
                        aReadTemp[i] = aReadTemp[i + 9];
                    }
                    nLen = read(nPointTimeRecordFd, (UINT8 *)&aReadTemp[2], 9);
                    if(nLen < 5)
                    {
                        return 0;
                    }
                    for(j=0; j<7; j++)
                    {
                        if((aReadTemp[j]==0x7E)&&(aReadTemp[j+1]==0xAA)&&(aReadTemp[j+2]==0x55)&&(aReadTemp[j+3]==0x6E))
                        {
                            if(callback_time_flag)
                                return 0;
                            nGotoFlag=1;
                            break;
                        }
                    }
                    if(nGotoFlag)
                    {
                        break;
                    }
                }
                else
                {
                        return nGetDataFlag;
                }
                break;
            }
        }
        if(nGotoFlag)
        {
            continue;
        }
        if((aReadTemp[0]>40)||(aReadTemp[0]==0))
        {
            for(i=0;i<10;i++)
            {
                aReadTemp[i]=aReadTemp[1+i];
            }
            nLen = read(nPointTimeRecordFd,(UINT8 *)&aReadTemp[10],1);
            if(nLen==1)
            {
                nLen=7;
                continue;
            }
            else
            {
                break;
            }
        }
        nGetDataFlag=1;
        HandleRecordData(nPointTimeRecordFd,aReadTemp);
        for(i=0;i<4;i++)
            aReadTemp[i]=aReadTemp[7+i];
        nLen = read(nPointTimeRecordFd,(UINT8 *)&aReadTemp[4],7);
    }while(nLen >= 7);
    if(callback_time_flag)
        return nGetDataFlag;
    else
        return 0;
}

void HandleRecordData(int nPointTimeRecordFd,UINT8 *aReadTemp)
{
    UINT16 nUpAddr;
    UINT32 nValue;
    INT32 nValueTemp;
    UINT8 nDataType;
    UINT8 nDeviceID;
    UINT8 nOffset=0;
    union uIEEE754 uIETemp;

    nDeviceID=aReadTemp[0];
    memcpy((UINT8 *)&nUpAddr,(UINT8 *)&aReadTemp[1],2);
    memcpy((UINT8 *)&nValue,(UINT8 *)&aReadTemp[3],4);
    //DbgPrintf("[HandleRecordData]555 start checking Record Link Table,DeviceID is %d MBaddr is %X ,value is %X\n",nDeviceID,nUpAddr,nValue);
    gPointTablePossessFlag|=(1<<4);
    nOffset=GetOffset(nUpAddr,nDeviceID,&njjjPointType,&nDataType);
    gPointTablePossessFlag&=~(1<<4);
    if(nOffset==0xFF)
        return;
    switch(njjjPointType)
    {
        case 1:
            nUpAddr=gDeviceInfo[nDeviceID].nYCAddr+nOffset;
            break;
        case 2:
            nUpAddr=gDeviceInfo[nDeviceID].nYXAddr+nOffset;
            break;
        case 3:
            nUpAddr=gDeviceInfo[nDeviceID].nDDAddr+nOffset;
            break;
        default:
            return;
    }
    if((nUpAddr>0x4000)&&(nUpAddr<0x6000))/*ң�����͵�����*/
    {
        if(nDataType==Type_Data_FLOAT)
            {
                memcpy((UINT8 *)&aPointTimeBuf[nUpAddr],(void *)nValue,4);
                return;
            }
        if(nDataType==Type_Data_INT16)
        {
            if((nValue&0x8000)!=0)
            {
                nValueTemp = nValue - pow(2,16);
                uIETemp.nIndex=nValueTemp;
            }
            else
                uIETemp.nIndex=nValue;
        }
        else if(nDataType==Type_Data_INT32)
        {
            if((nValue&0x80000000)!=0)
            {
                nValueTemp = nValue - pow(2,32);
                uIETemp.nIndex=nValueTemp;
            }
            else
                uIETemp.nIndex=nValue;
        }
        else
            uIETemp.nIndex=nValue;
        memcpy((UINT8 *)&aPointTimeBuf[nUpAddr],uIETemp.nChar,4);
    }
    else/*ң������*/
        aPointTimeBuf[nUpAddr]=nValue;
}

void SetPointType(UINT8 gVal,UINT8 Type)
{
    gVal=Type;
    DbgPrintf("gVal is %d,actually Type is %d",njjjPointType,Type);
}

void *IVThread()
{
    UINT8 i;
    //nUartFd = UartOper(2,9600);
    while(1)
    {
        if(gMainDeviceStatus==10)
        {
            if(gIVLicenseStatus==0xFF)
            {
                for(i=1;i<=MAXDEVICE;i++)
                {
                    if((gDeviceInfo[i].nInUse==1)&&(gDeviceInfo[i].nProtocolType == Type_Huawei_Modbus))
                    {
                        UINT8 aSendBuf[256],aRecvBuf[256];
                        UINT32 nRecvTemp;

                        memset(aSendBuf,0,sizeof(aSendBuf));

                        memset(aRecvBuf,0,sizeof(aRecvBuf));
                        pthread_mutex_lock(&Uartsem);
                        if(ComNormalRead(i,32126,2,aRecvBuf)==-1)
                        {
                            pthread_mutex_unlock(&Uartsem);
                            continue;
                        }
                        nRecvTemp=aRecvBuf[3];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                        memcpy(aSendBuf,(UINT8 *)&nRecvTemp,4);

                        memset(aRecvBuf,0,sizeof(aRecvBuf));
                        if(ComNormalRead(i,32380,17,aRecvBuf)==-1)
                        {
                            pthread_mutex_unlock(&Uartsem);
                            continue;
                        }
                        nRecvTemp=aRecvBuf[3];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                        memcpy((UINT8 *)&aSendBuf[4],(UINT8 *)&nRecvTemp,4);
                        nRecvTemp=aRecvBuf[7];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[8];
                        memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nRecvTemp,2);
                        nRecvTemp=aRecvBuf[9];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[10];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[11];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[12];
                        memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&nRecvTemp,4);
                        nRecvTemp=aRecvBuf[13];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[14];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[15];
                        nRecvTemp=(nRecvTemp<<8)+aRecvBuf[16];
                        memcpy((UINT8 *)&aSendBuf[14],(UINT8 *)&nRecvTemp,4);

                        memcpy((UINT8 *)&aSendBuf[18],(UINT8 *)&aRecvBuf[17],20);

                        memset(aRecvBuf,0,sizeof(aRecvBuf));
                        if(ComNormalRead(i,33107,64,aRecvBuf)==-1)
                        {
                            pthread_mutex_unlock(&Uartsem);
                            continue;
                        }
                        pthread_mutex_unlock(&Uartsem);
                        memcpy((UINT8 *)&aSendBuf[38],(UINT8 *)&aRecvBuf[3],128);
                        SendFrameToPlatform(i,IV_LICE_INFO_CMD,S_R_InfoReport,aSendBuf,166);
                    }
                }
                gIVLicenseStatus=0;
                //gIVLicenseLoad[0]=1;
                /*UINT8 aSendBuf[256];

                memset(aSendBuf,0,sizeof(aSendBuf));
                aSendBuf[2]=0x08;
                aSendBuf[4]=0x01;

                SendFrameToPlatform(1,IV_LICE_INFO_CMD,S_R_InfoReport,aSendBuf,166);
                gIVLicenseStatus=0;
                //gIVLicenseLoad[0]=1;*/
            }
            else
            {
                UINT8 aRecvBuf[256];

                for(i=1;i<MAXDEVICE;i++)
                {
                    if(gDeviceIVInfo[i].nIVLicenseLoadFlag)
                    {
                        UINT8 aPathBuf[100];
                        int nFileSize=0;

                        sprintf((void *)aPathBuf,"/mnt/flash/OAM/%d.dat",i);
                        if((access((void *)aPathBuf,F_OK)==-1))
                            continue;
                        nFileSize =GetFileSize((void *)aPathBuf);
                        if(nFileSize!=-1)
                        {
                            UINT16 nFilePacketNum=nFileSize/224;
                            UINT16 nFilePacketCount=0;
                            UINT8 aSendBuf[256];
                            int nLicenseLoadFile =open((void *)aPathBuf,O_RDONLY);

                            pthread_mutex_lock(&Uartsem);
                            aSendBuf[0]=i;
                            aSendBuf[1]=0x41;
                            aSendBuf[2]=0x01;
                            aSendBuf[3]=0x06;
                            aSendBuf[4]=0x80;
                            aSendBuf[5]=nFileSize>>24;
                            aSendBuf[6]=(nFileSize>>16)&0xFF;
                            aSendBuf[7]=(nFileSize>>8)&0xFF;
                            aSendBuf[8]=nFileSize&0xFF;
                            aSendBuf[9]=224;
                            ComWrite(gDeviceInfo[i].nDownlinkPort,aSendBuf,10);
                            //usleep(100000);
                            if((nFileSize%224)!=0)
                                nFilePacketNum++;
                            while((nFilePacketCount<nFilePacketNum)&&(gDeviceIVInfo[i].nIVLicenseLoadFlag ==1))
                            {
                                UINT8 aReadBuf[256];
                                int nReadLen;
                                UINT16 nCRC;

                                memset(aSendBuf,0,sizeof(aSendBuf));
                                memset(aReadBuf,0,sizeof(aReadBuf));
                                nReadLen = read(nLicenseLoadFile,aReadBuf,224);
                                aSendBuf[0]=i;
                                aSendBuf[1]=0x41;
                                aSendBuf[2]=0x02;
                                aSendBuf[3]=nReadLen+4;
                                aSendBuf[4]=0x80;
                                aSendBuf[5]=nFilePacketCount>>8;
                                aSendBuf[6]=nFilePacketCount&0xFF;
                                aSendBuf[7]=nReadLen;
                                memcpy((UINT8 *)&aSendBuf[8],aReadBuf,nReadLen);
                                nCRC=FileCRC16(aReadBuf,nReadLen);
                                gIVCRCHi = nCRC>>8;
                                gIVCRCLo = nCRC&0xFF;
                                ComWrite(gDeviceInfo[i].nDownlinkPort,aSendBuf,nReadLen+8);
                                nFilePacketCount++;
                            }
                            if(gDeviceIVInfo[i].nIVLicenseLoadFlag==0)
                            {
                                gDeviceIVInfo[i].nIVLicenseReportLoadCount=0;
                                pthread_mutex_unlock(&Uartsem);
                                continue;
                            }
                            memset(aSendBuf,0,sizeof(aSendBuf));
                            aSendBuf[0]=i;
                            aSendBuf[1]=0x41;
                            aSendBuf[2]=0x03;
                            aSendBuf[3]=0x03;
                            aSendBuf[4]=0x80;
                            aSendBuf[5]=gIVCRCHi;
                            aSendBuf[6]=gIVCRCLo;
                            ComWrite(gDeviceInfo[i].nDownlinkPort,aSendBuf,7);
                            gIVCRCHi=0xFF;
                            gIVCRCLo=0xFF;

                            memset(aSendBuf,0,sizeof(aSendBuf));
                            aSendBuf[0]=i;
                            aSendBuf[1]=0x41;
                            aSendBuf[2]=0x04;
                            aSendBuf[3]=0x01;
                            aSendBuf[4]=0x80;
                            ComWrite(gDeviceInfo[i].nDownlinkPort,aSendBuf,5);
                            gDeviceIVInfo[i].nIVLicenseReportLoadFlag=1;
                            gDeviceIVInfo[i].nIVLicenseReportLoadCount=0;
                            gDeviceIVInfo[i].nIVLicenseLoadFlag=0;
                            pthread_mutex_unlock(&Uartsem);
                        }
                    }
                    usleep(10);
                }
                for(i=1;i<MAXDEVICE;i++)
                {
                    if(gDeviceInfo[i].nInUse)
                    {
                        if(gDeviceIVInfo[i].nIVLicenseReportLoadFlag==1)
                        {
                            pthread_mutex_lock(&Uartsem);
                            if(ComNormalRead(i,32382,1,aRecvBuf)!=-1)
                            {
                                if(aRecvBuf[4]==1)
                                {
                                    UINT8 aRemoveCMD[50];
                                    UINT8 aSendBuf[256];
                                    UINT32 nRecvTemp;

                                    memset(aSendBuf,0,sizeof(aSendBuf));
                                    if(ComNormalRead(i,32126,2,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    nRecvTemp=aRecvBuf[3];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                                    memcpy(aSendBuf,(UINT8 *)&nRecvTemp,4);

                                    memset(aRecvBuf,0,sizeof(aRecvBuf));
                                    if(ComNormalRead(i,32380,17,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    nRecvTemp=aRecvBuf[3];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                                    memcpy((UINT8 *)&aSendBuf[4],(UINT8 *)&nRecvTemp,4);
                                    nRecvTemp=aRecvBuf[7];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[8];
                                    memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nRecvTemp,2);
                                    nRecvTemp=aRecvBuf[9];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[10];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[11];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[12];
                                    memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&nRecvTemp,4);
                                    nRecvTemp=aRecvBuf[13];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[14];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[15];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[16];
                                    memcpy((UINT8 *)&aSendBuf[14],(UINT8 *)&nRecvTemp,4);

                                    memcpy((UINT8 *)&aSendBuf[18],(UINT8 *)&aRecvBuf[17],20);

                                    memset(aRecvBuf,0,sizeof(aRecvBuf));
                                    if(ComNormalRead(i,33107,64,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    memcpy((UINT8 *)&aSendBuf[38],(UINT8 *)&aRecvBuf[3],128);
                                    SendFrameToPlatform(i,IV_LICE_INFO_CMD,S_R_InfoReport,aSendBuf,166);
                                    gDeviceIVInfo[i].nIVLicenseReportLoadFlag=0;
                                    gDeviceIVInfo[i].nIVLicenseReportLoadCount=0;
                                    sprintf((void *)aRemoveCMD,"rm /mnt/flash/OAM/%d.dat",i);
                                    system((void *)aRemoveCMD);
                                }
                                else if((++gDeviceIVInfo[i].nIVLicenseReportLoadCount)>5)
                                {
                                    UINT8 aRemoveCMD[50];
                                    UINT8 aReponseValue[2];

                                    aReponseValue[0] = aRecvBuf[4];
                                    aReponseValue[1] = aRecvBuf[3];
                                    UINT8 aSendBuf[256];
                                    UINT32 nRecvTemp;

                                    memset(aSendBuf,0,sizeof(aSendBuf));
                                    if(ComNormalRead(i,32126,2,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    nRecvTemp=aRecvBuf[3];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                                    memcpy(aSendBuf,(UINT8 *)&nRecvTemp,4);

                                    memset(aRecvBuf,0,sizeof(aRecvBuf));
                                    if(ComNormalRead(i,32380,17,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    nRecvTemp=aRecvBuf[3];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[4];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[5];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[6];
                                    memcpy((UINT8 *)&aSendBuf[4],(UINT8 *)&nRecvTemp,4);
                                    nRecvTemp=aRecvBuf[7];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[8];
                                    memcpy((UINT8 *)&aSendBuf[8],(UINT8 *)&nRecvTemp,2);
                                    nRecvTemp=aRecvBuf[9];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[10];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[11];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[12];
                                    memcpy((UINT8 *)&aSendBuf[10],(UINT8 *)&nRecvTemp,4);
                                    nRecvTemp=aRecvBuf[13];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[14];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[15];
                                    nRecvTemp=(nRecvTemp<<8)+aRecvBuf[16];
                                    memcpy((UINT8 *)&aSendBuf[14],(UINT8 *)&nRecvTemp,4);

                                    memcpy((UINT8 *)&aSendBuf[18],(UINT8 *)&aRecvBuf[17],20);

                                    memset(aRecvBuf,0,sizeof(aRecvBuf));
                                    if(ComNormalRead(i,33107,64,aRecvBuf)==-1)
                                    {
                                        UINT8 nErrorValue=0xEE;

                                        pthread_mutex_unlock(&Uartsem);
                                        SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_Error,(UINT8 *)&nErrorValue,1);
                                        continue;
                                    }
                                    memcpy((UINT8 *)&aSendBuf[38],(UINT8 *)&aRecvBuf[3],128);
                                    SendFrameToPlatform(i,IV_LICE_INFO_CMD,S_R_InfoReport,aSendBuf,166);

                                    //SendFrameToPlatform(i,IV_LICE_STATUS_CMD,S_R_InfoReport,aReponseValue,2);
                                    gDeviceIVInfo[i].nIVLicenseReportLoadFlag=0;
                                    gDeviceIVInfo[i].nIVLicenseReportLoadCount=0;
                                    sprintf((void *)aRemoveCMD,"rm /mnt/flash/OAM/%d.dat",i);
                                    system((void *)aRemoveCMD);
                                }
                            }
                            else
                            {
                                if((++gDeviceIVInfo[i].nIVLicenseReportLoadCount)>5)
                                {
                                    UINT8 aRemoveCMD[50];

                                    gDeviceIVInfo[i].nIVLicenseReportLoadFlag=0;
                                    gDeviceIVInfo[i].nIVLicenseReportLoadCount=0;
                                    sprintf((void *)aRemoveCMD,"rm /mnt/flash/OAM/%d.dat",i);
                                    system((void *)aRemoveCMD);
                                }
                            }
                            pthread_mutex_unlock(&Uartsem);
                        }
                    }
                    usleep(10);
                }
            }
            for(i=1;i<MAXDEVICE;i++)
            {
                if(gDeviceIVInfo[i].nIVScanfLoadFlag)
                {
                    UINT8 aSendBuf[256],aRecvBuf[256];
                    int nRecvLen;
                    UINT32 nFileSize=0;
                    UINT16 nFrameSize=0,nFrameNum=0,nFrameCount=0;

                    pthread_mutex_lock(&Uartsem);
                    gIVCRCHi=0xFF;
                    gIVCRCLo=0xFF;
                    LVScanfFileOpen(i);
                    aSendBuf[0]=i;
                    aSendBuf[1]=0x41;
                    aSendBuf[2]=0x05;
                    aSendBuf[3]=0x01;
                    aSendBuf[4]=0x1C;
                    nRecvLen = ComRead(gDeviceInfo[i].nDownlinkPort,aSendBuf,5,aRecvBuf);
                    nFileSize = aRecvBuf[5];
                    nFileSize = (nFileSize<<8)+aRecvBuf[6];
                    nFileSize = (nFileSize<<8)+aRecvBuf[7];
                    nFileSize = (nFileSize<<8)+aRecvBuf[8];
                    nFrameSize = aRecvBuf[9];
                    nFrameNum = nFileSize/nFrameSize;
                    if((nFileSize%nFrameSize)!=0)
                        nFrameNum++;
                    while(nFrameCount<nFrameNum)
                    {
                        UINT8 nFileLen;

                        memset(aSendBuf,0,sizeof(aSendBuf));
                        aSendBuf[0]=i;
                        aSendBuf[1]=0x41;
                        aSendBuf[2]=0x06;
                        aSendBuf[3]=0x03;
                        aSendBuf[4]=0x1C;
                        aSendBuf[5]=nFrameCount>>8;
                        aSendBuf[6]=nFrameCount&0xFF;
                        nRecvLen = ComRead(gDeviceInfo[i].nDownlinkPort,aSendBuf,7,aRecvBuf);
                        if(nRecvLen>0)
                        {
                            UINT16 nRecvFrameNum=0,nFrameCRC=0;

                            nFileLen = aRecvBuf[3]-3;
                            nRecvFrameNum = aRecvBuf[5];
                            nRecvFrameNum = (nRecvFrameNum<<8)+aRecvBuf[6];
                            nFrameCRC = CRC16(aRecvBuf,nRecvLen-2);
                            DbgPrintf("Packet No.%02d CRC = 0x%04X\r\n",nRecvFrameNum,nFrameCRC);
                            if(((nFrameCRC>>8)!=aRecvBuf[nRecvLen-1])||((nFrameCRC&0xFF)!=aRecvBuf[nRecvLen-2]))
                            {
                                DbgPrintf("IV Scanf Frame CRC Error!-----Local = %X  Recv = %X%X\r\n",nFrameCRC,aRecvBuf[nRecvLen-1],aRecvBuf[nRecvLen-2]);
                                continue;
                            }
                            if(nRecvFrameNum==nFrameCount)
                            {
                                UINT16 nCRC;

                                FileWrite(nIVScanfFd,(UINT8 *)&aRecvBuf[7],nFileLen);
                                nCRC = FileCRC16((UINT8 *)&aRecvBuf[7],nFileLen);
                                gIVCRCHi = nCRC>>8;
                                gIVCRCLo = nCRC&0xFF;
                                nFrameCount++;
                                continue;
                            }
                            else
                                DbgPrintf("IV Scanf Recv Data Error!-----Recv Num = %d  Need Num = %d\r\n",nRecvFrameNum,nFrameCount);
                        }
                        break;
                    }
                    if(nFrameCount==nFrameNum)
                    {
                        memset(aSendBuf,0,sizeof(aSendBuf));
                        aSendBuf[0]=i;
                        aSendBuf[1]=0x41;
                        aSendBuf[2]=0x0C;
                        aSendBuf[3]=0x01;
                        aSendBuf[4]=0x1C;
                        nRecvLen = ComRead(gDeviceInfo[i].nDownlinkPort,aSendBuf,5,aRecvBuf);
                        DbgPrintf("FILE CRC = 0x%02X%02X\r\n",gIVCRCHi,gIVCRCLo);
                        if((gIVCRCHi==aRecvBuf[5])||(gIVCRCLo==aRecvBuf[6]))
                        {
                            UINT8 aSendPlatformBuf[4];
                            DbgPrintf("IV Scanf File Recv OK!!!!!!!\r\n");
                            gDeviceIVInfo[i].nIVScanfLoadFlag=0;
                            fclose((void *)nIVScanfFd);
                            aSendPlatformBuf[0] = i;
                            nFrameNum = nFileSize/200;
                            if((nFileSize%nFrameSize)!=0)
                                nFrameNum++;
                            memcpy((UINT8 *)&aSendPlatformBuf[1],(UINT8 *)&nFrameNum,2);
                            SendFrameToPlatform(0,IV_SCANF_TRANS_CMD,S_R_BeginToTransConfirm,aSendPlatformBuf,4);
                        }
                        else
                        {
                            DbgPrintf("IV Scanf File CRC Error!-----Local = %02X%02X  Recv = %02X%02X\r\n",gIVCRCHi,gIVCRCLo,aRecvBuf[6],aRecvBuf[5]);
                        }
                    }
                    pthread_mutex_unlock(&Uartsem);
                }
                usleep(10);
            }
            /*if(gIVLicenseStatus==0xFF)
            {

            UINT8 aSendBuf[256];

            memset(aSendBuf,0,sizeof(aSendBuf));
            aSendBuf[2]=0x08;
            aSendBuf[4]=0x01;

            SendFrameToPlatform(1,IV_LICE_INFO_CMD,S_R_InfoReport,aSendBuf,166);
            gIVLicenseStatus=0;
            }*/
        }
        sleep(1);
    }
}
void *TestThread() //Definatelly for own test
{
    /*
     printf("[Test]77777TestThread7777\n");
     UINT8 nLen=19;
     UINT8 testMsg[]={0x68,0x11,0x18,0x00,0xb2,0x00,0xd9,0x01,0x92,0x00,0x01,0x00,0x02,0x00,0x00,0xb6,0x9c,0x02,0x00};
     HandleYT(testMsg);
     testMsg[8]=0x07;
     //ע��CRC������λ����Щʱ����ҪŶ cRc������·У�����
     DEBUG("[Test]77777Retrospect7777");
     Socket_Send(testMsg, nLen);
     */
     return 0;
}

/*UINT8 PlcScanDev(UINT8 uCom,UINT8 *uDeviceCharacter)
{
    UINT8 aEsnTemp[25],aRecvBuf[255];
    int nRecvLen;
    UINT8 uRes;
    UINT8 uEsnLen=0;
    UINT8 uCharacterLen=0;
    int i;
    UINT8 *pEsnPoint,*pEsnEnd;
    int nSecAddr=249;
    char Section[MAX_VALUE]="PLC";
    UINT8 Tap=0;//��վ���
    UINT8 Alarm=0;// �澯���
    UINT8 Esn[20]={0};
    UINT16 nCRC;
    //UINT8 nPort=sPlcInfoBuf[uCom].nDownlinkPort;
    UINT8 nPort=0;
    if(uCom==1)
        {
            nPort=1;
            cnf_add_option(PlcCnf, "PLC1","Port","1");
        }
    else
        {
            nPort=2;
            cnf_add_option(PlcCnf, "PLC2","Port","2");
        }
    p_catstring(Section,nPort,Section,MAX_VALUE);
    UINT8 uSendtoPlatformBuff[18+66]={0};
    //uRes=sPlcInfoBuf[uCom].nInUse;
    nRecvLen=PlcModbusSend(nPort,nSecAddr,aEsnTemp,aRecvBuf);
    // ��ѯʧ��
    if((nRecvLen==-1)||(nRecvLen<20)||(aRecvBuf[0]!=nSecAddr))
    {
        DEBUG("[PLC]No PLC Device \r\n");

        if(uCom==0&&sPlcInfoBuf[0].nInUse==1)
            {
                switch(PLC0ConnectiveStatus)
                    {
                        case 0:break;
                        case 1:PlcAlarmActSend(uCom);PLC0ConnectiveStatus=3;break;
                        case 2:PlcAlarmActSend(uCom);PLC0ConnectiveStatus=3;break;
                        case 3:PlcAlarmActSend(uCom);PLC0ConnectiveStatus=3;break;
                        case 4:break;
                        default:break;
                    }
            }
        if(uCom==1&&sPlcInfoBuf[1].nInUse==1)
            {
                switch(PLC1ConnectiveStatus)
                    {
                        case 0:break;
                        case 1:PlcAlarmActSend(uCom);PLC1ConnectiveStatus=3;break;
                        case 2:PlcAlarmActSend(uCom);PLC1ConnectiveStatus=3;break;
                        case 3:PlcAlarmActSend(uCom);PLC1ConnectiveStatus=3;break;
                        case 4:break;
                        default:break;
                    }
            }
        cnf_get_value(PlcCnf,Section,"Port");
        Tap=PlcCnf->re_int;
        cnf_get_value(PlcCnf,Section,"Alarm");
        Alarm=PlcCnf->re_int;
        if(Tap==1&&Alarm==0)
            {
                printf("[PLC]Alarm Happen  Com is %d \r\n",nPort);
                PlcAlarmActSend(nPort);
            }
        return 0;
    }
    //��ѯ�ɹ�
    else
    {

        if(uCom==0&&sPlcInfoBuf[0].nInUse==1)
            {
                switch(PLC0ConnectiveStatus)
                    {
                        case 0:PLC0ConnectiveStatus=1;break;
                        case 1:break;
                        case 2:DEBUG("[PLC]Need Alarm Confirm\r\n");PlcAlarmActSend(uCom);break;
                        case 3:DEBUG("[PLC]Need Alarm Confirm\r\n");PlcAlarmActSend(uCom);break;
                        case 4:PlcAlarmEraseSend(uCom);break;
                        default:break;
                    }
            }
        if(uCom==1&&sPlcInfoBuf[1].nInUse==1)
            {
                switch(PLC1ConnectiveStatus)
                    {
                        case 0:PLC1ConnectiveStatus=1;break;
                        case 1:break;
                        case 2:DEBUG("[PLC]Need Alarm Confirm\r\n");break;
                        case 3:DEBUG("[PLC]Need Alarm Confirm\r\n");break;
                        case 4:PlcAlarmEraseSend(uCom);break;
                        default:break;
                    }
            }

        cnf_get_value(PlcCnf,Section,"Port");
        Tap=PlcCnf->re_int;
        cnf_get_value(PlcCnf,Section,"Alarm");
        Alarm=PlcCnf->re_int;
        if(Alarm==1&&Tap==1)
            {
                printf("[PCL]Alarm Erase Happen Com is %d!\r\n",nPort);
                PlcAlarmEraseSend(nPort);
            }
        nCRC = CRC16(aRecvBuf,nRecvLen-2);
        if(((nCRC>>8)!=aRecvBuf[nRecvLen-1])||((nCRC&0xFF)!=aRecvBuf[nRecvLen-2]))
        {
            usleep(10);
            nRecvLen = -1;
            DEBUG("[PLC] CRC Wrong \r\n");
            return 0;
        }
        //DevicePlc.nDownlinkPort=2-nPort;
        //memset(uDeviceCharacter,0,100);
        uCharacterLen=aRecvBuf[9];
        memcpy(uDeviceCharacter,(UINT8 *)&aRecvBuf[10],uCharacterLen);
        DEBUG("[PLC]Device character Num is %d:\n",uCharacterLen);
        memset(aEsnTemp,0,sizeof(aEsnTemp));
        pEsnPoint = strstr(uDeviceCharacter,"4=");
        if(pEsnPoint==NULL)
            {
                DEBUG("[PLC] pEsnPoint ERROR \r\n");
                return 0;
            }
        pEsnPoint += strlen("4=");
        pEsnEnd = strstr(pEsnPoint,";");
        if(pEsnEnd==NULL)
            {
                DEBUG("[PLC] pEsnEnd ERROR \r\n");
                return 0;
            }
        uEsnLen=pEsnEnd - pEsnPoint;
        memcpy(aEsnTemp, pEsnPoint,uEsnLen);
        DEBUG("ESN=%s\r\n",aEsnTemp);

        //memcpy(DevicePlc.aESN, pEsnPoint,pEsnEnd - pEsnPoint);
        //DEBUG("ESN=%s\r\n",DevicePlc.aESN);
    }


    if(Tap==0)//���豸 ��C7
        {
            cnf_add_option(PlcCnf, Section,"Tap","1");
            cnf_add_option(PlcCnf, Section,"Esn",aEsnTemp);
            sPlcInfoBuf[uCom].nInUse = _YES;
            sPlcInfoBuf[uCom].nProtocolType = PTYPE_HUAWEI_MODBUS;
            sPlcInfoBuf[uCom].nEndian = _BIG_ENDIAN;
            sPlcInfoBuf[uCom].nRate = 9600;
            //memcpy(DevicePlc.aESN, pEsnPoint,pEsnEnd - pEsnPoint);
            memcpy(sPlcInfoBuf[uCom].aESN, aEsnTemp,uEsnLen);
            DEBUG("ESN=%s\r\n",sPlcInfoBuf[uCom].aESN);
            DEBUG("[PLC]First Finding PLC,Send C7 msg to Platform\n");
            //����ͱ���
            uSendtoPlatformBuff[0]=0x68;
            uSendtoPlatformBuff[1]=16+66;
            uSendtoPlatformBuff[6]=0xC7;
            uSendtoPlatformBuff[7]=0x01;
            uSendtoPlatformBuff[8]=0x92;
            uSendtoPlatformBuff[9]=0;
            uSendtoPlatformBuff[10]=0x01;
            uSendtoPlatformBuff[15]=249+nPort-1;//�豸������ַ
            uSendtoPlatformBuff[16]=nPort;//COM �ڱ��
            uSendtoPlatformBuff[17]=0x42;//���ݳ��� ESN +�̼���ַ
            memcpy(&uSendtoPlatformBuff[18],uDeviceCharacter,uSendtoPlatformBuff[17]);
            uRes=1;
            Socket_Send(uSendtoPlatformBuff,84);
            DEBUG("[PLC] Copy  uSendtoPlatformBuff Firstly\r\n ");
            if(nPort==1)
            {
                memcpy(PlcC7Buff0,&uSendtoPlatformBuff[0],84);
            }
            if(nPort==2)
            {
                memcpy(PlcC7Buff1,&uSendtoPlatformBuff[0],84);
            }
        }
    else   //�����豸 ��ESN���б� �� D4 �������豸�޸�
        {
            //memcpy(aEsnTemp, pEsnPoint,pEsnEnd - pEsnPoint);

            if (memcmp(sPlcInfoBuf[uCom].aESN, aEsnTemp,20) != 0)
            {
               DEBUG("[PLC]Old ESN: %s\r\n", sPlcInfoBuf[uCom].aESN);
               DEBUG("[PLC]New ESN: %s\r\n", aEsnTemp);

               SendFrameToPlatform((nSecAddr+sPlcInfoBuf[uCom].nDownlinkPort),DEVICE_MODIFY_CMD,S_R_Set,aEsnTemp,sizeof(aEsnTemp));
             }
            else
            {

                DEBUG("[PLC]In Use, ESN: %s\r\n", sPlcInfoBuf[uCom].aESN);
            }
            uSendtoPlatformBuff[0]=0x68;
            uSendtoPlatformBuff[1]=16+66;
            uSendtoPlatformBuff[6]=0xC7;
            uSendtoPlatformBuff[7]=0x01;
            uSendtoPlatformBuff[8]=0x92;
            uSendtoPlatformBuff[9]=0;
            uSendtoPlatformBuff[10]=0x01;
            uSendtoPlatformBuff[15]=249+nPort;//�豸������ַ
            uSendtoPlatformBuff[16]=nPort;//COM �ڱ��
            uSendtoPlatformBuff[17]=0x42;//���ݳ��� ESN +�̼���ַ
            memcpy(&uSendtoPlatformBuff[18],uDeviceCharacter,uSendtoPlatformBuff[17]);
            DEBUG("[PLC] Copy  uSendtoPlatformBuff \r\n ");
            if(uCom==0)
                {
                    memcpy(PlcC7Buff0,&uSendtoPlatformBuff[0],84);
                }
            if(uCom==1)
                {
                    memcpy(PlcC7Buff1,&uSendtoPlatformBuff[0],84);
                }
            uRes=1;

            printf("[PLC]Esn Changed or not Happened\r\n");
        }
    DEBUG("[PLC] Copy  uSendtoPlatformBuff\r\n ");
    if(nPort==1)
    {
        memcpy(PlcC7Buff0,uSendtoPlatformBuff,84);
    }
    if(nPort==2)
    {
        memcpy(PlcC7Buff1,uSendtoPlatformBuff,84);
    }

    p_erase_ini(PLCPATH);
    //print_config(cnfmb);
    cnf_write_file(PlcCnf,PLCPATH,"PLCCCCCCCINI");
    return uRes;

}*/
UINT8 PlcModbusSend(UINT8 nPort,UINT8 nSecAddr,UINT8 *aEsnTemp,UINT8 *aRecvBuf)//1 ����COM1  ��������COM2
{
    int nRecvLen;
    UINT8 aSendBuf[7]={0x00,0x2B,0x0E,0x03,0x88,0x00,0x00};
    UINT16 nCRC,nFPGAValue,k;

    nRecvLen = -1;
    DbgPrintf("[PLC]PLC Device Send 2B Commond Port : %d \r\n",nPort);
    pthread_mutex_lock(&Uartsem);
    do
    {
        nFPGAValue = nPort;
        FpgaWrite(3,nFPGAValue);
    }while((FpgaRead(0x03))!=nFPGAValue);

    aSendBuf[0] = nSecAddr; /** ������ַ */
    nCRC=CRC16(aSendBuf,5);
    aSendBuf[5] = (UINT8)nCRC;
    aSendBuf[6] = (UINT8)(nCRC>>8);

    memset(aRecvBuf,0,sizeof(aRecvBuf));
    GPIOSet(2,19,1);
    usleep(1000);
    DbgPrintf("COM.%d Send:",nPort);
    for(k=0;k<7;k++)
        DbgPrintf("%02X ",aSendBuf[k]);
    DbgPrintf("\r\n");
    writeDev(nUartFd,aSendBuf,7);
    //usleep(100);
    GPIOSet(2,19,0);
    nRecvLen=readDevEsn(nUartFd,aRecvBuf);
    usleep(1000);
    pthread_mutex_unlock(&Uartsem);
    DbgPrintf("[PLC]Finish PLC 2B Commond \r\n");
    return nRecvLen;
}
/**
nPort:COM�� 0 ����COM1   1����COM2
MbAddr�������ַ
nState������״̬ 1����ѯ��0��д��
Data����nState=0ʱ��ָ��д�����ݣ���֮ΪNULL
**/
void HandleRegister(UINT8 nPort,UINT8 *MbAddr,UINT8 nState,UINT8 *Data)
{
    UINT8 uModbusCmd[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    if(nState==1)
        {
            uModbusCmd[1]=0x03;
        }
    else
        {
            uModbusCmd[1]=0x06;
        }

}
void PlcC7SendtoPlatform()
{
    DbgPrintf("[PLC]PlcC7SendtoPlatform | buff0 is %d  buff1 is %d",PlcC7Buff0[0],PlcC7Buff0[0]);
    if(PlcC7Buff0[0]==0x68&&sPlcInfoBuf[0].nInUse==1)
        {
            Socket_Send(PlcC7Buff0,84);
            DbgPrintf("[PLC]Send C7 COM 0\r\n");
        }
    if(PlcC7Buff1[0]==0x68&&sPlcInfoBuf[1].nInUse==1)
        {
            Socket_Send(PlcC7Buff1,84);
            DbgPrintf("[PLC]Send C7 COM 1\r\n");
        }
}
void PlcAlarmActSend(UINT8 uCom)
{
    UINT8 uDeviceId;
    UINT8 uSendBuf[21]={0};
    uDeviceId=249+uCom-1;
    uSendBuf[0]=0x68;
    uSendBuf[1]=0x13;//����
    uSendBuf[6]=0xBF;
    uSendBuf[7]=0x01;
    uSendBuf[8]=0x06;//����ԭ��
    uSendBuf[10]=0x01;//������ַ
    uSendBuf[15]=uDeviceId;//�豸��ַ
    uSendBuf[16]=1;//�쳣����
    uSendBuf[20]=1;
    DbgPrintf("[PLC]Send Alarm Act Device is %d,Status 1 is %d,Status 2 is %d\r\n",uDeviceId,PLC0ConnectiveStatus,PLC1ConnectiveStatus);
    Socket_Send(uSendBuf,21);

}
void PlcAlarmEraseSend(UINT8 uCom)
{
    UINT8 uDeviceId;
    UINT8 uSendBuf[21]={0};
    uDeviceId=249+uCom-1;
    uSendBuf[0]=0x68;
    uSendBuf[1]=0x13;//����
    uSendBuf[6]=0xBF;
    uSendBuf[7]=0x01;
    uSendBuf[8]=0x06;//����ԭ��
    uSendBuf[10]=0x01;//������ַ
    uSendBuf[15]=uDeviceId;//�豸��ַ
    uSendBuf[16]=1;//�쳣����
    uSendBuf[20]=0;
    DbgPrintf("[PLC]Send Alarm Erase Device is %d,Status 1 is %d,Status 2 is %d\r\n",uDeviceId,PLC0ConnectiveStatus,PLC1ConnectiveStatus);
    Socket_Send(uSendBuf,21);
}
UINT8 PlcGetAlarmStatus()
{
    UINT8 uCom;
    UINT8 uStatus[18];
    UINT8 uAlarm;
    //PlcAlarmfd=open(PLCALARMPATH,O_RDONLY);
    FILE *fp=fopen(PLCALARMPATH,"r");
    fgets((void *)uStatus,10,fp);
    uCom=uStatus[15];
    uAlarm=uStatus[17];
    DbgPrintf("[PLC]Alarm Status is %d\r\n",uAlarm);
    fclose(fp);
    return uAlarm;
}
/*void PlcIni()
{
    FILE * fp;
    fp=fopen(PLCPATH,"r");
    if(fp==NULL)
    {
        printf("[PLC]Create File \r\n");
        fp=fopen(PLCPATH,"w+");
        //fprintf(fp,"[PLC1]\n");
        if(fp==0)
        {
            printf("[PLC]Create File Fail!!!\r\n");
            return 0;
        }
        else
        {
            fwrite("[PLC1]",strlen("[PLC1]"),1,fp);
            fclose(fp);

            DEBUG("[PLC]Ini First Set\r\n");
            PlcCnf=cnf_read_config(PLCPATH, '#', '=');
            if(PlcCnf==NULL)
                {
                    printf("[PLC]PlcCnf is NULL \r\n");
                    return ;
                }
            print_config(PlcCnf);
            cnf_add_option(PlcCnf,"PLC1","Tap","0");//0:���豸δ��վ 1�����豸�ѿ�վ�ɹ�
            cnf_add_option(PlcCnf,"PLC1","Port","0");//�����˿ڣ� 0--COM1  1--COM2
            cnf_add_option(PlcCnf,"PLC1","Alarm","0");//0 �� �޸澯 1�������澯
            cnf_add_option(PlcCnf,"PLC1","Esn","0");//Esn ��
            cnf_add_option(PlcCnf,"PLC1","Steam","0");//���������汾��

            cnf_add_option(PlcCnf,"PLC2","Tap","0");//0:���豸δ��վ 1�����豸�ѿ�վ�ɹ�
            cnf_add_option(PlcCnf,"PLC2","Port","0");//�����˿ڣ� 0--COM1  1--COM2
            cnf_add_option(PlcCnf,"PLC2","Alarm","0");//0 �� �޸澯 1�������澯
            cnf_add_option(PlcCnf,"PLC2","Esn","0");//Esn ��
            cnf_add_option(PlcCnf,"PLC2","Steam","0");//���������汾��
            p_erase_ini(PLCPATH);
            cnf_write_file(PlcCnf,PLCPATH,"Text");
        }
    }
    else
    {
        close(fp);
    }
    PlcCnf=cnf_read_config(PLCPATH, '#', '=');
    print_config(PlcCnf);
}*/

void MoveFile(UINT8 type, char *file_name, char *path)
{
    char file_path_temp[50],file_path[50],cmd_line[50];
    UINT32 run_version;

    if(type == UPDATA_FILE)
    {
        run_version = SYS_OAM_VERSION;
        E2promWrite((UINT8 *)&run_version, DeviceSwOldVerAddr, 3);
        usleep(1000);
        system("rm /mnt/flash/OAM/OAM_PRE");
        DbgPrintf("[Move File] cmd_line = rm /mnt/flash/OAM/OAM_PRE\n");
        system("mv /mnt/flash/OAM/OAM /mnt/flash/OAM/OAM_PRE");
        DbgPrintf("[Move File] cmd_line = mv /mnt/flash/OAM/OAM /mnt/flash/OAM/OAM_PRE\n");
    }
    else if(type == CERT_FILE)
    {
        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "rm -rf %s", CERT_FOLDER_PATH);
        DbgPrintf("[Move File] cmd_line = %s\n", cmd_line);
        system(cmd_line);

        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "mkdir %s", CERT_FOLDER_PATH);
        DbgPrintf("[Move File] cmd_line = %s\n", cmd_line);
        system(cmd_line);
    }
    else if(type == THIRDPARTYCONFIG_FILE)
    {
        if(access(THIRD_PARTY_FOLDER, F_OK) == -1)
        {
            memset(cmd_line, 0, sizeof(cmd_line));
            sprintf(cmd_line, "mkdir %s", THIRD_PARTY_FOLDER);
            DbgPrintf("[Move File] cmd_line = %s\n", cmd_line);
            system(cmd_line);
        }
    }

    sprintf(file_path_temp, "%s/%s", IMPORT_FOLDER_PATH, file_name);

    if(access(file_path_temp, F_OK) == 0)
    {
        sprintf(file_path, "%s/%s", path, file_name);
        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "mv %s %s", file_path_temp, file_path);
        DbgPrintf("[Move File] cmd_line = %s\n", cmd_line);
        system(cmd_line);

        sprintf(file_path, "%s/%s", path, file_name);
        memset(cmd_line, 0, sizeof(cmd_line));
        sprintf(cmd_line, "chmod 777 %s", file_path);
        DbgPrintf("[Move File] cmd_line = %s\n", cmd_line);
        system(cmd_line);
    }
    else
    {
        DbgPrintf("[Move File] no file %s\n", file_path_temp);
    }

    if(type == UPDATA_FILE)
    {
        DbgPrintf("[Move File] reboot system\n");
        system("ls /mnt/flash/OAM/");
        system("reboot -f");
    }
    else if(type == CERT_FILE)
    {
        DbgPrintf("[Move File] upload certificate\n");
        UploadCertificate();
        DbgPrintf("[Move File] upload certificate finish\n");
    }
}

//��ʱ���� andre 2019.3.8
void *PeriodicResetThread()
{

    UINT8 a,hour,min,protect_hour,protect_min,i;
    UINT16 protect_count=0;
    time_t timep;
    struct tm *pTime;

    srand((unsigned)time(NULL));
    a=rand()%180+1;
    hour=a/60;
    min=a%60;

    time(&timep);
    pTime=gmtime(&timep);
    protect_hour=pTime->tm_hour+3;
    protect_min=pTime->tm_min;

    for(protect_count=0;protect_count<1080;protect_count++)
    {
        //printf("[PeriodicResetThread]Protect time until %02d:%02d\n",protect_hour,protect_min);
        //printf("[PeriodicResetThread]gTypePointClearFlag=%d, gPointTablePossessFlag=%d\n",gTypePointClearFlag,gPointTablePossessFlag);
        //printf("[PeriodicResetThread]g_nRemotesockfd=%d\n",g_nRemotesockfd);
        if((gTypePointClearFlag==1)&&(gPointTablePossessFlag==0))
        {
            TypePointClear();
            //printf("555Erase Point Table gTypeHead");
            gTypeHead=gTypeHeadBuf;
            gTypeHeadBuf=NULL;
            PointInfoFileWrite();

            gDeviceTypeNum=gDeviceTypeNumBuf;
            for(i=0;i<gDeviceTypeNumBuf;i++)
            {
                E2promWrite((UINT8 *)&gDeviceTypeBuf[i],TypeTableInfoAddr+i*2,2);
            }
            gDeviceTypeNumBuf=0;
            E2promWrite((UINT8 *)&gDeviceTypeNum,TypeNumberAddr,1);

            gConnectDeviceNum=gConnectDeviceNumBuf;
            gConnectDeviceNumBuf=0;
            E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);

            memcpy((UINT8 *)&gYXPointNum,(UINT8 *)&gYXPointNumBuf,2);
            gYXPointNumBuf=0;
            E2promWrite((UINT8 *)&gYXPointNum,DeviceYXNumberAddr,2);

            memcpy((UINT8 *)&gYCPointNum,(UINT8 *)&gYCPointNumBuf,2);
            gYCPointNumBuf=0;
            E2promWrite((UINT8 *)&gYCPointNum,DeviceYCNumberAddr,2);

            memcpy((UINT8 *)&gYKPointNum,(UINT8 *)&gYKPointNumBuf,2);
            gYKPointNumBuf=0;
            E2promWrite((UINT8 *)&gYKPointNum,DeviceYKNumberAddr,2);

            memcpy((UINT8 *)&gSDPointNum,(UINT8 *)&gSDPointNumBuf,2);
            gSDPointNumBuf=0;
            E2promWrite((UINT8 *)&gSDPointNum,DeviceSDNumberAddr,2);

            memcpy((UINT8 *)&gDDPointNum,(UINT8 *)&gDDPointNumBuf,2);
            gSDPointNumBuf=0;
            E2promWrite((UINT8 *)&gDDPointNum,DeviceDDNumberAddr,2);

            gMainDeviceStatus = DEVSTATUS_NORMAL_WORKMODE;
            E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
            gMBQueryDeviceCountBuf=0;
            //gTypeGroupPointMB=NULL;
            if(gModuleChannel1InitFlag == 2)
                ReportDeviceInfoToThirdPartyServer(1);
        }
        sleep(10);
    }

    while(1)
    {
        //printf("[PeriodicResetThread]System reset at %02d:%02d\n",hour+1,min);
        time(&timep);
        pTime=gmtime(&timep);
        if((pTime->tm_hour==(hour+1))&&(pTime->tm_min==min))
        {
            sleep(1);
            system("reboot -f");
        }
        if((gTypePointClearFlag==1)&&(gPointTablePossessFlag==0))
        {
            TypePointClear();
            //printf("555Erase Point Table gTypeHead");
            gTypeHead=gTypeHeadBuf;
            gTypeHeadBuf=NULL;
            PointInfoFileWrite();

            gDeviceTypeNum=gDeviceTypeNumBuf;
            for(i=0;i<gDeviceTypeNumBuf;i++)
            {
                E2promWrite((UINT8 *)&gDeviceTypeBuf[i],TypeTableInfoAddr+i*2,2);
            }
            gDeviceTypeNumBuf=0;
            E2promWrite((UINT8 *)&gDeviceTypeNum,TypeNumberAddr,1);

            gConnectDeviceNum=gConnectDeviceNumBuf;
            gConnectDeviceNumBuf=0;
            E2promWrite((UINT8 *)&gConnectDeviceNum,DeviceNumberAddr,1);

            memcpy((UINT8 *)&gYXPointNum,(UINT8 *)&gYXPointNumBuf,2);
            gYXPointNumBuf=0;
            E2promWrite((UINT8 *)&gYXPointNum,DeviceYXNumberAddr,2);

            memcpy((UINT8 *)&gYCPointNum,(UINT8 *)&gYCPointNumBuf,2);
            gYCPointNumBuf=0;
            E2promWrite((UINT8 *)&gYCPointNum,DeviceYCNumberAddr,2);

            memcpy((UINT8 *)&gYKPointNum,(UINT8 *)&gYKPointNumBuf,2);
            gYKPointNumBuf=0;
            E2promWrite((UINT8 *)&gYKPointNum,DeviceYKNumberAddr,2);

            memcpy((UINT8 *)&gSDPointNum,(UINT8 *)&gSDPointNumBuf,2);
            gSDPointNumBuf=0;
            E2promWrite((UINT8 *)&gSDPointNum,DeviceSDNumberAddr,2);

            memcpy((UINT8 *)&gDDPointNum,(UINT8 *)&gDDPointNumBuf,2);
            gSDPointNumBuf=0;
            E2promWrite((UINT8 *)&gDDPointNum,DeviceDDNumberAddr,2);

            gMainDeviceStatus = DEVSTATUS_NORMAL_WORKMODE;
            E2promWrite((UINT8 *)&gMainDeviceStatus,DeviceStatusAddr,1);
            gMBQueryDeviceCountBuf=0;
            //gTypeGroupPointMB=NULL;
            if(gModuleChannel1InitFlag == 2)
                ReportDeviceInfoToThirdPartyServer(1);
        }
        sleep(10);
    }
}


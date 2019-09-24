/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : Port_Fun.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/ConstDef.h"
#include "../include/DataTypeDef.h"
#include "../protocal/ProtocolDef.h"
#include "../protocal/ToolExtern.h"
#include "../TPMsgDef/TPMsgDef.h"
#include "../ThreadPool/SPThreadExtern.h"
#include "../ThreadPool/SPThreadPoolExtern.h"
#include "../ThreadPool/SPThreadPool.h"
#include "../interface/interfaceExtern.h"

extern Struct_ThreadPool g_ThreadPool;      //threadpool global identification
extern int g_nRemotesockfd;
extern UINT8 gBufCount;
extern UINT8 gIBufCount;
extern UINT8 g_TransSta;
extern UINT16 gSendCount;
extern UINT8 gSocketMode;
extern struct sSocketRecvBuf *gRecvBufHead;
extern struct sSocketRecvBuf *gRecvBufPoint;
extern struct sSocketRecvBuf *gRecvIBufHead;
extern struct sSocketRecvBuf *gRecvIBufPoint;
extern UINT8 gTotalCallFlag;
extern UINT8 gTimeTotalCallFlag;
extern UINT8 gEleTotalCallFlag;
extern void TotalCallPacket();
extern void TotalTimeCallPacket();

/*****************************************************************************
* Description: 		�����յ����ݴ������ݴ洢����
* Parameters:        nType: 1:I֡  0:S/U֡
                     aBuf:  ���յ�����
                     nLen:  ���յ����ݳ���
* Returns:
*****************************************************************************/
void RecvBufferAdd(UINT8 nType,UINT8 *aBuf,UINT8 nLen)
{
	UINT8 j;
	struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    pRecvPoint=(struct sSocketRecvBuf *)malloc(sizeof(struct sSocketRecvBuf));/*�������ݿռ�*/
    memcpy(pRecvPoint->aDataBuf,aBuf,nLen);
    pRecvPoint->nLen=nLen;
    pRecvPoint->nType=nType;
    pRecvPoint->pNext=NULL;
    pRecvPoint->pPre=NULL;
    if(gRecvBufHead==NULL)/*�洢�����޻�������,ֱ�ӽ����ݼ���洢��*/
    {
        gRecvBufHead=pRecvPoint;
        gRecvBufPoint=pRecvPoint;
    }
    else/*��������,�����ݷ���洢��ĩβ*/
    {
        gRecvBufPoint->pNext=pRecvPoint;
        pRecvPoint->pPre=gRecvBufPoint;
        gRecvBufPoint=pRecvPoint;
    }
    gBufCount++;/*�洢�����ݼ����ۼ�*/
    NorthPrintf("Save %d Byte:",pRecvPoint->nLen);
    for(j=0;j<pRecvPoint->nLen;j++)
    {
        NorthPrintf("%02X ",pRecvPoint->aDataBuf[j]);
    }
    NorthPrintf("to Buffer No.%d\r\n",gBufCount);
    pthread_mutex_unlock(&socketbufsem);
}

/*****************************************************************************
* Description:         ���������ݴ洢���е�����
* Parameters:
* Returns:
*****************************************************************************/
void RecvBufferDeal(void)
{
    UINT8 nRes=0,nFlag=0;
    StrSocketRecvInfo aDataTemp;
    struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    nFlag=0;/*0:δ��ȡ������ 1:��ȡ������*/
    pRecvPoint = gRecvBufHead;
    while(pRecvPoint!=NULL)/*�����ݴ洢����Ϊ��ʱ*/
    {
        //if((g_TransSta==0)||(pRecvPoint->nType==0))/*����������״̬Ϊ��������ʱ*/
        //if(pRecvPoint->nType==0)
        {
            DbgPrintf("Buffer %03d Byte to be Deal:",pRecvPoint->nLen);
            gBufCount--;/*�洢�����ݼ�������*/
            memcpy((UINT8 *)&aDataTemp.aDataBuf,(UINT8 *)&pRecvPoint->aDataBuf,pRecvPoint->nLen);
            aDataTemp.nType=pRecvPoint->nType;
            aDataTemp.nLen=pRecvPoint->nLen;
            if((pRecvPoint->pPre==NULL)&&(pRecvPoint->pNext==NULL))/*��������Ϊ�洢�����һ������ʱ*/
            {
                gRecvBufHead=NULL;
                gRecvBufPoint=NULL;
            }
            else if(pRecvPoint->pPre==NULL)/*��������Ϊ�洢�����ݶ��е�һ��*/
            {
                gRecvBufHead=pRecvPoint->pNext;
                gRecvBufHead->pPre=NULL;
            }
            else if(pRecvPoint->pNext==NULL)/*��������Ϊ�洢�����ݶ���ĩβ*/
            {
                gRecvBufPoint=pRecvPoint->pPre;
                gRecvBufPoint->pNext=NULL;
            }
            else/*��������Ϊ�洢�����ݶ����м�*/
            {
                pRecvPoint->pPre->pNext=pRecvPoint->pNext;
                pRecvPoint->pNext->pPre=pRecvPoint->pPre;
            }
            nFlag=1;
            free(pRecvPoint);
            break;
        }
        /*else if((g_TransSta!=1))//����ʱ����������
        {
            pRecvPoint=pRecvPoint->pNext;
        }*/
    }
    pthread_mutex_unlock(&socketbufsem);
    if(nFlag)
    {
        nRes=PackMainFunction(aDataTemp.nType,aDataTemp.aDataBuf,aDataTemp.nLen);/*���ô�����*/
    }
}

/*****************************************************************************
* Description:     �߳�:��ⱱ�����ݴ洢��������
* Parameters:
* Returns:
*****************************************************************************/
void *SocketRecvbufThread(void)
{
    while(1)
    {
        /**
        *  ��Pro_Monitor�߳��л�ͨ����̫�����Կͻ��˵��������ƽ̨������ɹ���g_nRemotesockfd����һ����0ֵ.
        */
        if((g_nRemotesockfd!=0) ||(gSocketMode!=0))/*����ģʽ���������� ���� ����ģʽ*/
        {
            RecvBufferDeal();/*�������ݴ洢������*/
            if((gTotalCallFlag>0)||(gEleTotalCallFlag>0))/*��������״̬*/
            {
                //if((gSendCount>0)&&(gSendCount%8!=0))
                {
                    TotalCallPacket();/*�������ٴ���*/
                }
            }
            if(gTimeTotalCallFlag>0)/*���ڲ���״̬*/
            {
                {
                    TotalTimeCallPacket();/*���в��ɴ���*/
                }
            }
        }
        usleep(100);
    }
}

/*****************************************************************************
* Description: �߳�:����Socket���յ�������
* Parameters:
* Returns:
*****************************************************************************/
void *Fun_Thread_Port(void *param)
{
    int nPos = (int)param;
    Struct_SPThreadInofPtr pThreadInfo = &g_ThreadPool.arrThreadType_Port[nPos]; //get thread info
    Struct_Port_Msg message;                     //get message from port  TPMsgDef.h
    int pid=pThreadInfo->nMsgQueueID;
    UINT8 aTemp[SYS_FRAME_LEN];

    while(1)
    {
        bzero(message.PortMsgBuf, MaxMsgBufLength);
        //get info from port queue
        if((msgrcv(pid,(void *)&message,Port_Msg_Length(Struct_Port_Msg),0,0))<0)  /*��ȡ�����̴߳�����������*/
        {
            DbgPrintf("logic_prs msgrcv error!\n");
            exit(1);
        }

        int nLen;
        nLen = 0;
        do
        {
            UINT8 nTempLen;

            memset(aTemp,sizeof(aTemp),0);
            nTempLen=message.PortMsgBuf[nLen+1];
            memcpy(aTemp,(UINT8 *)&message.PortMsgBuf[nLen],nTempLen+2);
            gSocketHeartChannel0Count=0;
            if((aTemp[2]&0x01)==0)/*I֡�Ĵ���*/
            {
                RecvBufferAdd(1,aTemp,nTempLen+2);
            }
            else/*S֡�Ĵ���*/
            {
                RecvBufferAdd(0,aTemp,nTempLen+2);
            }
            nLen = nLen+nTempLen+2;
        }while(nLen<message.nDateLength);
    }
}

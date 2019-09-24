/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : Port_Fun.c
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
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
* Description: 		将接收的数据存入数据存储区中
* Parameters:        nType: 1:I帧  0:S/U帧
                     aBuf:  接收的数据
                     nLen:  接收的数据长度
* Returns:
*****************************************************************************/
void RecvBufferAdd(UINT8 nType,UINT8 *aBuf,UINT8 nLen)
{
	UINT8 j;
	struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    pRecvPoint=(struct sSocketRecvBuf *)malloc(sizeof(struct sSocketRecvBuf));/*申请数据空间*/
    memcpy(pRecvPoint->aDataBuf,aBuf,nLen);
    pRecvPoint->nLen=nLen;
    pRecvPoint->nType=nType;
    pRecvPoint->pNext=NULL;
    pRecvPoint->pPre=NULL;
    if(gRecvBufHead==NULL)/*存储区中无缓存数据,直接将数据加入存储区*/
    {
        gRecvBufHead=pRecvPoint;
        gRecvBufPoint=pRecvPoint;
    }
    else/*存在数据,则将数据放入存储区末尾*/
    {
        gRecvBufPoint->pNext=pRecvPoint;
        pRecvPoint->pPre=gRecvBufPoint;
        gRecvBufPoint=pRecvPoint;
    }
    gBufCount++;/*存储区数据计数累加*/
    NorthPrintf("Save %d Byte:",pRecvPoint->nLen);
    for(j=0;j<pRecvPoint->nLen;j++)
    {
        NorthPrintf("%02X ",pRecvPoint->aDataBuf[j]);
    }
    NorthPrintf("to Buffer No.%d\r\n",gBufCount);
    pthread_mutex_unlock(&socketbufsem);
}

/*****************************************************************************
* Description:         处理北向数据存储区中的数据
* Parameters:
* Returns:
*****************************************************************************/
void RecvBufferDeal(void)
{
    UINT8 nRes=0,nFlag=0;
    StrSocketRecvInfo aDataTemp;
    struct sSocketRecvBuf *pRecvPoint=NULL;

    pthread_mutex_lock(&socketbufsem);
    nFlag=0;/*0:未获取到数据 1:获取到数据*/
    pRecvPoint = gRecvBufHead;
    while(pRecvPoint!=NULL)/*当数据存储区不为空时*/
    {
        //if((g_TransSta==0)||(pRecvPoint->nType==0))/*当数采运行状态为正常工作时*/
        //if(pRecvPoint->nType==0)
        {
            DbgPrintf("Buffer %03d Byte to be Deal:",pRecvPoint->nLen);
            gBufCount--;/*存储区数据计数减少*/
            memcpy((UINT8 *)&aDataTemp.aDataBuf,(UINT8 *)&pRecvPoint->aDataBuf,pRecvPoint->nLen);
            aDataTemp.nType=pRecvPoint->nType;
            aDataTemp.nLen=pRecvPoint->nLen;
            if((pRecvPoint->pPre==NULL)&&(pRecvPoint->pNext==NULL))/*此条数据为存储区最后一条数据时*/
            {
                gRecvBufHead=NULL;
                gRecvBufPoint=NULL;
            }
            else if(pRecvPoint->pPre==NULL)/*此条数据为存储区数据队列第一条*/
            {
                gRecvBufHead=pRecvPoint->pNext;
                gRecvBufHead->pPre=NULL;
            }
            else if(pRecvPoint->pNext==NULL)/*此条数据为存储区数据队列末尾*/
            {
                gRecvBufPoint=pRecvPoint->pPre;
                gRecvBufPoint->pNext=NULL;
            }
            else/*此条数据为存储区数据队列中间*/
            {
                pRecvPoint->pPre->pNext=pRecvPoint->pNext;
                pRecvPoint->pNext->pPre=pRecvPoint->pPre;
            }
            nFlag=1;
            free(pRecvPoint);
            break;
        }
        /*else if((g_TransSta!=1))//总召时不处理数据
        {
            pRecvPoint=pRecvPoint->pNext;
        }*/
    }
    pthread_mutex_unlock(&socketbufsem);
    if(nFlag)
    {
        nRes=PackMainFunction(aDataTemp.nType,aDataTemp.aDataBuf,aDataTemp.nLen);/*调用处理函数*/
    }
}

/*****************************************************************************
* Description:     线程:检测北向数据存储区中数据
* Parameters:
* Returns:
*****************************************************************************/
void *SocketRecvbufThread(void)
{
    while(1)
    {
        /**
        *  在Pro_Monitor线程中会通过以太网口以客户端的身份连接平台，如果成功，g_nRemotesockfd就是一个非0值.
        */
        if((g_nRemotesockfd!=0) ||(gSocketMode!=0))/*有线模式并且已连接 或者 无线模式*/
        {
            RecvBufferDeal();/*处理数据存储区数据*/
            if((gTotalCallFlag>0)||(gEleTotalCallFlag>0))/*处于总召状态*/
            {
                //if((gSendCount>0)&&(gSendCount%8!=0))
                {
                    TotalCallPacket();/*进行总召处理*/
                }
            }
            if(gTimeTotalCallFlag>0)/*处于补采状态*/
            {
                {
                    TotalTimeCallPacket();/*进行补采处理*/
                }
            }
        }
        usleep(100);
    }
}

/*****************************************************************************
* Description: 线程:处理Socket接收到的数据
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
        if((msgrcv(pid,(void *)&message,Port_Msg_Length(Struct_Port_Msg),0,0))<0)  /*获取接收线程传过来的数据*/
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
            if((aTemp[2]&0x01)==0)/*I帧的处理*/
            {
                RecvBufferAdd(1,aTemp,nTempLen+2);
            }
            else/*S帧的处理*/
            {
                RecvBufferAdd(0,aTemp,nTempLen+2);
            }
            nLen = nLen+nTempLen+2;
        }while(nLen<message.nDateLength);
    }
}

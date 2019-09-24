/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : store.c
* Author            : Alex
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

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
#include <time.h>
#include "protocol.h"
#include "../protocal/ProtocolDef.h"
#include "../interface/uartExtern.h"
#include "../Ethernet/EthernetExtern.h"

struct sThirdPartyFrameBuffer *p_sgcc_send_msg_head, *p_sgcc_send_msg_point, *p_sgcc_recv_msg_head, *p_sgcc_recv_msg_point;

extern UINT8 gModuleChannel1InitFlag;
extern int g_ethernet_connect_sub;

/*****************************************************************************
* Description: 		������ͻ�����
* Parameters:
* Returns:
*****************************************************************************/
void ClearMsgFromSGCCSendBuff(void)
{
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    while(p_sgcc_send_msg_buff != NULL)
    {
        p_sgcc_send_msg_head = p_sgcc_send_msg_buff->p_next;
        p_sgcc_send_msg_buff->p_next = NULL;
        free(p_sgcc_send_msg_buff);
        p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    }
    p_sgcc_send_msg_head = NULL;
    p_sgcc_send_msg_point = NULL;
    pthread_mutex_unlock(&sgcc_send_buff_sem);
}

/*****************************************************************************
* Description: 		��ӷ��ͱ��������ͻ�����
* Parameters:		pipe:����ͨ��
					p_msg_buff:���ͱ���
					length:���ͱ��ĳ���
* Returns:
*****************************************************************************/
UINT8 AddMsgToSGCCSendBuff(UINT8 *p_msg_buff, UINT8 length)
{
    UINT8 res = 1,i;
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    /*pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = (struct sThirdPartyFrameBuffer *)malloc(sizeof(struct sThirdPartyFrameBuffer));//��������ڵ�ռ�
    p_sgcc_send_msg_buff->msg_length = length;
    memset(p_sgcc_send_msg_buff->msg, 0, sizeof(p_sgcc_send_msg_buff->msg));
    memcpy(p_sgcc_send_msg_buff->msg, p_msg_buff, length);
    p_sgcc_send_msg_buff->p_next = NULL;

    NorthPrintf("**add send buff**");
    for(i =0 ;i<length;i++)
    {
        NorthPrintf("%02X ",p_msg_buff[i]);
    }
    NorthPrintf("\r\n");
    if(p_sgcc_send_msg_head == NULL)
    {*/
        /*������*/
        /*p_sgcc_send_msg_head = p_sgcc_send_msg_buff;
        p_sgcc_send_msg_point = p_sgcc_send_msg_buff;
    }
    else
    {*/
        /*�ǿ������������β��*/
        /*p_sgcc_send_msg_point->p_next = p_sgcc_send_msg_buff;
        p_sgcc_send_msg_point = p_sgcc_send_msg_buff;
    }
    pthread_mutex_unlock(&sgcc_send_buff_sem);*/
    return res;
}

/*****************************************************************************
* Description: 		�ӱ����ͻ������л�ȡ�����͵ı���
* Parameters:		p_msg_buff:���ͱ���
					p_length:���ͱ��ĳ���
* Returns:          0:������      ����ֵ:���͵�socket id
*****************************************************************************/
INT32 GetMsgFromSGCCSendBuff(UINT8 *p_msg_buff,UINT8 *p_length)
{
    INT32 res = 0;
    struct sThirdPartyFrameBuffer *p_sgcc_send_msg_buff;

    pthread_mutex_lock(&sgcc_send_buff_sem);
    p_sgcc_send_msg_buff = p_sgcc_send_msg_head;
    while(p_sgcc_send_msg_buff != NULL)
    {
        if(1)
        {
            memcpy(p_msg_buff, p_sgcc_send_msg_buff->msg, p_sgcc_send_msg_buff->msg_length);
            memcpy(p_length, (UINT16 *)&p_sgcc_send_msg_buff->msg_length, sizeof(p_sgcc_send_msg_buff->msg_length));
            if(p_sgcc_send_msg_buff->p_next == NULL)
            {
                p_sgcc_send_msg_head = NULL;
                p_sgcc_send_msg_point = NULL;
            }
            else
            {
                p_sgcc_send_msg_head = p_sgcc_send_msg_buff->p_next;
            }
            free(p_sgcc_send_msg_buff);
            break;
        }
    }
    pthread_mutex_unlock(&sgcc_send_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		�����������������
* Parameters:		p_msg_buff:���ͱ���
					p_length:���ͱ��ĳ���
* Returns:          1:��ӳɹ�
*****************************************************************************/
UINT8 AddMsgToSGCCReceiveBuff(UINT8 *p_msg_buff,UINT8 length)
{
    UINT8 res = 1;
    struct sThirdPartyFrameBuffer *p_sgcc_recv_msg_buff;

    pthread_mutex_lock(&sgcc_recv_buff_sem);
    p_sgcc_recv_msg_buff = (struct sThirdPartyFrameBuffer *)malloc(sizeof(struct sThirdPartyFrameBuffer));
    p_sgcc_recv_msg_buff->msg_length = length;
    memset(p_sgcc_recv_msg_buff->msg, 0, sizeof(p_sgcc_recv_msg_buff->msg));
    memcpy(p_sgcc_recv_msg_buff->msg, p_msg_buff, length);
    p_sgcc_recv_msg_buff->p_next = NULL;
    if(p_sgcc_recv_msg_head == NULL)
    {
        p_sgcc_recv_msg_head = p_sgcc_recv_msg_buff;
        p_sgcc_recv_msg_point = p_sgcc_recv_msg_buff;
    }
    else
    {
        p_sgcc_recv_msg_point->p_next = p_sgcc_recv_msg_buff;
        p_sgcc_recv_msg_point = p_sgcc_recv_msg_buff;
    }
    pthread_mutex_unlock(&sgcc_recv_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		�ӽ��ջ������л�ȡ�ѽ��յı���
* Parameters:		p_msg_buff:���ͱ���
					p_length:���ͱ��ĳ���
* Returns:          0:������      ����ֵ:���յ�socket id
*****************************************************************************/
UINT8 GetMsgFromSGCCReceiveBuff(UINT8 *p_msg_buff,UINT8 *p_length)
{
    UINT8 res = 0;
    struct sThirdPartyFrameBuffer *p_sgcc_recv_msg_buff;

    pthread_mutex_lock(&sgcc_recv_buff_sem);
    p_sgcc_recv_msg_buff = p_sgcc_recv_msg_head;
    if(p_sgcc_recv_msg_buff != NULL)
    {
        memcpy(p_msg_buff, p_sgcc_recv_msg_buff->msg, p_sgcc_recv_msg_buff->msg_length);
       	memcpy(p_length, (UINT8 *)&p_sgcc_recv_msg_buff->msg_length, sizeof(p_sgcc_recv_msg_buff->msg_length));
        if(p_sgcc_recv_msg_buff->p_next == NULL)
        {
            p_sgcc_recv_msg_head = NULL;
            p_sgcc_recv_msg_point = NULL;
        }
        else
        {
            p_sgcc_recv_msg_head = p_sgcc_recv_msg_buff->p_next;
        }
        free(p_sgcc_recv_msg_buff);
        res = 1;
    }
    pthread_mutex_unlock(&sgcc_recv_buff_sem);
    return res;
}

/*****************************************************************************
* Description: 		�߳�-���ձ��Ĵ����߳�
* Parameters:
* Returns:
*****************************************************************************/
void *SGCCSocketReceiveThread()
{
    UINT8 recv_buff[2000];
    UINT8 recv_length = 0;

    while(1)
    {
        if(GetMsgFromSGCCReceiveBuff(recv_buff, (UINT8 *)&recv_length))
        {
        	/**�����д��ڴ�������*/
            UINT8 recv_msg_count = 0;

            while(recv_msg_count < recv_length)
            {
                struct MsgSGCCStruct recv_msg_struct;
                UINT8 send_temp[255], recv_temp[255];
                UINT8 send_length_temp = 0, recv_length_temp = 0, i;

                while(recv_msg_count <= recv_length)
                {
                    if(recv_buff[recv_msg_count] == 0x68)//���Ļ������ҵ���ʼ��0x68
                        break;
                    else
                        recv_msg_count++;
                }
                if((recv_buff[recv_msg_count + 1] + 2) > (recv_length - recv_msg_count))//�����ȴ��ڻ���ʣ������
                    break;
                recv_length_temp = recv_buff[recv_msg_count + 1] + 2;
                memcpy(recv_temp, (UINT8 *)&recv_buff[recv_msg_count], recv_length_temp);
                //if(recv_length != recv_length_temp)
                {
                    NorthPrintf("**South recv**");
                    for(i =0 ;i<recv_length_temp;i++)
                    {
                        NorthPrintf("%02X ",recv_temp[i]);
                    }
                    NorthPrintf("\r\n");
                }
                DecodeMsg(recv_temp, recv_length_temp, (struct MsgSGCCStruct *)&recv_msg_struct);//��������
                send_length_temp = DealPacket(recv_msg_struct, send_temp);//������
                recv_msg_count += recv_length_temp;
                if(send_length_temp >=6)
                    AddMsgToSGCCSendBuff(send_temp, send_length_temp);
            }
        }
        //usleep(50000);
        usleep(10);
    }
    pthread_testcancel();
}

void *SendSGCCMsgThread()
{
    while(1)
    {
        UINT8 socket_send_buff[SGCC_MSG_MAX_LENGTH], socket_send_length=0;

        if(1)//��������
        {
            //GetMsgFromSGCCSendBuff(socket_send_buff, &socket_send_length);//�ӻ������л�ȡ����
            if(socket_send_length>0)
            {

                if(gModuleChannel1InitFlag == 2)
                {
                    pthread_mutex_lock(&modemsem);
                    usleep(200000);
                    ModemSend(1,socket_send_buff,socket_send_length);
                    pthread_mutex_unlock(&modemsem);
                }
                else if(g_ethernet_connect_sub == 1)
                {
                    EthernetSend(socket_send_buff, socket_send_length,0,1);
                }
                else if(g_ethernet_connect_status == 1)
                {
                    EthernetSend(socket_send_buff, socket_send_length,1,1);
                }
            }
        }
        usleep(1000);
    }
}


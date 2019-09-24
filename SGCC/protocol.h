/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : protocol.h
* Author            : Alex
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include "../interface/common.h"

#define SGCC_MSG_MAX_LENGTH		255
#define SGCC_PROTOCOL_HEAD_LENGTH   12
/*
#define SGCC_CMD_LOGOIN			0x00
#define SGCC_CMD_REGISTER		0x01
#define SGCC_CMD_REPORT_DATA	0x04
#define SGCC_CMD_RESUME_REPORT	0x05
#define SGCC_CMD_REPORT_ALARM	0x06
#define SGCC_CMD_REAL_TIME_DATE	0x10
#define SGCC_CMD_HISTORY_DATE	0x11
#define SGCC_CMD_TIME			0x21
#define SGCC_CMD_HEART_PACKET	0x99*/
#define SGCC_PACKET_TYPE_I                 0
#define SGCC_PACKET_TYPE_S                 1
#define SGCC_PACKET_TYPE_U                 3


#define SGCC_PROTOCOL_HEAD_ORIGIN          0
#define SGCC_PROTOCOL_LENGTH_ORIGIN        1
#define SGCC_PROTOCOL_SEND_ORIGIN          2
#define SGCC_PROTOCOL_RECEVIE_ORIGIN       4
#define SGCC_PROTOCOL_CMD_ORIGIN           6
#define SGCC_PROTOCOL_DETERMINER_ORIGIN    7
#define SGCC_PROTOCOL_CAUSE_ORIGIN         8
#define SGCC_PROTOCOL_ADDRESS_ORIGIN       10
#define SGCC_PROTOCOL_MSG_ORIGIN           12
#define SGCC_PROTOCOL_DATA_ORIGIN          15


/**104��Լ���ݽṹ*/
struct MsgSGCCAnalysisStruct
{
    UINT8 length;//���ĳ���,������ͷ������
    UINT16 receive_msg_number;//���հ����
    UINT16 send_msg_number;//���Ͱ����
    UINT8 cmd_id;//�����ʶ
    UINT8 determiner;//�ṹ�޶���
    UINT16 transmission_cause;//����ԭ��
    UINT16 address;//������ַ
    UINT8 data_buff[SGCC_MSG_MAX_LENGTH];//���ݶ�
    UINT8 data_length;//���ݶγ���
}__attribute__((__packed__));

struct MsgSGCCStruct
{
    UINT8 socket_pipe;//ͨ��ͨ��
    UINT8 packet_type;//��������
    struct MsgSGCCAnalysisStruct msg_analysis;
    struct MsgSGCCStruct *p_next;
}__attribute__((__packed__));

extern UINT16 gYXPointSumTemp;
extern UINT16 gYCPointSumTemp;

extern void *TotalCallThread();
extern UINT8 DecodeMsg(UINT8 *msg_data, UINT8 data_length, struct MsgSGCCStruct *recv_msg_struct);
extern UINT8 DealPacket(struct MsgSGCCStruct recv_msg_struct, UINT8 *response_buff);


#endif /* LOGIC_PROCSEXTERN_H_ */


/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : ProtocolDef.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef PROTOCOLDEF_H_
#define PROTOCOLDEF_H_
#include <memory.h>
#include "../include/DataTypeDef.h"
#include "../include/ConstDef.h"
#include "../interface/common.h"
#pragma pack(push, 1)

typedef struct StrRemoteInfor
{
    UINT1 nEquipmentID;
    int fdSocket;
}__attribute__((__packed__)) StrRemoteInfor,*StrRemoteInforPtr;

typedef struct StrSmsInfor
{
    UINT1 nDataFlag;
    UINT1 aDataBuf[512];
    UINT1 nLen;
    UINT1 aPhoneNum[20];
}__attribute__((__packed__)) StrSmsInfor;

typedef struct StrSocketRecvInfo
{
    UINT1 nLen;
    UINT1 nType;
    UINT1 nFlag;
    UINT1 aDataBuf[256];
}__attribute__((__packed__)) StrSocketRecvInfo,*StrSocketRecvInfoPtr;

struct sSocketRecvBuf
{
    UINT1 nLen;
    UINT1 nType;
    UINT1 aDataBuf[256];
    struct sSocketRecvBuf *pNext;
    struct sSocketRecvBuf *pPre;
};

typedef struct
{
    UINT8 nInUse;/** 0:�豸������ 1���豸����*/
    UINT8 nEndian;/** ��С��, 1:С�� 0:��� */
    UINT16 nPointTableNo; /** ����� */
    UINT16 nType;
    UINT16 nYXAddr;
    UINT16 nYCAddr;
    UINT16 nYKAddr;
    UINT16 nSDAddr;
    UINT16 nDDAddr;
    UINT32 nRate;   /** ���䲨���� */
    UINT8 aESN[21]; /** �豸ESN�� */
    UINT8 nProtocolType;/** Э������, 1:��ΪMODBUS 0:��׼MODBUS */
    UINT8 nDownlinkPort;/** �豸���ӵ����ɵ�COM��, 1����COM1, 2����COM2*/
    UINT8 aSofeVersion[21];//���������汾��
}sDeviceInfo,*ptrsDeviceInfo;


typedef struct
{
    UINT8 nCmdID;
    UINT8 nMessNum;                 //variable structure qualifier
    UINT16 nSendFlag;
    UINT16 nPublicAddr;
    UINT32 nMessAddr;
}sASDU,*ptrsASDU;

typedef struct
{
    UINT8 nHead;
    UINT8 nPktLen;
    UINT16 nSendCtrl;
    UINT16 nRecvCtrl;
}sAPCI,*ptrsAPCI;

typedef struct
{
    sAPCI mAPCI;
    sASDU mASDU;
}sAPDU,*ptrsAPDU;

typedef struct
{
    UINT8 nLen;
    UINT8 nStatus;
    UINT8 nEnable;
    UINT32 nPreValue;
    UINT32 nValue;
}s104Point,*ptrs104Point;

typedef struct
{
    UINT8 nIVLicenseLoadFlag;
    UINT8 nIVLicenseReportLoadFlag;
    UINT8 nIVLicenseReportLoadCount;
    UINT8 nIVScanfLoadFlag;
}sIVStruct,*ptrsIVStruct;

union uIEEE754
{
    float nIndex;
    UINT8 nChar[4];
};
extern union uIEEE754 uIE;

union U32_F_Char_Convert
{
    float    f;
    UINT32   u;
    INT32    i;
    UINT16   u16;
    INT16    i16;
    UINT8    c[4];
};

extern union U32_F_Char_Convert U32_F_Char;


typedef struct
{
    UINT16 nMBAddr;
    UINT8  nLen;
    UINT8  nType;
}sTypePoint,*ptrsTypePoint;

struct sTypeParam
{
    UINT16 nMBAddr;
    UINT8  nLen;
    UINT8  nType;
    UINT8  nDataType;
    struct sTypeParam *pNext;
};

struct sTypeGroup
{
    UINT16 nTypeID;
    UINT8  nProtocalTypeID;
    struct sTypeGroup *pNext;
    struct sTypeParam *pParamNext;
};

struct sAlarmGroup
{
    UINT8 nDeviceID;
    UINT8 nAlarmID;
    UINT16 nModbusAddr;
    UINT8 nAlarmExternID;
    UINT8 nAlarmCount;                  //�澯������ֻ���ڶ����澯
    UINT8 nUploadStatusMasterChannel;   //��ͨ����      0:�澯δ�ϱ�       1��2��3:�澯���ϱ���δȷ��            5:�澯���ϱ�����ȷ��
                                        //6:�澯�ָ����ϱ�����ȷ��
    UINT8 nUploadStatusSlaveChannel;    //��ͨ����      0:�澯δ�ϱ�       1:�澯���ϱ���δȷ��           2:�澯���ϱ�����ȷ��
                                        //3:�澯�ָ�δ�ϱ� 4:�澯�ָ����ϱ���δȷ�� 5:�澯�ָ����ϱ�����ȷ��
    struct sAlarmGroup *pNext;
    struct sAlarmGroup *pPre;
};

//����Ϊ�������ƽ̨����Ľṹ�� Andre 2018.10.09
struct sThirdPartyStationInfo           //����վ����Ϣ������IP���˿ڡ�վ����
{
    UINT8  nIP[20];
    UINT16 nPort;
    UINT16 nStationID;
};

struct sThirdPartySinglePoint           //����������Ϣ������104��ַ���ӵ���
{
    UINT8  nType;
    UINT16 n104Addr;
    UINT8  nNumberOfSubPoint;
    struct sThirdPartySinglePoint *pNext;
    struct sThirdPartySubPoint *pSubPoint;
};

struct sThirdPartySubPoint              //�����ӵ���Ϣ�������������ַ��modbus��ַ
{
    UINT8  nAddr;
    UINT32 nModbus;
};

struct sThirdPartyFrameBuffer
{
    UINT8 msg[255];
    UINT8 msg_length;
    struct sThirdPartyFrameBuffer *p_next;
};

//����Ϊ�����Դ��Ŀ����Ľṹ�� Andre 2018.11.12
struct sSouthernAlarm
{
    UINT16 upper_limit;
    UINT16 lower_limit;
};

struct sDt1000Update
{
    UINT8   updata_mark;      // ������־0xAA������0xBB�ع���0x55��
    UINT8   vertion[17];      // ����汾
    UINT8   uDevAddr;        //�豸��ַ
    UINT8   uData;           // ����
    UINT32  nDataLen;
    UINT16  frame_sum;        // ������֡����256�ֽ�һ֡
    UINT16  CRC;              // ����CRCУ��
};

//struct sDt1000Update *gDt1000Update=NULL;



#pragma pack (pop)
#endif /*PROTOCOLDEF_H_*/

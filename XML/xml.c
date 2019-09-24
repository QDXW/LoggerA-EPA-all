/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : xml.c
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <termio.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "xml.h"
#include "../protocal/ProtocolDef.h"
#include "../interface/common.h"
#include "../alarm/alarmExtern.h"

extern sDeviceInfo gDeviceInfo[MAXDEVICE];
extern struct sTypeGroup *gTypeHead;
extern struct sTypeGroup *gTypeGroupPoint;
extern struct sTypeParam *gTypeParamPoint;
extern UINT8 gDeviceTypeNum;
extern struct sAlarmGroup *gAlarmHead;
extern UINT16 gDeviceAlarm;
extern UINT16 aAlarmInfo[40][20];

/*****************************************************************************
* Description:      ��ȡ�ļ���С
* Parameters:       path:�ļ�·��
* Returns:          �ļ���С
*****************************************************************************/
unsigned long GetFileSize(const char *path)
{
    unsigned long filesize=-1;
    struct stat statbuf;

    if(stat(path,&statbuf)<0)
    {
        return filesize;
    }
    else
    {
        filesize=statbuf.st_size;
    }
    return filesize;
}

/*****************************************************************************
* Description:      ��ȡ�豸������Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void ReadTagBaseFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;

    fTagBasefd=open(TagBaseFilePath,O_RDONLY);
    DbgPrintf("ReadTagBaseFile-----Get File size=");
    filesize =GetFileSize(TagBaseFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)//�ļ�����0ʱ���ж�ȡ
    {
        pHead=malloc(filesize);//�����ļ����ݻ���
        pPoint=pHead;//ָ��ָ�򻺴��ײ�
        if(read(fTagBasefd,pHead,filesize)>0)//��ȡ�ļ���Ϣ
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//����Ƿ����豸��Ϣ����
            {
                int nDeviceID;

                //��ȡ�豸ID
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);
                if(strstr((void *)pPoint,TagUseHead)==NULL)
                {
                    DbgPrintf("There is no PointAddrHead");
                    return;
                }
                //��ȡ�豸ʹ��״̬ 0:δʹ�� 1:ʹ��
                pPoint = (void *)strstr((void *)pPoint,TagUseHead);
                pPoint +=strlen(TagUseHead);
                if(strstr((void *)pPoint,TagUseEnd)==NULL)
                {
                    DbgPrintf("There is no TagUseEnd");
                    return;
                }
                gDeviceInfo[nDeviceID].nInUse=pPoint[0]-0x30;
                if(((pPoint[0]-0x30)==1) && (nDeviceID<=40))//���豸ʹ����,�����豸��Ų�����40ʱ
                {
                    int i;

                    gDeviceInfo[nDeviceID].nInUse=1;
                    if(strstr((void *)pPoint,TagESNHead)==NULL)
                    {
                        DbgPrintf("There is no TagESNHead");
                        return;
                    }
                    //��ȡ�豸ESN
                    pPoint = (void *)strstr((void *)pPoint,TagESNHead);
                    pPoint +=strlen(TagESNHead);
                    if(strstr((void *)pPoint,TagESNEnd)==NULL)
                    {
                        DbgPrintf("There is no TagESNEnd");
                        return;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagESNEnd);
                    memset(gDeviceInfo[nDeviceID].aESN,0,21);
                    if(pEnd-pPoint>20)
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aESN,pPoint,20);
                    else
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aESN,pPoint,pEnd-pPoint);
                    if(strstr((void *)pPoint,TagTableHead)==NULL)
                    {
                        DbgPrintf("There is no TagTableHead");
                        return;
                    }
                    //��ȡ�豸�����
                    pPoint = (void *)strstr((void *)pPoint,TagTableHead);
                    pPoint +=strlen(TagTableHead);
                    if(strstr((void *)pPoint,TagTableEnd)==NULL)
                    {
                        DbgPrintf("There is no TagTableEnd");
                        return;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagTableEnd);
                    for(i=0;i<pEnd-pPoint;i++)
                    {
                        gDeviceInfo[nDeviceID].nPointTableNo=gDeviceInfo[nDeviceID].nPointTableNo+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                    }

                    //��ȡ�豸���Ӷ˿���Ϣ
                    if((strstr((void *)pPoint,TagPortHead)!=NULL)&&(strstr((void *)pPoint,TagPortEnd)!=NULL))
                    {
                        pPoint = (void *)strstr((void *)pPoint,TagPortHead);
                        pPoint +=strlen(TagPortHead);
                        gDeviceInfo[nDeviceID].nDownlinkPort=HsCharToInt(pPoint[0]);
                    }
                    //��ȡ�豸Э��������Ϣ
                    if((strstr((void *)pPoint,TagProtocolHead)!=NULL)&&(strstr((void *)pPoint,TagProtocolEnd)!=NULL))
                    {
                        pPoint = (void *)strstr((void *)pPoint,TagProtocolHead);
                        pPoint +=strlen(TagProtocolHead);
                        gDeviceInfo[nDeviceID].nProtocolType=HsCharToInt(pPoint[0]);
                    }
                    //��ȡ���������汾��
                    if((strstr((void *)pPoint,TagSoftVsersionHead)==NULL))
                    {
                        DbgPrintf("There is no TagSoftVsersionHead");
                        continue;
                    }
                    pPoint = (void *)strstr((void *)pPoint,TagSoftVsersionHead);
                    pPoint +=strlen(TagSoftVsersionHead);
                    if(strstr((void *)pPoint,TagSoftVsersionEnd)==NULL)
                    {
                        DbgPrintf("There is no TagSoftVsersionEnd");
                        continue;
                    }
                    pEnd = (void *)strstr((void *)pPoint,TagSoftVsersionEnd);
                    memset(gDeviceInfo[nDeviceID].aSofeVersion,0,21);
                    if(pEnd-pPoint>20)
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aSofeVersion,pPoint,20);
                    else
                        memcpy((UINT8 *)&gDeviceInfo[nDeviceID].aSofeVersion,pPoint,pEnd-pPoint);
                }
            }
        }
        free(pHead);
    }
    close(fTagBasefd);
}

/*****************************************************************************
* Description:      ��ȡ�豸��������Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void ReadTagInfoFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;

    fTagInfofd=open(TagInfoFilePath,O_RDONLY);
    DbgPrintf("ReadTagInfoFile-----Get File size=");
    filesize =GetFileSize(TagInfoFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)//�ļ�����0ʱ���ж�ȡ
    {
        pHead=malloc(filesize);//���뻺��ռ�
        pPoint=pHead;
        if(read(fTagBasefd,pHead,filesize)>0)//��ȡ�ļ�����
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//����Ƿ����豸��Ϣ����
            {
                int nDeviceID;

                //��ȡ�豸���
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);//Get DeviceID
                if((gDeviceInfo[nDeviceID].nInUse==1) && (nDeviceID<=40))//���豸ʹ����,�����豸��Ų�����40ʱ
                {
                    int i;

                    gDeviceInfo[nDeviceID].nInUse=1;

                    //��ȡ�豸���ͱ��,��ͬ���豸�����
                    pPoint = (void *)strstr((void *)pPoint,TagTypeHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagTypeHead);

                    pEnd = (void *)strstr((void *)pPoint,TagTypeEnd);
                    for(i=0;i<pEnd-pPoint;i++)
                    {
                        gDeviceInfo[nDeviceID].nType=gDeviceInfo[nDeviceID].nType+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                    }

                    //��ȡ�豸ң����ʼ��ַ
                    pPoint = (void *)strstr((void *)pPoint,TagYXHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYXHead);

                    gDeviceInfo[nDeviceID].nYXAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //��ȡ�豸ң����ʼ��ַ
                    pPoint = (void *)strstr((void *)pPoint,TagYCHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYCHead);

                    gDeviceInfo[nDeviceID].nYCAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //��ȡ�豸ң����ʼ��ַ
                    pPoint = (void *)strstr((void *)pPoint,TagYKHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagYKHead);

                    gDeviceInfo[nDeviceID].nYKAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //��ȡ�豸�����ʼ��ַ
                    pPoint = (void *)strstr((void *)pPoint,TagSDHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagSDHead);

                    gDeviceInfo[nDeviceID].nSDAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);

                    //��ȡ�豸�����ʼ��ַ
                    pPoint = (void *)strstr((void *)pPoint,TagDDHead);
                    if(pPoint==NULL)
                        {
                            return;
                        }
                    pPoint +=strlen(TagDDHead);

                    gDeviceInfo[nDeviceID].nDDAddr=HsCharToInt(pPoint[0])*pow(16,3)
                                                         +HsCharToInt(pPoint[1])*pow(16,2)
                                                         +HsCharToInt(pPoint[2])*pow(16,1)
                                                         +HsCharToInt(pPoint[3])*pow(16,0);
                }
            }
        }
        free(pHead);
    }
    close(fTagBasefd);
}

/*****************************************************************************
* Description:      �����豸������Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void TagBaseFileWrite(void)
{
    UINT8 nDeviceCount;
    UINT8 aBuf[50];
    FILE *fd=fopen(TagBaseFilePath,"w+");

    //д���ļ�ͷ
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",FileHead);
    fwrite(aBuf,1,strlen((void *)aBuf),fd);
    //д��XML�ļ��ṹ��
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);

    for(nDeviceCount=1;nDeviceCount<=40;nDeviceCount++)
    {
        if(gDeviceInfo[nDeviceCount].nInUse==0)
        {
            continue;
        }
        //д���豸���
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д��ʹ��״̬
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagUseRegex,1);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д��ESN
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagESNRegex,gDeviceInfo[nDeviceCount].aESN);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸���ͱ��
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagTableRegex,gDeviceInfo[nDeviceCount].nType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸����COM��
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagPortRegex,gDeviceInfo[nDeviceCount].nDownlinkPort);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸Э������
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagProtocolRegex,gDeviceInfo[nDeviceCount].nProtocolType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д�����������汾��
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagSoftVsersionRegex,gDeviceInfo[nDeviceCount].aSofeVersion);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);

        //д�뱾�豸�����ڵ�
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);

        //�����豸��С��
        gDeviceInfo[nDeviceCount].nEndian=0;
        E2promWrite((UINT8 *)&gDeviceInfo[nDeviceCount].nEndian,DeviceTypeInfoAddr+(nDeviceCount-1)*5+4,1);
        //�����豸������
        gDeviceInfo[nDeviceCount].nRate=9600;
        E2promWrite((UINT8 *)&gDeviceInfo[nDeviceCount].nRate,DeviceTypeInfoAddr+(nDeviceCount-1)*5,4);
    }
    //д��XML�ļ��ṹ��β
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);
    fclose(fd);
}

/*****************************************************************************
* Description:      �����豸��������Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void TagInfoFileWrite(void)
{
    UINT8 nDeviceCount;
    UINT8 aBuf[50];
    FILE *fd=fopen(TagInfoFilePath,"w+");

    //д���ļ�ͷ
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",FileHead);
    fwrite(aBuf,1,strlen((void *)aBuf),fd);

    //д��XML�ļ��ṹ��
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);

    for(nDeviceCount=1;nDeviceCount<=40;nDeviceCount++)
    {
        if(gDeviceInfo[nDeviceCount].nInUse==0)
        {
            continue;
        }
        //д���豸���
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸���ͱ��
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagTypeRegex,gDeviceInfo[nDeviceCount].nType);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸ң����ʼ��ַ
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYXRegex,gDeviceInfo[nDeviceCount].nYXAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸ң����ʼ��ַ
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYCRegex,gDeviceInfo[nDeviceCount].nYCAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸ң����ʼ��ַ
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagYKRegex,gDeviceInfo[nDeviceCount].nYKAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸�����ʼ��ַ
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagSDRegex,gDeviceInfo[nDeviceCount].nSDAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸�����ʼ��ַ
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,TagDDRegex,gDeviceInfo[nDeviceCount].nDDAddr);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
        //д���豸�����ڵ�
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
        fwrite(aBuf,strlen((void *)aBuf),1,fd);
    }
    //д��XML�ļ������ṹ��
    memset(aBuf,0,sizeof(aBuf));
    sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
    fwrite(aBuf,strlen((void *)aBuf),1,fd);
    fclose(fd);
}

/*****************************************************************************
* Description:      ��������Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void PointInfoFileWrite(void)
{
    UINT8 aBuf[50];
    FILE *fd;
    struct sTypeGroup *pTypeGroup=NULL;
    struct sTypeParam *pTypeParam=NULL;

    pTypeGroup=gTypeHead;//ָ��������
    while(pTypeGroup!=NULL)//������Ͳ�Ϊ��
    {
        pTypeParam=pTypeGroup->pParamNext;//��ȡ�õ���һ���źŵ�
        if(pTypeParam!=NULL)
        {
            UINT8 nDeviceCount=0;//�źŵ����
            //���ݵ���Ž�������ļ�
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,PointFilePath,pTypeGroup->nTypeID);
            fd=fopen((void *)aBuf,"w+");
            //д���ļ�ͷ
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",FileHead);
            fwrite(aBuf,1,strlen((void *)aBuf),fd);
            //д����Э������
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,ProtocolRegex,pTypeGroup->nProtocalTypeID);
            fwrite(aBuf,1,strlen((void *)aBuf),fd);
            //д��XML�ļ��ṹ��
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",TagBaseHead);
            fwrite(aBuf,strlen((void *)aBuf),1,fd);
            while(pTypeParam!=NULL)
            {
                //д���źŵ���
                nDeviceCount++;
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,TagIDRegex,nDeviceCount);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //д���źŵ�MODBUS��ַ
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointAddrRegex,pTypeParam->nMBAddr);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //д���źŵ�Ĵ�������
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointLenRegex,pTypeParam->nLen);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //д���źŵ��Ӧ104��Լ����
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointTypeRegex,pTypeParam->nType);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //д���źŵ���������
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,PointDataTypeRegex,pTypeParam->nDataType);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
                //д���źŵ�����ڵ�
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                pTypeParam=pTypeParam->pNext;
            }
            //д��XML�ļ��ṹ�����
            memset(aBuf,0,sizeof(aBuf));
            sprintf((void *)aBuf,"%s\r\n",TagBaseEnd);
            fwrite(aBuf,strlen((void *)aBuf),1,fd);
            fclose(fd);
            pTypeGroup=pTypeGroup->pNext;
        }
    }
}

/*****************************************************************************
* Description:      ��ȡ�����Ϣ
* Parameters:
* Returns:
*****************************************************************************/
void ReadPointTableFile(void)
{
    UINT8 i,j;
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    UINT8 nPointFlag=0;

    E2promRead((UINT8 *)&gDeviceTypeNum,TypeNumberAddr,1);//��ȡ���ɵ����
    if(gDeviceTypeNum==255)
        gDeviceTypeNum=0;
    DbgPrintf("TypeNum = %d\r\n",gDeviceTypeNum);
    //��һ��ȡȫ��������Ϣ
    E2promRead((UINT8 *)&nPointFlag,UpdateRestoreAddr,1);//��ȡ�����
    for(i=0;i<gDeviceTypeNum;i++)
    {
        UINT16 nTableNum;
        UINT8 nTableProtocolType=0;
        UINT8 aBuf[50];

        E2promRead((UINT8 *)&nTableNum,0x6A5+i*2,2);//��ȡ�����
        if(nTableNum==0xFFFF)
            continue;
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,PointFilePath,nTableNum);
        //printf("PointFilePath = %s\r\n", aBuf);
        fTypePointfd=open((void *)aBuf,O_RDONLY);
        DbgPrintf("ReadPointTypeFile-----Get File size=");
        filesize =GetFileSize((void *)aBuf);//��ȡ����ļ���С
        DbgPrintf("%ld\r\n",filesize);
        if(filesize>0)
        {
            pHead=malloc(filesize);
            pPoint=pHead;
            if(read(fTypePointfd,pHead,filesize)>0)
            {
                //��ȡ���Э������
                if((strstr((void *)pPoint,ProtocolHead)!=NULL)&&(strstr((void *)pPoint,ProtocolEnd)!=NULL))
                {
                    pPoint = (void *)strstr((void *)pPoint,ProtocolHead);
                    pPoint +=strlen(ProtocolHead);
                    pEnd = (void *)strstr((void *)pPoint,ProtocolEnd);
                    for(j=0;j<(pEnd-pPoint);j++)
                    {
                        nTableProtocolType = nTableProtocolType+HsCharToInt(pPoint[j])*pow(10,(pEnd-pPoint)-j-1);
                    }
                }
                else
                    nTableProtocolType=1;
                TypeGroupAdd(1,nTableNum,nTableProtocolType);
                //printf("ADD Point Table %d\r\n",nTableNum);
                while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)//��һ��ȡ���ÿ���źŵ����Ϣ
                {
                    UINT8 nParaLen,nParaType,nParaDataType;
                    UINT16 nAddr=0;
                    if(strstr((void *)pPoint,">")==NULL)
                    {
                        DbgPrintf("There is no TagEnd >");
                        return;
                    }
                    //д���źŵ�MODBUS��ַ
                    pPoint +=strlen("<Tag id=\"");
                    if(strstr((void *)pPoint,PointAddrHead)==NULL)
                    {
                        DbgPrintf("There is no PointAddrHead");
                        return;
                    }
                    pPoint = (void *)strstr((void *)pPoint,PointAddrHead);
                    if(strstr((void *)pPoint,PointAddrEnd)==NULL)
                    {
                        DbgPrintf("There is no PointAddrEnd");
                        return;
                    }
                    pPoint +=strlen(PointAddrHead);
                    pEnd = (void *)strstr((void *)pPoint,PointAddrEnd);
                    for(j=0;j<(pEnd-pPoint);j++)
                    {
                        nAddr = nAddr+HsCharToInt(pPoint[j])*pow(10,(pEnd-pPoint)-j-1);
                    }
                    if(strstr((void *)pPoint,PointLenHead)==NULL)
                    {
                        DbgPrintf("There is no PointLenHead");
                        return;
                    }
                    //��ȡ�źŵ�ļĴ�������
                    pPoint = (void *)strstr((void *)pPoint,PointLenHead);
                    pPoint +=strlen(PointLenHead);
                    if(strstr((void *)pPoint,PointLenEnd)==NULL)
                    {
                        DbgPrintf("There is no PointLenEnd");
                        return;
                    }
                    nParaLen=HsCharToInt(pPoint[0])*pow(10,1)+HsCharToInt(pPoint[1])*pow(10,0);
                    if(strstr((void *)pPoint,PointTypeHead)==NULL)
                    {
                        DbgPrintf("There is no PointTypeHead");
                        return;
                    }
                    //��ȡ�źŵ��Ӧ104������
                    pPoint = (void *)strstr((void *)pPoint,PointTypeHead);
                    pPoint +=strlen(PointTypeHead);
                    if(strstr((void *)pPoint,PointTypeEnd)==NULL)
                    {
                        DbgPrintf("There is no PointTypeEnd");
                        return;
                    }
                    nParaType=HsCharToInt(pPoint[0]);
                    if(strstr((void *)pPoint,PointDataTypeHead)==NULL)
                    {
                        DbgPrintf("There is no PointDataTypeHead");
                        return;
                    }
                    //д���źŵ���������
                    pPoint = (void *)strstr((void *)pPoint,PointDataTypeHead);
                    pPoint +=strlen(PointDataTypeHead);
                    if(strstr((void *)pPoint,PointDataTypeEnd)==NULL)
                    {
                        DbgPrintf("There is no PointDataTypeEnd");
                        return;
                    }
                    nParaDataType=HsCharToInt(pPoint[0]);
                    /*printf("{%d,%d,%d},",nAddr,nParaLen,nParaType);*/
                    TypeParamAdd(1,nAddr,nParaLen,nParaType,nParaDataType);
                }
            }
            free(pHead);
        }
        close(fTypePointfd);
    }
    /*
    pTypeGroup = gTypeHead;
    while(pTypeGroup!=NULL)
    {
        pTypeParam=pTypeGroup->pParamNext;
        if(pTypeParam!=NULL)
        {
            while(pTypeParam!=NULL)
            {
                pTypeParam=pTypeParam->pNext;
            }
            pTypeGroup=pTypeGroup->pNext;
        }
    }*/
    gTypeGroupPoint=NULL;
}

void AlarmFileWrite(void)
{
    UINT8 aBuf[50];
    FILE *fd;
    struct sAlarmGroup *pPoint;

    //printf("1\r\n");
    pPoint = gAlarmHead;
    fd=fopen(AlarmFilePath,"w+");
    if(fd != 0)
    {
        memset(aBuf,0,sizeof(aBuf));
        sprintf((void *)aBuf,"%s\r\n",FileHead);
        fwrite(aBuf,1,strlen((void *)aBuf),fd);
        while(pPoint!=NULL)
        {
            if((pPoint->nUploadStatusMasterChannel>=2)&&(pPoint->nUploadStatusMasterChannel<=5))
            {
                //printf("%d %d %d %d\r\n",pPoint->nDeviceID,pPoint->nAlarmID,pPoint->nModbusAddr,pPoint->nAlarmExternID);
                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,TagIDRegex,pPoint->nDeviceID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmTypeRegex,pPoint->nAlarmID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmAddrRegex,pPoint->nModbusAddr);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,AlarmOffsetRegex,pPoint->nAlarmExternID);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);

                memset(aBuf,0,sizeof(aBuf));
                sprintf((void *)aBuf,"%s\r\n",TagIDEnd);
                fwrite(aBuf,strlen((void *)aBuf),1,fd);
            }
            pPoint=pPoint->pNext;
        }
        //printf("add end\r\n");
        fclose(fd);
    }
    else
    {
        DbgPrintf("[Alarm]Open alarm file fail!!!\r\n");
    }
}

void ReadAlarmFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    int fd;
    struct sAlarmGroup *pAlarmBufPoint;

    pthread_mutex_lock(&alarmsem);
    while(gAlarmHead!=NULL)
    {
        pAlarmBufPoint=gAlarmHead;
        gAlarmHead=pAlarmBufPoint->pNext;
        free(pAlarmBufPoint);
    }
    pthread_mutex_unlock(&alarmsem);

    memset(aAlarmInfo,0,sizeof(aAlarmInfo));
    if((access(AlarmFilePath,F_OK)==-1))
    {
        gAlarmHead=NULL;
        return;
    }
    fd=open(AlarmFilePath,O_RDONLY);
    DbgPrintf("ReadAlarmInfoFile-----Get File size=");
    filesize =GetFileSize(AlarmFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)
    {

        pHead=malloc(filesize);
        pPoint=pHead;
        if(read(fd,pHead,filesize)>0)
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)
            {
                UINT8 i,nDeviceID=0,nAlarmID=0,nOffSet=0;
                UINT16 nAlarmAddr;

                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);
                if(strstr((void *)pPoint,AlarmTypeHead)==NULL)
                {
                    DbgPrintf("There is no AlarmTypeHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmTypeHead);
                pPoint +=strlen(AlarmTypeHead);
                if(strstr((void *)pPoint,AlarmTypeEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmTypeEnd");
                    return;
                }
                pEnd = (void *)strstr((void *)pPoint,AlarmTypeEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nAlarmID=nAlarmID+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }
                if(strstr((void *)pPoint,AlarmAddrHead)==NULL)
                {
                    DbgPrintf("There is no AlarmAddrHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmAddrHead);
                pPoint +=strlen(AlarmAddrHead);
                if(strstr((void *)pPoint,AlarmAddrEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmAddrEnd");
                    return;
                }
                nAlarmAddr=HsCharToInt(pPoint[0])*pow(16,3)
                            +HsCharToInt(pPoint[1])*pow(16,2)
                            +HsCharToInt(pPoint[2])*pow(16,1)
                            +HsCharToInt(pPoint[3])*pow(16,0);
                if(strstr((void *)pPoint,AlarmOffsetHead)==NULL)
                {
                    DbgPrintf("There is no AlarmOffsetHead");
                    return;
                }
                pPoint = (void *)strstr((void *)pPoint,AlarmOffsetHead);
                pPoint +=strlen(AlarmOffsetHead);
                if(strstr((void *)pPoint,AlarmOffsetEnd)==NULL)
                {
                    DbgPrintf("There is no AlarmOffsetEnd");
                    return;
                }
                pEnd = (void *)strstr((void *)pPoint,AlarmOffsetEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nOffSet=nOffSet + HsCharToInt(pPoint[i]) * pow(10, pEnd - pPoint - i - 1);
                }
                AlarmReload(nDeviceID,nAlarmID,nAlarmAddr,nOffSet);
                if(nAlarmID == 2)
                    aAlarmInfo[nDeviceID-1][nAlarmAddr%10000] |=  (0x0001<<nOffSet);
            }
        }
        free(pHead);
    }
    close(fd);
}

void ReloadAlarmFile(void)
{
    unsigned long filesize;
    UINT8 *pHead,*pPoint,*pEnd;
    int fd;
    struct sAlarmGroup *pAlarmBufPoint;

    memset(aAlarmInfo,0,sizeof(aAlarmInfo));
    if((access(AlarmFilePath,F_OK)==-1))
    {
        gAlarmHead=NULL;
        return;
    }
    fd=open(AlarmFilePath,O_RDONLY);
    DbgPrintf("ReadAlarmInfoFile-----Get File size=");
    filesize =GetFileSize(AlarmFilePath);
    DbgPrintf("%ld\r\n",filesize);
    if(filesize>0)
    {
        pHead=malloc(filesize);
        pPoint=pHead;
        if(read(fd,pHead,filesize)>0)
        {
            while((pPoint=(void *)strstr((void *)pPoint,"<Tag id=\""))!=NULL)
            {
                UINT8 i,nDeviceID=0,nAlarmID=0,nOffSet=0;
                UINT16 nAlarmAddr;
                UINT8 nReloadFlag=0;

                pAlarmBufPoint=gAlarmHead;
                pPoint +=strlen("<Tag id=\"");
                nDeviceID = (pPoint[0]-0x30)*10+(pPoint[1]-0x30);

                pPoint = (void *)strstr((void *)pPoint,AlarmTypeHead);
                pPoint +=strlen(AlarmTypeHead);
                pEnd = (void *)strstr((void *)pPoint,AlarmTypeEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nAlarmID=nAlarmID+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }

                pPoint = (void *)strstr((void *)pPoint,AlarmAddrHead);
                pPoint +=strlen(AlarmAddrHead);
                nAlarmAddr=HsCharToInt(pPoint[0])*pow(16,3)
                            +HsCharToInt(pPoint[1])*pow(16,2)
                            +HsCharToInt(pPoint[2])*pow(16,1)
                            +HsCharToInt(pPoint[3])*pow(16,0);

                pPoint = (void *)strstr((void *)pPoint,AlarmOffsetHead);
                pPoint +=strlen(AlarmOffsetHead);
                pEnd = (void *)strstr((void *)pPoint,AlarmOffsetEnd);
                for(i=0;i<pEnd-pPoint;i++)
                {
                    nOffSet=nOffSet+HsCharToInt(pPoint[i])*pow(10,pEnd-pPoint-i-1);
                }
                while(pAlarmBufPoint!=NULL)
                {
                    if((pAlarmBufPoint->nDeviceID==nDeviceID)&&(pAlarmBufPoint->nAlarmID==nAlarmID)
                        &(pAlarmBufPoint->nModbusAddr==nAlarmAddr)&&(pAlarmBufPoint->nAlarmExternID==nOffSet))
                    {
                        nReloadFlag = 1;
                        break;
                    }
                    else
                    {
                        pAlarmBufPoint = pAlarmBufPoint->pNext;
                    }
                }
                if(nReloadFlag == 0)
                    AlarmReload(nDeviceID,nAlarmID,nAlarmAddr,nOffSet);
                if(nAlarmID == 2)
                    aAlarmInfo[nDeviceID-1][nAlarmAddr%10000] |=  (0x0001<<nOffSet);
            }
        }
        free(pHead);
    }
    close(fd);
}

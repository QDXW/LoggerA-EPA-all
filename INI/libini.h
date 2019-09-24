/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : libini.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef ALAN_LIBINI_H
#define ALAN_LIBINI_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../interface/interfaceExtern.h"

#define LINE_LEN 128    // INI�ļ��г���
#define SECT_LEN 64 // �ڵ����Ƴ���
#define VINT_LEN 64 // ������ֵ����
#define VCHR_LEN 64 // �ַ���ֵ����

#define DEF_SECT_ROOT "General" //Ĭ�ϸ��ڵ�����

//Third Party Device Info
#define THIRD_PARTY_FILE_PATH "/mnt/flash/OAM/thirdparty/config.ini"
#define CONFIG_FILE_PATH      "/mnt/flash/OAM/filetemp/config.ini"
#define SOUTH_ALARM_PATH      "/mnt/flash/OAM/southalarm/alarm.ini"

typedef struct ini_item_ {
    char            *key;
    char            *value;
    char            *section;
    struct ini_item_    *next;

} ini_item;

typedef struct {
    char            *fname;
    struct ini_item_    *header;
    struct ini_item_    *tailer;
    int          length;
} LIBINI;

/********************************************************
 * ini_open : ����INI�ļ���������
 *        ����ֵΪINI�ļ�����ָ��
 * @fname   : Ҫ������INI�ļ���
*/
LIBINI *ini_open(const char *fname);

/********************************************************
 * ini_create   : ����һ���µ�INI�ļ�
 *        ����ֵΪINI�ļ�����ָ��
 * @fname   : Ҫ������INI�ļ���
*/
LIBINI *ini_create(const char *fname);

/********************************************************
 * ini_save : ���������е�INI�ļ����ݱ��浽�ļ�
 *        ����ֵΪ0����ɹ�������Ϊ����ʧ��
 * @pini    : Ҫ�����INI�ļ�ָ��
*/
int ini_save(LIBINI *pini);

/********************************************************
 * ini_close    : �ͷ�INI�ļ�ռ�õ��ڴ���Դ
 *        �޷���ֵ
 * @pini    : Ҫ�ͷŵ�INI�ļ�ָ��
*/
void ini_close(LIBINI *pini);

/********************************************************
 * ini_get  : ȡ��ָ�����ļ�ֵ
 *        ����ֵΪ�����ֵ���ַ���ָ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
 * @value   : ���ڱ��淵��ֵ���ַ���ָ��
*/
char *ini_get(LIBINI *pini,const char *key,char *value);

/********************************************************
 * ini_get_int  : ȡ��ָ���������ͼ�ֵ
 *        ����ֵ���ͼ�ֵ
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
*/
int ini_get_int(LIBINI *pini,const char *key);

/********************************************************
 * ini_set  : ����ָ�����ļ�ֵ
 *        ����ֵΪ1���óɹ���0����ʧ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
 * @value   : ���ڱ���Ҫ���õ�ֵ���ַ���ָ��
*/
int ini_set(LIBINI *pini,const char *key,const char *value);

/********************************************************
 * ini_set_int  : ������ֵ����ָ�����ļ�ֵ
 *        ����ֵΪ1���óɹ���0����ʧ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
 * @value   : ���ڱ���Ҫ���õ�ֵ���ַ���ָ��
*/
int ini_set_int(LIBINI *pini,const char *key,int value);

/********************************************************
 * ini_append   : �����¼�
 *        ����ֵΪ1���óɹ���0����ʧ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
 * @value   : ��ֵ�ַ���ָ��
*/
int ini_append(LIBINI *pini,const char *key,const char *value);

/********************************************************
 * ini_append_int:��������ֵ�������¼�
 *        ����ֵΪ1���óɹ���0����ʧ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
 * @value   : ���ͼ�ֵ
*/
int ini_append_int(LIBINI *pini,const char *key,int value);

/********************************************************
 * ini_remove   : �Ƴ�ָ����
 *        ����ֵΪ1���óɹ���0����ʧ��
 * @pini    : INI�ļ�ָ��
 * @key     : �����ַ���ָ��
*/
int ini_remove(LIBINI *pini,const char *key);

/********************************************************
 * ReadConfigFile   : ��ȡ�����ļ�
*/
void ReadConfigFile();

/********************************************************
 * ReadThirdPartyInfo   : ��ȡ����ƽ̨��Ϣ
 *        ����ֵΪ1�ǳɹ���0��ʧ��
*/
UINT8 ReadThirdPartyFile();

/********************************************************
 * ReadThirdPartyInfo   : ��ӡ�������
*/
void PrintThirdPartyList(void);

/********************************************************
 * ReadThirdPartyInfo   : ��ȡ����澯��Ϣ
 *        ����ֵΪ1�ǳɹ���0��ʧ��
*/
UINT8 ReadSouthernAlarmFile();

/********************************************************
 * ReadThirdPartyInfo   : ��ӡ�澯��ֵ����
*/
void PrintfLimits();

#endif//ALAN_LIBINI_H
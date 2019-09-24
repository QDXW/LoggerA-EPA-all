/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : exporthelper.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef EXPORTHELPER_H_INCLUDED
#define EXPORTHELPER_H_INCLUDED

int startCompressLogOfDay(int day);
int getLogPackageFrameNumber();
int getFrameData(char *buff, long frameNum);
int getFileData(char *aFilePath,char *buff, long frameNum, unsigned short frame_size);
int GetFileLength(char *file_path);
int GetFileData(char *aFilePath,char *buff, long frameNum, unsigned short frame_size);
int GetLogFrameData(char *buff, long frameNum);
int StartCompressFile(char *aFilePath);

#endif // EXPORTHELPER_H_INCLUDED

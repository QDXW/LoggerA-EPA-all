/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : Module.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#include "../interface/interfaceExtern.h"

extern UINT16 gLocationReport;                 // 0: not report yet  others: already reported

void ModulePowerSet(UINT16 nStatus);
void ModelEnable();
void ModelRFEnable(UINT16 nStatus);
void *ModuleThreadMasterChannel();
void *ModuleThreadSlaveChannel();
void UploadCertificate();

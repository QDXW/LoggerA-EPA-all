/*****************************************Copyright(C)******************************************
*******************************************����Ʒ��*********************************************
*------------------------------------------�ļ���Ϣ---------------------------------------------
* FileName          : Port_FunExtern.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------��ʷ�汾��Ϣ-------------------------------------------
* History           :
* Description       :
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/

#ifndef LOGIC_PROCSEXTERN_H_
#define LOGIC_PROCSEXTERN_H_

#include "../interface/interfaceExtern.h"

extern void *Fun_Thread_Port(void *param);
extern void *SocketRecvbufThread(void);
extern void RecvBufferAdd(UINT8 nType,UINT8 *aBuf,UINT8 nLen);

#endif /* LOGIC_PROCSEXTERN_H_ */

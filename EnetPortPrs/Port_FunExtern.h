/*****************************************Copyright(C)******************************************
*******************************************杭州品联*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName          : Port_FunExtern.h
* Author            : Andre
* Date First Issued :
* Version           :
* Description       :
*----------------------------------------历史版本信息-------------------------------------------
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

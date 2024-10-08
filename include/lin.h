/** @file lin.h
*   @brief LIN Driver Definition File
*   @date 11-Dec-2018
*   @version 04.07.01
*   
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/



#ifndef __LIN_H__
#define __LIN_H__

#include "reg_lin.h"

#ifndef BOOL
#define BOOL  unsigned char
#endif

/** @def LIN_BREAK_INT
*   @brief Alias for break detect interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_BREAK_INT 0x00000001U


/** @def LIN_WAKEUP_INT
*   @brief Alias for wakeup interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_WAKEUP_INT 0x00000002U


/** @def LIN_TO_INT
*   @brief Alias for time out interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_TO_INT 0x00000010U


/** @def LIN_TOAWUS_INT
*   @brief Alias for time out after wakeup signal interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_TOAWUS_INT 0x00000040U


/** @def LIN_TOA3WUS_INT
*   @brief Alias for time out after 3 wakeup signals interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_TOA3WUS_INT 0x00000080U


/** @def LIN_TX_READY
*   @brief Alias for transmit buffer ready flag
*
*   Used with linIsTxReady.
*/
#define LIN_TX_READY 0x00000100U


/** @def LIN_RX_INT
*   @brief Alias for receive buffer ready interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_RX_INT 0x00000200U


/** @def LIN_ID_INT
*   @brief Alias for received matching identifier interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_ID_INT 0x00002000U


/** @def LIN_PE_INT
*   @brief Alias for parity error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_PE_INT 0x01000000U


/** @def LIN_OE_INT
*   @brief Alias for overrun error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_OE_INT 0x02000000U


/** @def LIN_FE_INT
*   @brief Alias for framing error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_FE_INT 0x04000000U


/** @def LIN_NRE_INT
*   @brief Alias for no response error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_NRE_INT 0x08000000U


/** @def LIN_ISFE_INT
*   @brief Alias for inconsistent sync field error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_ISFE_INT 0x10000000U


/** @def LIN_CE_INT
*   @brief Alias for checksum error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_CE_INT 0x20000000U


/** @def LIN_PBE_INT
*   @brief Alias for physical bus error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_PBE_INT 0x40000000U


/** @def LIN_BE_INT
*   @brief Alias for bit error interrupt flag
*
*   Used with linEnableNotification, linDisableNotification.
*/
#define LIN_BE_INT 0x80000000U


/** @struct linBase
*   @brief LIN Register Definition
*
*   This structure is used to access the LIN module registers.
*/
/** @typedef linBASE_t
*   @brief LIN Register Frame Type Definition
*
*   This type is used to access the LIN Registers.
*/

enum linPinSelect
{
    PIN_LIN_TX   = 4U,
    PIN_LIN_RX = 2U
};

/** 
 *  @defgroup LIN LIN
 *  @brief Local Interconnect Network Module.
 *  
 *  The LIN standard is based on the SCI (UART) serial data link format. The communication concept is
 *  single-master/multiple-slave with a message identification for multi-cast transmission between any network
 *  nodes.
 *
 *	Related Files
 *   - reg_lin.h
 *   - lin.h
 *   - lin.c
 *  @addtogroup LIN
 *  @{
 */
/* LIN Interface Functions */
void     linInit(void);
void     linSetFunctional(linBASE_t *lin, uint32 port);
void     linSendHeader(linBASE_t *lin, uint8 identifier);
void     linSendWakupSignal(linBASE_t *lin);
void     linEnterSleep(linBASE_t *lin);
void     linSoftwareReset(linBASE_t *lin);
uint32 linIsTxReady(linBASE_t *lin);
void     linSetLength(linBASE_t *lin, uint32 length);
void     linSend(linBASE_t *lin, uint8 * data);
uint32 linIsRxReady(linBASE_t *lin);
uint32 linTxRxError(linBASE_t *lin);
uint32 linGetIdentifier(linBASE_t *lin);
void     linGetData(linBASE_t *lin, uint8 * const data);
void     linEnableNotification(linBASE_t *lin, uint32 flags);
void     linDisableNotification(linBASE_t *lin, uint32 flags);
uint32   linGetStatusFlag(linBASE_t *lin);
void     linClearStatusFlag(linBASE_t *lin, uint32 flags);

/** @fn void linNotification(linBASE_t *lin, uint32 flags)
*   @brief Interrupt callback
*   @param[in] lin   - lin module base address
*   @param[in] flags - copy of error interrupt flags
*
* This is a callback that is provided by the application and is called upon
* an interrupt.  The parameter passed to the callback is a copy of the 
* interrupt flag register.
*/
void linNotification(linBASE_t *lin, uint32 flags);
/**@}*/
#endif

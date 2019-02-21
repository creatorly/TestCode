/**
 * @file radio_server.h
 * @brief Define AT Server interface
*/
#ifndef RADIO_SERVER_H
#define RADIO_SERVER_H

#include "wm_interface.h"
#include "call_manager.h"
#include "msg_manager.h"
#include "stk_manager.h"
#include "sim_manager.h"
#include "dataconn_manager.h"

typedef enum
{
    CONNSTATE_RINGON,        /**RING incoming*/
    CONNSTATE_RINGOFF,        /**RING already processed(sent AT+CLCC)*/

    CONNSTATE_NEWMSGON,        /**new message incoming*/
    CONNSTATE_NEWMSGOFF        /**new message already processed*/

}T_CONNECT_STATE;

#define    CONNECT_STATE_RING    0x00000001
#define CONNECT_STATE_MSG    0x00000002

typedef struct tagatserver{
    HCALLMANAGER    hCallManager; /**<Call manager*/
    HMSGMANAGER        hMSGManager;  /**<sms manager*/
    HSTKMANAGER        hSTKManager;  /**<STK manager*/
    PT_DATACONNMANAGER hDataConnManager; /**<Data connection manager*/
    HWM                hWM;          /**<handle to wireless module device*/
    _UINT32            connectState;
} T_ATSERVER, *HATSERVER;

_BOOL ATServer_CheckHandleValid(HATSERVER hATServer);
_UINT32 ATServer_Setup(_UINT32 ChnlNum, _USHORT16 *ChID, _UINT32 Type);
_UINT32 ATServer_Stop(_VOID);
_UINT32 ATServer_UpdateChannel(HATSERVER hATServer, _UINT32 ChnlNum, _USHORT16 *ChID);

_UINT32 ATServer_SetConnState(HATSERVER hATServer, T_CONNECT_STATE state);
_UINT32 ATServer_GetConnState(HATSERVER hATServer, _UINT32 *state);

_UINT32 ATServer_IgniteModule(_UINT32 hLine, _UINT32 customParam);
_UINT32 ATServer_ResetModule(_UINT32 hLine, _UINT32 customParam);
#endif


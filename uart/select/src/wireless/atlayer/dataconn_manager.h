/**
 * @file dataconn_manager.h
 * @brief Define data connection manager interface
 */
#ifndef DATACONN_MANAGER_H
#define DATACONN_MANAGER_H

#include "eos.h"

typedef struct tagdataconnmanager{
    _UINT32    ConnCmdID;
    _BOOL    bConnect;
    _BOOL    bRelease;
}T_DATACONNMANAGER, *PT_DATACONNMANAGER;

_UINT32 DataConnManager_Init(PT_DATACONNMANAGER *phDataConnManager, _USHORT16 *ChID);
_UINT32 DataConnManager_DeInit(PT_DATACONNMANAGER hManager);
_UINT32 DataConn_InquireGprsState(_UINT32 hLine, _UINT32 customParam);
_UINT32 DataConn_EstablishWithGPRS(_UINT32 hLine, _CHAR8* strAPN, _BOOL bMUX, _UINT32 customParam);
_UINT32 DataConn_EstablishWithCSD(_UINT32 hLine, _CHAR8* dial, _CHAR8* user, _CHAR8* pwd, _BOOL bMUX, _UINT32 customParam);
_UINT32 DataConn_Release(_UINT32 hLine, _BOOL bMUX, _USHORT16 step, _UINT32 customParam);
_UINT32 DataConn_Suspend(_UINT32 hLine, _BOOL bMUX, _UINT32 customParam);
_UINT32 DataConn_Resume(_UINT32 hLine, _BOOL bMUX, _UINT32 customParam);
_UINT32 DataConn_ResultCallback(PT_DATACONNMANAGER hManager, 
                              _UINT32 resultType, _VOID* lpData, _UINT32 totalSize, _UINT32 usedSize,
                              _UINT32 customParam);


#endif


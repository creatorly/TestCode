
#ifdef __VXWORKS_OS__
#include <vxWorks.h>
#include <semLib.h>
#include <tickLib.h>
#endif

#if (ACOM_BOARD_TYPE == RALINK)
#include <stdlib.h>
#include "stdio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#include "eos.h"
#include "ut_sbuf.h"
#include "ut_charset.h"
#include "ut_task.h"
#include "ut_exc.h"
#include <utils/ut_fselect.h>

#include "wireless_common.h"
#include "wireless_debug.h"
#include "wireless_app.h"
#include "wireless_interface_inc.h"
#include "wireless_interface.h"

#if (ACOM_BOARD_TYPE == MDM9X07)
#include "ril.h"
#include "ril_nas.h"
#endif

_INT32 giLedTestHdl = -1;

WIRELESSROOTST     stWirelessRootSt;

_INT32 wirelessExcHandler(_INT32 task, _VOID* data);

_INT32 wirelessReadUartData(_INT32 fd, const _CHAR8* file, _VOID* usrData)
{
    PWIRELESSROOTST pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    PWIRELESSCHANNELST pWirelessChannelCtl = (PWIRELESSCHANNELST) usrData;
    ioctl_channel_read_st stComRead;

    stComRead.channelid = pWirelessChannelCtl->uiChannelId;
    stComRead.size = read(fd, stComRead.buff, sizeof(stComRead.buff) - 1);
    WIRELESS_INFO("wirelessReadUartData-fd:%d,buff:%s\r\n", fd, stComRead.buff);
    if (stComRead.size > 0)
    {
        _INT32 length = sizeof(stComRead) - sizeof(stComRead.buff) + stComRead.size;

        stComRead.buff[stComRead.size] = '\0';
        UT_MsgqSend(pWirelessRootSt->ATRspMsgq, &stComRead, length);
        
        WIRELESS_WakeUp();
    }

    return stComRead.size;
}

_INT32 WIRELESS_initUart( _UINT32 channelID)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[channelID];
    pWirelessChannelCtl->fUartHandle = WrlUart_Init(channelID);
    if(pWirelessChannelCtl->fUartHandle == EOS_ERROR)
    {
        WIRELESS_INFO("Channel %d InitUart Error\r\n", channelID);
        return EOS_ERROR;
    }
    else
    {
        WIRELESS_INFO("Channel %d InitUart OK\r\n", channelID);
        UT_FSelectAddFd(pWirelessChannelCtl->fUartHandle, wirelessReadUartData, pWirelessChannelCtl);

        return EOS_OK;
    }
}

_INT32 WIRELESS_freeUart( _UINT32 channelID)
{
    PWIRELESSROOTST     pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessChannelCtl = &pWirelessRootSt->stChannelSt[channelID];
    WrlUart_unInit(channelID, pWirelessChannelCtl->fUartHandle);
    pWirelessChannelCtl->fUartHandle = EOS_ERROR;
    WIRELESS_INFO("WIRELESS_freeUart: Free the handle of the uart device.channelID=%d\n",channelID);
    printf("WIRELESS_freeUart: Free the handle of the uart device.channelID=%d\n",channelID);
    return EOS_OK;
}

_INT32 WIRELESS_Init(_LONG32 param)
{
    _CHAR8 WirelessMName[64];
    _CHAR8 ChnMutex[64];
    _CHAR8 ATCMDChnMutex[64];
    _CHAR8 ATRSPChnMutex[64];
    T_HCHNL* pChnl = NULL;
    _CHAR8  acChnM[16];
    _UINT32 uiPortIndex = 0;
    _UINT32 uiTemp = 0;
    _UINT32 uiSlotIndex = 0;
    _UINT32 i = 0;
    
    PWIRELESSROOTST pWirelessRootSt;
    PWIRELESSCHANNELST  pWirelessChannelCtl;

    UT_LogSetModEx(DEBUG_MODULE_WIRELESS, EOS_TRUE, EOS_TRUE);
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

    memset(WirelessMName, 0, sizeof(WirelessMName));
    memset(ChnMutex, 0, sizeof(ChnMutex));
    memset(ATCMDChnMutex, 0, sizeof(ATCMDChnMutex));
    memset(ATRSPChnMutex, 0, sizeof(ATRSPChnMutex));

    strcpy(WirelessMName, "wirelessm");
    strcpy(ChnMutex, "wchnm");
    strcpy(ATCMDChnMutex, "ATCMD");
    strcpy(ATRSPChnMutex, "ATRSP");
    
    eos_sem_create(WirelessMName, &pWirelessRootSt->mutex);
    eos_sem_create(ATCMDChnMutex, &pWirelessRootSt->ATCmdLstMutex);     
    eos_sem_create(ATRSPChnMutex, &pWirelessRootSt->RSPLstMutex);     

    printf("WIRELESS PCB VER:%d\r\n", (pWirelessRootSt->iPCBVer));
    
    pWirelessRootSt->uiMaxChannelNum = ACOM_PORT_NUM;
#ifdef ANDROID 
    pWirelessRootSt->uiMaxModuleNum = pWirelessRootSt->uiMaxChannelNum - 1;
#elif (ACOM_BOARD_TYPE == MDM9X07)
    pWirelessRootSt->uiMaxModuleNum = pWirelessRootSt->uiMaxChannelNum - 1;
#elif (ACOM_BOARD_TYPE == RALINK)
    pWirelessRootSt->uiMaxModuleNum = pWirelessRootSt->uiMaxChannelNum;
#endif
    
    pWirelessRootSt->uiStartChannel = 0;
    pWirelessRootSt->uiEndChannel = pWirelessRootSt->uiMaxChannelNum;
    pWirelessRootSt->uiWirelessMaxTick = 0;
    pWirelessRootSt->uiWirelessMaxTick1 = 0;
    pWirelessRootSt->uiWirelessMaxTick2 = 0;
    pWirelessRootSt->uiWirelessMaxTick3 = 0;
    pWirelessRootSt->uiWirelessMaxTick4 = 0;
    pWirelessRootSt->uiTickWatchDogTriger = 0;
    pWirelessRootSt->bIsATDbgMode = EOS_FALSE;
    pWirelessRootSt->locMsgq = NULL;
    pWirelessRootSt->bNeedSaveNv = EOS_FALSE;

    ut_mem_config(UT_MEM_SIZE_32, WIRELESS_MEM_32_CNT);
    ut_mem_config(UT_MEM_SIZE_64, WIRELESS_MEM_64_CNT);
    ut_mem_config(UT_MEM_SIZE_128, WIRELESS_MEM_128_CNT);
    ut_mem_config(UT_MEM_SIZE_256, WIRELESS_MEM_256_CNT);
    ut_mem_config(UT_MEM_SIZE_512, WIRELESS_MEM_512_CNT);
    ut_mem_config(UT_MEM_SIZE_1024, WIRELESS_MEM_1024_CNT);
    ut_mem_config(UT_MEM_SIZE_2048, WIRELESS_MEM_2048_CNT);
    ut_mem_config(UT_MEM_SIZE_4096, WIRELESS_MEM_4096_CNT);
    pWirelessRootSt->stChannelSt = (PWIRELESSCHANNELST)malloc(sizeof(WIRELESSCHANNELST)*(pWirelessRootSt->uiMaxChannelNum));
    if(pWirelessRootSt->stChannelSt == NULL)
    {
        printf("Wireless stChannelSt malloc error\r\n");
        return EOS_ERROR;
    }
    memset(pWirelessRootSt->stChannelSt, 0, sizeof(WIRELESSCHANNELST)*(pWirelessRootSt->uiMaxChannelNum));

    if(ut_mem_init(pWirelessRootSt->uiMaxChannelNum) < 0)
    {
        printf("Wireless malloc error\r\n");
        return EOS_ERROR;
    }

    ut_mem_debug(EOS_TRUE);
    
    pWirelessRootSt->FSMMsgq = UT_MsgqInit((pWirelessRootSt->uiMaxChannelNum*16),sizeof(ST_WRL_FSM_MSGQ));
    pWirelessRootSt->ATRspMsgq = UT_MsgqInit((pWirelessRootSt->uiMaxChannelNum*16),sizeof(ioctl_channel_read_st));
    
    pWirelessRootSt->semATSendData = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
    pWirelessRootSt->semATRcvData = semBCreate(SEM_Q_FIFO, SEM_EMPTY);

    utl_select_init(&pWirelessRootSt->sel, NULL, NULL);

    for(uiPortIndex = 0; uiPortIndex < pWirelessRootSt->uiMaxChannelNum; uiPortIndex++)
    {
        pWirelessChannelCtl =  &pWirelessRootSt->stChannelSt[uiPortIndex];
        pWirelessChannelCtl->uiChannelId = uiPortIndex;
        
        pChnl = pWirelessChannelCtl->pChnl;

        pWirelessChannelCtl->pucExtComBuf = malloc(WIRELESS_EXTCOMBUF_MAXSIZE);        
        if(pWirelessChannelCtl->pucExtComBuf == NULL)
        {
            printf("Wireless pucExtComBuf malloc error\r\n");
            return EOS_ERROR;
        }
        memset(pWirelessChannelCtl->pucExtComBuf, 0, WIRELESS_EXTCOMBUF_MAXSIZE);

        pWirelessChannelCtl->pucCompleteInfoBuf = malloc(WIRELESS_COMPLETEINFO_MAXSIZE);        
        if(pWirelessChannelCtl->pucCompleteInfoBuf == NULL)
        {
            printf("Wireless pucCompleteInfoBuf malloc error\r\n");
            return EOS_ERROR;
        }
        memset(pWirelessChannelCtl->pucCompleteInfoBuf, 0, WIRELESS_COMPLETEINFO_MAXSIZE);

#if (ACOM_HW_SUBTYPE == HWST_2G4G)
        if(0 == uiPortIndex)
        {
#if((ACOM_BOARD_SUBTYPE == RL02) || (ACOM_BOARD_TYPE == MDM9X07))
            pWirelessChannelCtl->ModuleType = NVM_GOIP_MODTYPE_SIM800;
#else
            pWirelessChannelCtl->ModuleType = NVM_GOIP_MODTYPE_M26;
#endif
        }
        else if(1 == uiPortIndex)
        {
            pWirelessChannelCtl->ModuleType = NVM_GOIP_MODTYPE_EC20;            
        }
#endif

        printf("Channel %d Module Type:%d\r\n", uiPortIndex, pWirelessChannelCtl->ModuleType);

        eos_lst_init(&pWirelessChannelCtl->stCmdList);

        sprintf(acChnM, "%s%d", ChnMutex, uiPortIndex);
        eos_sem_create(acChnM, &pWirelessChannelCtl->ComBufMutex);   
        
        pWirelessChannelCtl->pChnl = (T_HCHNL *)malloc(sizeof(T_HCHNL));
        if(pWirelessChannelCtl->pChnl == NULL)
        {
            printf("Wireless pChnl malloc error\r\n");
            return EOS_ERROR;
        }
        memset(pWirelessChannelCtl->pChnl, 0, sizeof(T_HCHNL));

        if(0 == uiPortIndex)
        {
            pWirelessChannelCtl->bESPEnable = EOS_FALSE;

            strcpy(pWirelessChannelCtl->pppdParam.ifname, "ppp0");
            strcpy(pWirelessChannelCtl->pppdParam.ipaddr, "");
            strcpy(pWirelessChannelCtl->pppdParam.proto, "3g");
            strcpy(pWirelessChannelCtl->pppdParam.service, "");
            strcpy(pWirelessChannelCtl->pppdParam.apn, "umts");
            strcpy(pWirelessChannelCtl->pppdParam.num, "");
            strcpy(pWirelessChannelCtl->pppdParam.usr, "");
            strcpy(pWirelessChannelCtl->pppdParam.pwd, "");
            strcpy(pWirelessChannelCtl->pppdParam.device, "/dev/ttyS0");

            pWirelessChannelCtl->pppdParam.isParamAlter = 0;    
        }
        else if (1 == uiPortIndex)
        {
            pWirelessChannelCtl->bESPEnable = EOS_TRUE;        
#ifdef VAD_TCP_ON_AT_ENABLED
            strcpy(pWirelessChannelCtl->pppdParam.ifname, "ppp0");
#else        
            strcpy(pWirelessChannelCtl->pppdParam.ifname, "ppp1");
#endif
            strcpy(pWirelessChannelCtl->pppdParam.ipaddr, "");
            strcpy(pWirelessChannelCtl->pppdParam.proto, "3g");
            strcpy(pWirelessChannelCtl->pppdParam.service, "");
            strcpy(pWirelessChannelCtl->pppdParam.apn, "umts");
            strcpy(pWirelessChannelCtl->pppdParam.num, "");
            strcpy(pWirelessChannelCtl->pppdParam.usr, "");
            strcpy(pWirelessChannelCtl->pppdParam.pwd, "");
#if (ACOM_HW_SUBTYPE == HWST_2G4G)
            strcpy(pWirelessChannelCtl->pppdParam.device, "/dev/ttyUSB3");
#else
            strcpy(pWirelessChannelCtl->pppdParam.device, "/dev/ttyACM0");
#endif
            pWirelessChannelCtl->pppdParam.isParamAlter = 0;
        }
        else if (2 == uiPortIndex)
        {
            pWirelessChannelCtl->bESPEnable = EOS_TRUE;        
            strcpy(pWirelessChannelCtl->pppdParam.ifname, "lsim");
            pWirelessChannelCtl->pppdParam.isParamAlter = 0;
        }
        
        pWirelessChannelCtl->uiUartWriteErrCnt = 0;

        pWirelessChannelCtl->ModuleDSIMIndex = -1;
        
        pWirelessChannelCtl->bFirstSwitch = EOS_TRUE;
            
        pWirelessChannelCtl->uiRespTimeoutCnt = 0;
        pWirelessChannelCtl->uiModJammingCnt = 0;
        pWirelessChannelCtl->uiNoAlertCnt = 0;
        pWirelessChannelCtl->uiToneDetectCnt = 0;
        pWirelessChannelCtl->uiUnderVoltageWarningCnt = 0;
        pWirelessChannelCtl->uiUnderVoltagePwroffCnt = 0;
        pWirelessChannelCtl->uiOverVoltageWarningCnt = 0;
        pWirelessChannelCtl->uiOverVoltagePwroffCnt = 0;
        
        pWirelessChannelCtl->bModulePowerOff = EOS_FALSE;   
        pWirelessChannelCtl->uiModulePowerOffTick = 0;   
        pWirelessChannelCtl->bModuleNeedPowerOn = EOS_FALSE;   
        pWirelessChannelCtl->bModuleNeedHWPwrOn = EOS_FALSE;
        pWirelessChannelCtl->uiModuleHWPwrOnTick = 0;     

        pWirelessChannelCtl->enState = EN_WIRELESS_ST_SET_BAUDRATE;
        
        pWirelessChannelCtl->enFSMSTATEIndex = EN_WRL_STATE_PWRON;
        pWirelessChannelCtl->enFSMSubSTATEIndex = EN_WRL_SUBSTATE_PWRON_IDLE;
                
        pWirelessChannelCtl->uiModuleDetectTick = 0;
        pWirelessChannelCtl->uiIMEIReadTick = 0;
        pWirelessChannelCtl->uiSIMDetectTick = 0;
        pWirelessChannelCtl->uiOprInquireTick = 0;
        pWirelessChannelCtl->uiSetNegRegTick = 0;
        pWirelessChannelCtl->uiSetNewSMSStoreModeTick = 0;

        pWirelessChannelCtl->uiGetUSSDErr = 0;

        pWirelessChannelCtl->iCallIndex = -1;

        pWirelessChannelCtl->bModuleUpdating = EOS_FALSE;
        
        pWirelessChannelCtl->OperatorID = 0;
        pWirelessChannelCtl->OprtMCC = 0;
        pWirelessChannelCtl->OprtMNC = 0;
        
        pWirelessChannelCtl->uiInquireNetRegStatusTick = 0;
        pWirelessChannelCtl->uiLastATCmdSendTick = 0;
                
        memset(pWirelessChannelCtl->USSDCmdBuffer, 0, sizeof(pWirelessChannelCtl->USSDCmdBuffer));
        memset(pWirelessChannelCtl->USSDBuff, 0, sizeof(pWirelessChannelCtl->USSDBuff));
        memset(pWirelessChannelCtl->USSDInvalidBalanceKeyStr, 0, sizeof(pWirelessChannelCtl->USSDInvalidBalanceKeyStr));
        memset(pWirelessChannelCtl->USSDInvalidSimKeyStr, 0, sizeof(pWirelessChannelCtl->USSDInvalidSimKeyStr));
        
        memset(pWirelessChannelCtl->strSMSFwdNumber, 0, sizeof(pWirelessChannelCtl->strSMSFwdNumber));
    }
    
    pWirelessRootSt->wirelessInitFunPtr     = WIRELESS_AppInit;
    pWirelessRootSt->wirelessMainTask       = WIRELESS_AppMainTask;
    pWirelessRootSt->wirelessStat           = WIRELESS_AppStat;    

    /*    wireless init    */
    if(EOS_ERROR == pWirelessRootSt->wirelessInitFunPtr(param))
    {
        printf("wireless init error\r\n");
        return EOS_ERROR;
    }
        
    return EOS_OK;
}

int WIRELESS_unInit(void)
{
    unsigned int channelID = 0;
    PWIRELESSROOTST pWirelessRootSt;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    for(channelID = 0; channelID < 2; channelID++)
    {
        WIRELESS_freeUart(channelID);
    }
    printf("WIRELESS_unInit: Free the resource ok!\n");
}

_INT32 WIRELESS_Start(_VOID)
{
    PWIRELESSROOTST pWirelessRootSt;
    _INT32 task = 0;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;

#ifdef ACOM_DEBUG
    UT_LogSetBuf(DEBUG_MODULE_WIRELESS, 32);
#endif
 
    task = UT_TaskSpawn("tWirelessTask", 10, 0, 256*1024, (FUNCPTR)(pWirelessRootSt->wirelessMainTask),
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (task > 0)
    {
        /* set the wireless task exception handler */
        UT_ExcTaskHookAdd(task, wirelessExcHandler, NULL);
    }

   return EOS_OK;
}

/*    for wireless statistics    */
_VOID WIRELESS_Stat(EOS_INFO_PRINTF fpPrint, _VOID* pData)
{
    PWIRELESSROOTST pWirelessRootSt;
    
    pWirelessRootSt = (PWIRELESSROOTST)&stWirelessRootSt;
    pWirelessRootSt->wirelessStat(fpPrint, pData);
} 

/* the function only print the wireless task statistics for now */
_INT32 wirelessExcHandler(_INT32 task, _VOID* data)
{
    _INT32 size = 4*1024;
    ST_UT_SBUF sbuf;

    if (UT_SbufInit(&sbuf, NULL, size))
    {
        printf("out of memory(%d)!\r\n", size);
        return -1;
    }
    
    WIRELESS_Stat((EOS_INFO_PRINTF)UT_SbufAdd, &sbuf);

    UT_LogLongBuf(sbuf.buf, sbuf.len, 1024, 100);
    
    /* 2011.12.15 pengjian added */
    /* clear the string buffer */
    sbuf.len = 0;
    
    UT_LogStat((EOS_INFO_PRINTF)UT_SbufAdd, &sbuf);

    /* send the buffer to log server */
    UT_LogLongBuf(sbuf.buf, sbuf.len, 1024, 100);

    UT_SbufFree(&sbuf);

    WIRELESS_TRACE;

    taskDelay(3*sysClkRateGet());

    return EOS_OK;
}


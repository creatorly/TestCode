/********************************************************
* 
*
* Ejoin ViFi platform
* Copyright (C) 2015-2015, All rights reserved
* Designed by Ejoin XP Development Team.
* 
* 
* File:         vad_config.h
* PACKAGE:      
*   Date:               2015.10.13
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/

#ifndef  __VAD_CONFIG_H__
#define  __VAD_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

/* boot simcard is 2G, if not, comment out this macro definition */
#define __VAD_EJOIN_BOOT_SIM_2G__

/* EVCP: Ejoin ViFi Communication Protocol */
#define VAD_EVCP_VDC_VDG    1
#define VAD_EVCP_VDC_VDS    2
#define VAD_EVCP_VDC_VNS    3               /* temporary value */
#define VAD_EVCP_VDS_VNS    4

/* VDX: VDG and VDS */
#define VAD_EVCP_VSW_VDX    4

#define VAD_EVCP_VDC_PORT   10100
#define VAD_EVCP_VDG_PORT   10110
#define VAD_EVCP_VDS_PORT   10120

#define VAD_EVCP_APPD_PORT  8090

#define VAD_BOOT_SIM_CHANNEL        0
#define VAD_REM_SIM_CHANNEL         1
#define VAD_LOC_SIM_CHANNEL         2

#define VAD_PORTAL_AUTH_AUTO        0
#define VAD_PORTAL_AUTH_DISABLE     1
#define VAD_PORTAL_AUTH_ENABLE      2

#define VAD_PORTAL_AUTH_TYPE        VAD_PORTAL_AUTH_AUTO

/* using AT command replace the ppp dialing of boot SIM */
#if (ACOM_BOARD_TYPE == MT673X)
#define __VAD_VER_U3_AT__     1
#define VAD_FAC_CHK_FUNCTION  0
#define VAD_FAC_CHK_SIGNAL    0

#elif (ACOM_BOARD_TYPE == MDM9X07)
#define __VAD_VER_U3_AT__     1
#if (ACOM_BOARD_SUBTYPE == MDM_01)
#define VAD_FAC_CHK_FUNCTION  0
#endif
#elif (ACOM_BOARD_TYPE == RALINK) 
#define __VAD_VER_U3_AT__     1
#endif

#define VAD_WDT_INCLUDED            EOS_FALSE

#if (ACOM_BOARD_TYPE == MDM9X07)
/* define the configuration for board MDM9X07 */

#if (ACOM_BOARD_SUBTYPE == MDM_01)
/* define the macro VAD_UI_ENABLED if you want to enable it, otherwise undef it */
#define VAD_UI_ENABLED          1
#define VAD_PIN_LOCK_ENABLED    1

#define VAD_WIFI_PASSWORD       "12345678"
#define VAD_WIFI_MAX_CLIENTS    8
#define VAD_NETSIM_SELECT       "L"

#define VAD_POWER_IDLE_ACTION   1
#endif

#if (ACOM_BOARD_SUBTYPE == MDM_02)
#undef VAD_UI_ENABLED
#undef VAD_PIN_LOCK_ENABLED

#define VAD_WIFI_PASSWORD       ""
#define VAD_WIFI_MAX_CLIENTS    0

#define VAD_NETSIM_SELECT       "R"
#define VAD_POWER_IDLE_ACTION   0
#endif

#if (ACOM_BOARD_SUBTYPE == MDM_03)
#define VAD_UI_ENABLED          1
#undef VAD_PIN_LOCK_ENABLED

#define VAD_WIFI_PASSWORD       ""
#define VAD_WIFI_MAX_CLIENTS    0

#define VAD_NETSIM_SELECT       "R"
#define VAD_POWER_IDLE_ACTION   2
#endif

#else
/* for other board type */

#undef VAD_UI_ENABLED
#define VAD_PIN_LOCK_ENABLED    1

#define VAD_WIFI_PASSWORD       "12345678"
#define VAD_WIFI_MAX_CLIENTS    8

#define VAD_NETSIM_SELECT       "L"
#define VAD_POWER_IDLE_ACTION   1
#endif

/*******************************************************************************
 * define the rules chain for iptables
 */
#if (ACOM_BOARD_TYPE == MDM9X07)
#define VAD_FORWARDING_RULE     "FORWARD"
#define VAD_PREROUTING_RULE     "PREROUTING"
#define VAD_BRIDGE_IPADDR       "192.168.0.1"
#elif (ACOM_BOARD_TYPE == RALINK)
#define VAD_FORWARDING_RULE     "forwarding_rule"
#define VAD_PREROUTING_RULE     "prerouting_rule"
#define VAD_BRIDGE_IPADDR       "192.168.1.102"
#else
#define VAD_FORWARDING_RULE     "FORWARD"
#define VAD_PREROUTING_RULE     "PREROUTING"
#define VAD_BRIDGE_IPADDR       "192.168.1.1"
#endif
/******************************************************************************/

#define VAD_UPGRADE_MIN_BATTERY     20

/* the period in seconds to get vdg information */
#define VAD_GET_VDG_INFO_PERIOD     5

/* the period in seconds to get vdg information */
#define VAD_CHK_ETH_PERIOD      1

#ifdef ANDROID
#define VAD_HOME                "/sdcard/ufi"
#define VAD_FSHOME              "/sdcard/ufi"
#else
#define VAD_HOME                "/opt/ejoin"
#if (ACOM_BOARD_TYPE == MDM9X07)
#define VAD_FSHOME              "/opt/ejoin"
#elif (ACOM_BOARD_TYPE == RALINK)
#define VAD_FSHOME              "/opt/ejoin_fs"
#endif
#endif

/* share memory file */
#define VAD_MMAP_FILENAME       "var/mmap"
#define VAD_MMAP_FILE           VAD_HOME"/"VAD_MMAP_FILENAME

/* share memory file in FLASH */
#define VAD_MMAP_FFILENAME      "var/flash_mmap"
#define VAD_MMAP_FFILE          VAD_HOME"/"VAD_MMAP_FFILENAME

#define VAD_PIPE_DIR            VAD_HOME"/var/pipe"
#define VAD_PIPE_SHELLD         VAD_PIPE_DIR"/vshd"

#define VAD_VAR_DIALTIME_FILE   VAD_HOME"/var/dialtime"
#define VAD_VAR_RSIM_FILE       VAD_HOME"/var/rsim"

#define VAD_VAR_VDC_PID         VAD_HOME"/var/vdc.pid"
#define VAD_VAR_VFD_PID         VAD_HOME"/var/vfd.pid"
#define VAD_VAR_VWA_PID         VAD_HOME"/var/vwa.pid"

#define VAD_APP_CFGFILE         "etc/app.conf"
#define VAD_APP_FACTESTFILE     "etc/factest.conf"
#define VAD_APP_SIMSTATEFILE    "etc/simstate.conf"
#define VAD_VWA_CFGFILE         "etc/vwad.conf"
#define VAD_VDG_CFGFILE         "etc/vdg.conf"
#define VAD_VDS_CFGFILE         "etc/vds.conf"
#define VAD_VDC_CFGFILE         "etc/vdc.conf"

#define VAD_VFD_LOGFILE         "log/vfd/vfd"
#define VAD_VWA_LOGFILE         "log/vwa/vwa"
#define VAD_VDC_LOGFILE         "log/vdc/vdc"

#ifdef ANDROID
#define VAD_LOGFILE_SIZE        1
#define VAD_LOGFILE_COUNT       10
#else
#define VAD_LOGFILE_SIZE        1
#define VAD_LOGFILE_COUNT       5
#endif

#define VAD_TPOOL_MAX_STACK_SIZE    128*1024

#define VAD_VFD_MAX_CACHED_FILES    10

#define VAD_VFD_HTML_INDEX      "/index.html"
#define VAD_VFD_HTML_FILE_LIST  "/file_list.json"
#define VAD_VFD_HTML_PROGRESS   "/progress.json"

#ifdef ANDROID
#define VAD_VFD_BINFILE         VAD_FSHOME"/vad.apk"
#else
#define VAD_VFD_BINFILE         VAD_FSHOME"/vad.tgz"
#endif

#define VAD_DEFAULT_OEM_NAME    "MiFi"

/* the apn info */
#define VAD_DEFAULT_APN     "3GNET"
#define VAD_DEFAULT_NUM     "*99#"
#define VAD_DEFAULT_USR     ""
#define VAD_DEFAULT_PWD     ""

typedef enum tagenVadDailingState
{
    VAD_DIAL_STATE_NONE = 0,        /* sleep or no dailing */
    VAD_DIAL_STATE_DAILING,
    VAD_DIAL_STATE_OK,
    VAD_DIAL_STATE_FAIL,            /* failed temporarily, retry after */
}EN_VAD_DIAL_STATE;

typedef enum tagenVadDialProgressCode
{
    VAD_DPC_STOPPED = 0,
    VAD_DPC_WAIT = 100,
    VAD_DPC_REG_3G_PORT = 105,
    VAD_DPC_REG_3G_SIM = 110,
    VAD_DPC_DIAL_3G_NET = 115,
    
    VAD_DPC_CONN_SERVER = 120,
    VAD_DPC_GET_4G_SIM = 125,
    
    VAD_DPC_REG_4G_PORT = 130,
    VAD_DPC_REG_4G_SIM = 135,
    VAD_DPC_DIAL_4G_NET = 140,

    VAD_DPC_OK = 200,
    VAD_DPC_CHARGE_MODE,

    VAD_DPC_SYS_KERNEL_FAIL = 300,
    VAD_DPC_SYS_NO_POWER,
    VAD_DPC_SYS_NO_3GPORT,
    VAD_DPC_SYS_NO_4GPORT,

    VAD_DPC_3G_REG_FAIL = 400,
    VAD_DPC_3G_SETPAR_FAIL,
    VAD_DPC_3G_DIAL_FAIL,
    VAD_DPC_3G_SETROUTE_FAIL,

    VAD_DPC_SVR_CONN_FAIL = 500,
    VAD_DPC_SVR_NO_COUNTRY,
    VAD_DPC_SVR_NO_MONEY,
    VAD_DPC_SVR_NO_SIM,

    VAD_DPC_4G_SIMDATA_FAIL = 600,
    VAD_DPC_4G_REG_FAIL,
    VAD_DPC_4G_SETPAR_FAIL,
    VAD_DPC_4G_DAIL_FAIL,
    VAD_DPC_4G_SETROUTE_FAIL,
}EN_VAD_DPC;

typedef enum
{
    VAD_APPSET_CFG_NONE = 0,
    VAD_DIAL_CFG_APNTYPE,
    VAD_DIAL_CFG_APN,
    VAD_DIAL_CFG_NUM, 
    VAD_DIAL_CFG_USR, 
    VAD_DIAL_CFG_PWD,
    VAD_APPSET_CFG_WIFIPWD,
    VAD_APPSET_CFG_WIFIFREQ, 
    VAD_APPSET_CFG_DATATYPE, 
    VAD_APPSET_CFG_GNETENB,
    VAD_APPSET_CFG_NETENB,
    VAD_APPSET_CFG_WIFISSID,
    VAD_APPSET_CFG_POWERMODE,
    VAD_APPSET_CFG_PWR_IDLETIME,    /* power off if this time expired without any WiFi connection */
    VAD_APPSET_CFG_PWR_ACTION,      /* 0:nune, 1,shutdown, 2,sleep */
    VAD_APPSET_CFG_OEM_NAME,
    VAD_APPSET_CFG_FIRSTMNC,
    VAD_APPSET_CFG_IMEI_1,
    VAD_APPSET_CFG_IMEI_2,
    VAD_APPSET_CFG_WIFIHIDDEN,
    VAD_APPSET_CFG_WIFIMAXASSOC,
    VAD_APPSET_CFG_SVRPINTYPE,
    VAD_APPSET_CFG_DEVPINTYPE,
    VAD_APPSET_CFG_DEBUGMODE,
    VAD_APPSET_CFG_SIMROAM,
    VAD_SWLSIM_CFG_RFAILTMS,
    VAD_SWLSIM_CFG_RTIMEOUT,
    
    VAD_APPSET_CFG_MAX
}EN_VAD_APPSET_CFG;

typedef enum
{
    VAD_FACTEST_CFG_STATUS = 0,
    VAD_FACTEST_CFG_ALL,
    VAD_FACTEST_CFG_FAC1,
    VAD_FACTEST_CFG_FAC2,
    VAD_FACTEST_CFG_WIFIDBM,
    VAD_FACTEST_CFG_2GDETECT,
    VAD_FACTEST_CFG_2GCPIN,
    VAD_FACTEST_CFG_2GCGREG,
    VAD_FACTEST_CFG_2GCSQ,
    VAD_FACTEST_CFG_4GLOCALCPIN,
    VAD_FACTEST_CFG_4GLOCALCGREG,
    VAD_FACTEST_CFG_4GLOCALCSQ,
    VAD_FACTEST_CFG_4GVIRTLCPIN,
    VAD_FACTEST_CFG_4GVIRTLCGREG,
    VAD_FACTEST_CFG_4GVIRTLCSQ,
    VAD_FACTEST_CFG_4GVIRTLTEST,
    VAD_FACTEST_CFG_LED, 
    VAD_FACTEST_CFG_KEY,
    VAD_FACTEST_CFG_SD,
    VAD_FACTEST_CFG_GPS,
    VAD_FACTEST_CFG_CHARGING,
    VAD_FACTEST_CFG_DISCHARGE,
    VAD_FACTEST_CFG_NULL1,
    VAD_FACTEST_CFG_NULL2
    
}EN_VAD_FACTEST_CFG;

typedef enum
{
    VAD_SIMSTATE_CFG_2GDETECT = 0,
    VAD_SIMSTATE_CFG_2GCGREG,
    VAD_SIMSTATE_CFG_4GLOCALCPIN,
    VAD_SIMSTATE_CFG_4GLOCALDIAL,
    VAD_SIMSTATE_CFG_4GVIRTLGETSIM,
    VAD_SIMSTATE_CFG_4GVIRTLDIAL,
    VAD_SIMSTATE_CFG_NULL1,
    VAD_SIMSTATE_CFG_NULL2
    
}EN_VAD_SIMSTATE_CFG;

typedef struct tagstVadVersion
{
    char ver[12];       /* x.x.x */
    int rev;            /* revision in svn server */
    
    char bdate[16];     /* build date */
    char btime[16];     /* build time */
}ST_VAD_VER;

typedef struct tagstVadDevBatteryInfo
{
    int health;           /*1: death, 0:good */
    int charging;         /*1: connected charging line, 0:disconnect*/
    int discharging;      /*1: connected discharging line, 0:disconnect*/
    int voltage;          /*MV*/
    int ecin;             /*MA*/
    int ecout;            /*MA*/ 
    char soc;             /* battery soc 0-100 */
    int temp;             /*temperature*/ 
}ST_VAD_DEV_BATTERY_INFO;


typedef struct tagstVadDevInfo
{
    char dev_id[32];
    char hw_ver[16];
    char fw_ver[128];
    char name[32];
    char password_md5[36];
    char ver[32];               /* x.x.x */
    char datatype[8];              /* net type, L/R/G -> local sim;remote sim; boot sim*/
    int powermode;             /* power save mode,second (0:disable, >0:enable) */
    int idle_action;            /* idleAction:0(NONE),1(shutdown),2(sleep)*/
    int firstmnc;
    char oem_name[16];
#ifdef ANDROID
    long int run_time;             /*start-up :uptime*/
#endif
int bDrvSysInitCurTime;
}ST_VAD_DEV_INFO;


#define VAD_SIM_LBS_NUM      6    /*the number of CELL ID & LAC*/
typedef struct tagstVadSimLbs
{
    char cellid[12];            /* the id of the base station */
    unsigned int lac_code;      /*Location area code*/
    unsigned int mcc;           /*the mcc of the base station*/
    unsigned int mnc;           /*the mnc of the base station*/
}ST_VAD_SIM_LBS;

typedef struct tagstVadSimInfo
{
    char iccid[24];     /* ccid ,20 byte */
    char imsi[16];
    char imei[16];
    int  mnoid;         /* the MNO id of the SIM CARD!!! */
    
    int  ispid;         /* the MNO id of the local mobile network */
    char ispname[32];   /* the MNO name */
    short sigint;        /* the signal intensity of sim card :1~31*/
#if ((ACOM_BOARD_TYPE == MT673X) || (ACOM_BOARD_TYPE == MDM9X07))
    short sigdbm;        /* the signal dbm of sim card :-1~-99*/
#endif    
    short sigtype;       /* the signal type of sim card, 2=2G;3=3G;4=4G;other=none; */
    char cellid[12];            /* the id of the base station */
    unsigned int dataup;        /* up fow */
    unsigned int datadown;      /* down fow */
    unsigned int uprate;        /* the speed of up flow */
    unsigned int downrate;      /* the speed of down flow */
    unsigned int rate;          /* the speed of the net. kb/s */
    char netenb[12];            /* support the net type of the sim card */
    char ethname[12];           /* pppoe dial,the name of the eth port */
    ST_VAD_SIM_LBS sim_lbs[VAD_SIM_LBS_NUM];
}ST_VAD_SIM_INFO;


typedef struct tagstVadSWLSimCfg
{
    int swlsim_rfailtms;
    int swlsim_rtimeout;
    int swlsim_flag;         /*switch to localsim reason:1 nettype local_sim,2 vns set,3 rfailtms, 4 rtimeout, 3 vns set*/
    char lsim_host[32];      /*host for localsim UPDATE */
    int lsim_port;
}ST_VAD_SWLSIM_CFG;


typedef struct tagstVadApnInfo
{
    char type[4];     /*R: remote sim apn, L: local sim apn*/
    char apn[32];
    char num[32];
    char usr[32];
    char pwd[32];
}ST_VAD_APN_INFO;

typedef struct tagstVadDnsAddr
{
    char    host[128];
    char    ip[32];
    int     port;
    char    sid[32];      /* session id */
    int     expires;      /*day*/
    unsigned int lasttime;
    int     serv_dur;     /*consume the time of request the server*/
    int     check_expires;/*expires > 0; check_expires=0,1; 1:next boot no request VRS, 0: need request VRS*/
    int     error_count;
}ST_VAD_DNS_ADDR;

typedef struct tagstVadSimFlow
{
    unsigned int f_kbs;
    unsigned int f_bs;
}ST_VAD_SIM_FLOW;


typedef struct tagstVadUpdateRSimFlow
{
    char ifname[32];
    unsigned int dataup;        /* up fow */
    unsigned int datadown;      /* down fow */
}ST_VAD_UPDATE_RSIM_FLOW;

typedef struct tagstVadRC4SvrKey
{
    char key[64];
    unsigned int klen;
}ST_VAD_RC4_SVR_KEY;

/***************************************************************************************
 * NOTE: the data of this structure CAN be wrote to FLASH!!! 
 */

#define VAD_FLASH_DATA_MAGIC    0xD6BC

/* you MUST increase the version if you changed the structure ST_VAD_FLASH_DATA */
#define VAD_FLASH_DATA_VER      5

typedef struct tagstVadFlashData
{
    unsigned short magic;
    unsigned short version;
    
    char gsim_iccid[24];
    
    ST_VAD_SIM_FLOW gsim_up;        /* the total up-data bytes of startup SIM */
    ST_VAD_SIM_FLOW gsim_down;      /* the total down-data bytes of startup SIM */
    ST_VAD_DNS_ADDR vrs_addr;
    ST_VAD_DNS_ADDR vns_addr;
    ST_VAD_UPDATE_RSIM_FLOW rsim_flow;
    unsigned short setpin;
    unsigned short pinerror;
    ST_VAD_RC4_SVR_KEY rc4_key;
}ST_VAD_FLASH_DATA;

typedef struct tagstVadPublicData
{
    ST_VAD_VER v_vdc;
    ST_VAD_VER v_vdg;
    ST_VAD_VER v_vds;
    ST_VAD_VER v_vwa;

    int syserr;         /* the system errno reserved */
    int dstate;         /* the dialing state, defined by EN_VAD_DIAL_STATE */
    int dpc;            /* dial progress code defined by EN_VAD_DPC */
    int syswarn;        /* the system warn,but not important*/

    int is_saving;      /* saving FLASH ..., DONOT poweroff */
    int is_idle;        /* 1:device into idle, not work; 0:working*/

    ST_VAD_DEV_INFO devinfo;    /* device information */
    ST_VAD_SIM_INFO gsiminfo;   /* gsim information */
    ST_VAD_SIM_INFO rsiminfo;   /* remote sim information */
    ST_VAD_SIM_INFO lsiminfo;   /* local sim information */
    ST_VAD_APN_INFO simapn;    /* sim apn information */
    ST_VAD_DEV_BATTERY_INFO batteryinfo;   /* the battery info*/

    char wifissid[16];     /* the ssid of the wifi */
    char wifikey[16];     /* the key of the wifi */
    int wificonnt;     /* the sum of the wifi client */
    int wifisig;       /* the signal of the wifi */
    int wififreq;      /* the channel of the wifi frequence */

    int cfgUpdate;      /* the config file has updated */

    char vns_ip[32];        /* vns server ip address */
    int  vns_port;             /* vns server port */

    char vsw_ip[32];        /* vsw server ip address */
    int  vsw_sport;         /* vsw server port */

    char location[32];        /*gsim:longitude and latitude */

    ST_VAD_SWLSIM_CFG swlsim_cfg;         /*switch local_sim config */
}ST_VAD_PUB_DATA;

#ifdef __cplusplus
}
#endif

#endif


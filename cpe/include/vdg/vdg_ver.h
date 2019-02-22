/********************************************************
* Ejoin utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         vdg_ver.h
* PACKAGE:      
*   Date:               2016.03.09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDG_VER_H__
#define  __VDG_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 1.0.1        the first version of vdg
 *     2        add member dial state to share memory
 *     3        ignore the SIM status 2(registering) when SIM offline
 *     4        weikk,a.when the vdc stoped dialing, giving a popup page to  affirm dialing again.
                      b.add a AT command XREG,get the net mode of the SIM card.and display on lcd.
       5  1657  weikk,a.support UDP
       6  1671  pj, change the default retransmit params to 500(first), 1000(interval), 4(count)
       7  1672  weikk,a.adjust the battery soc.
       8  1698  weikk,a.modify the battery ico,and blink.
                      b.add the display for net type of the 4G module.
       9        weikk,a.add the apn of the local sim vodafone
                      b.bug,when it(channel 0) sets pppd paramer, the MNO is 0. fix : Waiting the Sim ready.
       10 1751  pj, add the call mechnizm before dial the 4G network
       11 1757  weikk,a.delete the AT "XCESQ"
                      b.if it uses a new card,make a call to activate the sim card.
          1809        c.if the startup sim is not known,read the apn from /opt/ejoin_fs/etc/dp.conf
       12 1934  weikk,a.config the mode of 4G module.local sim or virturl sim. activate after restart.(delete)
                      b.add 3 tabs to the menu page.used to choose device mode.check the mode from the file "/opt/ejoin_fs/dev.conf".
                      c.shut down the 3g and 4g modules before powering off device.
                      d.if the config file is same, don't write it.
       13       weikk,modify reset simdat of the sim, separating the sim and usim card.
       14       pj, add the configuration of MCC, MNC, LAC and CELLID
       15       weikk,a.change the conditions of the connection dropped.
                      b.if the number of the wifi client is zero for ten minutes, disconnecting the network.
                      c.change the way to shut the pppoe connetion.needn't to restart the 3G module.
                      d.add the MMAD command. get the current of the battery.
       16       pj, add the feature of log uploading
       17       pj, cancel the NAT keepalive message
       18       dy, fixed Segmentation fault about comnRunShellCmd()
       19       weikk,a.lcd display, keyboard
                      b. sleep mode.
                      c. option file. /opt/ejoin_fs/etc/app.conf
       20       weikk,rewrite soc.
       21       weikk,add four modes,R/L/G/A, activate after restart. not support G mode.
       22       weikk,add G mode,display mode in the screen.
       23       pj, add the feature of sim data cache
       24       weikk,a. add 2+4G define,RL02 define
                      b. led blink,one key mode, lcd off, usb output
                      c. sim800,EC20,change the way to turn on/off module, AT command change,add new AT command.
       25       pj, enable the eos watch dog
       26       weikk,a.idle status.green led off
                      b.keyboard -> long press, when key up,consent 'long press'
       27       kinson, a.don't trun on green led when charging mode.
                        b.reset 4g module if report too many errors when write uart. 
       28       pj, fixed the issue of no SIM_STATUS reported when SIM is offline
       29       wkk,detect the module status when power on/off the module; 
       30       wkk,a.correct:4g module dead,wait timeout
                    b.support to config wifi-key/wifi-chnl 
       31       wkk,add AT+QNWINFO,query the network type.2g/3g/4g
       32       lxx,modify TLV_UPDATE session, add log of print TLV_NET_DATA , process tlv_send_data error. 
       33       pj, fixed the issue of socket loss when check the wifi client
       34       wkk,add ccid ,repair that the AT response is topsy-turvy.
       35       wkk,a. add RL03,for PCB v3
                    b. RL03,led,AT for m26.
       51       pj, fixed the issue of session disconnection when the default route is changed
       56       pj, cancel the UPDATE request and ntpdate if the remote SIM not dialed
                pj, set the default power idletime to 1800
       57       pj, add the power safing when system is updating
                ly, change the lowest battery of power saving from 30 to 20
                ly, fixed the issue of batter growing up when it is discharging
       58       ly, fixed the issue of poweroff 2g/4g
       59       ly, Modify LocalCard APN problem
                pj, add the feature of issuing a UPDATE when ppp1 is connected
                 lxx, added: gsim transport data by AT 
       60       kinson,process sim data unnormal after simreset.
                pj, fixed the issue of blocking in utl_sock_recv() for UDP block socket
                pj, add the evdg param EVDG_PARAM_TIMEZONE
                pj, add the common header file sys_debug.h
       61       kinson support imei modify for U3        
       62       kinson,u3,don't enter sleep for android.
                pj, add the param CCM_WMCTL_PARAM_LOCATION for wireless
                pj, no need to notify the location information immediately
       64       pj, fixed the issue of dailing remote SIM too slow
 **************************************************************************/

#define VDG_VERSION     "1.2"
#define VDG_BLD_NUM     64
#define VDG_SVN_VER     4907

/* the version information will be saved to the file */
#define VDG_VER_FILE    "/opt/ejoin/var/vdg.ver"

#if defined(__cplusplus)
}
#endif

#endif


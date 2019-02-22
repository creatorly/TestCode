/********************************************************
* Ejoin utils platform
* Copyright (C) 2016-2016, All rights reserved
* Designed by Ejoin Development Team.
* 
* 
* File:         vdc_ver.h
* PACKAGE:      
*   Date:               2016.03.09
*   Author:             PengJian
*   Version:            1.0
*
* Change
* 
*********************************************************/
    
#ifndef  __VDC_VER_H__
#define  __VDC_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 1.0.1    the first version of vdc
 * 1.1.1    add the exception flow process
 *     2    fixed the issue of dialing directly when 3G is being disconnected
 *          add the SET interface of dial param
 * 1.2.3    add the call mechnizm before dial the 4G network
 *     4    add the params mcc, mnc, lac and cellid for GET request
 *     5    add the feature of log uploading by GET-ACK message
 *     6    add the feature of log uploading by UPDATE-ACK message
 *     6    fixed segmentation fault bug about call utl_select_del
 * 2.0.1    the second version of vdc tlv protocol
 *     7    add watchdog
 * 2.1.8    add the feature of firmware updating vi HTTP
       9    add the feature of binary verification
       9    fixed the issue of failure to update via script load_vad.sh and lrz_vad.sh
       10   pj, fixed the issue of time count error for the failure of local SIM dialing
       11   lxx, add route of the ppp0 for vsw_ip
       12   lxx, add the params iccid and mnoid for GET request
       15   pj, fixed the issue of failure to update in web page
       17   pj, fixed the issue of reporting operator id error
                fixed the issue of no traffic data in local SIM mode
       19   pj, add the feature of notify the traffic of boot SIM
       20   pj, fixed the issue of rebooting twice when updating
       21   pj, set the default value of power idletime to 1800
       22   lxx, optimized the vrs/vns/vsw flow
            lxx, added the configuration of oem_name
            pj, added the feature of low battery safing when upgrading
       23   lxx, added the APP set firstmnc to switch remote sim
       24    lxx, added the APP set: ssid,ssid hidden, local_sim apn, reboot,shutdown
 * 2.3.24   pj, starting the processes(vdg/vdc/vfd/vwad) without gdb
            pj, using watch_dogd.sh script to replace the forked watchdog process
            pj, fixed the issue of starting the main task repeatedly perhaps
       25   pj, replaced the top process using the api eos_get_cpustat()
            pj, disabled the system ntpd, telnetd, sshd and httpd
            lxx, PORTAL type(AUTO):encryption=none,type(DISABLE):encryption=psk2
            pj, merged the process vdg to vdc
            pj, add the support for localcard mode in 2in1 process vdc
       26   pj, fixed the issue of crash when working as local-SIM mode
            pj, add the vad home directory for android
            lxx, Dev Used local_sim:get battery_soc,ischarging;set wifi_ssid,wifi_key
            lxx, added: gsim transport data by AT
            pj, add the feature of getting authorization from the first HTTP for vwad
            pj, add the feature of the verification of the update package
 * 3.0.27   pj, compiled in android for U3
            pj, removed the initialization of the config in VDC
            pj, modified the log home directory for vfd to corrected value
            pj, fixed the issue of failure to access the web pages via vfd in android
            lxx, get vdc APP information for vdg UPDATE
            pj, fixed the issue of failure to set logfile count more than 5
       28   pj, change the binary type from vad to vad_mdm
       29   pj, only unlock the root partion when updating
       30   pj, optimized the burnin bat script
       31   ly, fixed the issue of crash when timeout in sending or receiving data via AT
 * 3.1.31   pj, using the start script /usr/bin/start_oem_script for H1
            pj, fixed the issue of failure to get did sometimes
            pj, add the header file include/vad_tlvdef.h
            pj, fixed the issue of failure to get all logfiles
            pj, changed the default oem name to "MiFi"
 * 3.1.32   pj, add the support of the limitation of the uploading logfiles
            pj, fixed the issue of failure to upload logfile in device U3
            pj, add the feature of auto push test scripts
            pj, fixed the issue of failure to lock the rootfs when running a test script
            pj, add the feautre of confirmation before update app
            pj, add the feature of limitation of network speed
            pj, fixed the issue of no signal display in app for H1
            pj, fixed the issue of making the vdc_dst value error when boot SIM is online
 * 3.1.33   xjt, fixed the issue of failure to switch SIM
            pj, change the install and check bat to Setup.bat and Check.bat
 * 3.1.34   pj, fixed the issue of failure to start wifi when updated to ROM-201710280520
            pj, added the the param 80 and 81 in VNS request
 * 3.1.35   lxx, add the feature of VRS and DNS caching
            lxx, modified the retry interval to 30s,1m,2m,4m,4m... till 30 mins expired
            pj, fixed the issue of failure to start vdc when using gdb
            pj, enabled the UI local socket for H1-01
            pj, defined the default wifi password to "12345678" for H1-01 device
            pj, fixed the issue of error to limit the net speed
            lxx, fixed the issue of failure to get wifi connections via APP
            lxx, add the support of reuse of 2G connection
            xjt, fixed the issue of failure to dial the ppp
            xjt, decrease the start time of 2G
            ly, fixed the issue of wifi disabled in the first booting after updating
            pj, add the dailtime file(/opt/ejoin/var/dialtime)
            pj, add the remote SIM file(/opt/ejoin/var/rsim)
 * 3.1.36   pj, add the local SIM flow
            pj, add the feature of customization when updating via Setup.bat
            pj, add the version control of the flash data
            lxx, add the feature of logining in VRS agian when device not found or too many failure
            ly, clear the flash data when user resetting a device
 * 3.1.37   pj, fixed the issue of failure to start 4G dailing if the 2G network dailed OK too fast
            lxx, add the feature of SIM releasing controlled by server
            pj, fixed the issue of dailling too slow when a wifi client connected in IDLE state
            pj, add the feature of auto configuration of customized parameters
            pj, change the default value of network SIM select to "R" for H1-02 and H1-03
            pj, delay the starting of vfd and vwad
 * 3.1.38   pj, decrease the cpu usage of vdc and vfd
            pj, synchromous the changes to U2
            pj, add the default DNS 8.8.8.8
            pj, reduced the frequency of the updating of data usage
 * 3.1.39   pj, add the feature of ad pushing
            pj, add the feature of PIN
            ly, add the feature of waiting for the starting of process hostapd
            pj, add the boot scripts mechnizm via ejoin_fs/etc/rc0.d
            pj, fixed the issue of executing the commands in UPDATE_ACK continually
            pj, fixed the issue of crash if vdc started too fast
            pj, add the feature of appending version field to binary file
            pj, fixed the issue of failure to update binary for H1(2+1)
 * 3.1.40   pj, the first version of 110
            pj, forbidden the feature of PIN LOCK for H1-02 and H1-03
            pj, fixed the issue of failure to check the remain data traffic
            ly, fixed the issue of failure to set PIN sometimes
            ly, fixed the issue of failure to get IMEI of 2G module
            pj, fixed the issue of failure to disable the feature of USB network
            pj, fixed the issue of failure to send long http response
 * 3.1.41   pj, fixed the issue of failure to reboot after upgrading

 **************************************************************************/

#define VDC_VERSION     "3.2"
#define VDC_BLD_NUM     41
#define VDC_SVN_VER     5806

/* the version information will be saved to the file */
#if (ACOM_BOARD_TYPE == MT673X)
#define VDC_VER_FILE    "/sdcard/ufi/var/vdc.ver"
#else
#define VDC_VER_FILE    "/opt/ejoin/var/vdc.ver"
#endif

#if defined(__cplusplus)
}
#endif

#endif


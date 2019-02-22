#ifndef __EOS_EXEC_H
#define __EOS_EXEC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef _VOID(*eosWdgFunc)(_VOID* uiParam);
/* 20070122    */
typedef _VOID(*eosCheckFunc)(_VOID* uiParam);

_INT32 eos_wdg_init(_INT32 (*fpHwWdgInit)(), _INT32 (*fpHwWdgClr)());

_INT32 eos_wdg_add_handler(eosWdgFunc pFunc, _VOID* pData);
_INT32 eos_wdg_add_timer(_UINT32 period, _VOID (*fpHandler)(_VOID*), _VOID* data);

_INT32 eos_wdg_start(_INT32 maxIdleTime);
_INT32 eos_wdg_clear(_INT32 expires);

_VOID eos_wdg_pause();
_VOID eos_wdg_resume();

/* the default command for reboot is "reboot" */
_VOID eos_wdg_set_cmd_reboot(const char* cmd_reboot);

/* set deadtalk properties, if size is zero or negative no deadtalk is saved */
_VOID eos_wdg_set_dt_size(_INT32 size);
_VOID eos_wdg_set_dt_file(const _CHAR8* file);
_VOID eos_wdg_set_dt_logfile(const _CHAR8* logfile);

#ifdef __cplusplus
}
#endif

#endif  


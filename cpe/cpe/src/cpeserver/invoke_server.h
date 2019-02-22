#ifndef __UBUS_INVOKE_H__
#define __UBUS_INVOKE_H__
#include <json-c/json.h>
#include <libubox/blobmsg_json.h>
 
 
struct menu_attr {
	char menu_title[64];
	char title[64];
	char acls[64];
	char files[64];
	char view[64];
	int index;
};
#define MENU_MAX	28
#define EU_UBUS_DEFAULT_SID	"00000000000000000000000000000000"
 
/* __UBUS_INVOKE_H__ */ 
#endif


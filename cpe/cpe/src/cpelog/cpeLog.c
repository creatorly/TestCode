#include "cpeLog.h"
 

int main(void){
	CPE_LOG_DEBUG("=============== cpeLog start ===============");
	CPE_LOG_DEBUG("build time: %s:%s", __DATE__, __TIME__);
	return 0;
}

char *get_cur_time()
{
	static char s[20];
	time_t t;
	struct tm* ltime;
	time(&t);
	ltime = localtime(&t); 
	strftime(s, 20, "%Y-%m-%d %H:%M:%S", ltime);
	return s;
}


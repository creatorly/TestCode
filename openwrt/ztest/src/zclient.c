#include "ztest.h"

void log_init(void)
{
    ulog_open(ULOG_SYSLOG, LOG_USER, NULL);
    ulog_threshold(LOG_INFO);
}

int main(int argc, char **argv){
    int type = USOCK_TCP | USOCK_NOCLOEXEC | USOCK_IPV4ONLY;
    const char *host = "127.0.0.1";
    const char *service = "1212";
    
    log_init();
    ULOG_INFO("--------zclient--------\n");

    int c_fd = usock(type, host, service);
    if(c_fd < 0) {
        perror("usock");
        return -1;
    }

    send(c_fd, "helloworld", 10, 0);

    close(c_fd);
    
    return 1;
}


ZTEST           = ztest zclient
ZTEST_LDFLAGS = $(LDFLAGS) -L. -lubox -luci -lubus
ZTEST_OBJS    = ztest.o ubus.o zclient.o

ZTEST_CFLAGS  = $(CFLAGS) -fPIC -Wall -Werror -std=gnu99 -Os

ALL = $(ZTEST)

all: $(ALL)

zclient: zclient.o
	$(CC) $(ZTEST_LDFLAGS) -o $@ $^

ztest: ztest.o ubus.o
	$(CC) $(ZTEST_LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(ZTEST_CFLAGS) -c -o $@ $^

.PHONY: clean

clean:
	-rm -f *.o  $(ALL)

CC = gcc

COMPONENTS += pipe
COMPONENTS += fifo
COMPONENTS += signal
COMPONENTS += semaphore
COMPONENTS += msg
COMPONENTS += shm

SUBDIRS = $(COMPONENTS)

ALL_SUBDIRS = pipe fifo signal semaphore msg shm

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: $(SUBDIRS)

clean:clean_subdirs
	-rm -rf *.o

clean_subdirs:
	-for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done

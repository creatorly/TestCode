CC = gcc

COMPONENTS += create
COMPONENTS += communicate

SUBDIRS = $(COMPONENTS)

ALL_SUBDIRS = create communicate

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

.PHONY: $(SUBDIRS)

clean:clean_subdirs
	-rm -rf *.o

clean_subdirs:
	-for d in $(SUBDIRS); do $(MAKE) -C $$d clean; done

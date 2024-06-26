MAKE = make
ROOTPATH = $(realpath ./) 

ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else
    detected_OS := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

ifeq ($(detected_OS),Windows)
	RM         = del
	BINFILES = $(subst /,\,$(wildcard ./bin/*.exe ./bin/*.a))
endif
ifeq ($(detected_OS),Linux)
	RM         = rm
	BINFILES = $(wildcard ./bin/*. ./bin/*.a)
endif


all:
	@echo $(ROOTPATH)
	$(MAKE) -C lib all
	$(MAKE) -C examples all
	$(MAKE) -C demos all
	@echo All done!

clean:
	$(MAKE) -C lib clean
	$(MAKE) -C examples clean
	$(MAKE) -C demos clean
	$(RM) -f $(BINFILES)
	@echo Clean done!

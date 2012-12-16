#Modified from http://www.drdobbs.com/parallel/231002854
PSPLAT=psplat
PATHTEST=pathtest

CC=g++

CFLAGS=  -DUNIX -g -DDEBUG

# Check for 32-bit vs 64-bit
PROC_TYPE = $(strip $(shell uname -m | grep 64))
 
# Check for Mac OS
OS = $(shell uname -s 2>/dev/null | tr [:lower:] [:upper:])
DARWIN = $(strip $(findstring DARWIN, $(OS)))

# MacOS System
ifneq ($(DARWIN),)
	CFLAGS += -DMAC
	LIBS=-framework OpenCL

	ifeq ($(PROC_TYPE),)
		CFLAGS+=-arch i386
	else
		CFLAGS+=-arch x86_64
	endif
else

# Linux OS
LIBS=-lOpenCL -ffast-math -lm -lbz2 
ifeq ($(PROC_TYPE),)
	CFLAGS+=-m32
else
	CFLAGS+=-m64
endif

# Check for Linux-AMD
ifdef AMDAPPSDKROOT
   INC_DIRS=. $(AMDAPPSDKROOT)/include
	ifeq ($(PROC_TYPE),)
		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86
	else
		LIB_DIRS=$(AMDAPPSDKROOT)/lib/x86_64
	endif
else

# Check for Linux-Nvidia
ifdef CUDA
   INC_DIRS=. $(CUDA)/OpenCL/common/inc
endif

endif
endif
ALL: $(PSPLAT) $(PATHTEST)

$(PATHTEST): $(PATHTEST).o clutil.o
	$(CC) $(CFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)
	
$(PATHTEST).o: pathcalc.cpp 
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

$(PSPLAT): $(PSPLAT).o clutil.o itwomclwrapper.o 
	$(CC) $(CFLAGS) -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

$(PSPLAT).o: psplat.cpp 
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

clutil.o: clutil.cpp
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)

itwomclwrapper.o:itwomclwrapper.cpp 
	$(CC) $(CFLAGS) -c -o $@ $^ $(INC_DIRS:%=-I%) $(LIB_DIRS:%=-L%) $(LIBS)
   


.PHONY: clean all

clean:
	rm $(PSPLAT) $(PSPLAT).o itwomclwrapper.o clutil.o

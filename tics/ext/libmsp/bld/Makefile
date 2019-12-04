LIB = libmsp

# We need the device information for later in this makefile.
# Note that normally this is included automatically by Makefile.gcc
include $(MAKER_ROOT)/Makefile.board

# TODO: copy-pasting code from mspware is not nice
OBJECTS = \
	watchdog.o \
	mspware/pmm.o \
	mem.o \
	clock.o \

# pseudo-RNG is only supported with hardware multiplier
ifeq ($(HWMULT),1)
OBJECTS += rand.o
endif

override SRC_ROOT = ../../src

$(info toolchain $(TOOLCHAIN))
ifeq ($(TOOLCHAIN),ticl)
override CFLAGS += \
	--include_path="$(SRC_ROOT)/include/libmsp" \
	--include_path="$(SRC_ROOT)/mspware" 
else
override CFLAGS += \
	-I$(SRC_ROOT)/include/libmsp \
	-I$(SRC_ROOT)/mspware
endif
#include ../Makefile.config

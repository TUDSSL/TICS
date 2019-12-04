LIB = libwispbase

OBJECTS = \
	wired/uart.o \
#	internals/wisp-init.o \
#	rand/rand.o \
#	nvm/fram.o \
#	Timing/timer.o \
#	Math/crc16_LUT.o \
#	RFID/interface.o \

LIBWISPBASE_INCLUDE_SPI ?= 1
LIBWISPBASE_INCLUDE_ACCEL ?= 1
LIBWISPBASE_INCLUDE_RFID ?= 1

# Renamed a little for user-friendliness
ifneq ($(LIBMSP_DCO_FREQ),)
LOCAL_CFLAGS += -DCONFIG_DCOCLKDIV_FREQ=$(LIBMSP_DCO_FREQ)
else
$(error DCO clock frequency must be set via LIBMSP_DCO_FREQ)
endif

ifeq ($(LIBWISPBASE_INCLUDE_SPI),1)
LOCAL_CFLAGS += -DLIBWISPBASE_INCLUDE_SPI
OBJECTS += wired/spi.o
endif # // LIBWISPBASE_INCLUDE_SPI

ifeq ($(LIBWISPBASE_INCLUDE_ACCEL),1)
ifneq ($(LIBWISPBASE_INCLUDE_SPI),1)
$(error Invalid config: ACCEL requires SPI.
endif 
LOCAL_CFLAGS += -DLIBWISPBASE_INCLUDE_ACCEL
OBJECTS += Sensors/accel.o
endif # // LIBWISPBASE_INCLUDE_ACCEL

ifeq ($(LIBWISPBASE_INCLUDE_RFID),1)
ifneq ($(TOOLCHAIN),ticl)
$(error RFID is written in assembly that is specific to TI compiler and is not compilable by GCC or Clang.)
endif
LOCAL_CFLAGS += -DLIBWISPBASE_INCLUDE_RFID
OBJECTS += \
	RFID/rfid_BlockWriteHandle.c \
	RFID/rfid_Handles.c \
	RFID/rfid_ReadHandle.c \
	RFID/rfid_WriteHandle.c \

endif # // LIBWISPBASE_INCLUDE_SPI

override SRC_ROOT = ../../src

LOCAL_CFLAGS += \
	-I$(SRC_ROOT)/include \
	-I$(SRC_ROOT)/include/libwispbase \

override CFLAGS += $(LOCAL_CFLAGS)

export CFLAGS

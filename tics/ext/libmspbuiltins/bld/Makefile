LIB = libmspbuiltins

OBJECTS = \
	builtins.o

ifeq ($(TOOLCHAIN),ticl)
CFLAGS += \
	--include_path="../../src/include/libmspbuiltins"
else
CFLAGS += \
	-I ../../src/include/libmspbuiltins
endif

override SRC_ROOT = ../../src

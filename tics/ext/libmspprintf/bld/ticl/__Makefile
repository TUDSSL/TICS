include ../../../Makefile.env

CG_TOOL_LIB := $(CG_TOOL_ROOT)/bin
AR = $(CG_TOOL_LIB)/ar430
AS = $(CG_TOOL_LIB)/cl430
CC = $(CG_TOOL_LIB)/cl430

SRC_ROOT = ../src

LIB = libedb.lib

COMMON_FLAGS = \
	-g \
	-vmspx \
	--abi=eabi \
	--data_model=restricted \
	--opt_for_speed=5 \
	--use_hw_mpy=F5 \
	--define=__MSP430FR5969__ \
	--printf_support=minimal \
	--silicon_errata=CPU21 \
	--silicon_errata=CPU22 \
	--silicon_errata=CPU40 \
	--advice:power="none" \
	--advice:hw_config=1 \
	--diag_warning=225 \
	--display_error_number \
	--diag_wrap=off \


CFLAGS = \
	--preproc_with_compile \
	--include_path="$(CG_TOOL_ROOT)/include" \
	--include_path="$(PLATFORM_ROOT)/include" \
	--include_path="$(WISP_BASE_ROOT)" \
	--include_path="$(SRC_ROOT)" \
	--include_path="$(SRC_ROOT)/include/libedb" \

ASFLAGS = $(CFLAGS)

# Out-of-tree build: place build artifacts (object files) in the build dir
VPATH += $(SRC_ROOT)

# Specify the source files list explicitly avoiding wildcards, in order to make
# sure the build (esp. linker) doesn't pick up stray files
C_SRCS = \
	edb.c \

ASM_SRCS = \

OBJS = ${C_SRCS:.c=.obj} ${ASM_SRCS:.asm=.obj}

all: $(LIB)

-include ${C_SRCS:.c=.d} ${ASM_SRCS:.asm=.d}

%.obj: %.asm
	mkdir -p "./$(shell dirname $@)"
	$(AS) $(COMMON_FLAGS) -c $(ASFLAGS) --preproc_dependency="${@:.obj=.d}" \
		--asm_directory="$(shell dirname $@)" --output_file="$@" "$<"

%.obj: %.c
	mkdir -p "./$(shell dirname $@)"
	$(CC) $(COMMON_FLAGS) -c $(CFLAGS) --preproc_dependency="${@:.obj=.d}" \
		--asm_directory="$(shell dirname $@)" --output_file="$@" "$<"

$(LIB): $(OBJS)
	$(AR) r $@ $(OBJS)

clean:
	rm -f $(LIB) $(OBJS) ${OBJS:.obj=.d} ${OBJS:.obj=.asm}
	echo $(OBJS) | xargs -n 1 dirname | sort | uniq | xargs -n 1 -I {} \
		bash -c "[ -d {} ] && find {} -empty -type d -mindepth 0 -maxdepth 0" | \
		xargs -r rmdir -p

.PHONY: clean

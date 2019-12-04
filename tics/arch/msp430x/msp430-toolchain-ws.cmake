# CMAKE toolchain for the MSP430FR microcontroller

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR msp430)

set(DEVICE "msp430fr5969" CACHE STRING "")
string(TOUPPER ${DEVICE} DEVICE_DEFINE)
set(DEVICE_DEFINE "__${DEVICE_DEFINE}__")

# Clang/LLVM location
#set(LLVM_TOOLS_BINARY_DIR   "/opt/llvm-int/bin")
set(LLVM_TOOLS_BINARY_DIR   "/home/iikoe/devel/msp430-llvm/llvm-visp/src/llvm-build/bin")

# MSP430 support files (installation dependent)
set(PATH_MSP430_SUPPORT "/opt/msp430-gcc-support-files/include" CACHE STRING "")
set(PATH_MSP430_LIB "/home/iikoe/devel/msp430-gcc/msp430-gcc-7.3.1.24-source-full/install/usr/local/msp430-elf" CACHE STRING "")
set(PATH_MSP430_GCC "/home/iikoe/devel/msp430-gcc/msp430-gcc-7.3.1.24-source-full/install/usr/local/bin" CACHE STRING "")

# Device specific driverlib
set(PATH_MSP430_DRIVERS "${PROJECT_SOURCE_DIR}/arch/msp430x/lib/MSP430FR5xx_6xx" CACHE STRING "")

# default linkersctip
set(LINKER_SCRIPT
    "${PROJECT_SOURCE_DIR}/arch/msp430x/${DEVICE}.ld"
    CACHE
    FILEPATH "linkerscript"
    )

set(CMAKE_C_COMPILER    "${PATH_MSP430_GCC}/msp430-elf-gcc")
set(CMAKE_CXX_COMPILER  "${PATH_MSP430_GCC}/msp430-elf-g++")
set(CMAKE_AR            "${PATH_MSP430_GCC}/msp430-elf-ar")
set(CMAKE_LINKER        "${PATH_MSP430_GCC}/msp430-elf-ld")
set(CMAKE_NM            "${PATH_MSP430_GCC}/msp430-elf-nm")
set(CMAKE_OBJDUMP       "${PATH_MSP430_GCC}/msp430-elf-objdump")
set(CMAKE_STRIP         "${PATH_MSP430_GCC}/msp430-elf-strip")
set(CMAKE_RANLIB        "${PATH_MSP430_GCC}/msp430-elf-ranlib")
set(CMAKE_SIZE          "${PATH_MSP430_GCC}/msp430-elf-size")

# Compiler flags
set(COMMON_FLAGS "-g -gdwarf-3 -gstrict-dwarf -I${PATH_MSP430_SUPPORT} -I${PATH_MSP430_LIB} -I${PATH_MSP430_DRIVERS} -I/opt/msp430/lib/gcc/msp430-elf/7.3.1/include" CACHE STRING "")

set(MCU_SPECIFIC_CFLAGS "-MD -mmcu=${DEVICE} -mhwmult=f5series -mcode-region=none -mdata-region=none -msmall" CACHE STRING "")
# -fomit-frame-pointer
set(LLVM_SPECIFIC_CFLAGS "-DBOARD_WISP --target=msp430 -D${DEVICE_DEFINE} -nobuiltininc -nostdinc++ -MD" CACHE STRING "")

# Set GCC and LLVM compiler flags
set(CMAKE_C_FLAGS "${MCU_SPECIFIC_CFLAGS} ${COMMON_FLAGS}" CACHE STRING "")
set(LLVM_C_FLAGS "${LLVM_SPECIFIC_CFLAGS} ${COMMON_FLAGS}" CACHE STRING "")

# Linker flags
#set(MCU_SPECIFIC_LINKER_FLAGS "-L${PATH_MSP430_LIB}/lib/large" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "-g ${MCU_SPECIFIC_LINKER_FLAGS} -L${PATH_MSP430_SUPPORT} -T${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map,\"${PROJECT_NAME}.map\" -Wl,-lgcc -Wl,-lc" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

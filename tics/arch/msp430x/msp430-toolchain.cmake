# CMAKE toolchain for the MSP430FR microcontroller

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR msp430)

# MSP430 support files (installation dependent)
set(PATH_MSP430_SUPPORT "/opt/msp430-gcc-support-files/include" CACHE STRING "")
set(PATH_MSP430_LIB "/opt/msp430/msp430-elf" CACHE STRING "")

# Device specific driverlib
set(PATH_MSP430_DRIVERS "${PROJECT_SOURCE_DIR}/arch/msp430x/lib/MSP430FR5xx_6xx" CACHE STRING "")

# default linkersctip
set(LINKER_SCRIPT
    "${PROJECT_SOURCE_DIR}/arch/msp430x/msp430fr5994.ld"
    CACHE
    FILEPATH "linkerscript"
    )

set(CMAKE_C_COMPILER    "msp430-elf-gcc")
set(CMAKE_CXX_COMPILER  "msp430-elf-g++")
set(CMAKE_AR            "msp430-elf-ar")
set(CMAKE_LINKER        "msp430-elf-ld")
set(CMAKE_NM            "msp430-elf-nm")
set(CMAKE_OBJDUMP       "msp430-elf-objdump")
set(CMAKE_STRIP         "msp430-elf-strip")
set(CMAKE_RANLIB        "msp430-elf-ranlib")
set(CMAKE_SIZE          "msp430-elf-size")


# Compiler flags
set(MCU_SPECIFIC_CFLAGS "-mmcu=msp430fr5994 -mhwmult=f5series -mcode-region=none -mdata-region=none -mlarge" CACHE STRING "")
set(CMAKE_C_FLAGS "${MCU_SPECIFIC_CFLAGS} -g -gdwarf-3 -gstrict-dwarf -I${PATH_MSP430_SUPPORT} -I${PATH_MSP430_LIB} -I${PATH_MSP430_DRIVERS}" CACHE STRING "")

# Linker flags
set(MCU_SPECIFIC_LINKER_FLAGS "-L${PATH_MSP430_LIB}/lib/large" CACHE STRING "")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_SPECIFIC_LINKER_FLAGS} -L${PATH_MSP430_SUPPORT} -T${LINKER_SCRIPT} -Wl,--gc-sections -Wl,-Map,\"${PROJECT_NAME}.map\" -Wl,-lgcc -Wl,-lc" CACHE STRING "")

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

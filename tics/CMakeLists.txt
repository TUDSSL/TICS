cmake_minimum_required(VERSION 3.6)

include(ExternalProject)

project(checkpoint C ASM)

# Plugin to support partial compilation using LLVM
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
include(LLVMCompile)

# The default C Flags
set(CMAKE_C_FLAGS
"${CMAKE_C_FLAGS} \
-O1 \
-std=c99 \
-Wall \
-g \
-fstrict-aliasing \
-Wstrict-aliasing \
-save-temps \
-fstack-usage \
"
)

set(LLVM_C_FLAGS
"-O2 ${LLVM_C_FLAGS}"
)

## Instrument functions to track stack usage for figures
# -finstrument-functions \
# -finstrument-functions-exclude-function-list=main,init,printf,checkpoint_get_sp,__cyg_profile_func_enter,__cyg_profile_func_exit \

# Compile the source
add_subdirectory(arch/msp430x)

SET(CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS} -x assembler-with-cpp")

# Libraries for use with LLVM
add_subdirectory(ext)

# Compile functional tests
#if (BUILD_FUNCTIONAL_TESTS)
    add_subdirectory(test/functional)
#endif()

# Compile functional tests
if (BUILD_UNIT_TESTS)
    externalproject_add(
        tests
        SOURCE_DIR "${PROJECT_SOURCE_DIR}/test/unit"
        INSTALL_COMMAND ""
        EXCLUDE_FROM_ALL 1
        BUILD_ALWAYS 1
        BINARY_DIR "${PROJECT_BINARY_DIR}/testbuild/"
        )

    add_custom_target(
        tests-run
        "${PROJECT_SOURCE_DIR}/test/tests-run.sh" "${PROJECT_BINARY_DIR}/testbuild"
        DEPENDS tests
        )
endif()

#add_subdirectory(src)

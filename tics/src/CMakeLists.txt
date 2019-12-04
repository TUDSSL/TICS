
include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
    "${ARCH_INCLUDE_DIR}"
    )

#set(ELF_FILE ${PROJECT_NAME}.elf)
set(ELF_FILE ${PROJECT_NAME})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

add_executable(${ELF_FILE}
    "${PROJECT_SOURCE_DIR}/src/main.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${SOURCE_FILES}"
    )

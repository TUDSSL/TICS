
## External source files to be compiled using LLVM

# libmsp
set(LLVM_SOURCE_FILES
    "${LLVM_SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmsp/src/clock.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmsp/src/mem.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmsp/src/rand.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmsp/src/watchdog.c"
    )
set(INCLUDE_DIRS
    "${INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmsp/src/include/libmsp"
    )


# libmspmath
set(LLVM_SOURCE_FILES
    "${LLVM_SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspmath/src/divmodhi3.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspmath/src/mult.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspmath/src/sqrt.c"
    )
set(INCLUDE_DIRS
    "${INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspmath/src/include/libmspmath"
    )


## Source files to be compiled using GCC

# libmspbuiltins
set(SOURCE_FILES
    "${SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspbuiltins/src/builtins.S"
    )
set(INCLUDE_DIRS
    "${INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspbuiltins/src/include/libmspbuiltins"
    )

# libmspprintf
set(SOURCE_FILES
    "${SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspprintf/src/printf.c"
    )
# Printf include in in libmspconsole

# libmspconsole
set(SOURCE_FILES
    "${SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspconsole/src/io.c"
    )
set(INCLUDE_DIRS
    "${INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libmspconsole/src/include/libmspconsole"
    )

# libwispbase
set(SOURCE_FILES
    "${SOURCE_FILES}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libwispbase/src/wired/uart.c"
    )
set(INCLUDE_DIRS
    "${INCLUDE_DIRS}"
    "${CMAKE_CURRENT_SOURCE_DIR}/libwispbase/src/include/libwispbase"
    "${CMAKE_CURRENT_SOURCE_DIR}/libwispbase/src/include"
    )

set(LLVM_SOURCE_FILES ${LLVM_SOURCE_FILES} PARENT_SCOPE)
set(SOURCE_FILES ${SOURCE_FILES} PARENT_SCOPE)
set(INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

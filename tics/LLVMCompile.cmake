#
# Compile multiple source files to a signle assembly file using LLVM
#   Initially the source in converted to LLVM bitcode files, next they are
#   combined and converted to a single assembly file
#
# @param OUTPUT The absolute path of the assembly (*.S) file
# @param FLAGS Compilation flags
# @param INC_DIRS Include directories
# @param SRCS Source files
function(llvm_compile OUTPUT FLAGS INC_DIRS SRCS)
    # Set commands
    set(COMPILE ${LLVM_TOOLS_BINARY_DIR}/clang)
    set(LINK ${LLVM_TOOLS_BINARY_DIR}/llvm-link)
    set(ASSEMBLE ${LLVM_TOOLS_BINARY_DIR}/llc)

    # Convert flags string to a list of flags
    string(REPLACE " " ";" FLAGS_LIST ${FLAGS})

    # Create list of include directories with an appended -I
    set(INCS "")
    foreach(INC_DIR ${INC_DIRS})
        list(APPEND INCS "-I${INC_DIR}")
    endforeach()

    # Build all the individual bitcode files
    foreach(INPUT_FILE ${SRCS})
        get_filename_component(SRC_BASE ${INPUT_FILE} NAME_WE)
        set(BC_FILE ${CMAKE_CURRENT_BINARY_DIR}/${SRC_BASE}.bc)

        add_custom_command(OUTPUT ${BC_FILE}
            COMMAND ${COMPILE} -g -S ${INCS}
            -emit-llvm -c ${FLAGS_LIST}
            -o ${BC_FILE}
            ${INPUT_FILE}
            DEPENDS ${INPUT_FILE}
            )

        # Create a list of .bc files
        list(APPEND BC_FILES ${BC_FILE})
    endforeach()

    get_filename_component(OUTPUT_BASE ${OUTPUT} NAME_WE)
    set(COMBINED_BC_FILE ${OUTPUT_BASE}.bc)

    # Combine/link the bitcode files
    add_custom_command(OUTPUT ${COMBINED_BC_FILE}
        COMMAND ${LINK} -S -o ${COMBINED_BC_FILE} ${BC_FILES}
        DEPENDS ${BC_FILES}
        )

    # Convert the combined bitcode file to assembly
    add_custom_command(OUTPUT ${OUTPUT}
        COMMAND ${ASSEMBLE} -O0 ${COMBINED_BC_FILE} -o ${OUTPUT}
        DEPENDS ${COMBINED_BC_FILE}
        )

endfunction(llvm_compile)

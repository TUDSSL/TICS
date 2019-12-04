
set(INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}/include"
    "${PROJECT_SOURCE_DIR}/test/functional"
    "${ARCH_INCLUDE_DIR}"
    "${INCLUDE_DIRS}"
    )

include_directories("${INCLUDE_DIRS}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

# TODO Verify for stackpool stack blocks
#add_executable("ftest_checkpoint"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${SOURCE_FILES}"
#    )

# TODO fix for virtualmem
#add_executable("ftest_crc_checkpoint"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_crc_checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${SOURCE_FILES}"
#    )

add_executable("ftest_workingsack"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${SOURCE_FILES}"
    )

add_executable("ftest_jump"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_jump.c"
    "${PROJECT_SOURCE_DIR}/src/jump.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${SOURCE_FILES}"
    )

add_executable("ftest_component_overhead"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_component_overhead.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${SOURCE_FILES}"
    )

#add_executable("ftest_workingsack_vmem"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_workingstack_vmem.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${SOURCE_FILES}"
#    )

## Cuckoo
# Compile with LLVM
set(L_LLVM_SOURCE_FILES
    "${LLVM_SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/cuckoo/cuckoo.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    )

# Compile LLVM parts with GCC
set(L_SOURCE_FILES
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/llvm-workaround/checkpoint_restore_registers.c"
    )

# Compile using LLVM
set(LLVM_ASM_FILE "cuckoo-llvm.S")
llvm_compile("${PROJECT_BINARY_DIR}/${LLVM_ASM_FILE}"
    "${LLVM_C_FLAGS}"
    "${INCLUDE_DIRS}"
    "${L_LLVM_SOURCE_FILES}")

# Compile and link using GCC
add_executable("ftest_cuckoo_llvm"
    "${L_SOURCE_FILES}"
    "${PROJECT_BINARY_DIR}/cuckoo-llvm.S"
    )


# Compile with GCC
add_executable("ftest_cuckoo_gcc"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/cuckoo/cuckoo.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )

## Compile original cuckoo
add_executable("ftest_cuckoo_orig"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/cuckoo/cuckoo-orig.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )

### AR

# Compile with GCC
add_executable("ftest_ar_gcc"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/activity/activity-instr.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )


# Compile with LLVM
set(L_LLVM_SOURCE_FILES
    "${LLVM_SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/activity/activity-instr.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    )

# Compile LLVM parts with GCC
set(L_SOURCE_FILES
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/llvm-workaround/checkpoint_restore_registers.c"
    )

# Compile using LLVM
set(LLVM_ASM_FILE "ar-llvm.S")
llvm_compile("${PROJECT_BINARY_DIR}/${LLVM_ASM_FILE}"
    "${LLVM_C_FLAGS}"
    "${INCLUDE_DIRS}"
    "${L_LLVM_SOURCE_FILES}")

# Compile and link using GCC
add_executable("ftest_ar_llvm"
    "${L_SOURCE_FILES}"
    "${PROJECT_BINARY_DIR}/ar-llvm.S"
    )

## Compile original AR
add_executable("ftest_ar_orig"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/activity/activity.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )



### Bitcount

# Compile with LLVM
set(L_LLVM_SOURCE_FILES
    "${LLVM_SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/bitcount/bitcount-instr.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    )

# Compile LLVM parts with GCC
set(L_SOURCE_FILES
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/llvm-workaround/checkpoint_restore_registers.c"
    )

# Compile using LLVM
set(LLVM_ASM_FILE "bitcount-llvm.S")
llvm_compile("${PROJECT_BINARY_DIR}/${LLVM_ASM_FILE}"
    "${LLVM_C_FLAGS}"
    "${INCLUDE_DIRS}"
    "${L_LLVM_SOURCE_FILES}")

# Compile and link using GCC
add_executable("ftest_bitcount_llvm"
    "${L_SOURCE_FILES}"
    "${PROJECT_BINARY_DIR}/bitcount-llvm.S"
    )

# Compile with GCC
add_executable("ftest_bitcount_gcc"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/bitcount/bitcount-instr.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )

add_executable("ftest_bitcount_orig"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/bitcount/bitcount.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    "${SOURCE_FILES}"
    )

# Powercast test loop
add_executable("ftest_bitcount_powercast"
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/bitcount/bitcount-powercast.c"
    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
    "${PROJECT_SOURCE_DIR}/src/stackarray.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_nv_asm_small.S"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
    )


#add_executable("ftest_bitcount_stacksizetrace"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/bitcount/bitcount-stacksizetrace.c"
#    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
#    )

#add_executable("ftest_greenh_temp"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/greenh-temp/greenh_temp-instr.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
#    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
#    "${SOURCE_FILES}"
#    )

#add_executable("ftest_greenh_temp_time"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/greenh-temp-time/greenh_temp_time-instr.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
#    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
#    "${SOURCE_FILES}"
#    )
#target_link_libraries("ftest_greenh_temp_time" m)

#add_executable("ftest_greenh_temp_tinyos"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/greenh-temp-tinyos/greenh_temp_tinyos-instr.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
#    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
#    "${SOURCE_FILES}"
#    )

#add_executable("ftest_tinyos_temp"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/tinyos-temp/tinyos_temp.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/dma.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_timer.c"
#    "${PROJECT_SOURCE_DIR}/test/functional/ftest_util.c"
#    "${SOURCE_FILES}"
#    )

#add_executable("ftest_benchmark_checkpoint"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_benchmark_checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${SOURCE_FILES}"
#    )

#add_executable("ftest_virtualaddr"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${SOURCE_FILES}"
#    )

#add_executable("ftest_virtualmem"
#    "${PROJECT_SOURCE_DIR}/test/functional/checkpoint/ftest_virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/checkpoint.c"
#    "${PROJECT_SOURCE_DIR}/src/stackpool.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualmem.c"
#    "${PROJECT_SOURCE_DIR}/src/virtualaddr.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack.c"
#    "${PROJECT_SOURCE_DIR}/arch/msp430x/workingstack_asm.S"
#    "${SOURCE_FILES}"
#    )

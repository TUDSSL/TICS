set(ARCH_INCLUDE_DIR
    "${PROJECT_SOURCE_DIR}/arch/msp430x"
    PARENT_SCOPE
    )

set(SOURCE_FILES
    "${SOURCE_FILES}"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_stack.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_registers.c"
    "${PROJECT_SOURCE_DIR}/arch/msp430x/checkpoint_data.c"
    PARENT_SCOPE
    )

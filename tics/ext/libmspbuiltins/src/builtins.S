/* Builtins not (yet) implemented in Clang's compiler-rt. */

.global __mulhi3hw_noint
	.type __mulhi3hw_noint,@function
__mulhi3hw_noint:
    ; these addresses are for MSP430FR5969
	MOV R12, &0x04C0
	MOV R13, &0x04C8
	MOV &0x04CA, R12
	RET
/* Old argument ABI
__mulhi3hw_noint:
    ; these addresses are for MSP430FR5969
	MOV R15, &0x04C0
	MOV R14, &0x04C8
	MOV &0x04CA, R15
	RET
*/

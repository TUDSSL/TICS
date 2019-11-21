# The __morestack function.

	.global	__morestack
	.hidden	__morestack

#ifdef __ELF__
	.type	__morestack,@function
#endif

__morestack:
	add.w #0x04,sp

#ifdef __MSP430X_LARGE__
	reta
#else
	ret
#endif

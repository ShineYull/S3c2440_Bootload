SECTIONS
{
	. = 0xB0000000;
	
	_code_start = .;

	
	. = ALIGN(4);
	.text   :   { *(.text) }

	. = ALIGN(4);
	.rodata	:	{ *(.rodata) }

	. = ALIGN(4);
	.data	:	{ *(.data) }

	. = ALIGN(4);
	_bss_start = .;
	.bss	:	{ *(.bss) *(.COMMON) }
	_bss_end = .;
}


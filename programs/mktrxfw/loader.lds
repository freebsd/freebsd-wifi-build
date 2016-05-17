OUTPUT_ARCH(mips)
ENTRY(_startC)
SECTIONS {
	. = 0x80001000;
	.text : {
		*(.text.entry)
		*(.text)
		*(.rodata)
	}

	.data : {
		*(.data)
	}

	.bss : {
		*(.bss)
	}

	workspace = NEXT(0x10);
}

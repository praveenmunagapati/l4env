/* -*- c -*- */

_boot_sys_start = 0x300000;
_kernel_image	= 0xf0000000;	/* kernel link address */
_load		= 0x1000;	/* skip page 0 to preserve BIOS data and
				 * page 1 to reserve space for the KIP */

ENTRY(_boot_start)

PHDRS {
  btext PT_LOAD;
  bdata PT_LOAD;
  kip PT_LOAD;
  ktext PT_LOAD;
  kdata PT_LOAD;
  kitext PT_LOAD;
  kidata PT_LOAD;
}

SECTIONS {
  . = _boot_sys_start;
  .text_boot : {
    *(.bootstrap.text)
  } : btext
  
  . = ALIGN(0x10);
  .bss_boot : {
    *(.bootstrap.bss)
  } : bdata

  . = _kernel_image + _load;

  .kip : AT(ADDR(.kip) - 0xf0000000) {
    _kernel_image_start = .;
    *(.kernel_info_page)
    _initkip_start = .;
    KEEP(*(.initkip.version))
    KEEP(*(.initkip.features))
    KEEP(*(.initkip.features.fini))
    _initkip_end = .;
    . = ALIGN(4K);
  } : kip = 0

  .text : AT (ADDR(.text) - 0xf0000000) {
    crt0.o(.text)
    *(.init)
    *( .text SORT(.text.*) .gnu.linkonce.t.*)
    *(.fini)

    PROVIDE (_ecode = .);

    *(.rodata .rodata.* .gnu.linkonce.r.*)

    . = ALIGN(0x40);

    PROVIDE (_etext = .);
  } : ktext = 0x90909090

  . = ALIGN(0x10);
  .data : AT (ADDR(.data) - 0xf0000000) {
    *(.data .data.* .gnu.linkonce.d.*)
    *(.anno)

    /* Constructor and destructor lists, ordered by priority.  The lists
       are traversed from end to start, therefore the *_END__ symbols
       precede the *_LIST__ symbols. */

    __CTOR_END__ = .;
    CONSTRUCTORS
    KEEP (*(.ctors))
    KEEP (*(SORT (.ctors.*)))
    /* counter is illogical part of the CTOR__LIST, so we have to decrement the numbers by one */
    LONG ((__CTOR_LIST__ - __CTOR_END__) / 4 - 1) /* number of constructors */
    __CTOR_LIST__ = .;

    __DTOR_END__ = .;    
    KEEP (*(SORT (.dtors.*)))
    KEEP (*(.dtors))
    /* counter is illogical part of the DTOR__LIST, so we have to decrement the numbers by one */
    LONG ((__DTOR_LIST__ - __DTOR_END__) / 4 - 1) /* number of destructors */
    __DTOR_LIST__ = .;

    PROVIDE (_edata = .);
  } : kdata

  . = ALIGN(8);
  .per_cpu_data : AT (ADDR(.per_cpu_data) - 0xf0000000) {
    PROVIDE (_per_cpu_data_start = .);
    *(.per_cpu.data)
    . = ALIGN(8);
    PROVIDE (_per_cpu_data_end = .);
  } : kdata
  
  . = ALIGN(0x10);
  .bss : AT (ADDR(.bss) - 0xf0000000) {
    PROVIDE (_bss_start = .);
    *(.bss .bss.* .gnu.linkonce.b.* COMMON)
    PROVIDE (_bss_end = .);
  } : kdata


  PROVIDE (_end = .);

  . = ALIGN(4096);
  PROVIDE (_initcall_start = .);
  .initcall.text : AT (ADDR(.initcall.text) - 0xf0000000) {
    *(.initcall.text*)
  } : kitext = 0x90909090

  .initcall.data : AT (ADDR(.initcall.data) - 0xf0000000) {
    *(.initcall.data*)
    . = ALIGN(4096);
    PROVIDE (_initcall_end = .);
  } : kidata

  /DISCARD/ : {
    *(.note)
    *(.note.ABI-tag)
    *(.comment)
    *(.eh_frame)
  }
}

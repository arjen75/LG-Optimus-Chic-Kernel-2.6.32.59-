#include <asm-generic/vmlinux.lds.h>
#include <asm/page.h>

OUTPUT_FORMAT(ELF_FORMAT)
OUTPUT_ARCH(ELF_ARCH)
ENTRY(_start)
jiffies = jiffies_64;

SECTIONS
{
  /* This must contain the right address - not quite the default ELF one.*/
  PROVIDE (__executable_start = START);
  /* Static binaries stick stuff here, like the sigreturn trampoline,
   * invisibly to objdump.  So, just make __binary_start equal to the very
   * beginning of the executable, and if there are unmapped pages after this,
   * they are forever unusable.
   */
  __binary_start = START;

  . = START + SIZEOF_HEADERS;

  _text = .;
  _stext = .;
  __init_begin = .;
  INIT_TEXT_SECTION(PAGE_SIZE)
  . = ALIGN(PAGE_SIZE);

  .text      :
  {
    TEXT_TEXT
    SCHED_TEXT
    LOCK_TEXT
    *(.fixup)
    /* .gnu.warning sections are handled specially by elf32.em.  */
    *(.gnu.warning)
    *(.gnu.linkonce.t*)
  }

  . = ALIGN(PAGE_SIZE);
  .syscall_stub : {
	__syscall_stub_start = .;
	*(.__syscall_stub*)
	__syscall_stub_end = .;
  }

  /*
   * These are needed even in a static link, even if they wind up being empty.
   * Newer glibc needs these __rel{,a}_iplt_{start,end} symbols.
   */
  .rel.plt : {
	*(.rel.plt)
	PROVIDE_HIDDEN(__rel_iplt_start = .);
	*(.rel.iplt)
	PROVIDE_HIDDEN(__rel_iplt_end = .);
  }
  .rela.plt : {
	*(.rela.plt)
	PROVIDE_HIDDEN(__rela_iplt_start = .);
	*(.rela.iplt)
	PROVIDE_HIDDEN(__rela_iplt_end = .);
  }

  #include "asm/common.lds.S"

  init.data : { INIT_DATA }
  .data    :
  {
    INIT_TASK_DATA(KERNEL_STACK_SIZE)
    . = ALIGN(KERNEL_STACK_SIZE);
    *(.data.init_irqstack)
    DATA_DATA
    *(.gnu.linkonce.d*)
    CONSTRUCTORS
  }
  .data1   : { *(.data1) }
  .ctors         :
  {
    *(.ctors)
  }
  .dtors         :
  {
    *(.dtors)
  }

  .got           : { *(.got.plt) *(.got) }
  .dynamic       : { *(.dynamic) }
  .tdata	  : { *(.tdata .tdata.* .gnu.linkonce.td.*) }
  .tbss		  : { *(.tbss .tbss.* .gnu.linkonce.tb.*) *(.tcommon) }
  /* We want the small data sections together, so single-instruction offsets
     can access them all, and initialized data all before uninitialized, so
     we can shorten the on-disk segment size.  */
  .sdata     : { *(.sdata) }
  _edata  =  .;
  PROVIDE (edata = .);
  . = ALIGN(PAGE_SIZE);
  __bss_start = .;
  PROVIDE(_bss_start = .);
  SBSS(0)
  BSS(0)
  _end = .;
  PROVIDE (end = .);

  STABS_DEBUG

  DWARF_DEBUG

  DISCARDS
}

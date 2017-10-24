ENTRY(Reset_Handler)
_RAM_START_ = (0x1FFF8000);
_RAM_END_ = (0x20017FFF);
FREESCALE_PROD_DATA_BASE_ADDR = ((0x0007FFFF) - ( 2 * 1024 ) + 1);
        NV_STORAGE_SECTOR_SIZE = ( 2 * 1024 );
        NV_STORAGE_MAX_SECTORS = (4);
        NV_STORAGE_START_ADDRESS = ((0x0007FFFF) - ( 2 * 1024 ) + 1) - 1;
        NV_STORAGE_END_ADDRESS = NV_STORAGE_START_ADDRESS - (NV_STORAGE_MAX_SECTORS * NV_STORAGE_SECTOR_SIZE) + 1;
__RAM_VECTOR_TABLE_SIZE = ((48*4));
__BOOT_STACK_ADDRESS = ((((((0x20017FFF)) - (384)) - 1) - (0) - 0x4) - 1)-0x0F;
__dummy_start = 0x1FFFFFFB;
MEMORY
{
        TEXT_region1 (RX) : ORIGIN = (((0x00000000))), LENGTH = ((0x400) - (((0x00000000))))
        m_flash_config_region (RX) : ORIGIN = (0x400), LENGTH = ((0x410) - (0x400))
        TEXT_region2 (RX) : ORIGIN = (0x410)+1, LENGTH = ((((((0x0007FFFF) - ( 2 * 1024 ) + 1) - 1) - (((4)) * (( 2 * 1024 ))) + 1) - 1) - (0x410) - 1)
        NVM_region (RW) : ORIGIN = ((((0x0007FFFF) - ( 2 * 1024 ) + 1) - 1) - (((4)) * (( 2 * 1024 ))) + 1), LENGTH = ((((0x0007FFFF) - ( 2 * 1024 ) + 1) - 1) - ((((0x0007FFFF) - ( 2 * 1024 ) + 1) - 1) - (((4)) * (( 2 * 1024 ))) + 1))
        DATA_region (RW) : ORIGIN = (((0x1FFF8000))), LENGTH = ((0x20017FFF) - (0x1FFF8000) + 1)
        PRODUCT_INFO_region (RX) : ORIGIN = ((0x0007FFFF) - ( 2 * 1024 ) + 1), LENGTH = (((0x0007FFFF)) - ((0x0007FFFF) - ( 2 * 1024 ) + 1))
}
SECTIONS
{
  .interrupts :
  {
    __VECTOR_TABLE = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))
    . = ALIGN(4);
  } > TEXT_region1
  .freescale_prod_data :
  {
    . = ALIGN(4);
    KEEP(*(.FREESCALE_PROD_DATA))
    . = ALIGN(4);
  } > PRODUCT_INFO_region
  .flash_config :
  {
    . = ALIGN(4);
    KEEP(*(.FlashConfig))
    . = ALIGN(4);
  } > m_flash_config_region
  .text :
  {
    . = ALIGN(4);
    *(.text)
    *(.text*)
    *(.rodata)
    *(.rodata*)
    *(.glue_7)
    *(.glue_7t)
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > TEXT_region2
  .NVM_TABLE :
  {
    . = ALIGN(4);
    PROVIDE(__start_NVM_TABLE = .);
    KEEP(*(.NVM_TABLE));
    PROVIDE(__stop_NVM_TABLE = .);
    . = ALIGN(4);
  } > TEXT_region2
  .VERSION_TAGS :
  {
    . = ALIGN(4);
    PROVIDE(__start_VERSION_TAGS = .);
    KEEP(*(.VERSION_TAGS));
    PROVIDE(__stop_VERSION_TAGS = .);
    . = ALIGN(4);
  } > TEXT_region2
  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > TEXT_region2
  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > TEXT_region2
 .ctors :
  {
    __CTOR_LIST__ = .;
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END__ = .;
  } > TEXT_region2
  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > TEXT_region2
  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > TEXT_region2
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > TEXT_region2
  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > TEXT_region2
  __etext = .;
  __DATA_ROM = .;
  .mtb :
  {
    . = ALIGN(8);
    _mtb_start = .;
    KEEP(*(.mtb_buf))
    . = ALIGN(8);
    _mtb_end = .;
  } > DATA_region
  .interrupts_ram :
  {
    . = ALIGN(4);
    __VECTOR_RAM__ = .;
    __interrupts_ram_start__ = .;
    *(.m_interrupts_ram)
    . += ((48*4));
    . = ALIGN(4);
    __interrupts_ram_end__ = .;
  } > DATA_region
  __VECTOR_RAM = __VECTOR_RAM__;
  __RAM_VECTOR_TABLE_SIZE_BYTES = (__interrupts_ram_end__ - __interrupts_ram_start__);
  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;
    *(.data)
    *(.data*)
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;
  } > DATA_region
  __DATA_END = __DATA_ROM + (__data_end__ - __data_start__);
  text_end = ORIGIN(TEXT_region2) + LENGTH(TEXT_region2);
  ASSERT(__DATA_END <= text_end, "region m_text overflowed with text and data")
  .bss :
  {
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > DATA_region
  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += (0);
    __HeapLimit = .;
  } > DATA_region
  .stack :
  {
    . = ALIGN(8);
    . += (384);
  } > DATA_region
  __StackTop = ORIGIN(DATA_region) + LENGTH(DATA_region);
  __StackLimit = __StackTop - (384);
  PROVIDE(__stack = __StackTop);
  .ARM.attributes 0 : { *(.ARM.attributes) }
  .dummy __dummy_start :
  {
    . += 8;
  } =0x90909090
  ASSERT(__StackLimit >= __HeapLimit, "region m_data overflowed with stack and heap")
}

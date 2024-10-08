/**
 * @file XMC4700x2048.ld
 * @date 2016-03-08
 *
 * @cond
 *********************************************************************************************************************
 * Linker file for the GNU C Compiler v1.2
 * Supported devices: XMC4700-E196x2048
 *                    XMC4700-F144x2048
 *                    XMC4700-F100x2048
 *
 * Copyright (c) 2015-2016, Infineon Technologies AG
 * All rights reserved.                        
 *                                             
 * Redistribution and use in source and binary forms, with or without modification,are permitted provided that the 
 * following conditions are met:   
 *                                                                              
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer.                        
 * 
 * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following 
 * disclaimer in the documentation and/or other materials provided with the distribution.                       
 * 
 * Neither the name of the copyright holders nor the names of its contributors may be used to endorse or promote 
 * products derived from this software without specific prior written permission.                                           
 *                                                                              
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE  
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE  FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR  
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY,OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                                  
 *                                                                              
 * To improve the quality of the software, users are encouraged to share modifications, enhancements or bug fixes with 
 * Infineon Technologies AG dave@infineon.com).                                                          
 *********************************************************************************************************************
 *
 * Change History
 * --------------
 *
 * 2015-09-03:
 *     - Initial version
 *
 * 2016-03-08:
 *     - Fix size of BSS and DATA sections to be multiple of 4
 *     - Add assertion to check that region SRAM_combined does not overflowed no_init section
 *      
 * @endcond 
 *
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(Reset_Handler)

stack_size = DEFINED(stack_size) ? stack_size : 4096;
no_init_size = 64;

MEMORY
{
  FLASH_0_cached(RX) : ORIGIN = 0x08000000, LENGTH = 0x00010000
  FLASH_0_uncached(RX) : ORIGIN = 0x0C000000, LENGTH = 0x00010000
  FLASH_1_cached(RX) : ORIGIN = 0x08020000, LENGTH = 0x001E0000
  FLASH_1_uncached(RX) : ORIGIN = 0x0C020000, LENGTH = 0x001E0000
  PSRAM_1(!RX) : ORIGIN = 0x1FFE8000, LENGTH = 0x18000
  DSRAM_1_system(!RX) : ORIGIN = 0x20000000, LENGTH = 0x20000
  DSRAM_2_comm(!RX) : ORIGIN = 0x20020000, LENGTH = 0x20000
  SRAM_combined(!RX) : ORIGIN = 0x1FFE8000, LENGTH = 0x00058000
}

SECTIONS
{
    /* TEXT section */

  .reset :
  {
    KEEP(*(.reset));
  } > FLASH_0_cached AT > FLASH_0_uncached

  .text :
  {
    sText = .;
    *(.text .text.* .gnu.linkonce.t.*);

        /* C++ Support */
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata .rodata.*)
        *(.gnu.linkonce.r*)
        
        . = ALIGN(4);        
    } > FLASH_1_cached AT > FLASH_1_uncached

    /* Exception handling, exidx needs a dedicated section */
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > FLASH_1_cached AT > FLASH_1_uncached

    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH_1_cached AT > FLASH_1_uncached
    __exidx_end = .;
    
    /* DSRAM layout (Lowest to highest)*/
    Stack (NOLOAD) : 
    {
        __stack_start = .;
        . = . + stack_size;
        __stack_end = .;
        __initial_sp = .;
    } > SRAM_combined

    /* functions with __attribute__((section(".ram_code"))) */
    .ram_code :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __ram_code_start = .;
        *(.ram_code)
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __ram_code_end = .;
    } > SRAM_combined AT > FLASH_1_uncached
    __ram_code_load = LOADADDR (.ram_code);
    __ram_code_size = __ram_code_end - __ram_code_start;

    /* Standard DATA and user defined DATA/BSS/CONST sections */
    .data :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __data_start = .;
        * (.data);
        * (.data*);
        *(*.data);
        *(.gnu.linkonce.d*)
      
        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __data_end = .;
    } > SRAM_combined AT > FLASH_1_uncached
    __data_load = LOADADDR (.data);
    __data_size = __data_end - __data_start;
        
    /* BSS section */
    .bss (NOLOAD) : 
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __bss_start = .;
        * (.bss);
        * (.bss*);
        * (COMMON);
        *(.gnu.linkonce.b*)
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        __bss_end = .;
    } > SRAM_combined
    __bss_size = __bss_end - __bss_start;

    /* Shift location counter, so that ETH_RAM and USB_RAM are located above DSRAM_1_system */    
    __shift_loc =  (__bss_end >= ORIGIN(DSRAM_1_system)) ? 0 : (ORIGIN(DSRAM_1_system) - __bss_end);

    USB_RAM  (__bss_end + __shift_loc) (NOLOAD) :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        USB_RAM_start = .;
        *(USB_RAM)
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        USB_RAM_end = .;
    } > SRAM_combined
    USB_RAM_size = USB_RAM_end - USB_RAM_start;

    ETH_RAM (USB_RAM_end) (NOLOAD) :
    {
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        ETH_RAM_start = .;
        *(ETH_RAM)
        . = ALIGN(4); /* section size must be multiply of 4. See startup.S file */
        ETH_RAM_end = .;
        . = ALIGN(8);
        Heap_Bank1_Start = .;
    } > SRAM_combined
    ETH_RAM_size = ETH_RAM_end - ETH_RAM_start;

    /* .no_init section contains chipid, SystemCoreClock and trimming data. See system.c file*/
    .no_init ORIGIN(SRAM_combined) + LENGTH(SRAM_combined) - no_init_size (NOLOAD) : 
    {
        Heap_Bank1_End = .;
        * (.no_init);
    } > SRAM_combined

    /* Heap - Bank1*/
    Heap_Bank1_Size  = Heap_Bank1_End - Heap_Bank1_Start;

    ASSERT(Heap_Bank1_Start <= Heap_Bank1_End, "region SRAM_combined overflowed no_init section")

    /DISCARD/ :
    {
        *(.comment)
    }

    .stab       0 (NOLOAD) : { *(.stab) }
    .stabstr    0 (NOLOAD) : { *(.stabstr) }

    /* DWARF 1 */
    .debug              0 : { *(.debug) }
    .line               0 : { *(.line) }

    /* GNU DWARF 1 extensions */
    .debug_srcinfo      0 : { *(.debug_srcinfo) }
    .debug_sfnames      0 : { *(.debug_sfnames) }

    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges      0 : { *(.debug_aranges) }
    .debug_pubnames     0 : { *(.debug_pubnames) }
    .debug_pubtypes     0 : { *(.debug_pubtypes) }

    /* DWARF 2 */
    .debug_info         0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev       0 : { *(.debug_abbrev) }
    .debug_line         0 : { *(.debug_line) }
    .debug_frame        0 : { *(.debug_frame) }
    .debug_str          0 : { *(.debug_str) }
    .debug_loc          0 : { *(.debug_loc) }
    .debug_macinfo      0 : { *(.debug_macinfo) }

    /* DWARF 2.1 */
    .debug_ranges       0 : { *(.debug_ranges) }

    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames    0 : { *(.debug_weaknames) }
    .debug_funcnames    0 : { *(.debug_funcnames) }
    .debug_typenames    0 : { *(.debug_typenames) }
    .debug_varnames     0 : { *(.debug_varnames) }

    /* Build attributes */
    .build_attributes   0 : { *(.ARM.attributes) }
}

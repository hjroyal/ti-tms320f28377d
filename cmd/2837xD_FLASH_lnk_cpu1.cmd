

_Cla1Prog_Start = _Cla1funcsRunStart;

CLA_SCRATCHPAD_SIZE = 0x100;
--undef_sym=__cla_scratchpad_end
--undef_sym=__cla_scratchpad_start

MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

   BEGIN            : origin = 0x080000, length = 0x000002
   RAMM0            : origin = 0x000122, length = 0x0002DE
   RAMD0            : origin = 0x00B000, length = 0x000800
   RAMLS0           : origin = 0x008000, length = 0x000800
   RAMLS1           : origin = 0x008800, length = 0x000400
   RAMLS2           : origin = 0x008C00, length = 0x000400
   RAML3            : origin = 0x009000, length = 0x001000     /* CLA program RAM */
   RAML4L           : origin = 0x00A000, length = 0x000800     /* on-chip RAM block L4 */
   RAMGS14          : origin = 0x01A000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RAMGS15          : origin = 0x01B000, length = 0x001000     /* Only Available on F28379D, F28377D, F28375D devices. Remove line on other devices. */
   RESET            : origin = 0x3FFFC0, length = 0x000002

   /* Flash sectors */
   FLASHA           : origin = 0x080002, length = 0x001FFE	/* on-chip Flash */
   FLASHB           : origin = 0x082000, length = 0x002000	/* on-chip Flash */
   FLASHC           : origin = 0x084000, length = 0x002000	/* on-chip Flash */
   FLASHD           : origin = 0x086000, length = 0x002000	/* on-chip Flash */
   FLASHE           : origin = 0x088000, length = 0x008000	/* on-chip Flash */
   FLASHF           : origin = 0x090000, length = 0x008000	/* on-chip Flash */
   FLASHG           : origin = 0x098000, length = 0x008000	/* on-chip Flash */
   FLASHH           : origin = 0x0A0000, length = 0x008000	/* on-chip Flash */
   FLASHI           : origin = 0x0A8000, length = 0x008000	/* on-chip Flash */
   FLASHJ           : origin = 0x0B0000, length = 0x008000	/* on-chip Flash */
   FLASHK           : origin = 0x0B8000, length = 0x002000	/* on-chip Flash */
   FLASHL           : origin = 0x0BA000, length = 0x002000	/* on-chip Flash */
   FLASHM           : origin = 0x0BC000, length = 0x002000	/* on-chip Flash */
   FLASHN           : origin = 0x0BE000, length = 0x002000	/* on-chip Flash */

PAGE 1 : /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   RAMM1           : origin = 0x000400, length = 0x000400     /* on-chip RAM block M1 */


   CLARAM0     : origin = 0x00A000, length = 0x000800
   //CLARAM1     : origin = 0x00AA00, length = 0x000100
   //CLARAM2     : origin = 0x00AB00, length = 0x000100
   TASK_STK_RAM : origin = 0x00A800, length = 0x001000		///>hj 2024-09-06
   APPDATARAM      : origin  = 0x00B810, length = 0x009999


   CPU2TOCPU1RAM   : origin = 0x03F800, length = 0x000400
   CPU1TOCPU2RAM   : origin = 0x03FC00, length = 0x000400

   CLA1_MSGRAMLOW       : origin = 0x001480, length = 0x000080
   CLA1_MSGRAMHIGH      : origin = 0x001500, length = 0x000080
}

SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > FLASHB      PAGE = 0, ALIGN(4)
   .pinit              : > FLASHB,     PAGE = 0, ALIGN(4)
   .text               : >> FLASHB | FLASHC | FLASHD | FLASHE      PAGE = 0, ALIGN(4)
   codestart           : > BEGIN       PAGE = 0, ALIGN(4)

   /* Allocate uninitalized data sections: */
   .stack              : > RAMM1       PAGE = 1
   /* .ebss               : >> RAMLS5 | RAMGS0 | RAMGS1       PAGE = 1 */
   .ebss               : >APPDATARAM       PAGE = 1
   .esysmem            : >APPDATARAM        PAGE = 1

   /* Initalized sections go in Flash */
   .econst             : >> FLASHF | FLASHG | FLASHH      PAGE = 0, ALIGN(4)
   .switch             : > FLASHB      PAGE = 0, ALIGN(4)

   .reset              : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */
    dclfuncs        : > RAML4L,     PAGE = 0
    dcl32funcs      : > RAML4L,     PAGE = 0





#ifdef __TI_COMPILER_VERSION__
   ramfuncs            : LOAD = FLASHD,
                         RUN = RAMLS0 | RAMLS1 | RAMLS2,
                         LOAD_START(_RamfuncsLoadStart),
                         LOAD_SIZE(_RamfuncsLoadSize),
                         LOAD_END(_RamfuncsLoadEnd),
                         RUN_START(_RamfuncsRunStart),
                         RUN_SIZE(_RamfuncsRunSize),
                         RUN_END(_RamfuncsRunEnd),
                         PAGE = 0, ALIGN(4)
#endif

   /* The following section definitions are required when using the IPC API Drivers */
    GROUP : > CPU1TOCPU2RAM, PAGE = 1
    {
        PUTBUFFER
        PUTWRITEIDX
        GETREADIDX
    }

    GROUP : > CPU2TOCPU1RAM, PAGE = 1
    {
        GETBUFFER :    TYPE = DSECT
        GETWRITEIDX :  TYPE = DSECT
        PUTREADIDX :   TYPE = DSECT
    }
    .scratchpad      : > CLARAM0,   PAGE = 1
    .bss_cla         : > CLARAM0,   PAGE = 1
    .const_cla       : > CLARAM0,   PAGE = 1

    Cla1Prog         : > RAML3,
                     LOAD_START(_Cla1funcsLoadStart),
                     LOAD_END(_Cla1funcsLoadEnd),
                     LOAD_SIZE(_Cla1funcsLoadSize),
                     RUN_START(_Cla1funcsRunStart),
                     PAGE = 0

    Cla1ToCpuMsgRAM  : > CLA1_MSGRAMLOW,   PAGE = 1
    CpuToCla1MsgRAM  : > CLA1_MSGRAMHIGH,  PAGE = 1
    Cla1DataRam0     : > CLARAM0,      PAGE = 1


    CLAscratch       :
                     { *.obj(CLAscratch)
                     . += CLA_SCRATCHPAD_SIZE;
                     *.obj(CLAscratch_end) } > CLARAM0,
                     PAGE = 1
   /* The following section definition are for SDFM examples */
   //Filter1_RegsFile : > RAMGS1,	PAGE = 1, fill=0x1111
   //Filter2_RegsFile : > RAMGS2,	PAGE = 1, fill=0x2222
   //Filter3_RegsFile : > RAMGS3,	PAGE = 1, fill=0x3333
   //Filter4_RegsFile : > RAMGS4,	PAGE = 1, fill=0x4444
   //Difference_RegsFile : >RAMGS5, 	PAGE = 1, fill=0x3333
}

/*
//===========================================================================
// End of file.
//===========================================================================
*/

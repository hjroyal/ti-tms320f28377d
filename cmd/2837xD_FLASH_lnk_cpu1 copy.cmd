MEMORY
{
PAGE 0 :  /* Program Memory */
          /* Memory (RAM/FLASH) blocks can be moved to PAGE1 for data allocation */
          /* BEGIN is used for the "boot to Flash" bootloader mode   */

    CLA_PROG_RAM    :   origin=0x008000,length=0x002000  //RAM for cla program
    APP_PROG_RAM    :   origin=0x014E00,length=0x003600 //RAM for App program


    BEGIN           :       origin= 0x080000,length=0x000002    //Flash boot address 
    BOOT_FLASH      :       origin= 0x080002,length=0x003FAE     //BOOT loader program
    INV_TYPE_ADD    :       origin =0x083FB0,length=0x000010   //机型信息记录
    SOFT_VER_ADD    :       origin= 0x083FC0,length=0x000010    //B00T软件版本
    SN_FLAG         :       origin = 0x083FD0,length=0x000010    //SN信息记录
    HARD_VER_FLAG   :       origin=0x083FE0,length= 0x000010    //硬件版本信息记录
    EmulatorFLAG    :       origin =0x083FF0,length =0x000010
    CLA_FLASH       :       origin= 0x084010,length = 0x001FF0    //cla program
    BOOT_FLAG       :       origin =0x086000,length= 0x000010    //程序升级标志保存位置
    INV_TYPE_FLAG   :       origin=0x084000,length =0X000010    //机型名称保存位置
    APP_START       :       origin= 0x086020,length =0x000002    //codestart入口t地址
    APP_FLASH       :       origin =0x086030,length =0x079FD0    //APP程序空间
    RESET           :       origin=0x3FFFC0,length=0x000002

PAGE 1 : /* Data Memory */
         /* Memory (RAM/FLASH) blocks can be moved to PAGE0 for program allocation */

   BOOT_RSVD       : origin = 0x000002, length = 0x000120     /* Part of M0, BOOT rom will use this for stack */
   STACK_RAM       : origin = 0x000400, length = 0x000200     // for hard interrupt stack
   OS_TASK_STK     : origin = 0x000600, length = 0x000300       // for hard interrupt stack

   CLA1_MSGRAMLOW       : origin = 0x001480, length = 0x000080  //cla to cpu msgram
   CLA1_MSGRAMHIGH      : origin = 0x001500, length = 0x000080  //cpu to cla msgram

   LOG_RAM            : origin = 0x000124, length = 0x0002DC    /* for logging */
   CLA_DATA_RAM       : origin = 0x00A000, length = 0x000800     /* CLA data RAM */

   TASK_STK_RAM     : origin = 0x00A800, length = 0x001000     /* for task stack */
   APP_DATA_RAM     : origin = 0x00B810, length = 0x0095F0     /* for task stack */


}

SECTIONS
{
   /* Allocate program areas: */
   .cinit              : > APP_FLASH      PAGE = 0, ALIGN(8)
   .binit              : > APP_FLASH,     PAGE = 0, ALIGN(8)
   .pinit              : > APP_FLASH,     PAGE = 0, ALIGN(8)
   .text               : > APP_FLASH,     PAGE = 0, ALIGN(8)
   
   codestart           : > BEGIN       PAGE = 0, ALIGN(8)

   /* Allocate uninitalized data sections: */
   .stack              : > STACK_RAM          PAGE = 1
   .ebss               : > APP_DATA_RAM       PAGE = 1
   .esysmem            : > APP_DATA_RAM       PAGE = 1
   .cio                : > APP_DATA_RAM       PAGE = 1

    TaskRam             : > TASK_STK_RAM, PAGE = 1
    OsTaskRam          : > OS_TASK_STK, PAGE = 1
    LogRam              : > LOG_RAM, PAGE = 1


   /* Initalized sections go in Flash */
   .econst             : >APP_FLASH     PAGE = 0, ALIGN(8)
   .switch             : >APP_FLASH     PAGE = 0, ALIGN(8)
   .args               : >APP_FLASH     PAGE = 0, ALIGN(8)
   .reset              : > RESET,     PAGE = 0, TYPE = DSECT /* not used, */


}

/*
//===========================================================================
// End of file.
//===========================================================================
*/

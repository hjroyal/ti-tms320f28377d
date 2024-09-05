/**
 * @file app.c
 * @brief
 * @author hjroyal 
 * @version 1.0
 * @date 2024-08-31
 *
 * @copyright Copyright (c) 2024  XX鍏徃
 *
 * @par 淇敼鏃ュ織:
 * <table>
 * <tr><th>Date           <th>Version   <th>Author      <th>Description
 * <tr><td>2024-08-31     <td>1.0       <td>hjroyal     <td>鍐呭
 * </table>
 */

#include <app_cfg.h>
#include <cpu_core.h>
#include <lib_def.h>
#include <os_cpu.h>
#include <os.h>

#include "F28x_Project.h" // Device Headerfile and Examples Include File
#include "F2837xD_Examples.h"

#include "common.h"

#define LED1_ON GpioDataRegs.GPASET.bit.GPIO0 = 1
#define LED1_OFF GpioDataRegs.GPACLEAR.bit.GPIO0 = 1
#define LED1_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO0 = 1

#define LED2_ON GpioDataRegs.GPASET.bit.GPIO1 = 1
#define LED2_OFF GpioDataRegs.GPACLEAR.bit.GPIO1 = 1
#define LED2_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO1 = 1

#define LED3_ON GpioDataRegs.GPASET.bit.GPIO2 = 1
#define LED3_OFF GpioDataRegs.GPACLEAR.bit.GPIO2 = 1
#define LED3_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO2 = 1

#define LED4_ON GpioDataRegs.GPASET.bit.GPIO3 = 1
#define LED4_OFF GpioDataRegs.GPACLEAR.bit.GPIO3 = 1
#define LED4_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO3 = 1

#define LED5_ON GpioDataRegs.GPASET.bit.GPIO4 = 1
#define LED5_OFF GpioDataRegs.GPACLEAR.bit.GPIO4 = 1
#define LED5_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO4 = 1

#define LED6_ON GpioDataRegs.GPASET.bit.GPIO5 = 1
#define LED6_OFF GpioDataRegs.GPACLEAR.bit.GPIO5 = 1
#define LED6_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO5 = 1

//beep
#define BEEP_TOGGLE GpioDataRegs.GPATOGGLE.bit.GPIO30 = 1
/*
*************************************************************************
*                            USER STACK SIZE
*************************************************************************
*/
#define APP_TASK_LED1_STK_SIZE 160u
#define APP_TASK_LED2_STK_SIZE 160u
#define APP_TASK_BEEP_STK_SIZE 160u

#pragma DATA_SECTION(AppTaskLed1TCB, "RAMM1");
#pragma DATA_SECTION(AppTaskLed1Stk, "RAMM1");
#pragma DATA_SECTION(AppTaskLed2TCB, "RAMM1");
#pragma DATA_SECTION(AppTaskLed2Stk, "RAMM1");
#pragma DATA_SECTION(AppTaskBeepTCB, "RAMM1");
#pragma DATA_SECTION(AppTaskBeepStk, "RAMM1");
/*
**************************************************************
*                               TCB
**************************************************************
*/

static OS_TCB AppTaskLed1TCB;
static OS_TCB AppTaskLed2TCB;
static OS_TCB AppTaskBeepTCB;

/*
*************************************************************************
*                            STACKS
*************************************************************************
*/

static CPU_STK AppTaskLed1Stk[APP_TASK_LED1_STK_SIZE];
static CPU_STK AppTaskLed2Stk[APP_TASK_LED2_STK_SIZE];
static CPU_STK AppTaskBeepStk[APP_TASK_BEEP_STK_SIZE];

/*
************************************************************************
*                          FUNCTION PROTOTYPES
*************************************************************************
*/

static void AppTaskLed1(void *p_arg);
static void AppTaskLed2(void *p_arg);
static void AppTaskBeep(void *p_arg);

interrupt void cpu_timer0_isr(void);
interrupt void cpu_timer1_isr(void);
interrupt void cpu_timer2_isr(void);

void BSP_Tick_Init(void);
void GPIO_Setup(void);

INT32U flag = 0;

void main(void)
{
    OS_ERR err;

    InitSysCtrl();

    InitGpio();
    GPIO_Setup();

    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    BSP_Tick_Init();

    EINT;

    ERTM;


    OSInit(&err); /* Init uC/OS-III.*/

    /*  LED1  */
    OSTaskCreate((OS_TCB *)&AppTaskLed1TCB, 
                 (CPU_CHAR *)"App Task Led1",
                 (OS_TASK_PTR)AppTaskLed1,                  
                 (void *)0,                                 
                 (OS_PRIO)5,                                
                 (CPU_STK *)&AppTaskLed1Stk[0],             
                 (CPU_STK_SIZE)APP_TASK_LED1_STK_SIZE / 10, 
                 (CPU_STK_SIZE)APP_TASK_LED1_STK_SIZE,      
                 (OS_MSG_QTY)0u,                            
                 (OS_TICK)0u,                               
                 (void *)0,                                 
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err); 
    /*  LED2  */
    OSTaskCreate((OS_TCB *)&AppTaskLed2TCB,
                 (CPU_CHAR *)"App Task Led2",
                 (OS_TASK_PTR)AppTaskLed2,
                 (void *)0,
                 (OS_PRIO)2, // APP_TASK_LED2_PRIO,
                 (CPU_STK *)&AppTaskLed2Stk[0],
                 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_LED2_STK_SIZE,
                 (OS_MSG_QTY)0u,
                 (OS_TICK)0u,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);

    /*  beep */
    OSTaskCreate((OS_TCB *)&AppTaskBeepTCB,
                 (CPU_CHAR *)"App Task Beep",
                 (OS_TASK_PTR)AppTaskBeep,
                 (void *)0,
                 (OS_PRIO)3, // APP_TASK_LED2_PRIO,
                 (CPU_STK *)&AppTaskBeepStk[0],
                 (CPU_STK_SIZE)APP_TASK_BEEP_STK_SIZE / 10,
                 (CPU_STK_SIZE)APP_TASK_BEEP_STK_SIZE,
                 (OS_MSG_QTY)0u,
                 (OS_TICK)0u,
                 (void *)0,
                 (OS_OPT)(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR *)&err);
    
    OSStart(&err); /* Start multitasking(i.e.give control to 渭C/OS-III).*/
}

void BSP_Tick_Init(void)
{
    EALLOW;
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.TIMER1_INT = &cpu_timer1_isr;
    PieVectTable.TIMER2_INT = &cpu_timer2_isr;
    PieVectTable.RTOS_INT   = &RTOS_ISR; // RTOS
    EDIS;

    InitCpuTimers();

    ConfigCpuTimer(&CpuTimer0, 200, 2000000);
    CpuTimer0Regs.TCR.all = 0x4000;
    IER |= M_INT1;
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;

    flag = 20;
     ConfigCpuTimer(&CpuTimer1, 200, 1000000); // 1s 200MHz, INTERRUPT_period = 1e6 us
     CpuTimer1Regs.TCR.all = 0x4000;
     IER |= M_INT13;

     ConfigCpuTimer(&CpuTimer2, 200, 1000); //1ms // 200MHz, INTERRUPT_period = 1e6 us
     CpuTimer2Regs.TCR.all = 0x4000;
     IER |= M_INT14;
}

INT32U test1 = 0;
INT32U test2 = 0;
INT32U test3 = 0;

static void AppTaskLed1(void *p_arg)
{
    OS_ERR err;
    OS_REG value;

    (void)p_arg;

    test1=1000;

    while (DEF_TRUE)
    {
        EALLOW;
        LED1_TOGGLE; 
        LED2_TOGGLE; 

        EDIS;

        OSTimeDly(1000, OS_OPT_TIME_DLY, &err);
    }
}

static void AppTaskLed2(void *p_arg)
{
    OS_ERR err;

    (void)p_arg;

    test2=2000;

    while (DEF_TRUE)
    {
        EALLOW;
        
        LED3_TOGGLE; 
        LED4_TOGGLE; 
        GpioDataRegs.GPCTOGGLE.bit.GPIO67 = 1;
        EDIS;

        OSTimeDly(3000, OS_OPT_TIME_DLY, &err);
    }
}

static void AppTaskBeep(void *p_arg)
{
    OS_ERR err;

    (void)p_arg;

    test3=3000;

    while (DEF_TRUE) 
    {
        EALLOW;
        LED5_TOGGLE;
        LED6_TOGGLE; 
        BEEP_TOGGLE;
        EDIS;

        OSTimeDly(5000, OS_OPT_TIME_DLY, &err);
    }
}


interrupt void cpu_timer0_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

    CpuTimer0.InterruptCount++; // test

}


interrupt void cpu_timer1_isr(void)
{
    CpuTimer1.InterruptCount++; // test

    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void cpu_timer2_isr(void)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

    CpuTimer2.InterruptCount++; // test

    if (OSRunning == OS_STATE_OS_RUNNING)
    {
        flag=11;
        OSIntEnter();
        OSTimeTick();
        OSIntExit();
    }
}
void GPIO_Setup(void)
{
    EALLOW;
    //led1~6
    GPIO_SetupPinMux(0, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(0, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(1, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(1, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(2, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(3, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(4, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(4, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinMux(5, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(5, GPIO_OUTPUT, GPIO_PUSHPULL);

    //beep
    GPIO_SetupPinMux(30, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(30, GPIO_OUTPUT, GPIO_PUSHPULL);

    //relay
    GPIO_SetupPinMux(67, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(67, GPIO_OUTPUT, GPIO_PUSHPULL);
    EDIS;
}

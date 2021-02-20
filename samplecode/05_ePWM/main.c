
/* main.c
 * ePWM1 sample code
 * You can check the output pulse from ePWM01 port
 * This code is written by Eunjin Lee(https://github.com/EUNJINLEE96/f28379d/blob/main/EpwmSampleCode_Real)
 * @author Eunjin Lee
 */

#include "F28x_Project.h"
interrupt void epwm1_isr(void){

    EPwm1Regs.ETCLR.bit.INT=1; //clear
    PieCtrlRegs.PIEACK.all=PIEACK_GROUP3;
}

int main(){
    InitSysCtrl();
    InitGpio();
    CpuSysRegs.PCLKCR2.bit.EPWM1=1;

    InitEPwm1Gpio();

    //Configure Gpio
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0=0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0=1;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1=0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1=1;

    GpioCtrlRegs.GPADIR.bit.GPIO0=1;
    GpioCtrlRegs.GPADIR.bit.GPIO1=1;


    DINT;
    InitPieCtrl();
    IER=0x0000;
    IFR=0x0000;
    InitPieVectTable();

    EALLOW;
    PieVectTable.EPWM1_INT=&epwm1_isr;
    EDIS;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC=0;//time base clk of all ePWM modules is stopped
    EDIS;

    //////initialize the ePWM//////
    //Setup TBCLK
    EPwm1Regs.TBPRD=2000; //!!!!!set period of time base counter 801 TBCLKs
    EPwm1Regs.TBPHS.bit.TBPHS=0x0000; //phase is 0
    EPwm1Regs.TBCTR=0x0000; //clear counter
    //Set Compare values
    EPwm1Regs.CMPA.bit.CMPA=100; //set compare A value
    EPwm1Regs.CMPB.bit.CMPB=1950; //set compare B value
    //Setup counter mode
    EPwm1Regs.TBCTL.bit.CTRMODE=2; //Count up and down
    EPwm1Regs.TBCTL.bit.PHSEN=0; //Disable phase loading (TBPHS value)
    EPwm1Regs.TBCTL.bit.HSPCLKDIV=0; //tbclk prescale. TBCLK=EPWMCLK
    EPwm1Regs.TBCTL.bit.CLKDIV=0;//
    //Setup shadowing
    EPwm1Regs.CMPCTL.bit.SHDWAMODE=0; //Shadow mode
    EPwm1Regs.CMPCTL.bit.SHDWBMODE=0;
    EPwm1Regs.CMPCTL.bit.LOADAMODE=0; //Load on TBCTR=Zero
    EPwm1Regs.CMPCTL.bit.LOADBMODE=0;
    //Set actions
    EPwm1Regs.AQCTLA.bit.CAU=2; //Set PWM1A on event A, up count
    EPwm1Regs.AQCTLA.bit.CAD=1; //Clear PWM1A on event A, down count
    EPwm1Regs.AQCTLB.bit.CBU=2; //Set PWM1B on event B, up count
    EPwm1Regs.AQCTLB.bit.CBD=1; //Clear PWM1B on event B, down count
    //Interrupt where we will change the compare values
    EPwm1Regs.ETSEL.bit.INTSEL=1; //TBCTR=Zero event is trigger for EPWM1_INT
    EPwm1Regs.ETSEL.bit.INTEN=1; //Enable INT
    EPwm1Regs.ETPS.bit.INTPRD=3; //Generate INT on every 3rd TBCTR=Zero event

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC=1; //all TBCLK start counting
    EDIS;

    IER=M_INT3;
    PieCtrlRegs.PIEIER3.bit.INTx1=1; //EPWM1_INT
    EINT;
    ERTM; //Enable Global realtime interrupt DBGM


    return 0;
}

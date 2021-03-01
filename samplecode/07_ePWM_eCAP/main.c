#include <F28x_Project.h>

/**
 * main.c
 */
Uint32 duty[4];
int test;
interrupt void ecap1_isr(void){
    duty[0] = ECap1Regs.CAP1;
    duty[1] = ECap1Regs.CAP2;
    duty[2] = ECap1Regs.CAP3;
    duty[3] = ECap1Regs.CAP4;

    ECap1Regs.ECCLR.bit.INT = 1;
    ECap1Regs.ECCLR.bit.CEVT4 = 1;
    ECap1Regs.ECCTL2.bit.REARM = 1;

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP4;
}

interrupt void epwm1_isr(void){
    EPwm1Regs.ETCLR.bit.INT=1; //clear
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}


int main(void)
{

    InitSysCtrl();
    InitGpio();

    // EPWM1 CONFIGURATION
    CpuSysRegs.PCLKCR2.bit.EPWM1 = 1; // enable clock for EPWM1

    DINT;
    InitPieCtrl();

    IER = 0x0000;
    IFR = 0x0000;

    InitPieVectTable();
    EALLOW;
    PieVectTable.EPWM1_INT=&epwm1_isr;
    EDIS;
    IER |= M_INT3;
    PieCtrlRegs.PIEIER3.bit.INTx1=1; //EPWM1_INT

    InitEPwm1Gpio();
    GpioCtrlRegs.GPAGMUX1.bit.GPIO0=0;
    GpioCtrlRegs.GPAGMUX1.bit.GPIO1=0;

    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC=0;// time base clk of all ePWM modules is stopped
    EDIS;

    // initialize EPWM1
    //Setup TBCLK
    EPwm1Regs.TBPRD=1000;
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
    // END OF EPWM CONFIGURATION

    // eCAP CONFIGURATION
    // 1. Input-X-Bar
    EALLOW;
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 0; // use GPIO60 as gpio
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;
    GpioCtrlRegs.GPBDIR.bit.GPIO60 = 0; // GPIO FOR INPUT
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 0x0; // Input Qualification type - Sync

    InputXbarRegs.INPUT7SELECT = 60; // connect GPIO60 input to INPUT-X-BAR line no.7 -> eCAP1
    EDIS;

    // 2. Connect Interrupt

    EALLOW;
    PieVectTable.ECAP1_INT = ecap1_isr;
    EDIS;
    IER |= M_INT4;

    PieCtrlRegs.PIEIER4.bit.INTx1 = 1; // enable ECAP1 PIE Interrupt

    // 2. configure eCAP
    ECap1Regs.ECEINT.all = 0x0000; // Disable All ECAP Interrupt
    ECap1Regs.ECCLR.all = 0xFFFF; // clear All ECAP interrupt flag
    ECap1Regs.ECCTL1.bit.CAPLDEN = 0;       // Disable CAP1-CAP4 register loads
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 0;     // Make sure the counter is stopped

    ECap1Regs.ECCTL1.bit.PRESCALE = 0; // ECAP input signal bypasses prescaler
    ECap1Regs.ECCTL1.bit.CAP1POL = 1; // rising edge
    ECap1Regs.ECCTL1.bit.CAP2POL = 1;
    ECap1Regs.ECCTL1.bit.CAP3POL = 1;
    ECap1Regs.ECCTL1.bit.CAP4POL = 1;
    ECap1Regs.ECCTL1.bit.CTRRST1 = 1; // reset counter when the event happens
    ECap1Regs.ECCTL1.bit.CTRRST2 = 1;
    ECap1Regs.ECCTL1.bit.CTRRST3 = 1;
    ECap1Regs.ECCTL1.bit.CTRRST4 = 1;
    ECap1Regs.ECCTL1.bit.CAPLDEN = 1; // save CAP result in register
    ECap1Regs.ECCTL2.bit.CAP_APWM = 0; // eCAP capture mode


    ECap1Regs.ECCTL2.bit.CONT_ONESHT = 0; // CONTINUOUS
    ECap1Regs.ECCTL2.bit.SYNCO_SEL = 0; // pass the syncInput to syncOutput
    ECap1Regs.ECCTL2.bit.SYNCI_EN = 0; // Disable SyncIn. because In this example, there is no co-operation with other eCAP
    ECap1Regs.ECCTL2.bit.TSCTRSTOP = 1; // start eCAP_RUN;

    ECap1Regs.ECCTL2.bit.REARM = 1;
    ECap1Regs.ECEINT.bit.CEVT4 = 1; // only allow CEVT 4. 1 interrupt = 4 events

    EINT;
    ERTM;

    while(1){

    }

	return 0;
}

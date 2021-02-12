#include <F28x_Project.h>

/**
 * main.c
 * Timer interrupt samplecode using CpuTimer0
 * Built in LED blinks every 1sec
 * @author Jaeyeon Park
 */

interrupt void timer0_isr(){
    CpuTimer0.InterruptCount++;
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1; //flip the led state
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // clear PIEACK.1 == PIEACK switch closed
}


int main(void)
{
    // InitSysCtrl
        // FOR LAUNCHXL_F28379D
        // external clock is 10MHz oscillator
        // PLLSYSCLK = 10M * 40/2 = 200MHz
        // refer to 27page of https://www.ti.com/lit/ug/sprui77c/sprui77c.pdf?ts=1613144312872&ref_url=https%253A%252F%252Fwww.google.com%252F
        //
    InitSysCtrl();
    InitGpio();


    EALLOW;
    // GPIO SETUP
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1; // GPIO FOR OUTPUT

    // enable clock for timers
    CpuSysRegs.PCLKCR0.bit.CPUTIMER0 = 1;
    CpuSysRegs.PCLKCR0.bit.CPUTIMER1 = 1;
    EDIS;

    InitCpuTimers(); // initialize global variable CpuTimer0, CpuTimer1, CpuTimer2
    ConfigCpuTimer(&CpuTimer0,200,1000000); // timer, 200MHz(PLL CLK), 1000000us
    // after ConfigCpuTimer
    // Timer Interrupt is enabled
    // Timer is stopped
    // Timer's count is 0

    /************** SET INTERRUPT **************/
    DINT;
    IER = 0X0000;// disable all interrupt
    IFR = 0X0000;// initialize all interrupt flag

    IER = M_INT13 | M_INT1; // M_INT13 = CPU TIMER1 interrupt, M_INT1 = PIE1 (to enable timer0 interrupt)

    InitPieCtrl();
    InitPieVectTable();
    EALLOW;
    // enable interrupt for PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; // set total PIE Enable Register
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1; // allow TIMER0 interrupt signal

    PieVectTable.TIMER0_INT = &timer0_isr; // connect the isr
    EDIS;
    EINT;
    ERTM;
    CpuTimer0.RegsAddr->TCR.bit.TSS = 0; //StartCpuTimer0();
    // it is same with StartCpuTimer0

    while(1){

    }

	return 0;
}

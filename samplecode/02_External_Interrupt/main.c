
#include <F28x_Project.h>
/**
 * main.c
 */

interrupt void xint1_isr(){
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1; //flip the led state
}

int main(void)
{
    InitSysCtrl();
    InitGpio();

    /************** SET GPIO **************/
    // use pin 31 and 16 as GPIO
    EALLOW;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO16 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0;

    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1; // GPIO FOR OUTPUT
    GpioCtrlRegs.GPADIR.bit.GPIO16 = 0; // GPIO FOR INPUT
    EDIS;

    /************** SET INTERRUPT **************/
    DINT;
    IER = 0X0000;// disable all interrupt
    IFR = 0X0000;// initialize all interrupt flag

    // enable interrupt for INT1, INT2 in CPU interrupt Vectors
    // refer to "Technical Reference Manual - TMS320F2837xD : CPU Interrupt Vectors (103p)"
    IER = M_INT1 | M_INT2; //enable interrupt from PIE group 1&2

    InitPieCtrl();
    InitPieVectTable();

    EALLOW;
    // enable interrupt for PIE
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; // set total PIE Enable Register
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;
    // ENABLE INT CHANNEL 4 of PIE GROUP 1 == INT1.4 in PIE Interrupt Vectors == XINT1 interrupt

    // mapping the task(function) that the cpu does when the INT1.4 interrupts
    PieVectTable.XINT1_INT = &xint1_isr; // INT1.4 in PIE GROUP 1 vectors.
    EDIS;

    /************** CONNECT GPIO TO USE IT AS EXTERNAL INTERRUPT(RISING OR FALLING EDGE) **************/
    EALLOW;
    InputXbarRegs.INPUT4SELECT = 16; // INPUT4 is connected with XINT1 internally. InputXbar connects GPIO16 to INPUT4
    XintRegs.XINT1CR.bit.POLARITY = 3; // interrupt is selected as positive&negative edge triggered. refer to technical reference 247p.
    XintRegs.XINT1CR.bit.ENABLE = 1; // enable external interrupt
    EDIS;

    EINT;

    while(1){
        DELAY_US(1E3);

    }

	return 0;
}

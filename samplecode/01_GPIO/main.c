#include <F28x_Project.h>
/**
 * main.c
 * Sample code about the basic Usage and configuration for GPIO
 * @author Jaeyeon Park
 */
int main(void)
{
    InitSysCtrl();
    InitGpio();

    /** SET GPIO **/
    EALLOW;
    // set the role of physical pin as GPIO
    // refer to "Technical reference - GPIO Muxed Pins (959p)

    // use pin 31 as GPIO
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0; // On-board blue LED
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    // use pin 5 as GPUIO
    GpioCtrlRegs.GPAGMUX1.bit.GPIO5 = 0;
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;

    // GPIO for OUTPUT
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioDataRegs.GPACLEAR.bit.GPIO31 = 1; // led is on when the state of GPIO31 is LOW

    // GPIO for INPUT
    GpioCtrlRegs.GPADIR.bit.GPIO5 = 0;

    EDIS;

    while(1){
        DELAY_US(1E3);
        EALLOW;
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
        EDIS;

        //access the state of GPIO
        int state = GpioDataRegs.GPADAT.bit.GPIO5;
    }
	return 0;
}

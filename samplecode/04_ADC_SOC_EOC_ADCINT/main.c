
#include <F28x_Project.h>
/**
 * main.c
 */

extern interrupt void ADCAINT1_isr(){
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
}
extern interrupt void ADCAINT2_isr(){
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
}

int main(void)
{
    #ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    #endif
    InitSysCtrl();
    InitGpio();
    DINT;
    EALLOW;
    GpioCtrlRegs.GPAGMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;
    GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;


    //enable Module Clock to ADC
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    CpuSysRegs.PCLKCR13.bit.ADC_B = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_C = 0;
    CpuSysRegs.PCLKCR13.bit.ADC_D = 0;

    //set Prescale for ADC Clock
    AdcaRegs.ADCCTL2.bit.RESOLUTION = 0; // 0 : 12-bit resolution, 1: 16-bit resolution
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; // ADC clock diver to /4
    AdcaRegs.ADCCTL2.bit.SIGNALMODE = 0; // 0 : Single ended, 1: Differential

    //power up
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1E3);
    // delay 1000us.
    // The delay is needed after power up the adc module.
    // refer to technical reference 1574p

    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;   // set the timing when the EOC signal is generated. 0(end of acq windows), 1(end of conversion)
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;      // ADCINA2 is selected
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 63;     // SAMPLE window is acqps + 1sysclk cycles = 640ns
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0;    // No trigger selected from ADCTRIGn
    AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 1;    // enable ADCINT1 as a trigger

    AdcaRegs.ADCSOC15CTL.bit.CHSEL = 1;
    AdcaRegs.ADCSOC15CTL.bit.ACQPS = 63;
    AdcaRegs.ADCSOC15CTL.bit.TRIGSEL = 0;
    AdcaRegs.ADCINTSOCSEL2.bit.SOC15 = 2; // enable ADCINT2 as a trigger

    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
    AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1;

    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0;
    AdcaRegs.ADCINTSEL1N2.bit.INT2CONT = 0;

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;
    AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 15;
    EDIS;


    IER = 0x0000;
    IFR = 0x0000;
    IER = M_INT1|M_INT10;
    InitPieCtrl();
    InitPieVectTable();
    EALLOW;
    PieVectTable.ADCA1_INT = ADCAINT1_isr; //connect interrupt service routine
    PieVectTable.ADCA2_INT = ADCAINT2_isr;

    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; //enable PIE
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; //enable ADCINTA1
    PieCtrlRegs.PIEIER10.bit.INTx2 = 1; //enable ADCINTA2
    EDIS;
    EINT;

    AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1;
    while(1){
        DELAY_US(1E6);
        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }

	return 0;
}

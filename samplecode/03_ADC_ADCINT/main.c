
#include <F28x_Project.h>
/**
 * main.c
 * repetitive ADC using software ADC trigger, ADCINT1(ADC interrupt) and PIE ISR
 * @author Jaeyeon Park
 */

interrupt void ADCAINT1_isr(){
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

   // Software Trigger for ADC
   AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
   AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1;
}

int main(void)
{
    InitSysCtrl();
    InitGpio();


    EALLOW;
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

    //Congfigure SOC
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;   // set the timing when the EOC signal is generated. 0(end of acq windows), 1(end of conversion)
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;      // ADCINA2 is selected
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 63;     // SAMPLE window is acqps + 1sysclk cycles = 640ns
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 0;    // No trigger selected from ADCTRIGn
    AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 0;    // disable all ADCINT for trigger

    //Configure EOC
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1; //ADCINT1 interrupt ENABLE
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;
    //EOC0 IS trigger for ADCINT1 (SOCn <-> EOCn). the return signal for SOC0 start signal, is come from EOC1
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; // clear the adcintflg latch
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0;
    EDIS;

    DINT;
    IER = 0x0000;
    IFR = 0x0000;
    IER = M_INT1;
    InitPieCtrl();
    InitPieVectTable();
    EALLOW;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; //enable PIE
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; //enable ADCINTA1
    PieVectTable.ADCA1_INT = &ADCAINT1_isr; //connect interrupt service routine
    EINT;

    DELAY_US(2E6);
    AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1; // start ADC(software trigger)
    while(1){


    }



	return 0;
}


#include <F28x_Project.h>
/**
 * main.c
 * repetitive ADC using Self triggering
 * 
 * At the end of Analog-to-Digital Conversion,
 * EOC0 generates ADCINT1 Signal that pulls the trigger of SOC0 
 * to start Analog-to-digital conversion again
 * @author Jaeyeon Park
 */

// To upload program on the flash,
// remove comment mark to enable #define _FLASH
// and change 'Linker Command File' in Project Properties/General
// from 2837x_RAM_lnk_cpu1.cmd to 2837x_FLASH_lnk_cpu1.cmd
#define _FLASH


volatile long int count1 = 0;
extern interrupt void ADCAINT1_isr(){
    count1++;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // this line is needed to get the next ADCINT1 interrupt from EOC0
}

int main(void)
{
    #ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
    #endif
    InitSysCtrl();
    InitGpio();

    EALLOW;
    // enable ADC module clock
    CpuSysRegs.PCLKCR13.all = 0x00000000;
    CpuSysRegs.PCLKCR13.bit.ADC_A = 1;
    // Configure ADC
    AdcaRegs.ADCCTL2.bit.RESOLUTION = 0;
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6;
    AdcaRegs.ADCCTL2.bit.SIGNALMODE = 0; // SINGLE INPUT MODE
    // power up
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;
    DELAY_US(1E3);
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;
    // All Clear ADCINT in SOCn
    AdcaRegs.ADCINTSOCSEL1.all = 0x0000;
    AdcaRegs.ADCINTSOCSEL2.all = 0x0000;
    // Configure EOC
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;    // enable ADCINT1
    AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 1; // enable continuous interrupt. If it is discontinuous, SOC0-EOC0 pair only works once
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0;  // setup EOC0 to pull the trigger(ADCINT1)
    //Configure SOC
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 2;  // use ADCINA2
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = 15;
    AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 1; // ADCINT1 is the trigger for SOC0
    EDIS;

    DINT;
    IER = 0x0000;
    IFR = 0x0000;
    IER = M_INT1;
    InitPieCtrl();
    InitPieVectTable();

    EALLOW;
    PieVectTable.ADCA1_INT = ADCAINT1_isr; //connect interrupt service routine
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1; //enable PIE
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1; //enable ADCINTA1
    EDIS;

    EINT;
    AdcaRegs.ADCSOCFRC1.bit.SOC0 = 1; // PULL THE TRIGGER for the initial start


    while(1){
        //DELAY_US(1E3);
        //GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
    }

	return 0;
}

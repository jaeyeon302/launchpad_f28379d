
/**
 * main.c
 */

#include <F28x_Project.h>

int main(void)
{
    InitEPwmGpio();
    Config_EPwm();
    SyncSocRegs.SYNCSELECT.bit.SYNCOUT;
    CpuSysRegs.PCLKCR2.bit.EPWM1
    EPwm1Regs.AQTSRCSEL.bit.T1SEL


	return 0;
}

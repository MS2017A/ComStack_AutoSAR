#include "Std_Types.h"
#include "MCU.h"
#include "driverlib/sysctl.h"




uint32 Mcu_GetSysCloock(void)
{
	return SysCtlClockGet();
}


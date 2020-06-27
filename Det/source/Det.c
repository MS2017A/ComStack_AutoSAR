#include "Std_Types.h"
#include "Det.h"



void Det_ReportError(uint16 ModuleId ,uint8 Instanceid ,uint8 ApiId ,uint8 ErrorId)
{
    UARTprintf("Module=%x,ApiId=%x,error=%x",ModuleId,ApiId,ErrorId);
    while(1)
    {
    }
}

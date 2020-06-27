#include "Std_Types.h"
#define ENTER_CRITICAL_SECTION()             __asm("    cpsie   i\n")
#define EXIT_CRITICAL_SECTION()              __asm("    cpsid   i\n")

uint32 counter;

void Rte_enterCriticaSection()
{
    ENTER_CRITICAL_SECTION();
    counter++;
}

void Rte_exitCriticalSection()
{
    counter--;
    if(counter==0)
    {
        EXIT_CRITICAL_SECTION();
    }
}

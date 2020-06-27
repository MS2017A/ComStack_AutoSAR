#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "Platform_Types.h"
#include "Compiler.h"
#include "Std_Types.h"
#include "ComStack_Types.h"

#include "Can.h"
#include "CanIf.h"
#include "CanTp.h"
/* Task Priority*/
#define TASK1_PRIORITY      (5)
#define TASK2_PRIORITY      (4)
#define TASK3_PRIORITY      (4)

int flag = 1 ;

void InitConsole(void) ;
void Can_Mcu_Init(void);
void SimpleDelay(void) ;




uint8 Data[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18};

VAR(PduInfoType,AUTOMATIC) PduInfoPtr[] =
{
 {
  .MetaDataPtr = NULL,
  .SduDataPtr  = &Data[0],
  .SduLength   = 2
 },
 {
  .MetaDataPtr = NULL,
  .SduDataPtr  = &Data[2],
  .SduLength   = 2
 },
 {
  .MetaDataPtr = NULL,
  .SduDataPtr  = &Data[4],
  .SduLength   = 2
 },
 {
  .MetaDataPtr = NULL,
  .SduDataPtr  = &Data[6],
  .SduLength   = 2
 }
};


const Can_PduType Can_Pdu[4]={
                              {
                               .swPduHandle = 0    ,
                               .length      = 2    ,
                               .id          = 8    ,
                               .sdu         = &Data[0]
                              },{

                                 .swPduHandle = 1    ,
                                 .length      = 2    ,
                                 .id          = 7    ,
                                 .sdu         = &Data[2]
                              },
                              {
                               .swPduHandle = 5    ,
                               .length      = 2    ,
                               .id          = 2    ,
                               .sdu         = &Data[4]
                              },
                              {
                               .swPduHandle = 5    ,
                               .length      = 2    ,
                               .id          = 3    ,
                               .sdu         = &Data[6]
                              }
};





/* Private Function Prototype */
static void Task_1( void *pvParameters );
static void Task_2( void *pvParameters );
static void Task_SendData( void *pvParameters );
xSemaphoreHandle g_pUARTSemaphore;



//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}

#endif

int main(void)
{
    Can_Mcu_Init();
    InitConsole() ;

    Can_Init(NULL_PTR);
    CanIf_Init(0)     ;

    Can_SetControllerMode( CAN_CONTROLLER_ONE , CAN_CS_STARTED );

//    CanIf_SetPduMode(3, CANIF_ONLINE);


    CanIf_SetPduMode(0, CANIF_ONLINE);
    CanIf_SetPduMode(1, CANIF_ONLINE);
//    CanIf_SetPduMode(2, CANIF_ONLINE);
//    CanIf_SetPduMode(4, CANIF_ONLINE);


    CanTp_Init(NULL_PTR);



    UARTprintf("\n\nWelcome \n");


    xTaskCreate( Task_1, "Task_1", 1000, NULL, 5, NULL );
    xTaskCreate( Task_2, "Task_2", 1000, NULL, 1, NULL );

    //    xTaskCreate( Task_SendData , "Task_3", 5000, NULL, 3, NULL );

    vTaskStartScheduler();
    while(1)
    {
    }
}

#if MAIN_FUNCTION == TRUE
Can_DisableControllerInterrupts(CAN_CONTROLLER_ZERO);
#endif


uint32 ErrorState;
//    while(1)
//    {
//        uint8 i ;
//        for ( i = 0 ; i<4 ; i++)
//        {
//            CanIf_Transmit(i, &PduInfoPtr[i]);
//            SimpleDelay();
//            Can_GetControllerErrorState( 1,&ErrorState );
//            UARTprintf("Error status = %x",ErrorState)  ;
//
//        }
//    }
//}


static void Task_1( void *pvParameters ) // every 1 sec
{

    while(1)
    {
        CanTp_MainFunction();
        vTaskDelay(1000);    /* waiting 1 sec*/
    }
}

//static void Task_SendData( void *pvParameters ){
//    while(1)
//    {
//        uint8 i ,ErrorState;
//        for ( i = 0 ; i<4 ; i++)
//        {
//            CanIf_Transmit(i, &PduInfoPtr[i]);
//            Can_GetControllerErrorState( 1,&ErrorState );
//            UARTprintf("Error status = %x",ErrorState)  ;
//        }
//        vTaskDelay(3000);    /* waiting 1 sec*/
//    }
//
//}

//TASK(T2) // every 5 sec
static void Task_2( void *pvParameters )
{
    uint8 Array[]= {'I','T','I','_','4','0','_','C','a','n','T','p','_','T','e','a','m'};
    PduInfoType Frame;

    Frame.SduDataPtr = Array;
    Frame.SduLength=17;

    while(1)
    {
        if (flag == 1)
        {
            /* Transmit **********/
            CanTp_Transmit((uint16)1, &Frame);

            /* Receive ***********/
            /*CanTp_RxIndication(1,&FF);
           CanTp_RxIndication(1,&CF);*/

            flag = 0;
        }
        vTaskDelay(3000);    /* waiting 5 sec*/
    }
}


void
InitConsole(void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

void
SimpleDelay(void)
{
    //
    // Delay cycles for 1 second
    //
    SysCtlDelay(16000000 / 3);
}

void Can_Mcu_Init(void)
{
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);


    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);

}

void vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    while(1)
    {
    }
}


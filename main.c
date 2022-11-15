/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/


QueueHandle_t xQueue1 = NULL;
int deadline_misses[6] = {0};
typedef struct message{
	char *string;
}message;

void vApplicationIdleHook(void);
void Button_1_Monitor( void * pvParameters );
void Button_2_Monitor( void * pvParameters );
void Periodic_Transmitter( void * pvParameters );
void Uart_Receiver( void * pvParameters );
void Load_1_Simulation( void * pvParameters );
void Load_2_Simulation( void * pvParameters );


TaskHandle_t Button_1_MonitorHandle = NULL;
TaskHandle_t Button_2_MonitorHandle = NULL;
TaskHandle_t Periodic_TransmitterHandle = NULL;
TaskHandle_t Uart_ReceiverHandle = NULL;
TaskHandle_t Load_1_SimulationHandle = NULL;
TaskHandle_t Load_2_SimulationHandle = NULL;




/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	xQueue1 = xQueueCreate( 10, sizeof( message * )  );

	
    /* Create Tasks here */

		 xTaskPeriodicCreate(
							Button_1_Monitor,       			/* Function that implements the task. */
							"task 1",         					/* Text name for the task. */
							100,      							/* Stack size in words, not bytes. */
							( void * ) 0,   					/* Parameter passed into the task. */
							2,									/* Priority at which the task is created. */
							&Button_1_MonitorHandle , 50 );     /* Used to pass out the created task's handle. */
							
							
							
			xTaskPeriodicCreate(
							Button_2_Monitor,       			/* Function that implements the task. */
							"task 2",          					/* Text name for the task. */
							100,      							/* Stack size in words, not bytes. */
							( void * ) 0,    					/* Parameter passed into the task. */
							2,									/* Priority at which the task is created. */
							&Button_2_MonitorHandle  , 50);     /* Used to pass out the created task's handle. */

							
							
			xTaskPeriodicCreate(
							Periodic_Transmitter,       			/* Function that implements the task. */
							"task 3",          						/* Text name for the task. */
							100,      								/* Stack size in words, not bytes. */
							( void * ) 0,    						/* Parameter passed into the task. */
							2,										/* Priority at which the task is created. */
							&Periodic_TransmitterHandle  , 100);    /* Used to pass out the created task's handle. */
			

			xTaskPeriodicCreate(
							Uart_Receiver,      			/* Function that implements the task. */
							"task 4",          				/* Text name for the task. */
							100,      						/* Stack size in words, not bytes. */
							( void * ) 0,    				/* Parameter passed into the task. */
							2,								/* Priority at which the task is created. */
							&Uart_ReceiverHandle , 20 );    /* Used to pass out the created task's handle. */
							
							
			xTaskPeriodicCreate(
							Load_1_Simulation,      			/* Function that implements the task. */
							"task 5",          					/* Text name for the task. */
							100,      							/* Stack size in words, not bytes. */
							( void * ) 0,    					/* Parameter passed into the task. */
							2,									/* Priority at which the task is created. */
							&Load_1_SimulationHandle , 10 );   /* Used to pass out the created task's handle. */
							
			xTaskPeriodicCreate(
							Load_2_Simulation,      			/* Function that implements the task. */
							"task 6",          					/* Text name for the task. */
							100,      							/* Stack size in words, not bytes. */
							( void * ) 0,    					/* Parameter passed into the task. */
							2,									/* Priority at which the task is created. */
							&Load_2_SimulationHandle , 100 );    /* Used to pass out the created task's handle. */
							
			


	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void){
	GPIO_write(PORT_0, PIN0, PIN_IS_HIGH);
	GPIO_write(PORT_0, PIN0, PIN_IS_LOW);
}

/*-----------------------------------------------------------*/
void Button_1_Monitor( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t start_time = 0, end_time = 0;
    uint8_t  previous_state = GPIO_read(PORT_1 , PIN0);
	
	message Btn_Msg;
	message *pBtn_Msg = &Btn_Msg;

    for( ;; )
    {
		GPIO_write(PORT_0, PIN1, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
		
		if( previous_state != GPIO_read(PORT_1 , PIN0)){
			
			previous_state = GPIO_read(PORT_1 , PIN0);
			
			if(previous_state == 1){
				
				//send rising edge state to the uart task
					Btn_Msg.string = "Rising Edge Button 1\n";
					xQueueSend(xQueue1, (void*) &pBtn_Msg, 0);
			}
			else {
				//send falling edge state to the uart task
					Btn_Msg.string = "Falling Edge Button 1\n";
					xQueueSend(xQueue1, (void*) &pBtn_Msg, 0);
			}
		}
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 50) deadline_misses[0]++;
		GPIO_write(PORT_0, PIN1, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 50);
    }
}

/*-----------------------------------------------------------*/
void Button_2_Monitor( void * pvParameters )
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t start_time = 0, end_time = 0;
    uint8_t  previous_state = GPIO_read(PORT_1 , PIN1);
	message Btn_Msg;
	message *pBtn_Msg = &Btn_Msg;

    for( ;; )
    {
		GPIO_write(PORT_0, PIN2, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
		
		if( previous_state != GPIO_read(PORT_1 , PIN1)){
			
			previous_state = GPIO_read(PORT_1 , PIN1);
			
			if(previous_state == 1){
				//send rising edge state to the uart task
					Btn_Msg.string = "Rising Edge Button 2\n";
					xQueueSend(xQueue1, (void*) &pBtn_Msg, 0);
			}
			else {
				//send falling edge state to the uart task
					Btn_Msg.string = "Falling Edge Button 2\n";
					xQueueSend(xQueue1, (void*) &pBtn_Msg, 0);
			}
		}
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 50) deadline_misses[1]++;
		GPIO_write(PORT_0, PIN2, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 50);
    }
}

/*-----------------------------------------------------------*/
void Periodic_Transmitter( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
	message PrTrans_msg;
	message *pPrTrans_msg = &PrTrans_msg;
	PrTrans_msg.string = "Periodic MSG!\n";
		

    for( ;; )
    {
		GPIO_write(PORT_0, PIN3, PIN_IS_HIGH);		
		start_time = xTaskGetTickCount();

		xQueueSend(xQueue1, (void*) &pPrTrans_msg, 0);

		end_time = xTaskGetTickCount();
		if(end_time - start_time > 100) deadline_misses[2]++;		
		GPIO_write(PORT_0, PIN3, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 100);
    }
}
/*-----------------------------------------------------------*/
void Uart_Receiver( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
	
	message *received_msg;	

    for( ;; )
    {
		GPIO_write(PORT_0, PIN4, PIN_IS_HIGH);	
		start_time = xTaskGetTickCount();
		
		while(xQueueReceive( xQueue1,( void * )&received_msg,0 )== pdTRUE){
			vSerialPutString((const signed char *)received_msg->string, strlen( received_msg->string));
		}
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 20) deadline_misses[3]++;
		GPIO_write(PORT_0, PIN4, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 20);
    }
}
/*-----------------------------------------------------------*/
void Load_1_Simulation( void * pvParameters )
{
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
    uint32_t i = 0;

    for( ;; )
    {
		GPIO_write(PORT_0, PIN5, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
		
		while(i < 33220) i++;
		i = 0;
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 10) deadline_misses[4]++;
		GPIO_write(PORT_0, PIN5, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 10);
    }
}
/*-----------------------------------------------------------*/
void Load_2_Simulation( void * pvParameters )
{
    
	TickType_t xLastWakeTime = xTaskGetTickCount();	
	TickType_t start_time = 0, end_time = 0;
    uint32_t i = 0;


    for( ;; )
    {
		GPIO_write(PORT_0, PIN6, PIN_IS_HIGH);
		start_time = xTaskGetTickCount();
		
		while(i++ < 79740);
		i = 0;
		
		end_time = xTaskGetTickCount();
		if(end_time - start_time > 100) deadline_misses[5]++;
		GPIO_write(PORT_0, PIN6, PIN_IS_LOW);
		vTaskDelayUntil(&xLastWakeTime, 100);
    }
}





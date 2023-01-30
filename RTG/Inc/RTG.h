#ifndef INC_RTG_H_
#define INC_RTG_H_
#include "main.h"
#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>

//freeRTOS lib files

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "FreeRTOSConfig.h"
#include "cmsis_os.h"
#include "lwip.h"


#define QUEUE_ARRAY_LENGTH					12
#define NO_WAIT_VALUE						0
#define NUMBER_OF_INSTANCES_PER_DEPARTMENT	3
#define POLICE_VALUE						1
#define FIRE_VALUE							2
#define AMBULANCE_VALUE						3
#define MAINTENANCE_VALUE					4
#define NUMBER_OF_DEPARTMENTS				4
#define DEPARTMENT_SHIFT_VALUE				1


#define BAD_DEPARTMENT_VALUE				0
#define PRIORITY_MIN_VALUE					0
#define PRIORITY_MAX_VALUE					2

// Magic numbers:
#define TimeToWait                          0
#define qCritical                           1
#define Warning                             2
#define Notice                              3
#define MSG_SIZE                            150
#define QueueLength                         10
#define INIT_ZERO                           0
#define INIT_ONE                            1
#define TRUE                                1

#define POLICE_CRITICAL                     11
#define POLICE_WARNING                      12
#define POLICE_NOTICE                       13
#define FIRE_CRITICAL                       21
#define FIRE_WARNING                        22
#define FIRE_NOTICE                         23
#define AMBULANCE_CRITICAL                  31
#define AMBULANCE_WARNING                   32
#define AMBULANCE_NOTICE                    33
#define MAINTANANCE_CRITICAL                41
#define MAINTANANCE_WARNING                 42
#define MAINTANANCE_NOTICE                  43
#define MESSAGE_LENGTH                      150
#define MAX_CITY_LOG                        10
#define CONVERTION                          10
#define MAX_TASK_TO_CREATE                  3
#define Queue_Time_to_Wait                  10
#define mainDELAY_LOOP_COUNT                ( 0xffffff )
#define DISPATCHER_DELAY                    100
#define DELAY_WORKER                        300
extern UART_HandleTypeDef huart3;	//Change to match your UART number

#define UART_DEBUG &huart3				//Change to match your UART number
#define UART_1 &huart3

typedef struct Message{
	uint8_t Department;
	uint8_t Sevirity;
	char Address[80];
}msg_t;

void rtg_main();

//Functions declaration

//this function manage the "city" ,
//The function receives an event, transfers it to the handling department via a switch case,
//and uploads it to the queue message of that department
void Manager(void const * argument);

//This function expresses a call to the policeman,
//Checks the queue message of the police department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void PoliceWorker(void);

//This function expresses a call to the Fire fighters,
//Checks the queue message of the fire department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void FireWorker(void);

//This function expresses a call to the ambulance,
//Checks the queue message of the ambulance department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void AmbulanceWorker(void);

//This function expresses a call to the maintenance,
//Checks the queue message of the maintenance department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void MaintenanceWorker(void);

//This function checks the validations of the events
uint8_t isvalidevent(msg_t *buffer_dispatcher);

//This function is responsible for initializing LWIP and TFTP communication
void Communicationinit(void const * argument);


#endif /* INC_RTG_H_ */


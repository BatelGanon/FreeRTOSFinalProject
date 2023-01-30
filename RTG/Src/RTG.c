/*
 * RTG.c
 */
#include "RTG.h"
#include "queue.h"
#include <tftp.h>

#include "string.h"
#include "stdlib.h"

extern msg_t tftp_event_buffer;
extern uint8_t New_event_flag;

///GLOBAL VARIABLES: ///
uint16_t LogID = INIT_ONE;
uint8_t  tftp_payload_buffer[104];
uint8_t  LogCounter = INIT_ZERO;
uint8_t  flag_printer = INIT_ZERO;

struct tftp_context tftpd = {0};
char CityLog[1500];

QueueHandle_t queue_array[QUEUE_ARRAY_LENGTH];

QueueHandle_t tftp_event_queue;

QueueHandle_t PoliceCritical, PoliceWarning, PoliceNotice,
			  FireCritical, FireWarning, FireNotice,
			  AmbulanceCritical,AmbulceWarning ,AmbulanceNotice,
			  MaintenanceCritical, MaintenanceWarning, MaintenanceNotice ;

TaskHandle_t  Task1_handle = NULL;

SemaphoreHandle_t Sem;




void rtg_main()
{
	//Creates 12 Queue messages 4 for the departments, and 3 severity for any of them
	PoliceCritical = xQueueCreate(QueueLength, MSG_SIZE);
		if(PoliceCritical == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		PoliceWarning = xQueueCreate(QueueLength, MSG_SIZE);
		if(PoliceWarning == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		PoliceNotice = xQueueCreate(QueueLength, MSG_SIZE);
		if(PoliceNotice == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}

		FireCritical = xQueueCreate(QueueLength, MSG_SIZE);
		if(FireCritical == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		FireWarning = xQueueCreate(QueueLength, MSG_SIZE);
		if(FireWarning == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		FireNotice = xQueueCreate(QueueLength, MSG_SIZE);
		if(FireNotice == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}

		AmbulanceCritical = xQueueCreate(QueueLength, MSG_SIZE);
		if(AmbulanceCritical == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		AmbulceWarning = xQueueCreate(QueueLength, MSG_SIZE);
		if(AmbulceWarning == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		AmbulanceNotice = xQueueCreate(QueueLength, MSG_SIZE);
		if(AmbulanceNotice == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}

		MaintenanceCritical = xQueueCreate(QueueLength, MSG_SIZE);
		if(MaintenanceCritical == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		MaintenanceWarning = xQueueCreate(QueueLength, MSG_SIZE);
		if(MaintenanceWarning == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}
		MaintenanceNotice = xQueueCreate(QueueLength, MSG_SIZE);
		if(MaintenanceNotice == pdFALSE){
			printf("Error\r\n");
		exit(EXIT_FAILURE);
		}

		tftp_event_queue = xQueueCreate(1, sizeof(msg_t));

			BaseType_t taskStatus = pdPASS;
			//Creates task for the communication initialization,
			taskStatus = xTaskCreate((TaskFunction_t)Communicationinit, "TFTP 3", 1000, NULL, 3, NULL );
			if(taskStatus == pdFALSE){
			exit(EXIT_FAILURE);
			}
			//create task for the dispatcher with the highest priority
			taskStatus = xTaskCreate((TaskFunction_t)Manager, "manager", 1000, NULL, 2, NULL);
			if(taskStatus == pdFALSE){
			exit(EXIT_FAILURE);
			}
			//Creates 3 tasks for every department
			for (int var = 0; var < MAX_TASK_TO_CREATE; ++var) {
				taskStatus = xTaskCreate((TaskFunction_t)PoliceWorker, "Police1", 1000, NULL, 1, NULL);
				if(taskStatus == pdFALSE){
				exit(EXIT_FAILURE);
				}
				taskStatus = xTaskCreate((TaskFunction_t)AmbulanceWorker, "Ambulance1", 1000, NULL, 1, NULL);
				if(taskStatus == pdFALSE){
				exit(EXIT_FAILURE);
				}
				taskStatus = xTaskCreate((TaskFunction_t)FireWorker, "Fireman1", 1000, NULL, 1, NULL);
				if(taskStatus == pdFALSE){
				exit(EXIT_FAILURE);
				}
				taskStatus = xTaskCreate((TaskFunction_t)MaintenanceWorker, "Maintenance1", 1000, NULL, 1, NULL);
				if(taskStatus == pdFALSE){
				exit(EXIT_FAILURE);
				}

			}

	//creates binary semaphore
	Sem = xSemaphoreCreateBinary();
	xSemaphoreGive(Sem);
	printf("Start of program:\r\n");

	//start the program by the scheduler task
	vTaskStartScheduler();
	while(TRUE);
}

void tftpd_init(void)
{
	tftp_init(&tftpd);
}

//this function manage the "city" ,
//The function receives an event, transfers it to the handling department via a switch case,
//and uploads it to the queue message of that department
void Manager(void const * argument)
{

	BaseType_t xQueueSendStatus = pdFAIL;

    //get the asci values and calculate - 48 to get the values
	uint8_t department = tftp_event_buffer.Department -48;
	uint8_t sevirity  = tftp_event_buffer.Sevirity -48;

	//This buffer using to load the data by string and upload it the queue message
	char buff[MESSAGE_LENGTH];

	//Calculate the department *10 + severity so we can choose the specific case by one number
	//and we don't have to do case inside case
	uint8_t CaseChecker = department*(uint8_t)CONVERTION + sevirity;

	while(TRUE)
	{


		if(New_event_flag){

			///POLICE CASES:///
			switch(CaseChecker){

			case POLICE_CRITICAL:
				sprintf(buff,"Event ID: %d Department: Police, Priority: Critical, Address: %s",LogID,  tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(PoliceCritical,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case POLICE_WARNING:
				sprintf(buff,"Event ID: %d Department: Police, Priority: WARNING, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(PoliceWarning,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;

				break;
			case POLICE_NOTICE:
				sprintf(buff,"Event ID: %d Department: Police, Priority: NOTICE, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(PoliceNotice,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;

			///FIRE CASES: ///
			case FIRE_CRITICAL:
				sprintf(buff,"Event ID: %d Department: Fire, Priority: Critical, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(FireCritical,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case FIRE_WARNING:
				sprintf(buff,"Event ID: %d Department: Fire, Priority: WARNING, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(FireWarning,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case FIRE_NOTICE:
				sprintf(buff,"Event ID: %d, Department: Fire, Priority: NOTICE, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(FireNotice,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;

			//AMBULANCE CASES: ///
			case AMBULANCE_CRITICAL:
				sprintf(buff,"Event ID: %d ,Department: Ambulance, Priority: Critical, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(AmbulanceCritical,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case AMBULANCE_WARNING:
				sprintf(buff,"Event ID: %d ,Department: Ambulance, Priority: WARNING, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(AmbulceWarning,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case AMBULANCE_NOTICE:
				sprintf(buff,"Event ID: %d ,Department: Ambulance, Priority: NOTICE, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(AmbulanceNotice,buff,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;

				//MAINTENANCE CASES: ///
			case MAINTANANCE_CRITICAL:
				sprintf(buff,"Event ID: %d ,Department: Maintenance, Priority: NOTICE, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(MaintenanceCritical,tftp_event_buffer.Address,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case MAINTANANCE_WARNING:
				sprintf(buff,"Event ID: %d ,Department: Maintenance, Priority: NOTICE, Address: %s",LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(MaintenanceWarning,tftp_event_buffer.Address,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;
			case MAINTANANCE_NOTICE:
				sprintf(buff,"Event ID: %d ,Department: Maintenance, Priority: NOTICE, Address: %s", LogID, tftp_event_buffer.Address );
				xQueueSendStatus = xQueueSendToBack(MaintenanceNotice,tftp_event_buffer.Address,TimeToWait);
				if(xQueueSendStatus == pdFAIL)
					continue;
				LogID++;
				break;

		}
	New_event_flag = INIT_ZERO;
	}
	vTaskDelay(pdMS_TO_TICKS(DISPATCHER_DELAY));
	}
}

//This function expresses a call to the policeman,
//Checks the queue message of the police department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void PoliceWorker(void)
{

	BaseType_t xStatus = pdFALSE;
	TickType_t xTicksToWait = pdMS_TO_TICKS(Queue_Time_to_Wait);

	while(TRUE)
	{
		//check if there is an event in the queue message and then take a semaphore to using the UART
		//protocol so there us no case that tow events try to use in the same resource at the same time
		if(xQueueReceive(PoliceCritical,CityLog,TimeToWait) == pdTRUE)
		{
			xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
			if(xStatus == pdFAIL)
				continue;
			printf("%s\r\n",CityLog);
			flag_printer = INIT_ZERO;
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			HAL_Delay(DELAY_WORKER);
			memset(CityLog, INIT_ZERO, sizeof(CityLog));

		}
		else if(xQueueReceive(PoliceWarning,CityLog,TimeToWait) == pdTRUE)
		{
			xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
			if(xStatus == pdFAIL)
				continue;
			printf("%s\r\n",CityLog);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			HAL_Delay(DELAY_WORKER);
			memset(CityLog, INIT_ZERO, sizeof(CityLog));
		}
		else if(xQueueReceive(PoliceNotice,CityLog,TimeToWait) == pdTRUE)
		{
			xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
			if(xStatus == pdFAIL)
				continue;
			printf("%s\r\n",CityLog);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
			HAL_Delay(DELAY_WORKER);
			memset(CityLog, INIT_ZERO, sizeof(CityLog));
		}

		xSemaphoreGive(Sem);
		vTaskDelay(xTicksToWait);

	}
}

//This function expresses a call to the ambulance,
//Checks the queue message of the ambulance department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void AmbulanceWorker(void)
{

	BaseType_t xStatus = pdFALSE;
	TickType_t xTicksToWait = pdMS_TO_TICKS(Queue_Time_to_Wait);
	while(TRUE)
		{
		//check if there is an event in the queue message and then take a semaphore to using the UART
		//protocol so there us no case that tow events try to use in the same resource at the same time
			if(xQueueReceive(AmbulanceCritical,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}

			else if(xQueueReceive(AmbulceWarning,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}
			else if(xQueueReceive(AmbulanceNotice,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}

			vTaskDelay(xTicksToWait);
		}
}


//This function expresses a call to the Fire fighters,
//Checks the queue message of the fire department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void FireWorker(void)
{
	BaseType_t xStatus = pdFALSE;
	TickType_t xTicksToWait = pdMS_TO_TICKS(Queue_Time_to_Wait);

	while(TRUE)
		{
		//check if there is an event in the queue message and then take a semaphore to using the UART
		//protocol so there us no case that tow events try to use in the same resource at the same time
			if(xQueueReceive(FireCritical,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}
			else if(xQueueReceive(FireWarning,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}
			else if(xQueueReceive(FireNotice,CityLog,TimeToWait) == pdTRUE)
			{
				xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
				if(xStatus == pdFAIL)
					continue;
				printf("%s\r\n",CityLog);
				HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
				HAL_Delay(DELAY_WORKER);
				memset(CityLog, INIT_ZERO, sizeof(CityLog));
			}

			vTaskDelay(xTicksToWait);
		}
}

//This function expresses a call to the maintenance,
//Checks the queue message of the maintenance department in order of priority:
//first the critical queue, then the warning queue and then the notice queue
//if there is an event in the message queue, it pulls it out and prints it to the screen by UART protocol
void MaintenanceWorker(void)
{

	BaseType_t xStatus = pdFALSE;
	TickType_t xTicksToWait = pdMS_TO_TICKS(Queue_Time_to_Wait);

	while(TRUE)
		{
			    //check if there is an event in the queue message and then take a semaphore to using the UART
			    //protocol so there us no case that tow events try to use in the same resource at the same time
				if(xQueueReceive(MaintenanceCritical,CityLog,TimeToWait) == pdTRUE)
				{
					xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
					if(xStatus == pdFAIL)
						continue;
					printf("%s\r\n",CityLog);
					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
					HAL_Delay(DELAY_WORKER);
					memset(CityLog, INIT_ZERO, sizeof(CityLog));
				}

				else if(xQueueReceive(MaintenanceWarning,CityLog ,TimeToWait) == pdTRUE)
				{
					xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
					if(xStatus == pdFAIL)
						continue;
					printf("%s\r\n",CityLog);
					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
					HAL_Delay(DELAY_WORKER);
					memset(CityLog, INIT_ZERO, sizeof(CityLog));
				}
				else if(xQueueReceive(MaintenanceNotice,CityLog,TimeToWait) == pdTRUE)
				{
					xStatus = xSemaphoreTake(Sem, HAL_MAX_DELAY);
					if(xStatus == pdFAIL)
						continue;
					printf("%s\r\n",CityLog);
					HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
					HAL_Delay(DELAY_WORKER);
					memset(CityLog, INIT_ZERO, sizeof(CityLog));
				}

				vTaskDelay(xTicksToWait);
			}
}


//This function is responsible for initializing LWIP and TFTP communication
void Communicationinit(void const * argument)
{

	MX_LWIP_Init();
	tftpd_init();

	tftpd.open  = tftp_open;
	tftpd.write = &tftp_write;
	tftpd.read  = &tftp_read;
	tftpd.close = &tftp_close;
	while(TRUE)
		{
			if( xQueueReceive( tftp_event_queue, &( tftp_event_buffer ), ( TickType_t ) portMAX_DELAY ) != pdPASS )
			{
				exit(EXIT_FAILURE);
			}

		}
}


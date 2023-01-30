
#include <tftp.h>
#include <stdio.h>
#include "RTG.h"
#include <string.h>

#if LWIP_UDP

char event_str[MAX_PAYLOAD_SIZE];
msg_t tftp_event_buffer;
uint8_t New_event_flag = INIT_ZERO;
extern QueueHandle_t tftp_event_queue;

void* tftp_open_file(const char *fname, u8_t is_write)
{

	snprintf(event_str, sizeof(event_str), "%s", fname);
	event_str[sizeof(event_str) - 1] = 0;

	if (is_write) {
//    return (void*)fopen(event_str, "w");
		return event_str;
	}
	else
	{
		return (void*) fopen(event_str, "r");
	}
}

void* tftp_open(const char *fname, const char *mode, u8_t is_write)
{

	LWIP_UNUSED_ARG(mode);
	return tftp_open_file(fname, is_write);
}

void tftp_close(void *handle)
{

}

int tftp_read(void *handle, void *buf, int bytes)
{
	return 1;
}

int tftp_write(void *handle, struct pbuf *p)
{

	memcpy(event_str, p->payload, 82);

	msg_t buffer = { 0 };
	memcpy(&tftp_event_buffer, p->payload, sizeof(buffer));
	New_event_flag = TRUE;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE; // We have not woken a task at the start of the ISR.
	xQueueSendToBackFromISR(tftp_event_queue, &tftp_event_buffer,
			&xHigherPriorityTaskWoken);
	return 0;
}

#endif /* LWIP_UDP */


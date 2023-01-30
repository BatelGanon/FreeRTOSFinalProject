#ifndef INC_TFTP_H_
#define INC_TFTP_H_

#include "RTG.h"


#include "tftp_server.h"

#define MAX_PAYLOAD_SIZE 516

void * tftp_open_file(const char* fname, u8_t is_write);
void* tftp_open(const char* fname, const char* mode, u8_t is_write);
void tftp_close(void* handle);
int tftp_read(void* handle, void* buf, int bytes);
int tftp_write(void* handle, struct pbuf* p);

#endif /* INC_TFTP_H_ */

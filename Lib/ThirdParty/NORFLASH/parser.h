#ifndef PARSER_H
#define PARSER_H

#include "main.h"
#include "stdint.h"



#ifdef __cplusplus
extern "C" {
#endif

void parser_feed(uint8_t byte);
void process_queue(void);
void crc32_init(void);
void data_from_usb(uint8_t data);

#ifdef __cplusplus
}
#endif

#endif
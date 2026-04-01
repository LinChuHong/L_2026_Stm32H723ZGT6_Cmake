#ifndef PACKET_QUEUE_H
#define PACKET_QUEUE_H

#include <stdint.h>

#define QUEUE_SIZE 8
#define MAX_DATA_LEN 256

typedef struct {
    uint32_t addr;
    uint16_t len;
    uint8_t data[MAX_DATA_LEN];
} Packet;
#ifdef __cplusplus
extern "C" {
#endif

int queue_push(uint32_t addr, uint8_t *data, uint16_t len);
int queue_pop(Packet *pkt);

#ifdef __cplusplus
}
#endif
#endif
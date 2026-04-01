#include "packet_queue.h"
#include <string.h>

static Packet queue[QUEUE_SIZE];

static volatile uint8_t head = 0;
static volatile uint8_t tail = 0;

static int is_full(void)
{
    return ((head + 1) % QUEUE_SIZE) == tail;
}

static int is_empty(void)
{
    return head == tail;
}

int queue_push(uint32_t addr, uint8_t *data, uint16_t len)
{
    if (is_full())
        return 0;

    Packet *pkt = &queue[head];

    pkt->addr = addr;
    pkt->len = len;
    memcpy(pkt->data, data, len);

    head = (head + 1) % QUEUE_SIZE;

    return 1;
}

int queue_pop(Packet *pkt)
{
    if (is_empty())
        return 0;

    *pkt = queue[tail];

    tail = (tail + 1) % QUEUE_SIZE;

    return 1;
}
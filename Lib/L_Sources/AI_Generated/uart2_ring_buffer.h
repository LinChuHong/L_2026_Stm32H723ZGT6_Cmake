#ifndef __UART2_RING_BUFFER_H__
#define __UART2_RING_BUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "usart.h"
#include <stdint.h>

#define UART2_DMA_RX_BUFFER_SIZE         2048U
#define UART2_RING_BUFFER_SIZE           8192U
#define UART2_READLINE_OVERFLOW          0xFFFFU

HAL_StatusTypeDef UART2_RingBuffer_Init(void);
void UART2_RingBuffer_DeInit(void);
void UART2_RingBuffer_Service(void);

uint16_t UART2_RingBuffer_Available(void);
uint16_t UART2_RingBuffer_Read(uint8_t *data, uint16_t length);
int16_t UART2_RingBuffer_GetByte(void);
uint16_t UART2_RingBuffer_ReadLine(uint8_t *data, uint16_t max_length);

HAL_StatusTypeDef UART2_RingBuffer_Write(const uint8_t *data, uint16_t length);

uint32_t UART2_RingBuffer_GetDroppedBytes(void);
uint32_t UART2_RingBuffer_GetResetCount(void);
void UART2_RingBuffer_Clear(void);

#ifdef __cplusplus
}
#endif

#endif /* __UART2_RING_BUFFER_H__ */

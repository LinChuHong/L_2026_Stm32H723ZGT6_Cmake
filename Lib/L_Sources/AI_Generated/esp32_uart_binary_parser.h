#ifndef __ESP32_UART_BINARY_PARSER_H__
#define __ESP32_UART_BINARY_PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stddef.h>
#include <stdint.h>

#define ESP32_UART_BINARY_SYNC1                 0xAAU
#define ESP32_UART_BINARY_SYNC2                 0x55U
#define ESP32_UART_BINARY_MAX_PAYLOAD_SIZE      4096U

#define ESP32_UART_BINARY_ACK_OK                0x06U
#define ESP32_UART_BINARY_ACK_CRC_ERROR         0x15U
#define ESP32_UART_BINARY_ACK_FRAME_ERROR       0x16U
#define ESP32_UART_BINARY_ACK_HANDLER_ERROR     0x17U

/*
 * UART2 frame format:
 *   0xAA 0x55
 *   uint32_t payload_size_le
 *   payload bytes
 *   uint32_t payload_crc32_le
 *
 * CRC32 is calculated by the STM32 CRC peripheral with the current project
 * settings from crc.c. The sender should wait for one ACK/NACK byte before
 * sending the next frame.
 */

void ESP32_UartBinaryParser_Init(void);
void ESP32_UartBinaryParser_Reset(void);
void ESP32_UartBinaryParser_Process(void);

uint8_t ESP32_UartBinaryParser_HasPacket(void);
const uint8_t *ESP32_UartBinaryParser_GetPacketData(void);
uint32_t ESP32_UartBinaryParser_GetPacketSize(void);
uint32_t ESP32_UartBinaryParser_GetPacketCrc(void);
HAL_StatusTypeDef ESP32_UartBinaryParser_FinishPacket(uint8_t ack_code);

uint32_t ESP32_UartBinaryParser_GetDiscardedFrames(void);

#ifdef __cplusplus
}
#endif

#endif /* __ESP32_UART_BINARY_PARSER_H__ */

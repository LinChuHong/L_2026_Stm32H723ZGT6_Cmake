#include "esp32_uart_binary_parser.h"

#include "crc.h"
#include "uart2_ring_buffer.h"

typedef enum
{
  ESP32_UART_BINARY_STATE_WAIT_SYNC1 = 0,
  ESP32_UART_BINARY_STATE_WAIT_SYNC2,
  ESP32_UART_BINARY_STATE_READ_SIZE,
  ESP32_UART_BINARY_STATE_READ_PAYLOAD,
  ESP32_UART_BINARY_STATE_READ_CRC
} ESP32_UartBinaryParser_State;

static ESP32_UartBinaryParser_State esp32_uart_binary_state = ESP32_UART_BINARY_STATE_WAIT_SYNC1;
static uint8_t esp32_uart_binary_size_bytes[4];
static uint8_t esp32_uart_binary_crc_bytes[4];
static uint8_t esp32_uart_binary_payload[ESP32_UART_BINARY_MAX_PAYLOAD_SIZE];
static uint32_t esp32_uart_binary_expected_size = 0U;
static uint32_t esp32_uart_binary_payload_index = 0U;
static uint32_t esp32_uart_binary_size_index = 0U;
static uint32_t esp32_uart_binary_crc_index = 0U;
static uint32_t esp32_uart_binary_packet_size = 0U;
static uint32_t esp32_uart_binary_packet_crc = 0U;
static uint32_t esp32_uart_binary_discarded_frames = 0U;
static uint32_t esp32_uart_binary_last_reset_count = 0U;
static uint8_t esp32_uart_binary_packet_ready = 0U;

static void ESP32_UartBinaryParser_ResetFrameState(void)
{
  esp32_uart_binary_state = ESP32_UART_BINARY_STATE_WAIT_SYNC1;
  esp32_uart_binary_expected_size = 0U;
  esp32_uart_binary_payload_index = 0U;
  esp32_uart_binary_size_index = 0U;
  esp32_uart_binary_crc_index = 0U;
}

static uint32_t ESP32_UartBinaryParser_DecodeUint32LE(const uint8_t *bytes)
{
  return ((uint32_t)bytes[0]) |
         ((uint32_t)bytes[1] << 8) |
         ((uint32_t)bytes[2] << 16) |
         ((uint32_t)bytes[3] << 24);
}

static HAL_StatusTypeDef ESP32_UartBinaryParser_SendAck(uint8_t ack_code)
{
  return UART2_RingBuffer_Write(&ack_code, 1U);
}

static uint32_t ESP32_UartBinaryParser_CalculateCrc(const uint8_t *data, uint32_t size)
{
  if (size == 0U)
  {
    return 0U;
  }

  return HAL_CRC_Calculate(&hcrc, (uint32_t *)(void *)data, size);
}

static void ESP32_UartBinaryParser_ResetForRingRecovery(void)
{
  esp32_uart_binary_last_reset_count = UART2_RingBuffer_GetResetCount();
  esp32_uart_binary_packet_ready = 0U;
  esp32_uart_binary_packet_size = 0U;
  esp32_uart_binary_packet_crc = 0U;
  ESP32_UartBinaryParser_ResetFrameState();
}

static void ESP32_UartBinaryParser_HandleCompletedFrame(void)
{
  uint32_t received_crc;
  uint32_t calculated_crc;

  received_crc = ESP32_UartBinaryParser_DecodeUint32LE(esp32_uart_binary_crc_bytes);
  calculated_crc = ESP32_UartBinaryParser_CalculateCrc(esp32_uart_binary_payload,
                                                       esp32_uart_binary_expected_size);

  if (received_crc != calculated_crc)
  {
    esp32_uart_binary_discarded_frames++;
    (void)ESP32_UartBinaryParser_SendAck(ESP32_UART_BINARY_ACK_CRC_ERROR);
    ESP32_UartBinaryParser_ResetFrameState();
    return;
  }

  esp32_uart_binary_packet_size = esp32_uart_binary_expected_size;
  esp32_uart_binary_packet_crc = calculated_crc;
  esp32_uart_binary_packet_ready = 1U;
  ESP32_UartBinaryParser_ResetFrameState();
}

static void ESP32_UartBinaryParser_FeedByte(uint8_t byte)
{
  switch (esp32_uart_binary_state)
  {
    case ESP32_UART_BINARY_STATE_WAIT_SYNC1:
      if (byte == ESP32_UART_BINARY_SYNC1)
      {
        esp32_uart_binary_state = ESP32_UART_BINARY_STATE_WAIT_SYNC2;
      }
      break;

    case ESP32_UART_BINARY_STATE_WAIT_SYNC2:
      if (byte == ESP32_UART_BINARY_SYNC2)
      {
        esp32_uart_binary_size_index = 0U;
        esp32_uart_binary_state = ESP32_UART_BINARY_STATE_READ_SIZE;
      }
      else if (byte != ESP32_UART_BINARY_SYNC1)
      {
        esp32_uart_binary_state = ESP32_UART_BINARY_STATE_WAIT_SYNC1;
      }
      break;

    case ESP32_UART_BINARY_STATE_READ_SIZE:
      esp32_uart_binary_size_bytes[esp32_uart_binary_size_index++] = byte;

      if (esp32_uart_binary_size_index >= 4U)
      {
        esp32_uart_binary_expected_size =
            ESP32_UartBinaryParser_DecodeUint32LE(esp32_uart_binary_size_bytes);

        if (esp32_uart_binary_expected_size > ESP32_UART_BINARY_MAX_PAYLOAD_SIZE)
        {
          esp32_uart_binary_discarded_frames++;
          (void)ESP32_UartBinaryParser_SendAck(ESP32_UART_BINARY_ACK_FRAME_ERROR);
          ESP32_UartBinaryParser_ResetFrameState();
          break;
        }

        esp32_uart_binary_payload_index = 0U;
        esp32_uart_binary_crc_index = 0U;

        if (esp32_uart_binary_expected_size == 0U)
        {
          esp32_uart_binary_state = ESP32_UART_BINARY_STATE_READ_CRC;
        }
        else
        {
          esp32_uart_binary_state = ESP32_UART_BINARY_STATE_READ_PAYLOAD;
        }
      }
      break;

    case ESP32_UART_BINARY_STATE_READ_PAYLOAD:
      esp32_uart_binary_payload[esp32_uart_binary_payload_index++] = byte;

      if (esp32_uart_binary_payload_index >= esp32_uart_binary_expected_size)
      {
        esp32_uart_binary_crc_index = 0U;
        esp32_uart_binary_state = ESP32_UART_BINARY_STATE_READ_CRC;
      }
      break;

    case ESP32_UART_BINARY_STATE_READ_CRC:
      esp32_uart_binary_crc_bytes[esp32_uart_binary_crc_index++] = byte;

      if (esp32_uart_binary_crc_index >= 4U)
      {
        ESP32_UartBinaryParser_HandleCompletedFrame();
      }
      break;

    default:
      ESP32_UartBinaryParser_ResetFrameState();
      break;
  }
}

void ESP32_UartBinaryParser_Init(void)
{
  esp32_uart_binary_discarded_frames = 0U;
  ESP32_UartBinaryParser_ResetForRingRecovery();
}

void ESP32_UartBinaryParser_Reset(void)
{
  ESP32_UartBinaryParser_ResetForRingRecovery();
}

void ESP32_UartBinaryParser_Process(void)
{
  int16_t byte;
  uint32_t reset_count;

  reset_count = UART2_RingBuffer_GetResetCount();
  if (reset_count != esp32_uart_binary_last_reset_count)
  {
    ESP32_UartBinaryParser_ResetForRingRecovery();
  }

  if (esp32_uart_binary_packet_ready != 0U)
  {
    return;
  }

  while (esp32_uart_binary_packet_ready == 0U)
  {
    byte = UART2_RingBuffer_GetByte();
    if (byte < 0)
    {
      break;
    }

    ESP32_UartBinaryParser_FeedByte((uint8_t)byte);
  }
}

uint8_t ESP32_UartBinaryParser_HasPacket(void)
{
  return esp32_uart_binary_packet_ready;
}

const uint8_t *ESP32_UartBinaryParser_GetPacketData(void)
{
  if (esp32_uart_binary_packet_ready == 0U)
  {
    return NULL;
  }

  return esp32_uart_binary_payload;
}

uint32_t ESP32_UartBinaryParser_GetPacketSize(void)
{
  if (esp32_uart_binary_packet_ready == 0U)
  {
    return 0U;
  }

  return esp32_uart_binary_packet_size;
}

uint32_t ESP32_UartBinaryParser_GetPacketCrc(void)
{
  if (esp32_uart_binary_packet_ready == 0U)
  {
    return 0U;
  }

  return esp32_uart_binary_packet_crc;
}

HAL_StatusTypeDef ESP32_UartBinaryParser_FinishPacket(uint8_t ack_code)
{
  HAL_StatusTypeDef status;

  if (esp32_uart_binary_packet_ready == 0U)
  {
    return HAL_ERROR;
  }

  status = ESP32_UartBinaryParser_SendAck(ack_code);

  esp32_uart_binary_packet_ready = 0U;
  esp32_uart_binary_packet_size = 0U;
  esp32_uart_binary_packet_crc = 0U;

  return status;
}

uint32_t ESP32_UartBinaryParser_GetDiscardedFrames(void)
{
  return esp32_uart_binary_discarded_frames;
}

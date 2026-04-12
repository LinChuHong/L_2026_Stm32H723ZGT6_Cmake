#include "uart2_ring_buffer.h"

static uint8_t uart2_ring_buffer[UART2_RING_BUFFER_SIZE];
static volatile uint16_t uart2_ring_head = 0U;
static volatile uint16_t uart2_ring_tail = 0U;
static volatile uint16_t uart2_dma_last_pos = 0U;
static volatile uint32_t uart2_dropped_bytes = 0U;
static volatile uint32_t uart2_reset_count = 0U;
static volatile uint8_t uart2_initialized = 0U;
static volatile uint8_t uart2_restart_pending = 0U;

static uint8_t uart2_dma_rx_buffer[UART2_DMA_RX_BUFFER_SIZE]
    __attribute__((section(".RAM_D2_Section")));

static void UART2_RingBuffer_PushByte(uint8_t byte)
{
  uint16_t next_head = (uint16_t)((uart2_ring_head + 1U) % UART2_RING_BUFFER_SIZE);

  if (next_head == uart2_ring_tail)
  {
    uart2_ring_tail = (uint16_t)((uart2_ring_tail + 1U) % UART2_RING_BUFFER_SIZE);
    uart2_dropped_bytes++;
  }

  uart2_ring_buffer[uart2_ring_head] = byte;
  uart2_ring_head = next_head;
}

static void UART2_RingBuffer_CopyFromDma(uint16_t dma_pos)
{
  uint16_t index = uart2_dma_last_pos;

  if (dma_pos > UART2_DMA_RX_BUFFER_SIZE)
  {
    dma_pos = UART2_DMA_RX_BUFFER_SIZE;
  }

  while (index != dma_pos)
  {
    UART2_RingBuffer_PushByte(uart2_dma_rx_buffer[index]);
    index++;

    if (index >= UART2_DMA_RX_BUFFER_SIZE)
    {
      index = 0U;
    }
  }

  uart2_dma_last_pos = dma_pos;
}

static uint16_t UART2_RingBuffer_GetDmaPosition(void)
{
  uint16_t dma_pos = 0U;

  if ((huart2.hdmarx != NULL) && (uart2_initialized != 0U))
  {
    dma_pos = (uint16_t)(UART2_DMA_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx));

    if (dma_pos > UART2_DMA_RX_BUFFER_SIZE)
    {
      dma_pos = 0U;
    }
  }

  return dma_pos;
}

static void UART2_RingBuffer_StartRx(void)
{
  uart2_restart_pending = 1U;
  huart2.ErrorCode = HAL_UART_ERROR_NONE;

  __HAL_UART_CLEAR_FLAG(&huart2,
                        UART_CLEAR_PEF | UART_CLEAR_FEF |
                        UART_CLEAR_NEF | UART_CLEAR_OREF |
                        UART_CLEAR_IDLEF);
  __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);

  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart2, uart2_dma_rx_buffer, UART2_DMA_RX_BUFFER_SIZE) == HAL_OK)
  {
    if (huart2.hdmarx != NULL)
    {
      __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }

    uart2_restart_pending = 0U;
  }
}

static void UART2_RingBuffer_RecoverRx(void)
{
  (void)HAL_UART_AbortReceive(&huart2);

  huart2.ErrorCode = HAL_UART_ERROR_NONE;

  __HAL_UART_CLEAR_FLAG(&huart2,
                        UART_CLEAR_PEF | UART_CLEAR_FEF |
                        UART_CLEAR_NEF | UART_CLEAR_OREF |
                        UART_CLEAR_IDLEF);
  __HAL_UART_SEND_REQ(&huart2, UART_RXDATA_FLUSH_REQUEST);

  uart2_ring_head = 0U;
  uart2_ring_tail = 0U;
  uart2_dma_last_pos = 0U;
  uart2_reset_count++;

  UART2_RingBuffer_StartRx();
}

static uint16_t UART2_RingBuffer_PeekLineLength(uint8_t *has_newline)
{
  uint16_t tail = uart2_ring_tail;
  uint16_t length = 0U;

  *has_newline = 0U;

  while (tail != uart2_ring_head)
  {
    uint8_t byte = uart2_ring_buffer[tail];

    if (byte == '\n')
    {
      *has_newline = 1U;
      break;
    }

    length++;
    tail = (uint16_t)((tail + 1U) % UART2_RING_BUFFER_SIZE);
  }

  return length;
}

static void UART2_RingBuffer_DiscardUntilNewline(void)
{
  while (uart2_ring_tail != uart2_ring_head)
  {
    uint8_t byte = uart2_ring_buffer[uart2_ring_tail];

    uart2_ring_tail = (uint16_t)((uart2_ring_tail + 1U) % UART2_RING_BUFFER_SIZE);

    if (byte == '\n')
    {
      break;
    }
  }
}

HAL_StatusTypeDef UART2_RingBuffer_Init(void)
{
  uart2_ring_head = 0U;
  uart2_ring_tail = 0U;
  uart2_dma_last_pos = 0U;
  uart2_dropped_bytes = 0U;
  uart2_reset_count = 0U;
  uart2_initialized = 0U;
  uart2_restart_pending = 0U;

  UART2_RingBuffer_StartRx();

  if (uart2_restart_pending != 0U)
  {
    return HAL_ERROR;
  }

  uart2_initialized = 1U;
  return HAL_OK;
}

void UART2_RingBuffer_DeInit(void)
{
  (void)HAL_UART_DMAStop(&huart2);
  uart2_initialized = 0U;
}

void UART2_RingBuffer_Service(void)
{
  if (uart2_initialized == 0U)
  {
    return;
  }

  if (uart2_restart_pending != 0U)
  {
    UART2_RingBuffer_RecoverRx();
  }

  if (uart2_restart_pending != 0U)
  {
    return;
  }

  UART2_RingBuffer_CopyFromDma(UART2_RingBuffer_GetDmaPosition());
}

uint16_t UART2_RingBuffer_Available(void)
{
  uint16_t head;
  uint16_t tail;

  UART2_RingBuffer_Service();

  head = uart2_ring_head;
  tail = uart2_ring_tail;

  if (head >= tail)
  {
    return (uint16_t)(head - tail);
  }

  return (uint16_t)(UART2_RING_BUFFER_SIZE - tail + head);
}

uint16_t UART2_RingBuffer_Read(uint8_t *data, uint16_t length)
{
  uint16_t count = 0U;

  if (data == NULL)
  {
    return 0U;
  }

  UART2_RingBuffer_Service();

  while ((count < length) && (uart2_ring_tail != uart2_ring_head))
  {
    data[count] = uart2_ring_buffer[uart2_ring_tail];
    uart2_ring_tail = (uint16_t)((uart2_ring_tail + 1U) % UART2_RING_BUFFER_SIZE);
    count++;
  }

  return count;
}

int16_t UART2_RingBuffer_GetByte(void)
{
  uint8_t byte;

  if (UART2_RingBuffer_Read(&byte, 1U) == 0U)
  {
    return -1;
  }

  return (int16_t)byte;
}

uint16_t UART2_RingBuffer_ReadLine(uint8_t *data, uint16_t max_length)
{
  uint16_t raw_length;
  uint16_t copy_length;
  uint8_t has_newline;

  if ((data == NULL) || (max_length < 2U))
  {
    return 0U;
  }

  UART2_RingBuffer_Service();

  raw_length = UART2_RingBuffer_PeekLineLength(&has_newline);

  if (has_newline == 0U)
  {
    return 0U;
  }

  if (raw_length >= max_length)
  {
    UART2_RingBuffer_DiscardUntilNewline();
    data[0] = '\0';
    return UART2_READLINE_OVERFLOW;
  }

  copy_length = raw_length;
  (void)UART2_RingBuffer_Read(data, raw_length);

  if ((copy_length > 0U) && (data[copy_length - 1U] == '\r'))
  {
    copy_length--;
  }

  data[copy_length] = '\0';

  {
    uint8_t newline;
    (void)UART2_RingBuffer_Read(&newline, 1U);
  }

  return copy_length;
}

HAL_StatusTypeDef UART2_RingBuffer_Write(const uint8_t *data, uint16_t length)
{
  return HAL_UART_Transmit(&huart2, (uint8_t *)data, length, HAL_MAX_DELAY);
}

uint32_t UART2_RingBuffer_GetDroppedBytes(void)
{
  return uart2_dropped_bytes;
}

uint32_t UART2_RingBuffer_GetResetCount(void)
{
  return uart2_reset_count;
}

void UART2_RingBuffer_Clear(void)
{
  uint16_t dma_pos = 0U;

  if ((uart2_initialized != 0U) && (huart2.hdmarx != NULL))
  {
    dma_pos = (uint16_t)(UART2_DMA_RX_BUFFER_SIZE - __HAL_DMA_GET_COUNTER(huart2.hdmarx));

    if (dma_pos > UART2_DMA_RX_BUFFER_SIZE)
    {
      dma_pos = 0U;
    }
  }

  uart2_ring_head = 0U;
  uart2_ring_tail = 0U;
  uart2_dma_last_pos = dma_pos;
  uart2_dropped_bytes = 0U;
  uart2_restart_pending = 0U;
  uart2_reset_count++;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  (void)Size;

  if ((huart == &huart2) && (uart2_initialized != 0U))
  {
    /* Keep ISR work minimal so UART traffic can't starve the scheduler. */
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if ((huart == &huart2) && (uart2_initialized != 0U))
  {
    uart2_restart_pending = 1U;
  }
}

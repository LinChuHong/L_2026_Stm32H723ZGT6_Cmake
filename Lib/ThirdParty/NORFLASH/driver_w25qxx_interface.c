/**
 * @file    driver_w25qxx_interface.c
 * @brief   W25Qxx SPI interface implementation for STM32H7 SPI1
 */

#include "driver_w25qxx_interface.h"
#include "gpio.h"
#include "spi.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "malloc1.h"
/* -------------------------------------------------------------------------- */
/*                            Hardware Configuration                           */
/* -------------------------------------------------------------------------- */



#define W25QXX_SPI_HANDLE           hspi1

#define W25QXX_CS_PORT              SPI1_Chip_Select_GPIO_Port
#define W25QXX_CS_PIN               SPI1_Chip_Select_Pin

#define W25QXX_CS_LOW()             HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET)
#define W25QXX_CS_HIGH()            HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET)

#define W25QXX_DMA_CHUNK_SIZE       4096U
#define W25QXX_SPI_TIMEOUT_MS       1000U
#define W25QXX_PREFIX_INVALID       0xFFFFU


// __attribute__((section(".RAM_D2_Section"),aligned(32)))
static uint8_t gs_w25qxx_spi_tx_buffer[W25QXX_DMA_CHUNK_SIZE];
// __attribute__((section(".RAM_D2_Section"),aligned(32)))
static uint8_t gs_w25qxx_spi_rx_buffer[W25QXX_DMA_CHUNK_SIZE];

/* -------------------------------------------------------------------------- */
/*                              Private Functions                              */
/* -------------------------------------------------------------------------- */

static void w25qxx_cs_select(void)
{
    W25QXX_CS_LOW();
}

static void w25qxx_cs_deselect(void)
{
    W25QXX_CS_HIGH();
}

static void w25qxx_enable_dwt_cycle_counter(void)
{
    if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U)
    {
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0U;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    }
}

static uint8_t w25qxx_spi_transmit(const uint8_t *buffer, uint32_t length)
{
    uint16_t chunk;

    while (length > 0U)
    {
        chunk = (uint16_t)((length > W25QXX_DMA_CHUNK_SIZE) ? W25QXX_DMA_CHUNK_SIZE : length);
        if (HAL_SPI_Transmit(&W25QXX_SPI_HANDLE, (uint8_t *)buffer, chunk, W25QXX_SPI_TIMEOUT_MS) != HAL_OK)
        {
            return 1;
        }

        buffer += chunk;
        length -= chunk;
    }

    return 0;
}

static uint8_t w25qxx_spi_receive(uint8_t *buffer, uint32_t length)
{
    uint16_t chunk;

    while (length > 0U)
    {
        chunk = (uint16_t)((length > W25QXX_DMA_CHUNK_SIZE) ? W25QXX_DMA_CHUNK_SIZE : length);
        memset(gs_w25qxx_spi_tx_buffer, 0xFF, chunk);
        if (HAL_SPI_TransmitReceive(&W25QXX_SPI_HANDLE,
                                    gs_w25qxx_spi_tx_buffer,
                                    gs_w25qxx_spi_rx_buffer,
                                    chunk,
                                    W25QXX_SPI_TIMEOUT_MS) != HAL_OK)
        {
            return 1;
        }

        memcpy(buffer, gs_w25qxx_spi_rx_buffer, chunk);
        buffer += chunk;
        length -= chunk;
    }

    return 0;
}

static uint8_t w25qxx_spi_transmit_then_receive(const uint8_t *tx_buffer,
                                                uint32_t tx_length,
                                                uint8_t *rx_buffer,
                                                uint32_t rx_length)
{
    if ((tx_length > 0U) && (tx_buffer == NULL))
    {
        return 1;
    }
    if ((rx_length > 0U) && (rx_buffer == NULL))
    {
        return 1;
    }

    if ((tx_length > 0U) && (w25qxx_spi_transmit(tx_buffer, tx_length) != 0U))
    {
        return 1;
    }
    if ((rx_length > 0U) && (w25qxx_spi_receive(rx_buffer, rx_length) != 0U))
    {
        return 1;
    }

    return 0;
}


static uint16_t w25qxx_build_prefix(uint8_t instruction,
                                    uint8_t instruction_line,
                                    uint32_t address,
                                    uint8_t address_line,
                                    uint8_t address_len,
                                    uint32_t alternate,
                                    uint8_t alternate_line,
                                    uint8_t alternate_len,
                                    uint8_t dummy)
{
    uint16_t dummy_bytes;
    uint16_t offset;
    uint8_t i;

    offset = 0U;

    if (instruction_line > 1U)
    {
        return W25QXX_PREFIX_INVALID;
    }
    if ((instruction_line == 1U) && (offset < W25QXX_DMA_CHUNK_SIZE))
    {
        gs_w25qxx_spi_tx_buffer[offset++] = instruction;
    }

    if (address_line > 1U)
    {
        return W25QXX_PREFIX_INVALID;
    }
    if ((address_line == 0U) && (address_len != 0U))
    {
        return W25QXX_PREFIX_INVALID;
    }
    if (address_len > 4U)
    {
        return W25QXX_PREFIX_INVALID;
    }
    for (i = 0U; i < address_len; i++)
    {
        if (offset >= W25QXX_DMA_CHUNK_SIZE)
        {
            return W25QXX_PREFIX_INVALID;
        }
        gs_w25qxx_spi_tx_buffer[offset++] = (uint8_t)(address >> (8U * (address_len - 1U - i)));
    }

    if (alternate_line > 1U)
    {
        return W25QXX_PREFIX_INVALID;
    }
    if ((alternate_line == 0U) && (alternate_len != 0U))
    {
        return W25QXX_PREFIX_INVALID;
    }
    if (alternate_len > 4U)
    {
        return W25QXX_PREFIX_INVALID;
    }
    for (i = 0U; i < alternate_len; i++)
    {
        if (offset >= W25QXX_DMA_CHUNK_SIZE)
        {
            return W25QXX_PREFIX_INVALID;
        }
        gs_w25qxx_spi_tx_buffer[offset++] = (uint8_t)(alternate >> (8U * (alternate_len - 1U - i)));
    }

    /* The LibDriver interface provides dummy in clock cycles. For single-lane
     * SPI transfers, every 8 cycles is one transmitted dummy byte. */
    dummy_bytes = (uint16_t)((dummy + 7U) / 8U);

    if ((uint32_t)offset + dummy_bytes > W25QXX_DMA_CHUNK_SIZE)
    {
        return W25QXX_PREFIX_INVALID;
    }
    if (dummy_bytes > 0U)
    {
        memset(&gs_w25qxx_spi_tx_buffer[offset], 0xFF, dummy_bytes);
        offset = (uint16_t)(offset + dummy_bytes);
    }

    return offset;
}

/* -------------------------------------------------------------------------- */
/*                              Interface Functions                            */
/* -------------------------------------------------------------------------- */

uint8_t w25qxx_interface_spi_qspi_init(void)
{
    w25qxx_enable_dwt_cycle_counter();
    w25qxx_cs_deselect();

    return 0;
}

uint8_t w25qxx_interface_spi_qspi_deinit(void)
{
    w25qxx_cs_deselect();

    return 0;
}

uint8_t w25qxx_interface_spi_qspi_write_read(uint8_t instruction,
                                             uint8_t instruction_line,
                                             uint32_t address,
                                             uint8_t address_line,
                                             uint8_t address_len,
                                             uint32_t alternate,
                                             uint8_t alternate_line,
                                             uint8_t alternate_len,
                                             uint8_t dummy,
                                             uint8_t *in_buf,
                                             uint32_t in_len,
                                             uint8_t *out_buf,
                                             uint32_t out_len,
                                             uint8_t data_line)
{
    uint16_t prefix_length;
    uint32_t combined_length;

    if (((in_len > 0U) && (in_buf == NULL)) || ((out_len > 0U) && (out_buf == NULL)))
    {
        return 1;
    }

    if (((in_len > 0U) || (out_len > 0U)) && (data_line != 1U))
    {
        return 1;
    }
    if (data_line > 1U)
    {
        return 1;
    }

    if ((instruction_line == 0U) && (address_line == 0U) && (alternate_line == 0U) && (dummy == 0U))
    {
        w25qxx_cs_select();

        if ((out_len > 0U) && (w25qxx_spi_transmit_then_receive(in_buf, in_len, out_buf, out_len) != 0U))
        {
            goto error;
        }

        if ((out_len == 0U) && (in_len > 0U) && (w25qxx_spi_transmit(in_buf, in_len) != 0U))
        {
            goto error;
        }

        w25qxx_cs_deselect();

        return 0;
    }

    prefix_length = w25qxx_build_prefix(instruction,
                                        instruction_line,
                                        address,
                                        address_line,
                                        address_len,
                                        alternate,
                                        alternate_line,
                                        alternate_len,
                                        dummy);
    if (prefix_length == W25QXX_PREFIX_INVALID)
    {
        return 1;
    }

    combined_length = (uint32_t)prefix_length + in_len;
    if ((out_len > 0U) && (combined_length <= W25QXX_DMA_CHUNK_SIZE))
    {
        if (in_len > 0U)
        {
            memcpy(&gs_w25qxx_spi_tx_buffer[prefix_length], in_buf, in_len);
        }

        w25qxx_cs_select();

        if (w25qxx_spi_transmit_then_receive(gs_w25qxx_spi_tx_buffer, combined_length, out_buf, out_len) != 0U)
        {
            goto error;
        }

        w25qxx_cs_deselect();

        return 0;
    }

    w25qxx_cs_select();

    if ((prefix_length > 0U) && (w25qxx_spi_transmit(gs_w25qxx_spi_tx_buffer, prefix_length) != 0U))
    {
        goto error;
    }

    if ((in_len > 0U) && (w25qxx_spi_transmit(in_buf, in_len) != 0U))
    {
        goto error;
    }

    if ((out_len > 0U) && (w25qxx_spi_receive(out_buf, out_len) != 0U))
    {
        goto error;
    }

    w25qxx_cs_deselect();

    return 0;

error:
    w25qxx_cs_deselect();

    return 1;
}

void w25qxx_interface_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void w25qxx_interface_delay_us(uint32_t us)
{
    uint32_t start;
    uint32_t ticks;

    w25qxx_enable_dwt_cycle_counter();

    start = DWT->CYCCNT;
    ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000U);

    while ((DWT->CYCCNT - start) < ticks)
    {
    }
}

void w25qxx_interface_debug_print(const char *const fmt, ...)
{
    // char buffer[256];
    // va_list args;

    // va_start(args, fmt);
    // vsnprintf(buffer, sizeof(buffer), fmt, args);
    // va_end(args);

    // printf("%s", buffer);
}
/**
//  * @file    driver_w25qxx_interface.c
//  * @brief   W25Qxx SPI interface implementation for STM32H743 SPI2 DMA
//  */

// #include "driver_w25qxx_interface.h"
// #include "gpio.h"
// #include "main.h"
// #include "spi.h"

// #include <stdarg.h>
// #include <stdio.h>
// #include <string.h>

// /* -------------------------------------------------------------------------- */
// /*                            Hardware Configuration                           */
// /* -------------------------------------------------------------------------- */

// #define W25QXX_SPI_HANDLE           hspi1

// #define W25QXX_CS_PORT              SPI1_Chip_Select_GPIO_Port
// #define W25QXX_CS_PIN               SPI1_Chip_Select_Pin

// #define W25QXX_CS_LOW()             HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_RESET)
// #define W25QXX_CS_HIGH()            HAL_GPIO_WritePin(W25QXX_CS_PORT, W25QXX_CS_PIN, GPIO_PIN_SET)

// /* DMA1 on STM32H7 cannot access DTCM, so all DMA-owned buffers must live in
//  * a DMA-visible SRAM region. The linker maps .RAM_D2_Section into RAM_D2. */
// #define W25QXX_DMA_BUFFER_ATTR      __attribute__((section(".RAM_D2_Section"), aligned(32)))

// #define W25QXX_DMA_CHUNK_SIZE       4096U
// #define W25QXX_SPI_TIMEOUT_MS       1000U
// #define W25QXX_PREFIX_INVALID       0xFFFFU

// static uint8_t W25QXX_DMA_BUFFER_ATTR gs_w25qxx_spi_tx_buffer[W25QXX_DMA_CHUNK_SIZE];
// static uint8_t W25QXX_DMA_BUFFER_ATTR gs_w25qxx_spi_rx_buffer[W25QXX_DMA_CHUNK_SIZE];
// static volatile uint8_t g_w25qxx_spi_dma_done = 0U;
// /* -------------------------------------------------------------------------- */
// /*                              Private Functions                              */
// /* -------------------------------------------------------------------------- */

// static void w25qxx_cs_select(void)
// {
//     W25QXX_CS_LOW();
// }

// static void w25qxx_cs_deselect(void)
// {
//     W25QXX_CS_HIGH();
// }

// static void w25qxx_enable_dwt_cycle_counter(void)
// {
//     if ((DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) == 0U)
//     {
//         CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
//         DWT->CYCCNT = 0U;
//         DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
//     }
// }

// // static uint8_t w25qxx_wait_for_spi_dma(uint32_t timeout_ms)
// // {
// //     uint32_t start;

// //     start = HAL_GetTick();
// //     while (HAL_SPI_GetState(&W25QXX_SPI_HANDLE) != HAL_SPI_STATE_READY)
// //     {
// //         if ((HAL_GetTick() - start) > timeout_ms)
// //         {
// //             (void)HAL_SPI_Abort(&W25QXX_SPI_HANDLE);

// //             return 1;
// //         }
// //     }

// //     if (HAL_SPI_GetError(&W25QXX_SPI_HANDLE) != HAL_SPI_ERROR_NONE)
// //     {
// //         (void)HAL_SPI_Abort(&W25QXX_SPI_HANDLE);

// //         return 1;
// //     }

// //     return 0;
// // }
// static uint8_t w25qxx_wait_for_spi_dma(uint32_t timeout_ms)
// {
//     uint32_t start;
//     uint32_t timeout_cycles;
//     uint32_t elapsed;

//     w25qxx_enable_dwt_cycle_counter();

//     /* Convert milliseconds to CPU cycles */
//     timeout_cycles = timeout_ms * (HAL_RCC_GetHCLKFreq() / 1000U);
//     start = DWT->CYCCNT;

//     while (g_w25qxx_spi_dma_done == 0U)
//     {
//         elapsed = DWT->CYCCNT - start;

//         if (elapsed >= timeout_cycles)
//         {
//             (void)HAL_SPI_Abort(&W25QXX_SPI_HANDLE);
//             g_w25qxx_spi_dma_done = 0U;
//             return 1;
//         }
//     }

//     if (g_w25qxx_spi_dma_done != 1U)
//     {
//         (void)HAL_SPI_Abort(&W25QXX_SPI_HANDLE);
//         g_w25qxx_spi_dma_done = 0U;
//         return 1;
//     }

//     g_w25qxx_spi_dma_done = 0U;

//     start = DWT->CYCCNT;

//     while (__HAL_SPI_GET_FLAG(&W25QXX_SPI_HANDLE, SPI_FLAG_TXC) == RESET)
//     {
//         elapsed = DWT->CYCCNT - start;

//         if (elapsed >= timeout_cycles)
//         {
//             (void)HAL_SPI_Abort(&W25QXX_SPI_HANDLE);
//             return 1;
//         }
//     }

//     __HAL_SPI_CLEAR_EOTFLAG(&W25QXX_SPI_HANDLE);

//     return 0;
// }
// static uint8_t w25qxx_spi_dma_transmit(const uint8_t *buffer, uint32_t length)
// {
//     uint16_t chunk;

//     while (length > 0U)
//     {
//         chunk = (uint16_t)((length > W25QXX_DMA_CHUNK_SIZE) ? W25QXX_DMA_CHUNK_SIZE : length);
//         memmove(gs_w25qxx_spi_tx_buffer, buffer, chunk);
//         SCB_CleanDCache_by_Addr((uint32_t*)gs_w25qxx_spi_tx_buffer, chunk);
// g_w25qxx_spi_dma_done = 0U;
//         if (HAL_SPI_Transmit_DMA(&W25QXX_SPI_HANDLE, gs_w25qxx_spi_tx_buffer, chunk) != HAL_OK)
//         {
//             return 1;
//         }

//         if (w25qxx_wait_for_spi_dma(W25QXX_SPI_TIMEOUT_MS) != 0U)
//         {
//             return 1;
//         }

//         buffer += chunk;
//         length -= chunk;
//     }

//     return 0;
// }

// static uint8_t w25qxx_spi_dma_receive(uint8_t *buffer, uint32_t length)
// {
//     uint16_t chunk;

//     while (length > 0U)
//     {
//         chunk = (uint16_t)((length > W25QXX_DMA_CHUNK_SIZE) ? W25QXX_DMA_CHUNK_SIZE : length);
//         memset(gs_w25qxx_spi_tx_buffer, 0xFF, chunk);
//         SCB_CleanDCache_by_Addr((uint32_t*)gs_w25qxx_spi_tx_buffer, chunk);
//         SCB_InvalidateDCache_by_Addr((uint32_t*)gs_w25qxx_spi_rx_buffer, chunk);
// g_w25qxx_spi_dma_done = 0U;
//         if (HAL_SPI_TransmitReceive_DMA(&W25QXX_SPI_HANDLE,
//                                         gs_w25qxx_spi_tx_buffer,
//                                         gs_w25qxx_spi_rx_buffer,
//                                         chunk) != HAL_OK)
//         {
//             return 1;
//         }

//         if (w25qxx_wait_for_spi_dma(W25QXX_SPI_TIMEOUT_MS) != 0U)
//         {
//             return 1;
//         }

//         memcpy(buffer, gs_w25qxx_spi_rx_buffer, chunk);
//         buffer += chunk;
//         length -= chunk;
//     }

//     return 0;
// }

// static uint16_t w25qxx_build_prefix(uint8_t instruction,
//                                     uint8_t instruction_line,
//                                     uint32_t address,
//                                     uint8_t address_line,
//                                     uint8_t address_len,
//                                     uint32_t alternate,
//                                     uint8_t alternate_line,
//                                     uint8_t alternate_len,
//                                     uint8_t dummy)
// {
//     uint16_t dummy_bytes;
//     uint16_t offset;
//     uint8_t i;

//     offset = 0U;

//     if (instruction_line > 1U)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if ((instruction_line == 1U) && (offset < W25QXX_DMA_CHUNK_SIZE))
//     {
//         gs_w25qxx_spi_tx_buffer[offset++] = instruction;
//     }

//     if (address_line > 1U)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if ((address_line == 0U) && (address_len != 0U))
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if (address_len > 4U)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     for (i = 0U; i < address_len; i++)
//     {
//         if (offset >= W25QXX_DMA_CHUNK_SIZE)
//         {
//             return W25QXX_PREFIX_INVALID;
//         }
//         gs_w25qxx_spi_tx_buffer[offset++] = (uint8_t)(address >> (8U * (address_len - 1U - i)));
//     }

//     if (alternate_line > 1U)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if ((alternate_line == 0U) && (alternate_len != 0U))
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if (alternate_len > 4U)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     for (i = 0U; i < alternate_len; i++)
//     {
//         if (offset >= W25QXX_DMA_CHUNK_SIZE)
//         {
//             return W25QXX_PREFIX_INVALID;
//         }
//         gs_w25qxx_spi_tx_buffer[offset++] = (uint8_t)(alternate >> (8U * (alternate_len - 1U - i)));
//     }

//     /* The LibDriver interface provides dummy in clock cycles. For single-lane
//      * SPI transfers, every 8 cycles is one transmitted dummy byte. */
//     dummy_bytes = (uint16_t)((dummy + 7U) / 8U);

//     if ((uint32_t)offset + dummy_bytes > W25QXX_DMA_CHUNK_SIZE)
//     {
//         return W25QXX_PREFIX_INVALID;
//     }
//     if (dummy_bytes > 0U)
//     {
//         memset(&gs_w25qxx_spi_tx_buffer[offset], 0xFF, dummy_bytes);
//         offset = (uint16_t)(offset + dummy_bytes);
//     }

//     return offset;
// }

// /* -------------------------------------------------------------------------- */
// /*                              Interface Functions                            */
// /* -------------------------------------------------------------------------- */

// uint8_t w25qxx_interface_spi_qspi_init(void)
// {
//     w25qxx_enable_dwt_cycle_counter();
//     w25qxx_cs_deselect();

//     return 0;
// }

// uint8_t w25qxx_interface_spi_qspi_deinit(void)
// {
//     w25qxx_cs_deselect();

//     return 0;
// }

// uint8_t w25qxx_interface_spi_qspi_write_read(uint8_t instruction,
//                                              uint8_t instruction_line,
//                                              uint32_t address,
//                                              uint8_t address_line,
//                                              uint8_t address_len,
//                                              uint32_t alternate,
//                                              uint8_t alternate_line,
//                                              uint8_t alternate_len,
//                                              uint8_t dummy,
//                                              uint8_t *in_buf,
//                                              uint32_t in_len,
//                                              uint8_t *out_buf,
//                                              uint32_t out_len,
//                                              uint8_t data_line)
// {
//     uint16_t prefix_length;

//     if (((in_len > 0U) && (in_buf == NULL)) || ((out_len > 0U) && (out_buf == NULL)))
//     {
//         return 1;
//     }

//     if (((in_len > 0U) || (out_len > 0U)) && (data_line != 1U))
//     {
//         return 1;
//     }
//     if (data_line > 1U)
//     {
//         return 1;
//     }

//     prefix_length = w25qxx_build_prefix(instruction,
//                                         instruction_line,
//                                         address,
//                                         address_line,
//                                         address_len,
//                                         alternate,
//                                         alternate_line,
//                                         alternate_len,
//                                         dummy);
//     if (prefix_length == W25QXX_PREFIX_INVALID)
//     {
//         return 1;
//     }

//     w25qxx_cs_select();

//     if ((prefix_length > 0U) && (w25qxx_spi_dma_transmit(gs_w25qxx_spi_tx_buffer, prefix_length) != 0U))
//     {
//         goto error;
//     }

//     if ((in_len > 0U) && (w25qxx_spi_dma_transmit(in_buf, in_len) != 0U))
//     {
//         goto error;
//     }

//     if ((out_len > 0U) && (w25qxx_spi_dma_receive(out_buf, out_len) != 0U))
//     {
//         goto error;
//     }

//     w25qxx_cs_deselect();

//     return 0;

// error:
//     w25qxx_cs_deselect();

//     return 1;
// }

// void w25qxx_interface_delay_ms(uint32_t ms)
// {
//     HAL_Delay(ms);
// }

// void w25qxx_interface_delay_us(uint32_t us)
// {
//     uint32_t start;
//     uint32_t ticks;

//     w25qxx_enable_dwt_cycle_counter();

//     start = DWT->CYCCNT;
//     ticks = us * (HAL_RCC_GetHCLKFreq() / 1000000U);

//     while ((DWT->CYCCNT - start) < ticks)
//     {
//     }
// }

// void w25qxx_interface_debug_print(const char *const fmt, ...)
// {
//     char buffer[256];
//     va_list args;

//     va_start(args, fmt);
//     vsnprintf(buffer, sizeof(buffer), fmt, args);
//     va_end(args);

//     printf("%s", buffer);
// }
// void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi == &W25QXX_SPI_HANDLE)
//     {
//         g_w25qxx_spi_dma_done = 1U;
//     }
// }

// void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi == &W25QXX_SPI_HANDLE)
//     {
//         g_w25qxx_spi_dma_done = 1U;
//     }
// }

// void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi == &W25QXX_SPI_HANDLE)
//     {
//         g_w25qxx_spi_dma_done = 1U;
//     }
// }

// void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
// {
//     if (hspi == &W25QXX_SPI_HANDLE)
//     {
//         g_w25qxx_spi_dma_done = 2U;
//     }
// }
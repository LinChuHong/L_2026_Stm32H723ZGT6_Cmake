#include "parser.h"
#include "cmsis_os2.h"
#include "norflash.h"
#include "packet_queue.h"
#include "crc.h"
#include "usbd_cdc_if.h"
#include <cstring>
#include <string.h>
#include <stdio.h>

#include "L_Global.h"
#include "usbd_def.h"

std::bitset<100> L_States;
std::vector<std::string> L_Data(69,"");
std::vector<uint8_t> datatoflash;
uint32_t dataLen = 0;

#define MAX_DATA_LEN 256
#define DBG(...) printf(__VA_ARGS__)


// ===== STATE =====
typedef enum {
    WAIT_HEADER1,
    WAIT_HEADER2,
    READ_ADDR,
    READ_LEN,
    READ_DATA,
    READ_CRC
} ParserState;

static ParserState state = WAIT_HEADER1;

// ===== VARIABLES =====
static uint32_t addr;
static uint16_t len;
static uint16_t data_index;

static uint8_t data_buf[MAX_DATA_LEN];

static uint8_t temp[4];
static uint8_t temp_i;

static uint8_t crc_buf[4];
static uint8_t crc_i;

// ===== CRC32 =====
// static uint32_t crc32_calc(uint8_t *data, uint32_t length)
// {
//     uint32_t buffer = 0;
//     uint8_t index = 0;

//     __HAL_CRC_DR_RESET(&hcrc);

//     for (uint32_t i = 0; i < length; i++)
//     {
//         buffer |= data[i] << (8 * index);
//         index++;

//         if (index == 4)
//         {
//             HAL_CRC_Accumulate(&hcrc, &buffer, 1);
//             buffer = 0;
//             index = 0;
//         }
//     }

//     if (index > 0)
//     {
//         HAL_CRC_Accumulate(&hcrc, &buffer, 1);
//     }

//     return hcrc.Instance->DR;
// }

// static uint32_t crc32_calc(uint8_t *data, uint32_t length)
// {
//     __HAL_CRC_DR_RESET(&hcrc);

//     uint32_t words = length / 4;
//     uint32_t crc = HAL_CRC_Accumulate(&hcrc, (uint32_t *)data, words);

//     // handle remaining bytes
//     uint32_t remaining = length % 4;
//     if (remaining)
//     {
//         uint32_t last = 0;
//         memcpy(&last, &data[words * 4], remaining);
//         crc = HAL_CRC_Accumulate(&hcrc, &last, 1);
//     }

//     return crc;
// }
static uint32_t crc32_table[256];
static uint32_t crc32_calc(uint8_t *data, uint32_t len)
{
    uint32_t crc = 0xFFFFFFFF;

    for (uint32_t i = 0; i < len; i++)
    {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}



// ===== PARSER =====
void parser_feed(uint8_t byte)
{
    DBG("%02X ", byte);

    switch (state)
    {
    case WAIT_HEADER1:
        if (byte == 0xAA)
        {
            DBG("\nAA\n");
            state = WAIT_HEADER2;
        }
        break;

    case WAIT_HEADER2:
        if (byte == 0x55)
        {
            DBG("55 HEADER OK\n");
            temp_i = 0;
            state = READ_ADDR;
        }
        else
        {
            state = WAIT_HEADER1;
        }
        break;

    case READ_ADDR:
        temp[temp_i++] = byte;
        if (temp_i == 4)
        {
            addr = temp[0] |
                   (temp[1] << 8) |
                   (temp[2] << 16) |
                   (temp[3] << 24);

            DBG("ADDR=0x%08lX\n", addr);

            temp_i = 0;
            state = READ_LEN;
        }
        break;

    case READ_LEN:
        temp[temp_i++] = byte;
        if (temp_i == 2)
        {
            len = temp[0] | (temp[1] << 8);

            DBG("LEN=%d\n", len);

            if (len > MAX_DATA_LEN)
            {
                DBG("LEN ERROR\n");
                state = WAIT_HEADER1;
                break;
            }

            data_index = 0;
            state = READ_DATA;
        }
        break;

    case READ_DATA:
        data_buf[data_index++] = byte;

        if (data_index >= len)
        {
            DBG("DATA DONE\n");
            crc_i = 0;
            state = READ_CRC;
        }
        break;

    case READ_CRC:
        crc_buf[crc_i++] = byte;

        if (crc_i == 4)
        {
            uint32_t recv_crc =
                crc_buf[0] |
                (crc_buf[1] << 8) |
                (crc_buf[2] << 16) |
                (crc_buf[3] << 24);

            uint32_t calc_crc = crc32_calc(data_buf, len);

            DBG("CRC recv=%08lX calc=%08lX\n", recv_crc, calc_crc);

            if (recv_crc == calc_crc)
            {
                DBG("CRC OK\n");

                if (queue_push(addr, data_buf, len))
                {
                    uint8_t ack = 0x06;
                    while (CDC_Transmit_HS(&ack, 1) == USBD_BUSY);
                }
                else
                {
                    uint8_t nack = 0x15;
                    CDC_Transmit_HS(&nack, 1);
                }
            }
            else
            {
                DBG("CRC FAIL\n");
            }

            state = WAIT_HEADER1;
        }
        break;
    }
}



void process_queue(void)
{
    Packet pkt;

    while (queue_pop(&pkt))
    {
        flash_write_fast(pkt.addr, pkt.data, pkt.len);
    }
}



void crc32_init(void)
{
    uint32_t poly = 0xEDB88320;

    for (uint32_t i = 0; i < 256; i++)
    {
        uint32_t crc = i;

        for (uint32_t j = 0; j < 8; j++)
        {
            if (crc & 1)
                crc = (crc >> 1) ^ poly;
            else
                crc >>= 1;
        }

        crc32_table[i] = crc;
    }
}


// void data_from_usb(uint8_t data)
// {
//     // printf("Data from USB: %c\n", (char)data);
//     // usb_data[0].push_back((char)data);
//     if (data == '\n')
//     {
//         usb_data_state.set(6);
//     }
//     else
//     {
//         datatoflash.push_back(data);
//     }
    
//     // for( auto& v : usb_data[0] )
//     // {
//     //     if(v == '\n')
//     //     {
//     //         v = '\0';
//     //         printf("%s\n",usb_data[0].c_str());
//     //         if (std::strcmp(usb_data[0].c_str(),"lin") == 0)
//     //         {
//     //             usb_data_state.set(0);
//     //         }
//     //         else if (std::strcmp(usb_data[0].c_str(),"chu") == 0)
//     //         {
//     //             usb_data_state.set(1);
//     //         }
//     //         else if (std::strcmp(usb_data[0].c_str(),"hong") == 0)
//     //         {
//     //             usb_data_state.set(2);
//     //         }

//     //         CDC_Transmit_HS((uint8_t*)("linchuhong\n"), 12);

//     //         usb_data[0].clear();
//     //         break;
//     //     }
//     // }
    
// }


void data_from_usb(uint8_t data)
{

    #if USEPYTHONTOSENDDATATONORFLASH == 0
    if (data == '\n')
    {
        // L_Data[0].push_back('\n');
        L_States.set(10);
    }
    else
    {
        L_Data[0].push_back((char)data);

    }
    #endif

    #if USEPYTHONTOSENDDATATONORFLASH == 1
    static uint16_t expected_size = 0;
    static uint16_t received = 0;
    static uint8_t size_buf[2];
    static uint8_t size_index = 0;

    // Step 1: receive size (2 bytes)
    if (size_index < 2)
    {
        size_buf[size_index++] = data;

        if (size_index == 2)
        {
            // little-endian decode
            expected_size = size_buf[0] | (size_buf[1] << 8);

            datatoflash.clear();
            datatoflash.reserve(expected_size);

            received = 0;
        }
        return;
    }

    // Step 2: receive data
    datatoflash.push_back(data);
    received++;

    // Step 3: check if full packet received
    if (received >= expected_size)
    {
        L_States.set(69);

        // reset for next packet
        size_index = 0;
        expected_size = 0;
        received = 0;
    }
    #endif
}
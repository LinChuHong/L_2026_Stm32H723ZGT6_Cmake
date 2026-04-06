#pragma once
// for header file for .cpp files
#include "main.h"
#include "vector"
#include "string"
#include "bitset"
typedef enum
{
    USB_DATA_RECEIVE_STATE

}LDATA;

extern std::bitset<1000> L_States;
extern std::vector<std::string> L_Data;
extern std::vector<uint8_t> datatoflash;
extern uint32_t dataLen;

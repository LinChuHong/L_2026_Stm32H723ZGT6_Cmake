#pragma once
// for header file for .cpp files

#include "vector"
#include "string"
#include "bitset"
typedef enum
{
    USB_DATA_RECEIVE_STATE

}LGlobalState;

extern std::bitset<100> usb_data_state;
extern std::vector<std::string> usb_data;
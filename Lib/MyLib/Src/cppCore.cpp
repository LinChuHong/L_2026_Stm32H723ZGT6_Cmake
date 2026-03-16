#include "myh.h"


void cppCoreStart()
{

    for (;;)
    {
        HAL_UART_Transmit(&huart1,(uint8_t*)"hello\n",6,HAL_MAX_DELAY);
        HAL_Delay(1000);
    }

}

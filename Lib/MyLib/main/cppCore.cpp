#include "main.h"
#include "myh.h"


#include "FreeRTOS.h"
#include "stm32h723xx.h"
#include "cmsis_os2.h"
#include "stm32h7xx_hal_gpio.h"
#include "task.h"


#include "usart/usart1.h"
#include "SDRAM/sdram.h"
#include "KEY/key.h"
#include "LED/led.h"
#include "MPU/mpu.h"
#include "LCD/lcd.h"
#include "USMART/usmart.h"
#include "RTC/rtc.h"


#include <string>
#include "vector"

std::vector<std::string> debugStr;
std::vector<int> debugInt;

typedef struct
{
    uint64_t task1;
    uint64_t task2;
} myTaskState;
myTaskState myts;

osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};



/* LED状态设置函数 */
void led_set(uint8_t sta)
{
    LED1(sta);
}

/* 函数参数调用测试函数 */
void test_fun(void(*ledset)(uint8_t), uint8_t sta)
{
    ledset(sta);
}


void init()
{
    myts = { .task1=0, .task2=0 };
    myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

    led_init();                             /* 初始化LED */
    key_init();                             /* 初始化按键 */
    // mpu_memory_protection();
    usart_init(0);
    usmart_init(260);
    sdram_init();                           /* 初始化SDRA */
    lcd_init();

    rtc_init();                             /* 初始化RTC */
    rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);   /* 配置WAKE UP中断,1秒钟中断一次 */


    debugStr.push_back("Hello, World->1\r\n");
    debugStr.push_back("Hello, World->2\r\n");
}



void cppCoreStart(void *argument)
{
    for (;;)
    {
        // HAL_UART_Transmit(&huart1,(uint8_t*)"hello\n",6,HAL_MAX_DELAY);
        // printf("%s",debugStr[0].c_str());
        // if (g_usart_rx_sta & 0x8000)        /* 接收完了一次数据 */
		// {					    
        //     len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
        //     printf("\r\n您发送的消息为:\r\n");
          
        //     HAL_UART_Transmit(&huart1, (uint8_t *)g_usart_rx_buf, len, 1000);   /* 发送接收到的数据 */
        //     debugStr.emplace_back((char*)g_usart_rx_buf,len);
        //     while(__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != SET);           /* 等待发送完成 */      
          
        //     printf("\r\n\r\n");             /* 插入换行 */  
        //     g_usart_rx_sta = 0;
		// }

        myts.task1++;
        osDelay(1);
    }

}

void StartTask02(void *argument)
{
    uint8_t hour, min, sec, ampm;
    uint8_t year, month, date, week;
    uint8_t tbuf[40];
    uint8_t t = 0;


    for (;;)
    {

     
        t++;

        if ((t % 10) == 0)                  /* 每100ms更新一次显示数据 */
        {
            rtc_get_time(&hour, &min, &sec, &ampm);
            sprintf((char *)tbuf, "Time:%02d:%02d:%02d", hour, min, sec);
            lcd_show_string(30, 130, 210, 16, 16, (char *)tbuf, RED);
            rtc_get_date(&year, &month, &date, &week);
            sprintf((char *)tbuf, "Date:20%02d-%02d-%02d", year, month, date);
            lcd_show_string(30, 150, 210, 16, 16, (char *)tbuf, RED);
            sprintf((char *)tbuf, "Week:%d", week);
            lcd_show_string(30, 170, 210, 16, 16, (char *)tbuf, RED);
        }

        if ((t % 20) == 0)
        {
            LED0_TOGGLE();                  /* 每200ms,翻转一次LED0 */
        }
        

        myts.task2++;
        osDelay(10);
    }
}

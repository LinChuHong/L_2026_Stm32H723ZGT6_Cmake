#include "main.h"
#include "myh.h"


#include "FreeRTOS.h"
#include "stm32h723xx.h"
#include "cmsis_os2.h"
#include "stm32h7xx_hal.h"
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
#include "NORFLASH/norflash.h"
#include "QSPI/qspi.h"
#include "delay/delay.h"
#include "TOUCH/touch.h"
#include "MALLOC/malloc1.h"

#include "lvgl/lvgl.h"
#include "main/lv_port_disp_template.h"
#include "main/lv_port_indev_template.h"
#include "NORFLASH/lv_fs_rawfs.h"
#include "lv_demos.h"
#include "L_GUI_Guider/custom/custom.h"



#include <src/tick/lv_tick.h>
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



void init()
{
    myts = { .task1=0, .task2=0 };
    myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

    led_init();                             /* 初始化LED */
    key_init();                             /* 初始化按键 */
    mpu_memory_protection();
    usart_init(0);
    usmart_init(260);
    sdram_init();                           /* 初始化SDRA */
    lcd_init();
    norflash_init();
    my_mem_init(SRAMIN);                    /* 初始化内部内存池(AXI) */
    my_mem_init(SRAMEX);                    /* 初始化外部内存池(SDRAM) */
    my_mem_init(SRAMDTCM);                  /* 初始化DTCM内存池(DTCM) */
    my_mem_init(SRAMITCM);                  /* 初始化ITCM内存池(ITCM) */

    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */
    lv_fs_rawfs_init();
    // rtc_init();                             /* 初始化RTC */
    // rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);   /* 配置WAKE UP中断,1秒钟中断一次 */


    debugStr.push_back("Hello, World->1");
    debugStr.push_back("Hello, World->2");
}



void cppCoreStart(void *argument)
{
    

    for (;;)
    {
        lv_tick_inc(1);
        osDelay(1);
    }

}

void StartTask02(void *argument)
{

    // lv_demo_widgets();
    custom_init(&guider_ui);
    for(;;)
    {

        if (g_usart_rx_sta & 0x8000)        /* 串口接收完成？ */
        {
            uint8_t len;
            // char *pbuf = 0;
            len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
            g_usart_rx_buf[len] = '\0';     /* 在末尾加入结束符. */
            // pbuf = (char *)g_usart_rx_buf;
            debugStr.push_back((char*)g_usart_rx_buf);
            for (const auto& v :debugStr)
            {
                printf("%s\n",v.c_str());
            }
            g_usart_rx_sta = 0;             /* 开启下一次接收 */
        }
        lv_timer_handler();
        osDelay(1);
    }

}

#include "driver_w25qxx.h"
#include "main.h"
#include "myh.h"


#include "FreeRTOS.h"
#include "projdefs.h"
#include "stm32h723xx.h"
#include "cmsis_os2.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_uart.h"
#include "system_stm32h7xx.h"
#include "task.h"
#include "usbd_cdc_if.h"
#include "usb_device.h"

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
#include "SDMMC/sdmmc_sdcard.h"
#include "FATFS/exfuns/exfuns.h"
#include "FATFS/exfuns/fattester.h"

#include "lvgl/lvgl.h"
#include "main/lv_port_disp_template.h"
#include "main/lv_port_indev_template.h"
#include "NORFLASH/lv_fs_rawfs.h"
#include <src/libs/fsdrv/lv_fsdrv.h>
#include "lv_demos.h"
#include "L_GUI_Guider/custom/custom.h"
#include "parser.h"
#include "driver_w25qxx_basic.h"


#include <src/misc/lv_fs.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>
#include "L_Buttons.h"


#include "L_Global.h"
#include "uart2_ring_buffer.h"


osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
typedef struct
{
    uint32_t cnt;
    char name[100];
} Data;

Data wData = 
{
    .cnt = 69,
    .name = "LinChuHong"
};
Data rData {};
void init()
{
    myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);
    MX_USB_DEVICE_Init(); 
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

    while (sd_init()) { }
    exfuns_init();
    res = f_mount(fs[0],"0:",1);
    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */
    lv_fs_rawfs_init();
    lv_fs_fatfs_init();
    w25qxx_init_1(); 
    buttons_init();
    
    // rtc_init();                             /* 初始化RTC */
    // rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);   /* 配置WAKE UP中断,1秒钟中断一次 */
    // crc32_init();

}

void cppCoreStart(void *argument)
{

    uint32_t last = DWT->CYCCNT; 
 
    for (;;)
    {
        uint32_t now = DWT->CYCCNT;
        if ( (now - last) >= SystemCoreClock )
        {
            last += SystemCoreClock;
            
        } 

        #if USEPYTHONTOSENDDATATONORFLASH == 1
        if (L_States.test(6) == 1)
        {
            norflash_write(datatoflash.data(),dataLen,datatoflash.size());
            L_States.reset(6);
            dataLen+=datatoflash.size();
            datatoflash.clear();
            uint8_t byte = 0xAA;
            CDC_Transmit_HS(&byte, 1);
        }
        #endif
        osDelay(1);
    }

}

void startLvglTask(void *argument)
{

    // lv_demo_widgets();
    // lv_demo_music();
    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    for(;;)
    {

        if (g_usart_rx_sta & 0x8000)        /* 串口接收完成？ */
        {
            uint8_t len;
            len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
            g_usart_rx_buf[len] = '\0';     /* 在末尾加入结束符. */
            g_usart_rx_sta = 0;             /* 开启下一次接收 */
            if (std::strcmp((const char*)g_usart_rx_buf, "test") == 0)
            {   
                w25qxx_basic_write(0, (uint8_t*)&wData, sizeof(wData));
                w25qxx_basic_read(0, (uint8_t*)&rData, sizeof(rData));
                printf("%lu %s\n",rData.cnt,rData.name);
            }
        }
        if (L_States.test(10) == true)
        {
            if (L_Data.at(0) == "test")
            {

            }
            else if (L_Data.at(0) == "erase" and L_States.test(5) != true)
            {
                L_States.set(5);
            }
            L_Data.at(0).clear();
            L_States.reset(10);
        }
        if (L_States.test(100) == true)
        {
            printf("write norlfash done\n");
            L_States.reset(100);
        }
        lv_timer_handler();
        osDelay(5);
    }

}


void StartTask03(void *argument)
{
    for(;;)
    {
        if (L_States.test(5) == true)
        {
            printf("erase norflash start\n");
            norflash_erase_chip();
            L_States.reset(5);
            printf("norflash erased\n");
        }
        osDelay(1);
    }
}


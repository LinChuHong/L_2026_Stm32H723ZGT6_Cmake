#include "main.h"
#include "myh.h"


#include "FreeRTOS.h"
#include "stm32h723xx.h"
#include "cmsis_os2.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"
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
#include "lv_demos.h"
#include "L_GUI_Guider/custom/custom.h"
#include "parser.h"


#include <atomic>
#include <src/misc/lv_types.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <cstring>


#include "L_Global.h"


osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};



void init()
{
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

    while (sd_init()) { }
    exfuns_init();
    res = f_mount(fs[0],"0:",1);

    MX_USB_DEVICE_Init();    

    lv_init();                                          /* lvgl系统初始化 */
    lv_port_disp_init();                                /* lvgl显示接口初始化,放在lv_init()的后面 */
    lv_port_indev_init();                               /* lvgl输入接口初始化,放在lv_init()的后面 */
    lv_fs_rawfs_init();
    // rtc_init();                             /* 初始化RTC */
    // rtc_set_wakeup(RTC_WAKEUPCLOCK_CK_SPRE_16BITS, 0);   /* 配置WAKE UP中断,1秒钟中断一次 */
    // crc32_init();


}


void cppCoreStart(void *argument)
{

    uint32_t last = DWT->CYCCNT; 
    // norflash_erase_chip();
    // printf("chip erased\n");
    for (;;)
    {
        uint32_t now = DWT->CYCCNT;
        if ( (now - last) >= SystemCoreClock )
        {
            last += SystemCoreClock;
            // CDC_Transmit_HS((uint8_t*)debugStr[0].c_str(), debugStr[0].size());
        } 
        // if (L_States.test(10) == 1)
        // {
        //     // while (CDC_Transmit_HS((uint8_t*)L_Data[0].c_str(), L_Data[0].size()) == USBD_BUSY) osDelay(1);
        //     // L_States.reset(10);
        // }

        #if USEPYTHONTOSENDDATATONORFLASH == 1
        if (L_States.test(6) == 1)
        {
            norflash_write(datatoflash.data(),dataLen,datatoflash.size());
            L_States.reset(6);
            printf("%d\n",datatoflash.size());
            dataLen+=datatoflash.size();
            datatoflash.clear();
            uint8_t byte = 0xAA;
            CDC_Transmit_HS(&byte, 1);

        }
        #endif
        
        lv_tick_inc(1);
        osDelay(1);
    }

}

void StartTask02(void *argument)
{

    // lv_demo_widgets();
    // lv_demo_music();
    custom_init(&guider_ui);
    for(;;)
    {

        if (g_usart_rx_sta & 0x8000)        /* 串口接收完成？ */
        {
            uint8_t len;
            len = g_usart_rx_sta & 0x3fff;  /* 得到此次接收到的数据长度 */
            g_usart_rx_buf[len] = '\0';     /* 在末尾加入结束符. */
            g_usart_rx_sta = 0;             /* 开启下一次接收 */
        }
        
        lv_timer_handler();
        osDelay(1);
    }

}

void test_write_to_norflash()
{   
    static uint8_t cnt = 0;
    if (cnt == 0) res = f_open(fattester.file,"images.txt",FA_CREATE_ALWAYS | FA_WRITE);
    cnt = 69;

    #if USEPYTHONTOSENDDATATONORFLASH == 1
    if (L_States.test(69) == 1)
    {
        if (res) 
        {
            printf("can't open file(Shakespeare.txt) Error code ->%d\n",res);
            return;
        }
        res = f_write(fattester.file,datatoflash.data(),datatoflash.size(),&bw);
        if (res != FR_OK)
        {
            printf("read error -> %d",res);
        }
        if (bw != datatoflash.size())
        {
            printf("write incomplete! expected=%u, written=%u\n",
                datatoflash.size(), bw);
        }
        printf("%d\n",datatoflash.size());
        datatoflash.clear();
        L_States.reset(69);
        uint8_t byte = 0xAA;
        while (CDC_Transmit_HS(&byte, 1) == USBD_BUSY);
    }
    else if (L_States.test(100) == 1)
    {
        f_close(fattester.file);
        L_States.set(99);
        L_States.reset(100);
    }
    #endif

}
void test()
{
    if (L_States.test(99) == 1)
    {
        res = f_open(fattester.file,"images.txt",FA_READ);
        if (res) 
        {
            printf("can't open file(images.bin) Error code ->%d\n",res);
            return;
        }
        do
        {
            res = f_read(fattester.file,fattester.fatbuf,2048,&br);
            if (res != FR_OK)
            {
                printf("read error -> %d",res);
            }
            if (br > 0)
            {
                norflash_write(fattester.fatbuf, dataLen, br);
                // flash_write_fast(dataLen,fattester.fatbuf, br);
                printf("writing %u bytes to NOR flash at addr %lu\n", br, dataLen);
                dataLen+=br;
            }
        } while (br > 0);
        L_States.reset(99);
        f_close(fattester.file);

    }
}


void StartTask03(void *argument)
{
   

    for(;;)
    {
        test_write_to_norflash();
        test();

        // if (L_States.test(10) == 1)
        // {

        //     if (std::strcmp(L_Data.at(0).c_str(),"test") == 0)
        //     {
        //         test();
        
        //     }
        //     printf("%s\n",L_Data.at(0).c_str());
        //     L_Data.at(0).clear();
        //     L_States.reset(10);
        // }

        osDelay(1);
    }

}
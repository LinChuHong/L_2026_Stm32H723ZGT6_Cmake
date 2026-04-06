#include "burner.h"
#include "L_Global.h"
#include "norflash.h"
#include "FATFS/exfuns/exfuns.h"
#include "FATFS/exfuns/fattester.h"


void test_receive_data_from_usb_and_write_into_sdcard()
{   
    if (L_States.test(98) == 1)
    {
        static uint8_t file_opened = 0;

        if (!file_opened)
        {
            res = f_open(fattester.file, "images.txt", FA_CREATE_ALWAYS | FA_WRITE);

            if (res == FR_OK)
            {
                file_opened = 1;
            }
            else
            {
                printf("open failed: %d\n", res);
                L_States.reset(98);
            }
        }
        #if USEPYTHONTOSENDDATATONORFLASH == 1
        if (L_States.test(69) == 1)
        {
            if (res) 
            {
                printf("can't open file(Shakespeare.txt) Error code ->%d\n",res);
                L_States.reset(98);
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
            L_States.reset(100);
            L_States.reset(98);
            file_opened = 0;
            printf("Done\n");
        }
        #endif
    }   

}
void test_read_from_sdcard_and_write_into_norflash()
{
    static uint8_t file_opened = 0;

    if (L_States.test(99) == 1)
    {
        if (!file_opened)
        {
            res = f_open(fattester.file,"images.txt",FA_READ);
            if (res)
            {
                printf("open error %d\n",res);
                return;
            }
            dataLen = 0;
            file_opened = 1;
        }
 
        // do
        // {
        //     res = f_read(fattester.file,fattester.fatbuf,2048,&br);
        //     if (res != FR_OK)
        //     {
        //         printf("read error -> %d",res);
        //     }
        //     if (br > 0)
        //     {
        //         norflash_write(fattester.fatbuf, dataLen, br);
        //         // flash_write_fast(dataLen,fattester.fatbuf, br);
        //         printf("writing %u bytes to NOR flash at addr %lu\n", br, dataLen);
        //         dataLen+=br;
        //     }
        // } while (br > 0);
        res = f_read(fattester.file,fattester.fatbuf,4096,&br);
        if (res != FR_OK)
        {
            printf("read error %d\n",res);
            L_States.reset(99);
        }
        if (br > 0)
        {
            norflash_write(fattester.fatbuf, dataLen, br);
            printf("writing %u bytes to NOR flash at addr %lu\n", br, dataLen);
            dataLen+=br;
        }
        else
        {
            f_close(fattester.file);
            L_States.reset(99);
            file_opened = 1;
            printf("Done\n");
        }
    }
}
#include "lvgl.h"
#include "malloc1.h"   // your allocator header

// /* Choose which memory bank LVGL should use */
// #define LV_MEM_BANK  1   // 0=SRAM, 1=SDRAM, 2=DTCM, 3=ITCM

// void *lv_malloc(size_t size)
// {
//     return mymalloc(LV_MEM_BANK, size);
// }

// void lv_free(void *ptr)
// {
//     myfree(LV_MEM_BANK, ptr);
// }

// void *lv_realloc(void *ptr, size_t size)
// {
//     return myrealloc(LV_MEM_BANK, ptr, size);
// }
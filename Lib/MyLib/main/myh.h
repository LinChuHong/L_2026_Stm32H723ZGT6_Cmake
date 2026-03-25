#ifndef __MYH_H__
#define __MYH_H__

#include "main.h"
#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif
void init();
void cppCoreStart(void *argument);
void StartTask02(void *argument);

void led_set(uint8_t sta);
void test_fun(void(*ledset)(uint8_t), uint8_t sta);

#ifdef __cplusplus
}
#endif


#endif


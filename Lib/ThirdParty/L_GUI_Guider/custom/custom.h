/*
 * Copyright 2024 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

#ifndef __CUSTOM_H_
#define __CUSTOM_H_


#include "gui_guider.h"


#ifdef __cplusplus
extern "C" {
#endif
extern lv_ui guider_ui;
extern uint32_t srng;
typedef struct 
{
    char zen[1024]; // get zen from api.github.com/zen
    uint8_t led_state;
    uint8_t screen_state; // avoid accessing the objs already deleted by the load_screen
    uint8_t hppt_state;
} custom_data;
extern custom_data mydata;
#define LV_ATTRIBUTE_MEM_SDRAM  __attribute__((section(".sdram")))
void mycb(lv_event_t * e);
void custom_init(lv_ui *ui);

#ifdef __cplusplus
}
#endif


#endif /* EVENT_CB_H_ */

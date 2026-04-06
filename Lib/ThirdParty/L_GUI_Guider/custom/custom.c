/*
 * Copyright 2024 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */

/*********************
 *      INCLUDES
 *********************/
#include <src/widgets/label/lv_label.h>
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"
#include <custom_events_cb.h>
#include <gui_guider.h>

/*********************
 *      DEFINES
 *********************/
lv_ui guider_ui;
uint32_t srng = 0;
custom_data mydata =
{
    .led_state    = 1,
    .zen          = "hello, world",
    .screen_state = 3,
    .hppt_state = 0,
    .barValue = 0
};
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */
static lv_timer_t *screen_timer = NULL;

static void timer_cb(lv_timer_t *t)
{
    if (mydata.screen_state == 3)
    {
        lv_bar_set_value(guider_ui.screen_3_bar_2, mydata.barValue, LV_ANIM_ON);
        lv_bar_set_value(guider_ui.screen_3_bar_3, mydata.barValue, LV_ANIM_ON);
        if (mydata.barValue == 100)
        {
            lv_label_set_text(guider_ui.screen_3_label_1, "Data have been written to sd card");
        }
    }
}

void custom_init(lv_ui *ui)
{
    
    /* Add your codes here */
    // lv_obj_add_event_cb(ui->g_kb_top_layer,mycb,LV_EVENT_ALL,ui);
    screen_timer = lv_timer_create(timer_cb,1000,ui);
}

#if USEMYCBINC == 1
void mycb(lv_event_t * e)
{


}
#endif
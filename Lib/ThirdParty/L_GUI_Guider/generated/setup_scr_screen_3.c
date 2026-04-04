/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_screen_3(lv_ui *ui)
{
    //Write codes screen_3
    ui->screen_3 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_3, 1024, 600);
    lv_obj_set_scrollbar_mode(ui->screen_3, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
#if LV_USE_GUIDER_SIMULATOR
    lv_obj_set_style_bg_image_src(ui->screen_3, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Shinobu3.jpeg", LV_PART_MAIN|LV_STATE_DEFAULT);
#else
    lv_obj_set_style_bg_image_src(ui->screen_3, "F:/Shinobu3.bin", LV_PART_MAIN|LV_STATE_DEFAULT);
#endif
    lv_obj_set_style_bg_image_opa(ui->screen_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);


    //The custom code of screen_3.


    //Update current screen layout.
    lv_obj_update_layout(ui->screen_3);

}

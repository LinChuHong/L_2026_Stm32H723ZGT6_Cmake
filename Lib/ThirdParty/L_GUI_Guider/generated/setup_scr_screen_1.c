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
#include <lvgl.h>


void setup_scr_screen_1(lv_ui *ui)
{
    //Write codes screen_1
    ui->screen_1 = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen_1, 1024, 600);
    lv_obj_set_scrollbar_mode(ui->screen_1, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
#if LV_USE_GUIDER_SIMULATOR
    lv_obj_set_style_bg_image_src(ui->screen_1, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Chiyo_chan.jpg", LV_PART_MAIN|LV_STATE_DEFAULT);
#else
    lv_obj_set_style_bg_image_src(ui->screen_1, "F:/Chiyo_chan.bin", LV_PART_MAIN|LV_STATE_DEFAULT);
#endif
    lv_obj_set_style_bg_image_opa(ui->screen_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_1
    ui->screen_1_btn_1 = lv_button_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_btn_1, 52, 42);
    lv_obj_set_size(ui->screen_1_btn_1, 140, 81);
    ui->screen_1_btn_1_label = lv_label_create(ui->screen_1_btn_1);
    lv_label_set_text(ui->screen_1_btn_1_label, "Button");
    lv_label_set_long_mode(ui->screen_1_btn_1_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_1_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_1, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_1_label, LV_PCT(100));

    //Write style for screen_1_btn_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_btn_1, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_btn_1, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
#if LV_USE_GUIDER_SIMULATOR
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_1, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\IMG_2066.JPG", LV_PART_MAIN|LV_STATE_DEFAULT);
#else
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_1, "F:/IMG_2066.bin", LV_PART_MAIN|LV_STATE_DEFAULT);
#endif
    lv_obj_set_style_bg_image_opa(ui->screen_1_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_1_btn_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_1, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_1, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_2
    ui->screen_1_btn_2 = lv_button_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_btn_2, 224, 119);
    lv_obj_set_size(ui->screen_1_btn_2, 314, 131);
    ui->screen_1_btn_2_label = lv_label_create(ui->screen_1_btn_2);
    lv_label_set_text(ui->screen_1_btn_2_label, "To_Screen_2\n");
    lv_label_set_long_mode(ui->screen_1_btn_2_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_2_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_2, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_2_label, LV_PCT(100));

    //Write style for screen_1_btn_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_btn_2, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_btn_2, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_2, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
#if LV_USE_GUIDER_SIMULATOR
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_2, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\dmsler.png", LV_PART_MAIN|LV_STATE_DEFAULT);
#else
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_2, "F:/dmsler.bin", LV_PART_MAIN|LV_STATE_DEFAULT);
#endif
    lv_obj_set_style_bg_image_opa(ui->screen_1_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_1_btn_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_2, lv_color_hex(0x6b41a4), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_2, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_btn_3
    ui->screen_1_btn_3 = lv_button_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_btn_3, 283, 6);
    lv_obj_set_size(ui->screen_1_btn_3, 191, 87);
    ui->screen_1_btn_3_label = lv_label_create(ui->screen_1_btn_3);
    lv_label_set_text(ui->screen_1_btn_3_label, "myButton");
    lv_label_set_long_mode(ui->screen_1_btn_3_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->screen_1_btn_3_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->screen_1_btn_3, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->screen_1_btn_3_label, LV_PCT(100));

    //Write style for screen_1_btn_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen_1_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_1_btn_3, lv_color_hex(0x2195f6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->screen_1_btn_3, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->screen_1_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_1_btn_3, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->screen_1_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
#if LV_USE_GUIDER_SIMULATOR
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_3, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\myself.jpg", LV_PART_MAIN|LV_STATE_DEFAULT);
#else
    lv_obj_set_style_bg_image_src(ui->screen_1_btn_3, "F:/myself.bin", LV_PART_MAIN|LV_STATE_DEFAULT);
#endif
    lv_obj_set_style_bg_image_opa(ui->screen_1_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->screen_1_btn_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->screen_1_btn_3, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->screen_1_btn_3, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->screen_1_btn_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->screen_1_btn_3, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_3
    ui->screen_1_img_3 = lv_image_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_img_3, 46, 164);
    lv_obj_set_size(ui->screen_1_img_3, 150, 150);
    lv_obj_add_flag(ui->screen_1_img_3, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_image_set_src(ui->screen_1_img_3, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Shinobu1.jpeg");
#else
    lv_image_set_src(ui->screen_1_img_3, "F:/Shinobu1.bin");
#endif
    lv_image_set_pivot(ui->screen_1_img_3, 50,50);
    lv_image_set_rotation(ui->screen_1_img_3, 0);

    //Write style for screen_1_img_3, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_1_img_3, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_1_img_3, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_4
    ui->screen_1_img_4 = lv_image_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_img_4, 840, 20);
    lv_obj_set_size(ui->screen_1_img_4, 150, 150);
    lv_obj_add_flag(ui->screen_1_img_4, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_image_set_src(ui->screen_1_img_4, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Tamayo.jpg");
#else
    lv_image_set_src(ui->screen_1_img_4, "F:/Tamayo.bin");
#endif
    lv_image_set_pivot(ui->screen_1_img_4, 50,50);
    lv_image_set_rotation(ui->screen_1_img_4, 0);

    //Write style for screen_1_img_4, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_1_img_4, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_1_img_4, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_5
    ui->screen_1_img_5 = lv_image_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_img_5, 431, 329);
    lv_obj_set_size(ui->screen_1_img_5, 150, 150);
    lv_obj_add_flag(ui->screen_1_img_5, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_image_set_src(ui->screen_1_img_5, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Maki.jpeg");
#else
    lv_image_set_src(ui->screen_1_img_5, "F:/Maki.bin");
#endif
    lv_image_set_pivot(ui->screen_1_img_5, 50,50);
    lv_image_set_rotation(ui->screen_1_img_5, 0);

    //Write style for screen_1_img_5, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_1_img_5, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_1_img_5, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_1_img_6
    ui->screen_1_img_6 = lv_image_create(ui->screen_1);
    lv_obj_set_pos(ui->screen_1_img_6, 684, 440);
    lv_obj_set_size(ui->screen_1_img_6, 150, 150);
    lv_obj_add_flag(ui->screen_1_img_6, LV_OBJ_FLAG_CLICKABLE);
#if LV_USE_GUIDER_SIMULATOR
    lv_image_set_src(ui->screen_1_img_6, "F:\\projects\\CSEE\\L_2025_Stm32H723ZGT6_Cmake\\Lib\\ThirdParty\\L_GUI_Guider\\import\\image\\Shinobu2.jpeg");
#else
    lv_image_set_src(ui->screen_1_img_6, "F:/Shinobu2.bin");
#endif
    lv_image_set_pivot(ui->screen_1_img_6, 50,50);
    lv_image_set_rotation(ui->screen_1_img_6, 0);

    //Write style for screen_1_img_6, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->screen_1_img_6, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->screen_1_img_6, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of screen_1.
    //lll

    //Update current screen layout.
    lv_obj_update_layout(ui->screen_1);

    //Init events for screen.
    events_init_screen_1(ui);
}

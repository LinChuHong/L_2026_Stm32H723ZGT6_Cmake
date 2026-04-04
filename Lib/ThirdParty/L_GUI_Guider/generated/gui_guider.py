# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import utime as time
import usys as sys
import lvgl as lv
import ustruct
import fs_driver

lv.init()

# Register display driver.
disp_drv = lv.sdl_window_create(1024, 600)
lv.sdl_window_set_resizeable(disp_drv, False)
lv.sdl_window_set_title(disp_drv, "Simulator (MicroPython)")

# Regsiter input driver
mouse = lv.sdl_mouse_create()

# Add default theme for bottom layer
bottom_layer = lv.layer_bottom()
lv.theme_apply(bottom_layer)

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_scale(v)

def anim_img_rotate_cb(obj, v):
    obj.set_rotation(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.binfont_create(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.image_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = lv.calendar.__cast__(e.get_current_target())
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RESULT.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    datetext = lv.label.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = lv.screen_active()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = datetext.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, datetext), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    calendar = lv.calendar.__cast__(e.get_current_target())
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if calendar.get_pressed_date(date) == lv.RESULT.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.remove_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            calendar.delete()

def ta_event_cb(e,kb):
    code = e.get_code()
    ta = lv.textarea.__cast__(e.get_target())
    if code == lv.EVENT.FOCUSED:
        kb.set_textarea(ta)
        kb.move_foreground()
        kb.remove_flag(lv.obj.FLAG.HIDDEN)

    if code == lv.EVENT.DEFOCUSED:
        kb.set_textarea(None)
        kb.move_background()
        kb.add_flag(lv.obj.FLAG.HIDDEN)

# Create screen_1
screen_1 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_size(1024, 600)
screen_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Chiyo_chan_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_btn_1
screen_1_btn_1 = lv.button(screen_1)
screen_1_btn_1_label = lv.label(screen_1_btn_1)
screen_1_btn_1_label.set_text("Button")
screen_1_btn_1_label.set_long_mode(lv.label.LONG.WRAP)
screen_1_btn_1_label.set_width(lv.pct(100))
screen_1_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_1_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_1_btn_1.set_pos(52, 42)
screen_1_btn_1.set_size(140, 81)
# Set style for screen_1_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/IMG_2066_140_81.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_btn_2
screen_1_btn_2 = lv.button(screen_1)
screen_1_btn_2_label = lv.label(screen_1_btn_2)
screen_1_btn_2_label.set_text("To_Screen_2\n")
screen_1_btn_2_label.set_long_mode(lv.label.LONG.WRAP)
screen_1_btn_2_label.set_width(lv.pct(100))
screen_1_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_1_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_1_btn_2.set_pos(224, 119)
screen_1_btn_2.set_size(314, 131)
# Set style for screen_1_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/dmsler_314_131.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_text_color(lv.color_hex(0x6b41a4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_btn_3
screen_1_btn_3 = lv.button(screen_1)
screen_1_btn_3_label = lv.label(screen_1_btn_3)
screen_1_btn_3_label.set_text("myButton")
screen_1_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_1_btn_3_label.set_width(lv.pct(100))
screen_1_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_1_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_1_btn_3.set_pos(283, 6)
screen_1_btn_3.set_size(191, 87)
# Set style for screen_1_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/myself_191_87.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_1_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_img_3
screen_1_img_3 = lv.image(screen_1)
screen_1_img_3.set_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu1_150_150.png"))
screen_1_img_3.add_flag(lv.obj.FLAG.CLICKABLE)
screen_1_img_3.set_pivot(50,50)
screen_1_img_3.set_rotation(0)
screen_1_img_3.set_pos(46, 164)
screen_1_img_3.set_size(150, 150)
# Set style for screen_1_img_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_img_3.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_img_4
screen_1_img_4 = lv.image(screen_1)
screen_1_img_4.set_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Tamayo_150_150.png"))
screen_1_img_4.add_flag(lv.obj.FLAG.CLICKABLE)
screen_1_img_4.set_pivot(50,50)
screen_1_img_4.set_rotation(0)
screen_1_img_4.set_pos(840, 20)
screen_1_img_4.set_size(150, 150)
# Set style for screen_1_img_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_img_4.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_img_5
screen_1_img_5 = lv.image(screen_1)
screen_1_img_5.set_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Maki_150_150.png"))
screen_1_img_5.add_flag(lv.obj.FLAG.CLICKABLE)
screen_1_img_5.set_pivot(50,50)
screen_1_img_5.set_rotation(0)
screen_1_img_5.set_pos(431, 329)
screen_1_img_5.set_size(150, 150)
# Set style for screen_1_img_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_img_5.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_1_img_6
screen_1_img_6 = lv.image(screen_1)
screen_1_img_6.set_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu2_150_150.png"))
screen_1_img_6.add_flag(lv.obj.FLAG.CLICKABLE)
screen_1_img_6.set_pivot(50,50)
screen_1_img_6.set_rotation(0)
screen_1_img_6.set_pos(684, 440)
screen_1_img_6.set_size(150, 150)
# Set style for screen_1_img_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_1_img_6.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_1.update_layout()
# Create screen_2
screen_2 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2.set_size(1024, 600)
screen_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Clannad_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_2
screen_2_btn_2 = lv.button(screen_2)
screen_2_btn_2_label = lv.label(screen_2_btn_2)
screen_2_btn_2_label.set_text("To_Screen_3")
screen_2_btn_2_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_2_btn_2_label.set_width(lv.pct(100))
screen_2_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_2.set_pos(955, 448)
screen_2_btn_2.set_size(72, 151)
# Set style for screen_2_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Frieren_anime_profile_72_151.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_bg_image_opa(247, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_text_color(lv.color_hex(0x4146d4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_2_btn_1
screen_2_btn_1 = lv.button(screen_2)
screen_2_btn_1_label = lv.label(screen_2_btn_1)
screen_2_btn_1_label.set_text("To_Screen_1\n")
screen_2_btn_1_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_2_btn_1_label.set_width(lv.pct(100))
screen_2_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_2_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_2_btn_1.set_pos(0, 0)
screen_2_btn_1.set_size(72, 151)
# Set style for screen_2_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_2_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Frieren_anime_profile_72_151.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_bg_image_opa(247, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_text_color(lv.color_hex(0x4146d4), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_2_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_2.update_layout()
# Create screen_3
screen_3 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_size(1024, 600)
screen_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu3_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_3_btn_1
screen_3_btn_1 = lv.button(screen_3)
screen_3_btn_1_label = lv.label(screen_3_btn_1)
screen_3_btn_1_label.set_text("To_Screen_4")
screen_3_btn_1_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_3_btn_1_label.set_width(lv.pct(100))
screen_3_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_3_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_3_btn_1.set_pos(0, 0)
screen_3_btn_1.set_size(132, 66)
# Set style for screen_3_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_3_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/tamayo_and_shinobu_132_66.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_text_color(lv.color_hex(0x2fda64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_3_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_3.update_layout()
# Create screen_4
screen_4 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_size(1024, 600)
screen_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu3_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_btn_1
screen_4_btn_1 = lv.button(screen_4)
screen_4_btn_1_label = lv.label(screen_4_btn_1)
screen_4_btn_1_label.set_text("To_Screen_5")
screen_4_btn_1_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_4_btn_1_label.set_width(lv.pct(100))
screen_4_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_4_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_4_btn_1.set_pos(0, 0)
screen_4_btn_1.set_size(132, 66)
# Set style for screen_4_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/tamayo_and_shinobu_132_66.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_text_color(lv.color_hex(0x2fda64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_4.update_layout()
# Create screen_5
screen_5 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_size(1024, 600)
screen_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu3_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_5_btn_1
screen_5_btn_1 = lv.button(screen_5)
screen_5_btn_1_label = lv.label(screen_5_btn_1)
screen_5_btn_1_label.set_text("To_Screen_6")
screen_5_btn_1_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_5_btn_1_label.set_width(lv.pct(100))
screen_5_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_5_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_5_btn_1.set_pos(0, 0)
screen_5_btn_1.set_size(132, 66)
# Set style for screen_5_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_5_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Tamayo_1_132_66.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_text_color(lv.color_hex(0x2fda64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_5_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_5.update_layout()
# Create screen_6
screen_6 = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_size(1024, 600)
screen_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Shinobu3_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_6_btn_1
screen_6_btn_1 = lv.button(screen_6)
screen_6_btn_1_label = lv.label(screen_6_btn_1)
screen_6_btn_1_label.set_text("To_Screen_1")
screen_6_btn_1_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_6_btn_1_label.set_width(lv.pct(100))
screen_6_btn_1_label.align(lv.ALIGN.CENTER, 0, 0)
screen_6_btn_1.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_6_btn_1.set_pos(0, 0)
screen_6_btn_1.set_size(132, 66)
# Set style for screen_6_btn_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_6_btn_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_bg_image_src(load_image(r"/Users/linchuhong/Desktop/L_2026_Stm32H723ZGT6_Cmake/Lib/ThirdParty/L_GUI_Guider/generated/MicroPython/Tamayo_1_132_66.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_color(lv.color_hex(0x2fda64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_6_btn_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_6.update_layout()

def screen_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen_1.add_event_cb(lambda e: screen_1_event_handler(e), lv.EVENT.ALL, None)

def screen_1_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.CLICKED):
        pass
        

screen_1_btn_1.add_event_cb(lambda e: screen_1_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_1_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_2, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_1_btn_2.add_event_cb(lambda e: screen_1_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_3, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_2_btn_2.add_event_cb(lambda e: screen_2_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_2_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_1, lv.SCR_LOAD_ANIM.FADE_ON, 200, 200, False)
screen_2_btn_1.add_event_cb(lambda e: screen_2_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_3_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_4, lv.SCR_LOAD_ANIM.MOVE_TOP, 200, 200, False)
screen_3_btn_1.add_event_cb(lambda e: screen_3_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_4_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_5, lv.SCR_LOAD_ANIM.MOVE_TOP, 200, 200, False)
screen_4_btn_1.add_event_cb(lambda e: screen_4_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_5_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_6, lv.SCR_LOAD_ANIM.MOVE_RIGHT, 200, 200, False)
screen_5_btn_1.add_event_cb(lambda e: screen_5_btn_1_event_handler(e), lv.EVENT.ALL, None)

def screen_6_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_1, lv.SCR_LOAD_ANIM.OVER_BOTTOM, 200, 200, False)
screen_6.add_event_cb(lambda e: screen_6_event_handler(e), lv.EVENT.ALL, None)

def screen_6_btn_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_1, lv.SCR_LOAD_ANIM.OVER_TOP, 200, 200, False)
screen_6_btn_1.add_event_cb(lambda e: screen_6_btn_1_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(screen_1)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)

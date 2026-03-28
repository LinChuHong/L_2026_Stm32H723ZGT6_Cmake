# Copyright 2025 NXP
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

# Create screen
screen = lv.obj()
g_kb_top_layer = lv.keyboard(lv.layer_top())
g_kb_top_layer.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
g_kb_top_layer.add_flag(lv.obj.FLAG.HIDDEN)
g_kb_top_layer.set_style_text_font(test_font("montserratMedium", 18), lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_size(1024, 600)
screen.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for screen, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_style_bg_image_src(load_image(r"C:\Users\chuhong\OneDrive\Documents\PlatformIO\Projects\L_2025_Stm32H723ZGT6_Arduino\myLib\L_GUI_Guider\generated\MicroPython\Chiyo_chan_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_2
screen_btn_2 = lv.button(screen)
screen_btn_2_label = lv.label(screen_btn_2)
screen_btn_2_label.set_text("")
screen_btn_2_label.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_btn_2_label.set_width(lv.pct(100))
screen_btn_2_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_2.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_2.set_pos(440, 434)
screen_btn_2.set_size(271, 128)
# Set style for screen_btn_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_bg_image_src(load_image(r"C:\Users\chuhong\OneDrive\Documents\PlatformIO\Projects\L_2025_Stm32H723ZGT6_Arduino\myLib\L_GUI_Guider\generated\MicroPython\IMG_2066_271_128.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_text_font(test_font("montserratMedium", 46), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_img_1
screen_img_1 = lv.image(screen)
screen_img_1.set_src(load_image(r"C:\Users\chuhong\OneDrive\Documents\PlatformIO\Projects\L_2025_Stm32H723ZGT6_Arduino\myLib\L_GUI_Guider\generated\MicroPython\IMG_2066_158_149.png"))
screen_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
screen_img_1.set_pivot(50,50)
screen_img_1.set_rotation(0)
screen_img_1.set_pos(838, 94)
screen_img_1.set_size(158, 149)
# Set style for screen_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_img_1.set_style_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_3
screen_btn_3 = lv.button(screen)
screen_btn_3_label = lv.label(screen_btn_3)
screen_btn_3_label.set_text("")
screen_btn_3_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_3_label.set_width(lv.pct(100))
screen_btn_3_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_3.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_3.set_pos(811, 413)
screen_btn_3.set_size(187, 151)
# Set style for screen_btn_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_bg_image_src(load_image(r"C:\Users\chuhong\OneDrive\Documents\PlatformIO\Projects\L_2025_Stm32H723ZGT6_Arduino\myLib\L_GUI_Guider\generated\MicroPython\dmsler_187_151.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_text_font(test_font("montserratMedium", 46), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_label_1
screen_label_1 = lv.label(screen)
screen_label_1.set_text("林楚洪love")
screen_label_1.set_long_mode(lv.label.LONG.SCROLL_CIRCULAR)
screen_label_1.set_width(lv.pct(100))
screen_label_1.set_pos(362, 232)
screen_label_1.set_size(98, 20)
# Set style for screen_label_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_label_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_font(test_font("SourceHanSerifSC_Regular", 20), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_ta_1
screen_ta_1 = lv.textarea(screen)
screen_ta_1.set_text("linchuhong")
screen_ta_1.set_placeholder_text("")
screen_ta_1.set_password_bullet("*")
screen_ta_1.set_password_mode(False)
screen_ta_1.set_one_line(True)
screen_ta_1.set_accepted_chars("")
screen_ta_1.set_max_length(32)
screen_ta_1.add_event_cb(lambda e: ta_event_cb(e, g_kb_top_layer), lv.EVENT.ALL, None)
screen_ta_1.set_pos(54, 205)
screen_ta_1.set_size(182, 25)
# Set style for screen_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_ta_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_text_align(lv.TEXT_ALIGN.LEFT, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_color(lv.color_hex(0xe6e6e6), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_border_side(lv.BORDER_SIDE.FULL, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_top(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_right(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_pad_left(4, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_ta_1.set_style_radius(4, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for screen_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
screen_ta_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
screen_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create screen_label_2
screen_label_2 = lv.label(screen)
screen_label_2.set_text("Label")
screen_label_2.set_long_mode(lv.label.LONG.WRAP)
screen_label_2.set_width(lv.pct(100))
screen_label_2.set_pos(67, 270)
screen_label_2.set_size(100, 32)
# Set style for screen_label_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_label_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_label_3
screen_label_3 = lv.label(screen)
screen_label_3.set_text("696969")
screen_label_3.set_long_mode(lv.label.LONG.WRAP)
screen_label_3.set_width(lv.pct(100))
screen_label_3.set_pos(649, 159)
screen_label_3.set_size(100, 32)
# Set style for screen_label_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_label_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_label_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_4
screen_btn_4 = lv.button(screen)
screen_btn_4_label = lv.label(screen_btn_4)
screen_btn_4_label.set_text("Button")
screen_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_4_label.set_width(lv.pct(100))
screen_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_4.set_pos(0, 0)
screen_btn_4.set_size(256, 60)
# Set style for screen_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_5
screen_btn_5 = lv.button(screen)
screen_btn_5_label = lv.label(screen_btn_5)
screen_btn_5_label.set_text("Button")
screen_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_5_label.set_width(lv.pct(100))
screen_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_5.set_pos(256, 0)
screen_btn_5.set_size(256, 60)
# Set style for screen_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_bg_color(lv.color_hex(0x098D6B), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_7
screen_btn_7 = lv.button(screen)
screen_btn_7_label = lv.label(screen_btn_7)
screen_btn_7_label.set_text("Button")
screen_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_7_label.set_width(lv.pct(100))
screen_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_7.set_pos(512, 0)
screen_btn_7.set_size(256, 60)
# Set style for screen_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_bg_color(lv.color_hex(0x2FDAAE), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_btn_6
screen_btn_6 = lv.button(screen)
screen_btn_6_label = lv.label(screen_btn_6)
screen_btn_6_label.set_text("Button")
screen_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_btn_6_label.set_width(lv.pct(100))
screen_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_btn_6.set_pos(768, 0)
screen_btn_6.set_size(256, 60)
# Set style for screen_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_btn_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_bg_color(lv.color_hex(0x2F35DA), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen.update_layout()
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
screen_4.set_style_bg_image_src(load_image(r"C:\Users\chuhong\OneDrive\Documents\PlatformIO\Projects\L_2025_Stm32H723ZGT6_Arduino\myLib\L_GUI_Guider\generated\MicroPython\Chiyo_chan_1024_600.png"), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_image_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4.set_style_bg_image_recolor_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_btn_4
screen_4_btn_4 = lv.button(screen_4)
screen_4_btn_4_label = lv.label(screen_4_btn_4)
screen_4_btn_4_label.set_text("Button")
screen_4_btn_4_label.set_long_mode(lv.label.LONG.WRAP)
screen_4_btn_4_label.set_width(lv.pct(100))
screen_4_btn_4_label.align(lv.ALIGN.CENTER, 0, 0)
screen_4_btn_4.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_4_btn_4.set_pos(0, 0)
screen_4_btn_4.set_size(256, 60)
# Set style for screen_4_btn_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_btn_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_bg_color(lv.color_hex(0x2FDA64), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_btn_5
screen_4_btn_5 = lv.button(screen_4)
screen_4_btn_5_label = lv.label(screen_4_btn_5)
screen_4_btn_5_label.set_text("Button")
screen_4_btn_5_label.set_long_mode(lv.label.LONG.WRAP)
screen_4_btn_5_label.set_width(lv.pct(100))
screen_4_btn_5_label.align(lv.ALIGN.CENTER, 0, 0)
screen_4_btn_5.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_4_btn_5.set_pos(256, 0)
screen_4_btn_5.set_size(256, 60)
# Set style for screen_4_btn_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_btn_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_bg_color(lv.color_hex(0x098D6B), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_btn_7
screen_4_btn_7 = lv.button(screen_4)
screen_4_btn_7_label = lv.label(screen_4_btn_7)
screen_4_btn_7_label.set_text("Button")
screen_4_btn_7_label.set_long_mode(lv.label.LONG.WRAP)
screen_4_btn_7_label.set_width(lv.pct(100))
screen_4_btn_7_label.align(lv.ALIGN.CENTER, 0, 0)
screen_4_btn_7.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_4_btn_7.set_pos(512, 0)
screen_4_btn_7.set_size(256, 60)
# Set style for screen_4_btn_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_btn_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_bg_color(lv.color_hex(0x2FDAAE), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create screen_4_btn_6
screen_4_btn_6 = lv.button(screen_4)
screen_4_btn_6_label = lv.label(screen_4_btn_6)
screen_4_btn_6_label.set_text("Button")
screen_4_btn_6_label.set_long_mode(lv.label.LONG.WRAP)
screen_4_btn_6_label.set_width(lv.pct(100))
screen_4_btn_6_label.align(lv.ALIGN.CENTER, 0, 0)
screen_4_btn_6.set_style_pad_all(0, lv.STATE.DEFAULT)
screen_4_btn_6.set_pos(768, 0)
screen_4_btn_6.set_size(256, 60)
# Set style for screen_4_btn_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
screen_4_btn_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_bg_color(lv.color_hex(0x2F35DA), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_radius(5, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_text_font(test_font("montserratMedium", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
screen_4_btn_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)

screen_4.update_layout()

def screen_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.SCREEN_LOADED):
        pass
        

screen.add_event_cb(lambda e: screen_event_handler(e), lv.EVENT.ALL, None)

def screen_btn_2_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

    if (code == lv.EVENT.PRESSING):
        pass
        

screen_btn_2.add_event_cb(lambda e: screen_btn_2_event_handler(e), lv.EVENT.ALL, None)

def screen_btn_3_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        

    if (code == lv.EVENT.PRESSING):
        pass
        

screen_btn_3.add_event_cb(lambda e: screen_btn_3_event_handler(e), lv.EVENT.ALL, None)

def screen_ta_1_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.VALUE_CHANGED):
        pass
        

    if (code == lv.EVENT.KEY):
        pass
        

screen_ta_1.add_event_cb(lambda e: screen_ta_1_event_handler(e), lv.EVENT.ALL, None)

def screen_btn_5_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen_4, lv.SCR_LOAD_ANIM.OVER_BOTTOM, 200, 200, False)
screen_btn_5.add_event_cb(lambda e: screen_btn_5_event_handler(e), lv.EVENT.ALL, None)

def screen_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
screen_4.add_event_cb(lambda e: screen_4_event_handler(e), lv.EVENT.ALL, None)

def screen_4_btn_4_event_handler(e):
    code = e.get_code()
    if (code == lv.EVENT.PRESSED):
        pass
        lv.screen_load_anim(screen, lv.SCR_LOAD_ANIM.MOVE_BOTTOM, 200, 200, False)
screen_4_btn_4.add_event_cb(lambda e: screen_4_btn_4_event_handler(e), lv.EVENT.ALL, None)

# content from custom.py

# Load the default screen
lv.screen_load(screen)

if __name__ == '__main__':
    while True:
        lv.task_handler()
        time.sleep_ms(5)

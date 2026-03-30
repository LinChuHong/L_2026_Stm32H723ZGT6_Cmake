/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
// #include "lv_fs_rawfs.h"

#if LV_USE_FS_RAWFS

const rawfs_size_t rawfs_file_count = 2;
rawfs_file_t rawfs_files[2] = {
	0x0, 0, 1228812, "/Chiyo_chan.bin",
	0x12c00c, 0, 202572, "/IMG_2066.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 
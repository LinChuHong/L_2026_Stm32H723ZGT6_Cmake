/*
* Copyright 2026 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"

#if LV_USE_FS_RAWFS

const rawfs_size_t rawfs_file_count = 9;
rawfs_file_t rawfs_files[9] = {
	0x0, 0, 1228812, "/Chiyo_chan.bin",
	0x12c00c, 0, 104076, "/IMG_2066.bin",
	0x145698, 0, 70638, "/IMG_2066.bin",
	0x156a86, 0, 84723, "/dmsler.bin",
	0x16b579, 0, 1228812, "/Chiyo_chan.bin",
	0x297585, 0, 1228812, "/Chiyo_chan.bin",
	0x3c3591, 0, 104076, "/IMG_2066.bin",
	0x3dcc1d, 0, 70638, "/IMG_2066.bin",
	0x3ee00b, 0, 84723, "/dmsler.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 
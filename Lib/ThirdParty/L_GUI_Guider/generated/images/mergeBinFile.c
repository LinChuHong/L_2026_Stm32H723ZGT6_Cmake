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

const rawfs_size_t rawfs_file_count = 12;
rawfs_file_t rawfs_files[12] = {
	0x0, 0, 1228812, "/Chiyo_chan.bin",
	0x12c00c, 0, 34032, "/IMG_2066.bin",
	0x1344fc, 0, 123414, "/dmsler.bin",
	0x152712, 0, 49863, "/myself.bin",
	0x15e9d9, 0, 67512, "/Shinobu1.bin",
	0x16f191, 0, 67512, "/Tamayo.bin",
	0x17f949, 0, 67512, "/Maki.bin",
	0x190101, 0, 67512, "/Shinobu2.bin",
	0x1a08b9, 0, 1843212, "/Clannad.bin",
	0x3628c5, 0, 32628, "/Frieren_anime_profile.bin",
	0x36a839, 0, 32628, "/Frieren_anime_profile.bin",
	0x3727ad, 0, 1843212, "/Shinobu3.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 
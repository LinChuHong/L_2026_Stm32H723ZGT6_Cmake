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
	0x15e9d9, 0, 172272, "/IMG_2023122011920175424181.bin",
	0x188ac9, 0, 217260, "/IMG_2023122011920175424335.bin",
	0x1bdb75, 0, 67512, "/Shinobu1.bin",
	0x1ce32d, 0, 67512, "/Tamayo.bin",
	0x1deae5, 0, 67512, "/Maki.bin",
	0x1ef29d, 0, 67512, "/Shinobu2.bin",
	0x1ffa55, 0, 1843212, "/Clannad.bin",
	0x3c1a61, 0, 32628, "/Frieren_anime_profile.bin",

};

#endif  /*LV_USE_FS_RAWFS*/ 
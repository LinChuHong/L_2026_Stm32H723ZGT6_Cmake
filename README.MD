Need to add lv_fs_rawfs.h to lvgl.h so that GUI Guider generated source file common.c can include it otherwise if we include lv_fs_rawfs.h directly in common.h it will be overrided everytime when we generate contents from the GUI Guider. 2026.4.2




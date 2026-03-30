#ifndef __LV_FS_RAWF__H_
#define __LV_FS_RAWF__H_
#ifdef __cplusplus
extern "C" {
#endif

// #if LV_USE_FS_RAWFS
#include "stdint.h"

typedef uint32_t rawfs_addr_t;
typedef uint32_t rawfs_size_t;

typedef struct _rawfs_file_t {
    rawfs_addr_t base;
    rawfs_addr_t offset;
    rawfs_size_t size;
    char * name;
} rawfs_file_t;





void lv_fs_rawfs_init(void);
// #endif

#ifdef __cplusplus
}
#endif
#endif

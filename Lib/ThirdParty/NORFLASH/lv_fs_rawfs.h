#ifndef __LV_FS_RAWF__H_
#define __LV_FS_RAWF__H_

#include "stdint.h"

typedef uint32_t rawfs_addr_t;
typedef uint32_t rawfs_size_t;

typedef struct _rawfs_file_t {
    rawfs_addr_t base;
    rawfs_addr_t offset;
    rawfs_size_t size;
    char * name;
} rawfs_file_t;




#ifdef __cplusplus
extern "C" {
#endif

void lv_fs_rawfs_init(void);

#ifdef __cplusplus
}
#endif


#endif

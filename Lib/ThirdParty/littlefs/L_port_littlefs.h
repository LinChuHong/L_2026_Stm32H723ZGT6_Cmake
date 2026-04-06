#ifndef L_PORT_LITTLEFS_H_
#define L_PORT_LITTLEFS_H_
#include "lfs.h"

#define LFS_FLASH_START   (15 * 1024 * 1024)  // last 1MB
#define LFS_FLASH_SIZE    (1  * 1024 * 1024)
#define LFS_BLOCK_SIZE    4096


// variables used by the filesystem
extern lfs_t lfs;
extern lfs_file_t lfsfile;
extern const struct lfs_config cfg;


#endif

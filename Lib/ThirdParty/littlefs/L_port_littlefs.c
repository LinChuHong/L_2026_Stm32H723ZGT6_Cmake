#include "L_port_littlefs.h"
#include "norflash.h"

int lfs_read(const struct lfs_config *c,
             lfs_block_t block,
             lfs_off_t off,
             void *buffer,
             lfs_size_t size)
{
    uint32_t addr = LFS_FLASH_START + block * 4096 + off;
    norflash_read(buffer, addr, size);
    return 0;
}

int lfs_prog(const struct lfs_config *c,
             lfs_block_t block,
             lfs_off_t off,
             const void *buffer,
             lfs_size_t size)
{
    uint32_t addr = LFS_FLASH_START + block * 4096 + off;

    norflash_write_nocheck((uint8_t*)buffer, addr, size);

    return 0;
}

int lfs_erase(const struct lfs_config *c, lfs_block_t block)
{
    uint32_t sec = (LFS_FLASH_START / 4096) + block;
    norflash_erase_sector(sec);
    return 0;
}

int lfs_sync(const struct lfs_config *c)
{
    return 0;
}

// variables used by the filesystem
lfs_t lfs;
lfs_file_t lfsfile;

// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = lfs_read,
    .prog  = lfs_prog,
    .erase = lfs_erase,
    .sync  = lfs_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = 4096,
    .block_count = LFS_FLASH_SIZE / 4096,
    .cache_size = 64,
    .lookahead_size = 64,
    .block_cycles = 500,
};

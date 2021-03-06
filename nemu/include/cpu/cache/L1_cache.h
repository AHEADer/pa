#ifndef L1_CACHE_H_INCLUDED
#define L1_CACHE_H_INCLUDED

#include "common.h"

#define CACHE_BLOCK_SIZE 64
#define CACHE_G_NUM 7
#define CACHE_WAYS 8
#define CACHE_OFFSET 6
#define CACHE_NUM 7
#define CACHE_TAG (27 - CACHE_NUM - CACHE_OFFSET)
#define CACHE_MASK 0x3fL

typedef union
{
    struct
    {
        uint32_t tag:CACHE_TAG;
        uint32_t g_num:CACHE_G_NUM;
        uint32_t off:CACHE_OFFSET;
    };
    uint32_t addr;
} cache_addr_l1;

typedef struct {
	uint8_t content[CACHE_BLOCK_SIZE];
	uint32_t tag:CACHE_TAG;
	bool valid;
} CACHE_BLOCK;

CACHE_BLOCK cache1_buf[1<<CACHE_G_NUM][CACHE_WAYS];

void init_L1_cache();
uint32_t L1_cache_read(hwaddr_t addr, size_t len);
void l1_cache_raddress(cache_addr_l1 l1_addr, uint8_t* data);
void L1_cache_write(hwaddr_t addr, size_t len, uint32_t data);
void l1_cache_waddress(cache_addr_l1 l1_addr, void* temp, uint8_t* mask);

#endif

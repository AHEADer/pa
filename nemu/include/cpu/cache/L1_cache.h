#ifndef L1_CACHE_H_INCLUDED
#define L1_CACHE_H_INCLUDED

#include "common.h"

#define CACHE_BLOCK_SIZE 64
#define CACHE_G_NUM 7
#define CACHE_WAYS 8
#define CACHE_OFFSET 6
#define CACHE_NUM 7
#define CACHE_TAG (32 - CACHE_NUM - CACHE_OFFSET)

typedef union
{
    struct
    {
        uint32_t tag:CACHE_TAG;
        uint32_t num:CACHE_G_NUM;
        uint32_t off:CACHE_OFFSET;
    };
    uint32_t addr;
} cache_addr_l1;

typedef struct {
	uint8_t content[CACHE_BLOCK_SIZE];
	bool valid;
} CACHE_BLOCK;

CACHE_BLOCK cache1_buf[1<<CACHE_G_NUM][CACHE_WAYS];
//TODO: 8-way set associative
	

void init_L1_cache();
//void L1_cache_read(hwaddr_t addr, size_t len);
//void L1_cache_write(hwaddr_t addr, size_t len);

#endif
#include "cpu/cache/L1_cache.h"
#include "misc.h"
#include "cpu/cache/L2_cache.h"


#define HW_MEM_SIZE (1<<27)

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

void init_L1_cache(){
	int i;
	for (i = 0; i < (1<<CACHE_G_NUM); ++i)
	{
		int j;
		for (j = 0; j <CACHE_WAYS ; ++j)
		{
			cache1_buf[i][j].valid = false;
		}
	}
}

uint32_t L1_cache_read(hwaddr_t addr, size_t len){
	uint32_t offset = addr & CACHE_MASK;
	uint8_t temp[2 * CACHE_BLOCK_SIZE]; //CACHE_BLOCK_SIZE here is 64
	cache_addr_l1 l1_addr;
	l1_addr.addr = addr;

	l1_cache_raddress(l1_addr, temp);

	if(offset + len > CACHE_BLOCK_SIZE) {
		/* data cross the cache size boundary*/
		l1_addr.addr += CACHE_BLOCK_SIZE;
		l1_cache_raddress(l1_addr, temp + CACHE_BLOCK_SIZE);
	}

	return unalign_rw(temp + offset, 4);
}

void l1_cache_raddress(cache_addr_l1 l1_addr, uint8_t* data){
	//Assert(l1_addr.addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!From cache read", l1_addr.addr);

	l1_addr.addr &= ~CACHE_MASK;
	static int rand = 0;
	//uint32_t tag = l1_addr.tag;
	uint32_t g_num = l1_addr.g_num;

	int i, valid_num = -1;
	for (i = 0; i < CACHE_WAYS; ++i)
	{
		if (cache1_buf[g_num][i].valid&&l1_addr.tag == cache1_buf[g_num][i].tag)
		{
			printf("hit cache\n");
			break;
		}
		else
			valid_num = i;
	}
	if (i!=CACHE_WAYS)
	{
		memcpy(data, cache1_buf[g_num][i].content, CACHE_BLOCK_SIZE);
	}
	else
	{
		if (valid_num < 0)
		{
			valid_num = rand;
			rand = (rand + 1) % (CACHE_WAYS);
		}
		cache1_buf[g_num][valid_num].valid = true;
		cache1_buf[g_num][valid_num].tag = l1_addr.tag;
				
		cache_addr_l2 l2;		
		l2.addr = l1_addr.addr;
		l2_cache_get(l2, (uint8_t*)(cache1_buf[g_num][i].content));
		
		memcpy(data, cache1_buf[g_num][i].content, CACHE_BLOCK_SIZE);
	}


}

void L1_cache_write(hwaddr_t addr, size_t len, uint32_t data){
	
	uint32_t offset = addr & CACHE_MASK;
	uint8_t temp[2 * CACHE_BLOCK_SIZE];
	uint8_t mask[2 * CACHE_BLOCK_SIZE];
	memset(mask, 0, 2 * CACHE_BLOCK_SIZE);

	dram_write(addr, len, data);

	cache_addr_l1 l1_addr;
	l1_addr.addr = addr;

	memset(mask, 0, CACHE_BLOCK_SIZE * 2);
	*(uint32_t *)(temp + offset) = data;
	memset(mask + offset, 1, len);
	l1_cache_waddress(l1_addr, temp, mask);
	if(offset + len > CACHE_BLOCK_SIZE)
		{
			/* data cross the cache boundary */
			l1_addr.addr += CACHE_BLOCK_SIZE;
			l1_cache_waddress(l1_addr, temp + CACHE_BLOCK_SIZE, mask + CACHE_BLOCK_SIZE);
	}
}

void l1_cache_waddress(cache_addr_l1 l1_addr, void* temp, uint8_t* mask){
	Assert(l1_addr.addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!From cache write", l1_addr.addr);
	int i;
	l1_addr.addr &= ~CACHE_MASK;
	for (i=0; i<CACHE_WAYS; ++i)
		if ((cache1_buf[l1_addr.g_num][i].valid) && (l1_addr.tag == cache1_buf[l1_addr.g_num][i].tag))
			break;
	if (i != CACHE_WAYS)
	{
		memcpy_with_mask(cache1_buf[l1_addr.g_num][i].content, temp, CACHE_BLOCK_SIZE, mask);
	}
	cache_addr_l2 l2;
	l2.addr = l1_addr.addr;
	l2_cache_put(l2, temp, mask, 0);
}

#include "cpu/cache/L1_cache.h"
#include "misc.h"

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
	//Assert(addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", addr);

	uint32_t offset = addr & CACHE_MASK;
	uint8_t temp[2 * CACHE_BLOCK_SIZE];
	cache_addr_l1 l1_addr;
	l1_addr.addr = addr;

	l1_cache_raddress(l1_addr, temp);

	if(offset + len > CACHE_BLOCK_SIZE) {
		/* data cross the cache size boundary*/
		l1_cache_raddress(l1_addr, temp + CACHE_BLOCK_SIZE);
	}
	return unalign_rw(temp + offset, 4);
}

void l1_cache_raddress(cache_addr_l1 l1_addr, uint8_t* data){
	Assert(l1_addr.addr < HW_MEM_SIZE, "physical address %x is outside of the physical memory!", l1_addr.addr);

	static int rand = 0;
	//uint32_t tag = l1_addr.tag;
	uint32_t g_num = l1_addr.g_num;

	int i, valid_num = -1;
	for (i = 0; i < CACHE_WAYS; ++i)
	{
		if (cache1_buf[g_num][i].valid)
		{
			if (l1_addr.tag == cache1_buf[g_num][i].tag)
			{
				printf("hit cache\n");
				break;
			}
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
		//memcpy(cache1_buf[g_num][i].content, dram_read(l1_addr.addr, 0),CACHE_BLOCK_SIZE); read data from dram
		memcpy(data, cache1_buf[g_num][i].content, CACHE_BLOCK_SIZE);
	}

}
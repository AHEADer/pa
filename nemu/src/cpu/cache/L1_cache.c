#include "cpu/cache/L1_cache.h"

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
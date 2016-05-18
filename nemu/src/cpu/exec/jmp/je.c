#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "je-template.h"
#undef DATA_BYTE

make_helper(je_i_b)
{
	return 2;
}
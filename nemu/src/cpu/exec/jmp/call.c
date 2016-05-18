#include "cpu/exec/helper.h"

#define DATA_BYTE 2
#include "call-template.h"
#undef DATA_BYTE

make_helper(call_i_l)
{
	return 2;
}
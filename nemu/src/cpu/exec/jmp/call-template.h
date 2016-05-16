#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(int len){
	cpu.esp -= DATA_BYTE;
	MEM_W(cpu.esp, cpu.eip+4);
	cpu.eip += op_src->val;
	print_asm_template1();
}

make_helper(concat5(instr, _, rel, _, SUFFIX))
{
    int len = concat4(decode_, i, _, SUFFIX)(eip+1);
    do_execute(len);
    return len+1;
}
#include "cpu/exec/template-end.h"
#include "cpu/exec/template-start.h"

#define instr call

static void do_execute ()
{
    //cpu.esp -= DATA_BYTE;
	//MEM_W(cpu.esp, cpu.eip+4);
	//cpu.eip += op_src->val;

	if (cpu.eflags.ZF)
    {
        int32_t tsrc = (DATA_TYPE_S)op_src->val;
        cpu.eip += tsrc;
//#if DATA_BYTE == 2
//        cpu.eip &= 0x0000ffff;
//#endif
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
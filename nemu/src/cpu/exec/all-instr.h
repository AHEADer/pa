#include "prefix/prefix.h"

#include "data-mov/mov.h"
#include "data-mov/xchg.h"
#include "data-mov/movsx.h"
#include "data-mov/movzx.h"
#include "data-mov/movs.h"
#include "data-mov/stos.h"
#include "data-mov/setcc.h"
#include "data-mov/cwd.h"
#include "data-mov/cbw.h"
#include "data-mov/cmovcc.h"
//#include "data-mov/lgdt.h"

#include "arith/dec.h"
#include "arith/inc.h"
#include "arith/neg.h"
#include "arith/imul.h"
#include "arith/mul.h"
#include "arith/idiv.h"
#include "arith/div.h"
#include "arith/sub.h"
#include "arith/add.h"
#include "arith/adc.h"
#include "arith/sbb.h"

#include "logic/and.h"
#include "logic/or.h"
#include "logic/not.h"
#include "logic/xor.h"
#include "logic/sar.h"
#include "logic/shl.h"
#include "logic/shr.h"
#include "logic/shrd.h"
#include "logic/shld.h"
#include "logic/test.h"
#include "logic/cmp.h"
#include "logic/clst.h"
#include "logic/cmps.h"
#include "logic/setst.h"

#include "string/rep.h"

#include "stack/push.h"
#include "stack/pop.h"

#include "misc/misc.h"

#include "jmp/jmp.h"
#include "jmp/je.h"
#include "jmp/jbe.h"
#include "jmp/jb.h"
#include "jmp/call.h"
#include "jmp/leave.h"
#include "jmp/ret.h"
#include "jmp/ja.h"
#include "jmp/jae.h"
#include "jmp/jcxz.h"
#include "jmp/jg.h"
#include "jmp/jge.h"
#include "jmp/jl.h"
#include "jmp/jle.h"
#include "jmp/jne.h"
#include "jmp/jno.h"
#include "jmp/jnp.h"
#include "jmp/jns.h"
#include "jmp/jo.h"
#include "jmp/jp.h"
#include "jmp/js.h"

#include "special/special.h"

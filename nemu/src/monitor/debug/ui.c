#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "common.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>

void cpu_exec(uint32_t);
extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_info(char *args);
static int cmd_x(char *args);
static int cmd_p(char *args);
static int cmd_bt(char *args);
static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Execute next program line (after stopping);", cmd_q},
	{ "info", "Generic command for showing things about the program being debugged", cmd_info},
	{ "d", "Delete some breakpoints or auto-display expressions", cmd_q},
	{ "x", "Print N 4bits from address starting with the value of EXPRESSION", cmd_x},
	{ "w", "Add a watchpoint to watch the value of EXPR", cmd_q},
	{ "bt", "Backtrace: display the program stack.", cmd_bt},
	{ "p", "Print: display the program variable.", cmd_p},
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

static int cmd_info(char *args)
{
    if (args)
        if (*args == 'r')
        {
            int i;
            for (i=0; i<4; ++i)
                printf("%s\t0x%x\t\t%d\n", regsl[i],  cpu.gpr[i]._32, cpu.gpr[i]._32);
            for (i=4; i<8; ++i)
                printf("%s\t0x%x\t\t0x%x\n", regsl[i],  cpu.gpr[i]._32, cpu.gpr[i]._32);
            printf("eip\t0x%x\n", cpu.eip);
        }
        else
        {
            printf("Invalid argument!\n");
        }
    else
        printf("A argument is needed!\n");
    return 0;
}

static int cmd_x(char *args)
{
	char *nstr = strtok(args, " ");
    char *expr = nstr + strlen(nstr) + 1;
    //int i, n=atoi(nstr);
    int dex = 0;
    if (*nstr == '0' && strlen(nstr)!=1)
    {
    	if (*(nstr+1)=='x')	//hex
    	{
    		int hex = atoi(nstr+2);
    		int count = 1;
    		while(hex!=0)
    		{
    			dex += (hex%10)*count;
    			hex = hex/10;
    			count = count*16;
    		}
    	}
    	else{ 
    		printf("Only hex & dex is allowed!\n");
    		return 0;
    	}
    }
    dex = atoi(nstr);
    if (strlen(expr)==0)
    {
    	printf("Argument lacks!\n");
    	return 0;
    }
    int mem = cal_str(expr);
    printf("0x%x:", mem);
    for (int i=0; i<dex; ++i)
    {
        printf("\t0x%08x", swaddr_read(mem + i*4, 4));
    }
    printf("\n");
	return 0;
}

static int cmd_p(char *args)
{
	char address_flag=0;
	if (!args)
    {
    	printf("Argument lacks!\n");
    	return 0;
    }
    //printf("nr_symtab_entry is %d\n",nr_symtab_entry );
    if (*args == '*')
    {
    	args++;
    	address_flag = 1;
    }
    int section_num = 0;	//to calculate the section numbers
    for(int i=0; i<8; i++)
    {
    	if (symtab[i].st_info == STT_SECTION)
    	{
    		section_num++;
    	}  	
    }
    int locate_num = search_strtab(args, strtab);
    if (locate_num)
    {
    	if (address_flag)
    	{
    		printf("value is 0x%08x\n", swaddr_read(symtab[locate_num+section_num].st_value, 4));
    	}
    	else{
    		printf("%x\n", symtab[locate_num+section_num].st_value);
    	}
    }
    else{
    	printf("not found this variable\n");
    }
	return 0;
}

static void cal_bt(swaddr_t eip, uint32_t ebp, int num)
{
    if (ebp == 0)
        return;

    int i;
    for (i = 0; i< nr_symtab_entry; ++i)
    {
        Elf32_Sym *t = symtab + i;
        if (ELF32_ST_TYPE(t->st_info) == STT_FUNC)
            if ((eip >= t->st_value) && (eip < (t->st_value + t->st_size)))
                break;
    }
    cpu.current_sreg = 2;
    char *func_name = strtab + (symtab+i)->st_name;
    int offset = eip - (symtab+i)->st_value;
    int argv[5];
    int j;
    swaddr_t address = ebp + 8;
    for (j=4; j>=0; --j)
    {
        if (address < HW_MEM_SIZE)
        {
            argv[j] = swaddr_read(address, 4);
            address += 4;
        }
        else
            argv[j] = 0;
    }
    printf("#%d 0x%08x %s+0x%x(argv1=%x, argv2=%x, argv3=%x, argv4=%x, argv5=%x)\n", num, eip, func_name, offset, argv[0], argv[1], argv[2], argv[3], argv[4]);
    cal_bt(swaddr_read(ebp + 4, 4), swaddr_read(ebp, 4), num+1);
}

static int cmd_bt(char *args)
{
    cal_bt(cpu.eip, cpu.ebp, 0);
    return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>
#include "monitor/expr.h"
#include <elf.h>

enum {
	NOTYPE = 256, EQ,
	MULDIV, ADDSUB,
	REGISTER,LBRACKET,RBRACKET,INT,SPACE,
	DEX, HEX
	/* TODO: Add more token types */

};


static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	//{" +",	NOTYPE},				// spaces
	{" +", SPACE},
	{"\\+", ADDSUB},
	{"\\-", ADDSUB},
	{"\\*", MULDIV},
	{"\\/", MULDIV},
	{"\\$+[a-z]{2,3}",REGISTER },
	{"[1-9][0-9]+", DEX},
	{"0[xX][0-9a-fA-F]+",HEX},
	{"==", EQ}						// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);//Use Extended Regular Expressions.
		if(ret != 0) {		//init error
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */
				 //NOTICE: cal step is in cal_str
				/*switch(rules[i].token_type) {
					default: panic("something wrong!");
				}*/

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("1st no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	//panic("please implement me");
	return 0;
}

int seach_register(char* reg)
{
	if (!strcmp(reg+1, "eip"))
	{
		return cpu.eip;
	}
	int i;
	for(i=0;i<8;i++)
	{
		if (!strcmp(reg+1, regsl[i]))
		{
			return cpu.gpr[i]._32;
		}
	}

	for(i=0;i<8;i++)
	{
		if (!strcmp(reg+1, regsw[i]))
		{
			return (int)cpu.gpr[i]._16;
		}
	}

	for(i=0;i<8;i++)
	{
		if (!strcmp(reg+1, regsb[i]))
		{
			return (int)cpu.gpr[i%4]._8[i/4];
		}
	}
	return 0;
}

int cal_str(char *e)
{
	int position = 0;
	int i,sum = 0;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;
				switch(rules[i].token_type) {
					case SPACE:
						break;
					case REGISTER:
						sum+=seach_register(substr_start);
						printf("%s\n",substr_start);
						break;
					case DEX:
						sum+=atoi(substr_start);
						break;
					case HEX:
						sum+=hex_to_dex(atoi(substr_start+2));
						break;
					default: panic("something wrong!");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
		}
	}

	return sum;
}

int hex_to_dex(int hex)
{
	int dex = 0;
	int factor = 1;
	while(hex!=0)
	{
    	dex += (hex%10)*factor;
    	hex = hex/10;
    	factor = factor*16;
   	}
   	return dex;
}

int search_strtab(char* args, char* strtab)
{
	int location = 1;
	while(1)
	{
		if (*strtab=='\0')
		{
			strtab++;
			if (*strtab=='\0')
			{
				break;
			}
		}
		if(strcmp(args, strtab)==0)
			return location;
		else{
			location++;
			while(*strtab!='\0')
				strtab++;
		}
	}
	
	return 0;
}
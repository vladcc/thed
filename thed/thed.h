#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#define MAX 16
#define MAGIC 10
#define CSV_LN_LEN (MAX * 5 + 2)
#define BIN 'b'
#define CSV 'c'
#define OFFSET 'o'
#define OR 'o'
#define LEN 'l'
#define LN_NUM 'l'
#define SRCH 's'
#define STRING 's'
#define ASCII 'a'
#define AND 'a'
#define XOR 'x'
#define NOT 'n'
#define UNICODE 'u'
#define REPLACE 'r'
#define REVERSE 'r'
#define EVERYTHING 'e'
#define MIDDLE 'm'
#define INFO 'i'
#define TO 't'
#define HEX 'h'
#define DEC 'd'
#define HELP '?'
#define VER 'v'
#define OPT_DUMP 1
#define OPT_CSV 2
#define OPT_CSV_TO_BIN 3
#define OPT_SRCH 4
#define OPT_REPLACE 5
#define OPT_HDMP_TO_BIN 6
#define OPT_FILE_INFO 7
#define OPT_CONV 8
#define OPT_PRNT_ASCII 9
#define OPT_ASCII_RVRS 10
#define OPT_STR_LEN 11
#define OPT_AND_OR_XOR 12
#define OPT_HELP 13
#define OPT_VER 14
#define BAD_OPT -1

#define hex_chars_to_byte(chars_ptr, out_byte_ptr)\
do{\
	byte btmp = 0;\
	char chtmp = *(chars_ptr);\
	if ('0' <= chtmp && '9' >= chtmp)\
		btmp |= chtmp - '0';\
	else if ('A' <= chtmp && 'F' >= chtmp)\
		btmp |= chtmp - 'A' + MAGIC;\
	btmp <<= 4;\
	chtmp = *((chars_ptr) + 1);\
	if ('0' <= chtmp && '9' >= chtmp)\
		btmp |= chtmp - '0';\
	else if ('A' <= chtmp && 'F' >= chtmp)\
		btmp |= chtmp - 'A' + MAGIC;\
	*(out_byte_ptr) = btmp;\
} while(0)

const char * BINTBL[] = {
	"0000 ", "0001 ", "0010 ", "0011 ",
	"0100 ", "0101 ", "0110 ", "0111 ",
	"1000 ", "1001 ", "1010 ", "1011 ",
	"1100 ", "1101 ", "1110 ", "1111 "
						};
const char * NON_PRINT_ASCII[] =	{
	"(nul)",	"(soh)",	"(stx)",	"(etx)",	"(eot)",
	"(enq)", 	"(ack)", 	"(bel)",	"(bs)", 	"(ht)",   
	"(nl)",		"(vt)", 	"(np)", 	"(cr)", 	"(so)",  
	"(si)", 	"(dle)", 	"(dc1)", 	"(dc2)", 	"(dc3)",
	"(dc4)", 	"(nak)", 	"(syn)", 	"(etb)", 	"(can)", 
	"(em)", 	"(sub)", 	"(esc)", 	"(fs)", 	"(gs)", 
	"(rs)", 	"(us)",		"Space"
									};				
const char OFFSET_TBL[] = " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F";									
const char HEXTBL[] = "0123456789ABCDEF";
const char ORIG_EXE_NAME[] = "thed";
const char EXE_VER[] = "1.01";
const char DASH = '-';
const char END = '_';
const char SPRT = '|';

// globals for program arguments
static const char * input_file = NULL;
static const char * output_file = NULL;
static const char * search_rep_seq = NULL;
static const char * srch_rep_mode = NULL;
static const char * replace_only_seq = NULL;
static long offset = 0L;
static long line_num = 0L;
static int from_base = 0;
static int to_base = 0;
bool replace_everything = false;
bool negative_line = false;
bool hex_dump_middle = false;

typedef uint8_t byte;

// the LINE struct acts as a char buffer for a one hex view line
struct LINE
{
	char hxstr[MAX * 3 + 1];
	char chstr[MAX + 3];
};
typedef struct LINE LINE;

int check_args(int argc, char * argv[]);
FILE * open_file(const char * fname, const char * accs);
void hex_dump(const char * fname, long line_num);
void csv_dump(const char * fin, const char * fout);
void hex_dump_to_bin(const char * fin, const char * fout);
void csv_dump_to_bin(const char * fin, const char * fout);
void search(const char mode, const char * fname, const char * sequence);
void replace(const char mode, const char * fname, const char * sequence);
void print_conv_nums(const char * str, int from_base, int to_base);
void base_convert(unsigned long long num, int base);
void print_ascii(const char * str, bool whole_table, bool reverse);
byte * hexstr_to_bytes(const char * str, int * out_buff_size);
char * astr_to_ucstr(const char * str, int * out_buff_size);
void print_file_info(const char * fname);
void and_or_xor(char opt, const char * num1, const char * num2);
void bit_not(const char * num);
void check_hex_str(const char * num);
void print_strlen(const char * str);
void print_help(const char * exe_name);
void print_ver(void);

/* thed.c -- terminal hex editor
 * thed is a console tool for dumping, looking through, and editing the binary values of a file.
 * thed can dump files either in a standard hex view format with values on the left and their
 * string represantation on the right, or as comma separated values. thed can also reverse this
 * process and create binaries from both hex and csv dumps. When generating a binary from a hex dump, 
 * thed reads only the hex. Any changes in the strings section has no effect. 
 * thed can also search for and replace ASCII, Unicode and byte sequences.
 * Number base conversion, bitwise operations, and the ASCII table are added for convenience. 
 * Compiled with: gcc thed.c -o thed.exe -Wall -s -O2 -m32 */

#include "thed.h"

// code
int main(int argc, char * argv[])
{	
	// main() is used only for dispatching
	switch (check_args(argc, argv))
	{
		case OPT_DUMP:
			hex_dump(input_file, line_num);
			break;
		case OPT_SRCH:
			search(*(srch_rep_mode), input_file, search_rep_seq);
			break;
		case OPT_REPLACE:
			replace(*(srch_rep_mode), input_file, search_rep_seq);
			break;
		case OPT_HDMP_TO_BIN:
			hex_dump_to_bin(input_file, output_file);
			break;
		case OPT_CSV:
			csv_dump(input_file, output_file);
			break;
		case OPT_CSV_TO_BIN:
			csv_dump_to_bin(input_file, output_file);
			break;
		case OPT_FILE_INFO:
			print_file_info(input_file);
			break;
		case OPT_CONV:
			print_conv_nums(argv[2], from_base, to_base);
			break;
		case OPT_PRNT_ASCII:
			print_ascii(argv[2], (2 == argc) ? true : false, false);
			break;
		case OPT_ASCII_RVRS:
			print_ascii(argv[2], (2 == argc) ? true : false, true);
			break;
		case OPT_STR_LEN:
			print_strlen(argv[2]);
			break;
		case OPT_AND_OR_XOR:
			and_or_xor(argv[1][1], argv[2], argv[3]);
			break;
		case OPT_HELP:
			print_help(argv[0]);
			break;
		case OPT_VER:
			print_ver();
			break;
		case BAD_OPT:
			fprintf(stderr, "Err: bad option.\n");
			fprintf(stdout, "Use: %s <file> [<options> ...]\n", argv[0]);
			fprintf(stdout, "%s -%c for help.\n", argv[0], HELP);
			break;
		default:
			break;
	}
	
	return 0;
}

int check_args(int argc, char * argv[])
{
	/* here arguments are parsed and the results
	 * gets returned to main() */
	
	if (argc < 2)
		return BAD_OPT;
	
	int opt = OPT_DUMP; // assume by deafult
	
	// assume argv[1] is the input file
	input_file = argv[1];
	
	int i; // look for -o, -l, -s, -r and -i
	for (i = 2; i < argc; ++i) 
	{
		if (DASH == argv[i][0])
		{
			if (OFFSET == argv[i][1]) // -o
			{
				if ( (i + 1) < argc )	// if there is something after -o
					offset = strtol(argv[i + 1], NULL, 16);
			}	
			else if (LN_NUM == argv[i][1]) // -l
			{
				if ( (i + 1) < argc )	// if there is something after -l
				{
					if ('-' == argv[i + 1][0])
					{
						// replace the '-' so it won't be parsed as an argument
						negative_line = true;
						argv[i + 1][0] = '+'; 
					}
					line_num = strtol(argv[i + 1], NULL, 10);
					
					if (MIDDLE == argv[i][2]) // -lm
						hex_dump_middle = true;
				}
			}	
			else if (SRCH == argv[i][1]) // -s
			{
				if ( (i + 1) < argc ) // if there is a search sequence
				{
					srch_rep_mode = &(argv[i][2]);
					search_rep_seq = argv[i + 1];
					opt = OPT_SRCH;
				}
				else
				{
					fprintf(stderr, "Err: no search sequence.\n");
					exit(1);
				}	
			}
			else
				opt = BAD_OPT;
				
			if (REPLACE == argv[i][1] && EVERYTHING != argv[i][2]) // -r
			{
				if ( (i + 1) < argc ) // if there is a replace sequence
				{
					srch_rep_mode = &(argv[i][2]);
					search_rep_seq = argv[i + 1];
					opt = OPT_REPLACE;
				}
				else
				{
					fprintf(stderr, "Err: no replace sequence.\n");
					exit(1);
				}	
			}
			
			if (REPLACE == argv[i][1] && EVERYTHING == argv[i][2]) // -re 
			{
				if ( (i + 1) < argc && srch_rep_mode) // if there is a search sequence after it
				{
					replace_everything = true;
					replace_only_seq = argv[i + 1];
					opt = OPT_SRCH;
				}
				else
				{
					fprintf(stderr, "Err: no replace sequence.\n");
					exit(1);
				}	
			}
			
			if (INFO == argv[i][1]) // -i
				opt = OPT_FILE_INFO;
		}	
	} 
	
	// parse for thed -(opt)
	if (DASH == argv[1][0])
	{
		// parse for number base conversion
		if (TO == argv[1][2] && 2 < argc)
		{
			switch (argv[1][1]) // from base
			{
				case HEX:
					from_base = 16;
					break;
				case DEC:
					from_base = 10;
					break;
				case BIN:
					from_base = 2;
					break;
				default:
					fprintf(stderr, "Err: Invalid base option.\n");
					exit(1);
					break;
			}
			
			switch (argv[1][3]) // to base
			{
				case HEX:
					to_base = 16;
					break;
				case DEC:
					to_base = 10;
					break;
				case BIN:
					to_base = 2;
					break;
				default:
					fprintf(stderr, "Err: Invalid base option.\n");
					exit(1);
					break;
			}
			return OPT_CONV;
		}
		else if (TO == argv[1][2] && 2 == argc)
		{
			fprintf(stderr, "Err: no number.\n");
			exit(1);
		}
		
		switch (argv[1][1])
		{
			case BIN: // -b
				input_file = argv[2];
				output_file = argv[3];
				opt = OPT_HDMP_TO_BIN;
				break;
			case CSV: // -c					
				input_file = argv[2];
				output_file = argv[3];
				if (BIN == argv[1][2])
					opt = OPT_CSV_TO_BIN;
				else
					opt = OPT_CSV;
				break;
			case ASCII: // -a
				if (REVERSE == argv[1][2])
					opt = OPT_ASCII_RVRS; // -ar
				else
					opt = OPT_PRNT_ASCII;
				break;
			case HELP: // -?
				opt = OPT_HELP;
				break;
			case VER: // -v
				opt = OPT_VER;
				break;
			case STRING: // -sl
				if (LEN == argv[1][2])
				{
					if (2 < argc)
						opt = OPT_STR_LEN;
					else
					{
						fprintf(stderr, "Err: no string.\n");
						exit(1);
					}
				}
				break;
			default:
				opt = BAD_OPT;
				break;
		}
	}
	
	// and, or, xor, not
	// +a, +o, +x, +n
	if ('+' == argv[1][0])
	{
		if (4 == argc)
			opt = OPT_AND_OR_XOR;
		else if (NOT == argv[1][1] && 3 == argc)
			opt = OPT_AND_OR_XOR;
	}
	
	return opt;
}

FILE * open_file(const char * fname, const char * accs)
{
	// opens a file and sets the file pointer
	FILE * fp;
	
	if ( !(fp = fopen(fname, accs)) )
	{
		fprintf(stderr, "Couldn't open file %s\n", fname);
		exit(1);
	}
	
	if (offset > 0) // set offset
		fseek(fp, offset, SEEK_SET);
	
	return fp;
}

void csv_dump_to_bin(const char * fin, const char * fout)
{
	// generates a binary from a csv dump
	
	FILE * fpin, * fpout;
	byte buff[MAX];
	char csv_line[CSV_LN_LEN];
	
	fpin = open_file(fin, "r");		// open input file for character read
	fpout = open_file(fout, "wb");	// open output file for binary write
	
	while (fgets(csv_line, CSV_LN_LEN, fpin) != NULL)
	{
		int digit, bytes, pos;
		const int step = 5;
		for (pos = 0, bytes = 0; bytes < MAX; pos += step, ++bytes) 
		{
			int tmp_ch = (csv_line[pos] == '\n') ? csv_line[pos+1] : csv_line[pos];
			if ('\0' == tmp_ch || END == tmp_ch)
				break;
			
			// calculate digit values from characters
			digit = pos + 2;
			if ('0' <= csv_line[digit] && '9' >= csv_line[digit])
				buff[bytes] = csv_line[digit] - '0';
			else if ('A' <= csv_line[digit] && 'F' >= csv_line[digit])
				buff[bytes] = csv_line[digit] - 'A' + MAGIC;
				
			buff[bytes] <<= 4; // shift four to make space for lower nibble
			
			++digit;
			if ('0' <= csv_line[digit] && '9' >= csv_line[digit])
				buff[bytes] |= csv_line[digit] - '0';
			else if ('A' <= csv_line[digit] && 'F' >= csv_line[digit])
				buff[bytes] |= csv_line[digit] - 'A' + MAGIC;
		}
		
		if (fwrite(buff, sizeof(byte), bytes, fpout) != bytes) // write to output file
		{
			fprintf(stderr, "Err: write error. Writing to %s has failed.\n", fout);
			fclose(fpin);
			fclose(fpout);
			exit(1);
		}
	}
	fprintf(stdout, "%s was written successfully.\n", output_file);
	
	fclose(fpin);
	fclose(fpout);
}

void hex_dump_to_bin(const char * fin, const char * fout)
{
	// generates a binary from a hex dump
	
	FILE * fpin, * fpout;
	byte buff[MAX];
	LINE ln;
	size_t ln_hex_len = sizeof(ln.hxstr) / sizeof(char);
	size_t ln_char_len = sizeof(ln.chstr) / sizeof(char);
	
	fpin = open_file(fin, "r");		// open dump file for character read
	fpout = open_file(fout, "wb");	// open output file for binary write
	
	while (fgets(ln.hxstr, ln_hex_len, fpin) != NULL && END != ln.hxstr[0])
	{
		int digit, bytes, pos;
		const int step = 3;
		for (pos = 0, bytes = 0; bytes < MAX; pos += step, ++bytes) 
		{
			int tmp_ch = ln.hxstr[pos+1];
			if (END == tmp_ch || pos >= ln_hex_len-1)
				break;
				
			// calculate digital values from characters
			digit = pos + 1;
			if ('0' <= ln.hxstr[digit] && '9' >= ln.hxstr[digit])
				buff[bytes] = ln.hxstr[digit] - '0';
			else if ('A' <= ln.hxstr[digit] && 'F' >= ln.hxstr[digit])
				buff[bytes] = ln.hxstr[digit] - 'A' + MAGIC;
				
			buff[bytes] <<= 4; // shift four bytes to make space for lower nibble
			
			++digit;	
			if ('0' <= ln.hxstr[digit] && '9' >= ln.hxstr[digit])
				buff[bytes] += ln.hxstr[digit] - '0';
			else if ('A' <= ln.hxstr[digit] && 'F' >= ln.hxstr[digit])
				buff[bytes] += ln.hxstr[digit] - 'A' + MAGIC;
		}
		
		if (fwrite(buff, sizeof(byte), bytes, fpout) != bytes) // write to output file
		{
			fprintf(stderr, "Err: write error. Writing to %s has failed.", fout);
			fclose(fpin);
			fclose(fpout);
			exit(1);
		}
		
		// get the string from the string section in the dump file and dispose of it
		fgets(ln.chstr, ln_char_len, fpin);
	}
	
	fprintf(stdout, "%s was written successfully.\n", output_file);
	
	fclose(fpin);
	fclose(fpout);
}

void csv_dump(const char * fin, const char * fout)
{
	// generates a csv dump from binary
	
	FILE * fpin, * fpout;
	byte buff[MAX];
	char csv_line[CSV_LN_LEN];
	int n;
	
	fpin = open_file(fin, "rb");	// open input file for binary read
	fpout = open_file(fout, "w");	// open output file for character write
	
	while ( (n = fread(buff, sizeof(byte), MAX, fpin)) > 0 )
	{
		
		int i, j;
		for (i = 0, j = 0; i < n; ++i) // prepare csv string
		{
			csv_line[j++] = '0';
			csv_line[j++] = 'x';	
			csv_line[j++] = HEXTBL[(buff[i] >> 4) & 0xF];
			csv_line[j++] = HEXTBL[buff[i] & 0xF];
			csv_line[j++] = ',';
		}
		csv_line[j++] = '\n';
		csv_line[j] = '\0';
		
		fprintf(fpout, "%s", csv_line); // write to output file
	}
	
	if (ferror(fpin))
	{
		fprintf(stderr, "Err: read error.\n");
		exit(1);
	}
	
	// mark end of dump
	putc(END, fpout);
	putc(END, fpout);
	
	fprintf(stdout, "CSV written to %s.\n", output_file); // success
	fclose(fpin);
	fclose(fpout);
}

void hex_dump(const char * fname, long line_num)
{
	// generates a hex dump from binary
	
	FILE * fp;
	byte buff[MAX];
	LINE ln;
	int n;
	long lines_done = 0L;
	bool is_n_eof = false;
	
	fp = open_file(fname, "rb"); // open for binary read
	
	if (hex_dump_middle) // -lm
	{
		offset -= line_num * 16;
		line_num *= 2;
		++line_num;
		fseek(fp, offset, SEEK_SET);
		negative_line = false;
	}
	
	if (negative_line) // -l -<line num>
	{	
		offset -= line_num * 16;
		fseek(fp, offset, SEEK_SET);
	}
	
	if (0 > offset)
	{
		fprintf(stderr, "Err: negative offset.\n");
		exit(1);
	}
	
	// print offset table and first byte offset to stderr
	// so it won't get in the file if redirection is used
	fprintf(stderr, " First byte offset: %#lx\n", offset);
	fprintf(stderr, "%s\n\n", OFFSET_TBL);

	while ( (n = fread(buff, sizeof(byte), MAX, fp)) > 0 )
	{	
		if (line_num > 0)
			if (line_num == lines_done)
			{
				// don't print end characters if
				is_n_eof = true;
				break;
			}
		
		int i, j;
		for (i = 0, j = 0; i < n; ++i) // preapare hex string
		{
			ln.hxstr[j++] = (i % 4) ? ' ' : SPRT;	
			ln.hxstr[j++] = HEXTBL[(buff[i] >> 4) & 0xF];
			ln.hxstr[j++] = HEXTBL[buff[i] & 0xF];
		}
		
		if (n < MAX) // n < MAX if it's at the eof, or error
		{
			ln.hxstr[j++] = (i % 4) ? ' ' : SPRT;
			// mark end of dump
			ln.hxstr[j++] = END;
			ln.hxstr[j++] = END;
			/* if the buffer fits perfectly we won't detect eof
			 * in the current string and end marks won't be printed 
			 * since !(n < MAX) */
			is_n_eof = true;
		}
		
		ln.hxstr[j] = '\0';
		
		i = 0;	
		ln.chstr[i] = SPRT;					
		for (i = 1, j = 0; j < n; ++i, ++j)	// prepare the string section
			ln.chstr[i] = !iscntrl(buff[j]) ? buff[j] : '.';
			
		ln.chstr[i] = '\0';
		
		// print the whole thing
		fprintf(stdout, "%-*s%-*s\n", MAX*3, ln.hxstr, MAX, ln.chstr); 
		++lines_done;
	}
	
	if (ferror(fp))
	{
		fprintf(stderr, "Err: read error.\n");
		exit(1);
	}
	
		// print ending characters if n was never < MAX
		if (!is_n_eof)
			fprintf(stdout, "%c%c\n", END, END);
			
	fclose(fp);
}

void search(const char mode, const char * fname, const char * sequence)
{
	// string and byte sequence search
	
	// only -sa -su -sb are accepted
	if (BIN != mode && ASCII != mode && UNICODE != mode)
	{
		fprintf(stderr, "Err: invalid search mode.\n");
		exit(1);
	}
	
	FILE * fp;
	char ch;
	int n;
	byte * byte_buff;
	int buff_len = 0;
	
	/* the value in unicode_mode is used to calculate if we need to
	 * skip every second byte in the buffer when comparing the strings */
	int unicode_mode = (UNICODE == mode) ? 1 : 0;
	int srch_str_len = strlen(sequence);
	int srch_seq_len = srch_str_len * (unicode_mode + 1);
	unsigned int matches_found = 0;
	unsigned int matches_replaced = 0;
	
	// open for binary read
	fp = open_file(fname, "rb");
	
	if (BIN == mode)
	{
		byte_buff = hexstr_to_bytes(sequence, &buff_len);
		srch_seq_len = buff_len;
		srch_str_len = buff_len;
		sequence = (char *)byte_buff;
	}
	
	// create char buffer
	char buff[srch_seq_len];
	
	// read bytes one by one until the first one matches
	while ( (n = fread(&ch, sizeof(char), 1, fp)) > 0 )
	{
		if (ch == sequence[0])
		{
			// get match offset
			long saved_match_pos;
			fseek(fp, -1L, SEEK_CUR); 	// decrement one place because of n above
			saved_match_pos = ftell(fp);// that's the start of our match
			
			// read a string as long as the search sequence
			// NOTE: no '\0' character is read
			if (fread(buff, sizeof(char), srch_seq_len, fp) != srch_seq_len)
			{
				if (feof(fp))
					goto matches_and_go;
				else
					fprintf(stderr, "Err: read error.\n");
			}
			
			// compare the strings
			int i, j;
			bool is_match = true;
			for (i = 0, j = 0; i < srch_str_len; ++i, j += 1 + unicode_mode)
			{
				if (buff[j] != sequence[i])
				{
					is_match = false;
					break;
				}
			}
			
			// print match offset
			if (is_match)
			{
				fprintf(stdout, "Match found at: %#lx\n", saved_match_pos);
				++matches_found;
				is_match = false;
				
				// if -re go ahead and replace what is found
				if (replace_everything)
				{
					offset = saved_match_pos;
					replace(mode, fname, replace_only_seq);
					fprintf(stdout, "Match replaced.\n");
					++matches_replaced;
				}
			}
			/* set file pointer just one byte after the match of the first byte 
			 * so we won't skip recurring patterns */
			fseek(fp, saved_match_pos + 1L, SEEK_SET);
		}
	}
	
	if (ferror(fp))
	{
		fprintf(stderr, "Err: read error.\n");
		exit(1);
	}
	
	matches_and_go:
		// print number of matches found
		fprintf(stdout, "%u %s found.\n", matches_found, (matches_found != 1) ? "matches" : "match");
		
		if (replace_everything) // print number of replaced matches
			fprintf(stdout, "%u %s replaced.\n", matches_replaced, (matches_replaced != 1) ? "matches" : "match");
	
	if (BIN == mode)
		free((void *)sequence);
			
	fclose(fp);
}

void replace(const char mode, const char * fname, const char * sequence)
{
	// writes a sequence starting from an offset in the file
	
	// check replace mode
	// in case of -re it's the same as the search mode
	if (BIN != mode && ASCII != mode && UNICODE != mode)
	{
		fprintf(stderr, "Err: invalid replace mode.\n");
		exit(1);
	}
	
	FILE * fp;
	int buff_len = 0;
	
	if (BIN == mode)	// convert a string of hex number to binary values
		sequence = (char *)hexstr_to_bytes(sequence, &buff_len);
		
	if (ASCII == mode)
		buff_len = strlen(sequence);
		
	if (UNICODE == mode) // convert an ASCII string to Unicode
		sequence = astr_to_ucstr(sequence, &buff_len);
		
	// binary read/write allows random access
	fp = open_file(fname, "rb+");
	fseek(fp, offset, SEEK_SET);
	
	if (fwrite(sequence, sizeof(char), buff_len, fp) < buff_len )
	{
		fprintf(stderr, "Err: write error.\n");
		fclose(fp);
		exit(1);
	}
		
	if (!replace_everything) // in case of a single replace
		fprintf(stdout, "Replace successful.\n");
		
	if (BIN == mode)
		free((void *)sequence);
		
	if (UNICODE == mode)
		free((void *)sequence);
		
	fclose(fp);
}

char * astr_to_ucstr(const char * str, int * out_buff_size)
{
	/* converts an ASCII string to Unicode by inserting 
	 * 00 bytes inbetween characters 
	 * works for characters from the ASCII table only 
	 * returns a pointer to the string buffer, and writes down
	 * the buffer size at &out_buff_size */
	
	int i, j;
	char * char_buff;
	int str_len = strlen(str);
	
	// allocate buffer
	if (!(char_buff = (char *)malloc(str_len * 2)) )
	{
		fprintf(stderr, "Err: unable to allocate character buffer.\n");
		exit(1);
	}
	
	// make every second byte 0
	for (i = 0, j = 0; i < str_len; ++i) 
	{
		char_buff[j++] = str[i];
		char_buff[j++] = 0;
	}
	
	*out_buff_size = j; // save buffer size
	return char_buff;
}

byte * hexstr_to_bytes(const char * str, int * out_buff_size)
{
	/* converts a hex ASCII string to binary values
	 * returns a pointer to the buffer with the values
	 * and writes down the buffer size at &out_buff_size */
	
	int i, j, k;
	char str_hex[strlen(str)];
	char ch;
	byte * byte_buff;
	
	for (i = 0, j = 0; '\0' != str[i]; ++i) 
	{
		
		ch = toupper(str[i]);
		
		// get only hex characters	
		if (('0' <= ch && '9' >= ch) || ('A' <= ch && 'F' >= ch))
			str_hex[j++] = ch;
	}
	
	if (j % 2)
	{
		fprintf(stderr, "Err: odd number of hex digits.\n");
		exit(1);
	}
	
	str_hex[j] = '\0';
	
	// allocate buffer
	if (!(byte_buff = (byte *)malloc(j)) )
	{
		fprintf(stderr, "Err: unable to allocate byte buffer.\n");
		exit(1);
	}
	
	// convert hex characters to binary values
	for (i = 0, k = 0; i < j; i += 2, ++k) 
	{
		// same code as in hex_dump_to_bin
		if ('0' <= str_hex[i] && '9' >= str_hex[i])
			byte_buff[k] = str_hex[i] - '0';
		else if ('A' <= str_hex[i] && 'F' >= str_hex[i])
			byte_buff[k] = str_hex[i] - 'A' + MAGIC;
			
		byte_buff[k] <<= 4; // shift four bytes to make space for lower nibble
			
		if ('0' <= str_hex[i + 1] && '9' >= str_hex[i + 1])
			byte_buff[k] += str_hex[i + 1] - '0';
		else if ('A' <= str_hex[i + 1] && 'F' >= str_hex[i + 1])
			byte_buff[k] += str_hex[i + 1] - 'A' + MAGIC;
	}
	
	*out_buff_size = k; // save buffer size

	return byte_buff;
}

void print_conv_nums(const char * str, int from_base, int to_base)
{
	// number conversion output
	
	unsigned long long number;
	int i, j, k, l;
	
	i = strlen(str);
	char clean_str[i + 1];
	
	// prepare a clean string for strtoull()
	
	if ('0' == str[0] && 'X' == toupper(str[1])) // skip 0x
		str += 2;
	
	for (i = 0, l = 0, k = from_base - 1; '\0' != str[i]; ++i) 
	{
		j = toupper(str[i]);
		
		if (' ' == j) // skip spaces
			continue;
		
		if ('0' <= j && '9' >= j)
			j -= '0';
		else if ('A' <= j && 'F' >= j)
			j = (j - 'A') + MAGIC;
			
		/* check if the value derived from the character is > than 
		 * the maximum single digit value of the requested base */
		if (j > k)
		{
			fprintf(stderr, "Err: bad number.\n");
			exit(1);
		}
		
		clean_str[l++] = str[i];
	}
	clean_str[l] = '\0'; // end the string
	
	number = strtoull(clean_str, NULL, from_base); // convert it to a number
	
	// print original string
	fprintf(stdout, "%s%s%s= %s", (16 == from_base) ? "0x" : "", str, (2 == from_base) ? "b " : " ",
	(16 == to_base) ? "0x" : "");
	
	if (0 == number) // special case for binary zero
	{
		fprintf(stdout, "%d%c\n", 0, (2 == to_base) ? 'b' : ' ');
		return;
	}
	
	base_convert(number, to_base); // the actual conversion
	
	// print 'b' at the end if the conversion is to binary
	fprintf(stdout, "%s\n", (2 == to_base) ? "\bb" : "");
	
	if (~0ULL == number)
		fprintf(stdout, "Warning: maximum unsigned long long value is reached.\nMight be overflow.\n");
}

void base_convert(unsigned long long num, int base)
{
	/* converts numbers between bases <= 16 
	 * using lookup tables */
	
	if (0 == num)
		return;
	else
		/* if the base is 2 we still want to divide num by 16
		 * since it will give us the offset in the binary table */
		base_convert(num / ((2 != base) ? base : 16), base);
	
	// if the base is 2 look in the binary table
	(2 == base) ? fprintf(stdout, "%s", BINTBL[num % 16]) : putchar(HEXTBL[num % base]);
}

void print_file_info(const char * fname)
{
	// prints file size and last byte offset
	FILE * fp;
	long file_end;
	
	fp = open_file(fname, "rb");
	fseek(fp, 0, SEEK_END);
	file_end = ftell(fp);
	
	fprintf(stdout, "%-6s %.2f\n%-6s %.2f\n%-6s %ld\n", "MB:", (float)file_end / 1024.0 / 1024.0, "KB:", (float)file_end / 1024.0,
	"Bytes:", file_end);
	fprintf(stdout, "Last byte offset: %#lx\n", file_end - 1);
	fprintf(stdout, "File ends at: %#lx\n", file_end);
	
	fclose(fp);
}

void print_strlen(const char * str)
{
	// prints the length of str
	fprintf(stdout, "%d\n", strlen(str));
}

void check_hex_str(const char * num)
{
	// checks if num is a valid hex number
	int i;
	int ln = strlen(num);
	
	for (i = 0; i < ln; ++i) 
	{
		if ('X' == toupper(num[i]))
			continue;
		
		if (!(('0' <= num[i] && '9' >= num[i]) || 
		('A' <= toupper(num[i]) && 'F' >= toupper(num[i]))))
		{
			fprintf(stderr, "Err: bad number.\n");
			exit(1);
		}
	}
}

void and_or_xor(char opt, const char * num1, const char * num2)
{
	// ands, ors, or xors num1 with num2 and prints the result
	
	if (NOT == opt)
	{
		not(num1);
		return;
	}
	
	// check for bad numbers
	check_hex_str(num1);
	check_hex_str(num2);
	
	unsigned long a, b, result, temp;
	int la, lb, longer_string, result_str_len, i, j;
	char op;
	
	// discard any 0s or 'X'
	while ('0' == *num1 || 'X' == toupper(*num1))
		++num1;
	
	while ('0' == *num2 || 'X' == toupper(*num2))
		++num2;	
			
	// make strings into numbers
	a = strtoul(num1, NULL, 16);
	b = strtoul(num2, NULL, 16);
	// get the length again
	la = strlen(num1);
	lb = strlen(num2);
	
	if (8 < la || 8 < lb)
	{
		fprintf(stderr, "Err: overflow.\n");
		exit(1);
	}
	
	longer_string = (la > lb) ? la : lb;

	
	// perform the operation
	if (AND == opt)
	{
		result = a & b;
		op = '&';
	}
	else if (OR == opt)
	{
		result = a | b;
		op = '|';
	}
	else if (XOR == opt)
	{
		result = a ^ b;
		op = '^';
	}
	else
	{
		fprintf(stderr, "Err: invalid operation.\n");
		exit(1);
	}
	
	result_str_len = 0;
	temp = result;
	// get length of result
	while (temp != 0)
	{
		temp /= 16;
		++result_str_len;
	}
	
	// print the binary representation
	// pad a with 0s
	for (i = 0, j = lb - la; i < j; ++i) 
		fprintf(stdout, "%s", BINTBL[0]);
	
	if (0 == a && 0 == b)
		fprintf(stdout, "%s", BINTBL[0]);
	else
		base_convert(a, 2);
	fprintf(stdout, "\bb");
	fprintf(stdout, "\n%c\n", op);
	
	// pad b with 0s
	for (i = 0, j = la - lb; i < j; ++i) 
		fprintf(stdout, "%s", BINTBL[0]);
	
	if (0 == b && 0 == a)
		fprintf(stdout, "%s", BINTBL[0]);
	else
		base_convert(b, 2);
	fprintf(stdout, "\bb");
	fprintf(stdout, "\n----\n");
	
	// pad result with 0s
	for (i = 0, j = longer_string - result_str_len; i < j; ++i) 
		fprintf(stdout, "%s", BINTBL[0]);
	
	if (0 == a && 0 == b)
		fprintf(stdout, "%s", BINTBL[0]);
	else
		base_convert(result, 2);
		
	fprintf(stdout, "\bb\n\n");
	

	// print in hex
	fprintf(stdout, "0x");
	// pad a with 0s
	for (i = 0, j = lb - la; i < j; ++i) 
		fprintf(stdout, "%c", HEXTBL[0]);
			
	if (0 == a && 0 == b)
		fprintf(stdout, "%c", HEXTBL[0]);
	else
		base_convert(a, 16);
	fprintf(stdout, "\n%c\n", op);
	fprintf(stdout, "0x");
	
	// pad b with 0s
	for (i = 0, j = la - lb; i < j; ++i) 
		fprintf(stdout, "%c", HEXTBL[0]);
	
	if (0 == b && 0 == a)
		fprintf(stdout, "%c", HEXTBL[0]);
	else
		base_convert(b, 16);
	fprintf(stdout, "\n----\n");
	
	fprintf(stdout, "0x");
	// pad result with 0s
	for (i = 0, j = longer_string - result_str_len; i < j; ++i) 
		fprintf(stdout, "%c", HEXTBL[0]);
	
	if (0 == a && 0 == b)
		fprintf(stdout, "%c", HEXTBL[0]);
	else
		base_convert(result, 16);
		
	putchar('\n');
}

void not(const char * num)
{
	/* NOTing is implemented as XORing with the needed number
	 * of 0xFs in order to not always print a 32 bit number, which
	 * will happen if we use ~ */
	
	// check for bad number
	check_hex_str(num);
	
	unsigned long a, b, xor_val, temp;
	int i, ln, zero_pad = 0;
		
	// make the number string into a number
	a = strtoul(num, NULL, 16);
	
	// skip '0' and 'X' if any
	while ('0' == *num || 'X' == toupper(*num))
		++num;
		
	ln = strlen(num);
	
	// if string is > 8 characters it's overflow
	if (8 < ln)
	{
		fprintf(stderr, "Err: overflow.\n");
		exit(1);
	}
	
	/* ~0xF is 0, so base_convert() won't print anything
	 * the number of 0xFs in the beginning of the number
	 * is the the number of 0s that should be printed */
	for (i = 0; 'F' == toupper(num[i]); ++i, ++zero_pad)
		continue;
	
	/* if a is 0 make temp 15 so it will still
	 * go through the cycle once and add an 0xF
	 * to the xor value */
	temp = (0 == a) ? 15 : a;
	xor_val = 0x0;
	while (temp != 0)
	{
		temp /= 16;
		xor_val <<= 4;
		xor_val |= 0xF;
	}
	
	// xor 
	b = a ^ xor_val;
	
	// print the binary representation
	if (0 == a)
		fprintf(stdout, "%s", BINTBL[0]);
	else
		base_convert(a, 2);
	fprintf(stdout, "\bb");
	fprintf(stdout, "\n!\n");
	
	// pad with zeros if needed
	for (i = 0; i < zero_pad; ++i)
		fprintf(stdout, "%s", BINTBL[0]);
		
	base_convert(b, 2);
	fprintf(stdout, "\bb\n\n");
	
	
	// print in hex
	fprintf(stdout, "0x");
	if (0 == a)
		fprintf(stdout, "%c", HEXTBL[0]);
	else
		base_convert(a, 16);
	fprintf(stdout, "\n!\n");
	fprintf(stdout, "0x");
	
	// pad with zeros if needed
	for (i = 0; i < zero_pad; ++i)
		fprintf(stdout, "%c", HEXTBL[0]);
		
	base_convert(b, 16);
	putchar('\n');
}

void print_ascii(const char * str, bool whole_table, bool reverse)
{
	/* prints ASCII values by character, by number, 
	 * or prints the whole ASCII table */
	
	int i, j;
	
	if (whole_table) // only -a or -ar with nothing after
	{
		int k, l, m, n;
		
		// printf the header
		fprintf(stdout, "%-5s %-5s %-3s|", "Char", "Hex", "Dec");
		fprintf(stdout, "%-5s %-5s %-3s|", "Char", "Hex", "Dec");
		fprintf(stdout, "%-5s %-5s %-3s|", "Char", "Hex", "Dec");
		fprintf(stdout, "%-5s %-5s %-3s\n", "Char", "Hex", "Dec");
		fprintf(stdout, "---------------|");
		fprintf(stdout, "---------------|");
		fprintf(stdout, "---------------|");
		fprintf(stdout, "---------------\n");
		
		// first line is special case because of space
		i = 0; j = 32;
		k = i + j;
		l = k + j;
		m = l + j;
		fprintf(stdout, "%-5s %-#5X %-3d|", NON_PRINT_ASCII[i], i, i);
		fprintf(stdout, "%-5s %-#5X %-3d|", NON_PRINT_ASCII[32], k, k); // "Space", k, k
		fprintf(stdout, "  %-3c %-#5X %-3d|", l, l, l);
		fprintf(stdout, "  %-3c %-#5X %-3d\n", m, m, m);
		
		for (i = 1, n = j - 1; i < n; ++i) 
		{
			// print until the second to last line
			k = i + j;
			l = k + j;
			m = l + j;
			fprintf(stdout, "%-5s %-#5X %-3d|", NON_PRINT_ASCII[i], i, i);
			fprintf(stdout, "  %-3c %-#5X %-3d|", k, k, k);
			fprintf(stdout, "  %-3c %-#5X %-3d|", l, l, l);
			fprintf(stdout, "  %-3c %-#5X %-3d\n", m, m, m);

		}
		
		// last line is special case because of del
		k = i + j;
		l = k + j;
		m = l + j;
		fprintf(stdout, "%-5s %-#5X %-3d|", NON_PRINT_ASCII[i], i, i);
		fprintf(stdout, "  %-3c %-#5X %-3d|", k, k, k);
		fprintf(stdout, "  %-3c %-#5X %-3d|", l, l, l);
		fprintf(stdout, "%-5s %-#5X %-3d\n", "(del)", m, m);
		
		exit(0);
	}
	
	// print header
	fprintf(stdout, "%-5s %-5s %-3s\n", "Char", "Hex", "Dec");
	fprintf(stdout, "---------------\n");
	
	// -ar <number(s)>
	if (reverse)
	{
		int base = ('0' == str[0]) ? 16 : 10;
		int num = 0;
		
		if (16 != base) // check for hex if '0' != str[0]
		{	
			for (i = 0; '\0' != str[i]; ++i) 
			{
				j = toupper(str[i]);
				
				if (' ' == j) // skip spaces
					continue;
					
				if ('A' <= j && 'F' >= j)
				{
					base = 16;
					break;
				}
			}
		}
		
		int ln = strlen(str);
		char buff_str[ln];
		for (i = 0, j = 0; i <= ln; ++i, ++j) 
		{
			buff_str[j] = str[i];
			// get number string delimited by ' ' or '\0'
			if (' ' == buff_str[j] || '\0' == buff_str[j])
			{
				buff_str[j] = '\0';
				num = strtol(buff_str, NULL, base); // covert to number
				j = -1; // j will be ++ on the next iteration
			
			if (127 < num) // check boundry
			{
				fprintf(stderr, "Err: %s%s is outside of the ASCII table.\n", (16 == base) ? "0x" : "", buff_str);
				continue;
			}
				
			if (32 >= num)
				fprintf(stdout, "%-5s %-#5X %-3d\n", NON_PRINT_ASCII[num], num, num);
			else
				fprintf(stdout, "  %-3c %-#5X %-3d\n", num, num, num);
			}
		}
		
		return;
	}
	
	// -a <character(s)>
	for (i = 0, j = strlen(str); i < j; ++i) 
		fprintf(stdout, "  %-3c %-#5X %-3d\n", str[i], str[i], str[i]);
	
}

void print_help(const char * exe_name)
{
	// prints help information
	fprintf(stdout, "\n-------------------- General --------------------\n");
	fprintf(stdout, "%s is a terminal hex editor.\n", ORIG_EXE_NAME);
	fprintf(stdout, "File size limit is 2GB.\n");
	fprintf(stdout, "Number conversion limit is unsigned long long.\n");
	fprintf(stdout, "And, Or, Xor, and Not operations are limited to unsigned long.\n");
	fprintf(stdout, "\n-------------------- Hex Dumps --------------------\n");
	fprintf(stdout, "Note: Offset must be in hex. '0x' prefix can be omitted.\n");
	fprintf(stdout, "Number of lines must be in decimal.\n\n");
	fprintf(stdout, "%s <file> - dumps the whole file to stdout.\n", exe_name);
	fprintf(stdout, "%s <file> -%c <offset> -%c <n>\n", exe_name, OFFSET, LN_NUM);
	fprintf(stdout, "%s <file> -%c <n> -%c <offset>\n", exe_name, LN_NUM, OFFSET);
	fprintf(stdout, "Dumps <n> number of lines relative to <offset>.\n");
	fprintf(stdout, "<n> can be positive or negative. A positive value dumps <n> lines\n");
	fprintf(stdout, "after <offset>. A negative value dumps <n> lines before <offset>.\n");
	fprintf(stdout, "-lm <n> dumps <n> number of lines before and after <offset>.\n");
	fprintf(stdout, "-%c and -%c are 0 by default and can be omitted.\n", OFFSET, LN_NUM);
	fprintf(stdout, "-%c 0 dumps from <offset> untill EOF.\n", LN_NUM);
	fprintf(stdout, "To write the hex dump to a file use redirection.\n\n");
	fprintf(stdout, "%s -%c <file> <csv file>\n", exe_name, CSV);
	fprintf(stdout, "Writes a csv hex dump of <file> to <csv file>.\n");
	fprintf(stdout, "\n-------------------- Binary --------------------\n");
	fprintf(stdout, "%s -%c <dump file> <bin file> - makes a binary from hex dump.\n", exe_name, BIN);
	fprintf(stdout, "%s -%c%c <csv file> <bin file> - makes a binary from csv dump.\n", exe_name, CSV, BIN); 
	fprintf(stdout, "\n-------------------- Searching --------------------\n");
	fprintf(stdout, "%s <file> -%c%c \"string\" -%c <offset>\n", exe_name, SRCH, ASCII, OFFSET);
	fprintf(stdout, "%s <file> -%c <offset> -%c%c \"string\"\n", exe_name, OFFSET, SRCH, ASCII);
	fprintf(stdout, "Searches for ASCII strings from <offset>.\n\n");
	fprintf(stdout, "-%c%c looks for Unicode strings.\n", SRCH, UNICODE);
	fprintf(stdout, "Note: this options can detect Unicode strings in English only.\n");
	fprintf(stdout, "For other languages convert the string to binary and use -%c%c.\n\n", SRCH, BIN);
	fprintf(stdout, "-%c%c searches for byte sequence.\n", SRCH, BIN);
	fprintf(stdout, "The byte sequence must be presented as a string of hex values.\n");
	fprintf(stdout, "i.e. %s <file> -%c%c 48656c6c6f -%c <offset>\n", exe_name, SRCH, BIN, OFFSET);
	fprintf(stdout, "%s <file> -%c%c \"48 65 6c 6c 6f\" -%c <offset> is valid as well.\n", exe_name, SRCH, BIN, OFFSET);
	fprintf(stdout, "Capital letters are also accepted.\n");
	fprintf(stdout, "\n-------------------- Replacing --------------------\n");
	fprintf(stdout, "Note: What's in the original file gets overwritten permanently.\n\n");
	fprintf(stdout, "%s <file> -%c%c \"string\" -%c <offset>\n", exe_name, REPLACE, ASCII, OFFSET);
	fprintf(stdout, "%s <file> -%c <offset> -%c%c \"string\"\n", exe_name, OFFSET, REPLACE, ASCII);
	fprintf(stdout, "Writes an ASCII string over the original contents starting from <offset>.\n\n");
	fprintf(stdout, "-%c%c writes an Unicode string.\n", REPLACE, UNICODE);
	fprintf(stdout, "Note: English only.\n");
	fprintf(stdout, "For other languages convert the string to binary and use -%c%c.\n\n", REPLACE, BIN);
	fprintf(stdout, "-%c%c writes a byte sequence.\n", REPLACE, BIN);
	fprintf(stdout, "\n-------------------- Search and Replace --------------------\n");
	fprintf(stdout, "%s <file> -%c%c \"search string\" -%c%c \"replace string\"\n", exe_name, SRCH, ASCII, REPLACE, EVERYTHING);
	fprintf(stdout, "Looks for ASCII \"search string\" in <file> and replaces every\n");
	fprintf(stdout, "found instance with ASCII \"replace string\".\n");
	fprintf(stdout, "-%c%c -%c%c and -%c%c -%c%c work for Unicode and byte sequences respectively.\n", SRCH, UNICODE,
	REPLACE, EVERYTHING, SRCH, BIN, REPLACE, EVERYTHING);
	fprintf(stdout, "\n-------------------- ASCII --------------------\n");
	fprintf(stdout, "%s -%c \"string\"\n", exe_name, ASCII);
	fprintf(stdout, "Prints the ASCII value for every character in \"string\".\n");
	fprintf(stdout, "If \"string\" is omitted, the whole ASCII table gets printed.\n\n");
	fprintf(stdout, "%s -%c%c <number(s)>\n", exe_name, ASCII, REVERSE);
	fprintf(stdout, "%s -%c%c \"0x62 61 6e 61 6e 61\"\n", exe_name, ASCII, REVERSE);
	fprintf(stdout, "Performs a reverse ASCII lookup and prints the corresponding\n");
	fprintf(stdout, "character for every <number>. If there are more than one numbers,\n");
	fprintf(stdout, "they must be delimited by spaces. If one of the numbers is in hex\n");
	fprintf(stdout, "then all of the numbers are considered to be in hex. If no hex digits\n");
	fprintf(stdout, "or '0x' prefix is encountered, decimal is assumed.\n");
	fprintf(stdout, "\n-------------------- Numbers --------------------\n");
	fprintf(stdout, "Hex numbers can be prefixed, but postfixes are not accepted.\n\n");
	fprintf(stdout, "%s -%c%c%c <number> - coverts a hex value to decimal.\n", exe_name, HEX, TO, DEC);
	fprintf(stdout, "%s -%c%c%c <number> - coverts a hex value to binary.\n", exe_name, HEX, TO, BIN);
	fprintf(stdout, "%s -%c%c%c <number> - coverts a decimal value to hex.\n", exe_name, DEC, TO, HEX);
	fprintf(stdout, "%s -%c%c%c <number> - coverts a decimal value to binary.\n", exe_name, DEC, TO, BIN);
	fprintf(stdout, "%s -%c%c%c <number> - coverts a binary value to hex.\n", exe_name, BIN, TO, HEX);
	fprintf(stdout, "%s -%c%c%c <number> - coverts a binary value to decimal.\n", exe_name, BIN, TO, DEC);
	fprintf(stdout, "Note: in the <number> string all spaces are ignored, so\n");
	fprintf(stdout, "inputs like \"4 294 967 295\" and \"0010 1010\" are valid.\n");
	fprintf(stdout, "Unsigned values only.\n\n");
	fprintf(stdout, "%s +%c <a> <b> - ANDs <a> and <b>.\n", exe_name, AND);
	fprintf(stdout, "%s +%c <a> <b> - ORs <a> and <b>.\n", exe_name, OR);
	fprintf(stdout, "%s +%c <a> <b> - XORs <a> and <b>.\n", exe_name, XOR);
	fprintf(stdout, "%s +%c <a> - NOTs <a>.\n", exe_name, NOT);
	fprintf(stdout, "<a> and <b> must be hex values. '0x' prefix can be omitted.\n");
	fprintf(stdout, "Unsigned values only.\n");
	fprintf(stdout, "\n-------------------- Other --------------------\n");
	fprintf(stdout, "%s <file> -%c - prints file size info.\n", exe_name, INFO);
	fprintf(stdout, "%s -%c%c \"string\" - prints the length of \"string\".\n", exe_name, STRING, LEN);
	fprintf(stdout, "%s -%c for help.\n", exe_name, HELP);
	fprintf(stdout, "%s -%c for version info.\n", exe_name, VER);
}

void print_ver(void)
{
	// prints version info
	fprintf(stdout, "%-12s %s\n", "Name:", ORIG_EXE_NAME);
	fprintf(stdout, "%-12s %s\n", "Version:", EXE_VER);
}

/*
 * Copyright (C) 2012 
 * 	Dale Weiler
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "gmqcc.h"

//typedef int foo;

int usage(const char *name) {
	printf("Usage: %s -f infile -o outfile\n", name);
	return 0;
}

int main(int argc, char **argv) {
	const char *ofile = NULL;
	const char *ifile = NULL;
	int i;
	if (argc <= 2) {
		return usage(*argv);
	}
		
	for (i=0; i < argc; i++) {
		if (argc != i + 1) {
			switch(argv[i][0]) {
				case '-':
					switch(argv[i][1]) {
						case 'f': ifile = argv[i+1]; break;
						case 'o': ofile = argv[i+1]; break;
					}
					break;
			}
		}
	}
	
	if (!ofile || !ifile) {
		return usage(*argv);
	}
	
	printf("ifile: %s\n", ifile);
	printf("ofile: %s\n", ofile);
	
	/* Open file */
	FILE *fp = fopen(ifile, "r");
	if  (!fp) {
		fclose(fp);
		return error(ERROR_COMPILER, "Source file: %s not found\n", ifile);
	} else {
		struct lex_file *lex = lex_open(fp);
		parse    (lex);
		lex_close(lex);
	}
	return 0;
}

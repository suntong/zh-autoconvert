/*
AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
Copyright (C) 1999  ÓÚ¹ã»Ô  Yu Guanghui <ygh@debian.org>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.
*/
/* 
* author: Yu Guanghui <ygh@dlut.edu.cn>
*	  Network Center
* 	  Dalian Univ. of Tech.
*	  1999.5
*/
//$Header: /home/prog/autoconvert/autogb.c,v 1.7 2001/04/23 14:18:06 ygh Exp $

#include <stdio.h>
#include <getopt.h>
#include "hz.h"


/* Print help and exit */
void print_help(char* filename){
	printf("\nUsage: %s [-OPTION] < input > output\n", filename);
	printf("Now autoconvert can only judge gb/big5/hz encoding, so if you want to  \n do other conversion, you must set the option --input.  Without --input, \n the program will guess the encoding of the input. \n OPTION is one of (case insensitive): \n -i encoding, --input encoding:			Set the input encoding \n -o encoding, --output encoding:			Set the output encoding \n The encoding should be gb, big5, hz, uni, utf7 or utf8. \n");

	exit(0);
}


int main(int argc,char*argv[])
{
	int incode,outcode,dist;
	char buff[MAX_BUFFER*3];
	char *pbuf;
	char *ps;
	int need_judge=1;


	int count;

	incode = 0;

	pbuf=&buff[2];

	if( strstr(argv[0],"autogb") != '\0' ){
		outcode=GB_CODE;
	}else if(strstr(argv[0],"autob5") != '\0'){
	
		outcode=BIG5_CODE;
	}
	/* We need one option for encoding at least*/
	if (argc >= 2){ 
	
		/*
		if(argv[1][0] != '-')
			print_help(argv[0]);

		for ( i = 1; i < strlen(argv[1]); i++)
			argv[1][i] = tolower(argv[1][i]);
		
		*/
		/* Get options here. */
		while(1){
			int c;
			int option_index ;
			static struct option long_options[] = 
			{
				{"input", 1, 0, 'i'},
				{"output", 1, 0, 'o'},
				{0, 0, 0, 0},
			};
//			c = getopt_long_only(argc, argv, "io", long_options,
//				&option_index);
			c = getopt_long(argc, argv, "i:o:", long_options,
				&option_index);

			/* No more option, break 'while' loop */
			if ( c == -1 ){
				break; /* While loop */
			}

			switch( c ){
				case 'i':
					need_judge=0;
					if(optarg){
						if((incode=conv_str(optarg))==OTHER_CODE) print_help(argv[0]);
					}else{
						print_help(argv[0]);
					}
					break;

				case 'o':
					if(optarg){
						if((outcode=conv_str(optarg))==OTHER_CODE) print_help(argv[0]);
					}else{
						print_help(argv[0]);
					}
					break;
				case '?':
				default:
					print_help(argv[0]);
					break;
			} /* End switch */

		} /* End while */
	} /* End else if */

	if((count=read(0,pbuf,MAX_BUFFER))< 0){
		fprintf(stderr,"Can't read stdin\n");
	}
	
	/* If incode is set in command line, don't judge. */
	if (need_judge==1)
		incode=j_code(pbuf,count);

	if (incode != outcode){
		hz_setup();
		if((dist=hz_search(incode,outcode,8))==0){
			printf("I couldn't do this conversion\n");
			exit(0);
		}
	}

	do{
		if(incode==outcode){
			write(1,pbuf,count);
		}else{
			ps=hz_convert(pbuf,&count,0);
			write(1,ps,count);
		}
		pbuf=&buff[2];
	}while((count=read(0,pbuf,MAX_BUFFER))>0);
	fflush(0);
}

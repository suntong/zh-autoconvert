/*
AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
Copyright (C) 2000 ÓÚ¹ã»Ô <ygh@debian.org>

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
 *Author: Ha Shao <hashao@china.com>
 *Date:	2000.08.30
 */
 /*$Header: /home/prog/autoconvert/hzconvert/judge3.c,v 1.2 2000/11/28 17:31:19 ygh Exp $*/

/* Judge Encoding based on character frequency.
 * We will use 2 hash tables generated from GNU gperf
 */


#include <stdio.h>
#include "hz.h"
#include "zhstatis.h"


int j_code3(char * buff, int count)
{
	unsigned char *phz;
	double c_gb, c_b5;		/* Frequency count for char */
//	double o_gb, o_b5;		/* Holes in opposit encode.*/
					/* If we test user defined chars. */
	const struct charHz * temphz;	/* temperary char frequency. */

	c_gb = c_b5 = 0;

	for(phz=buff; (int)phz < ((int)buff + count); phz++){
		if(*phz & 0x80){
			/* First Byte not in GB range. 
			 * We don't do user defined chars. 
			 * Should not simply return if we consider
			 * user defined chars
			 * */
			if((*phz >= 0xF8 && *phz <= 0xF9) && ( (*(phz+1) >= 0x40 && *(phz+1) <= 0x7E) || (*(phz+1) >= 0xA1 && *(phz+1) <= 0xFE ))){
				return BIG5_CODE;
			}
			/* Second Byte not in GB range. */
			if ((*phz >= 0xA1 && *phz <= 0xF7) && (* (phz+1) >= 0x40 && *(phz+1) <= 0x7E)){

				return BIG5_CODE;
			}
			/* GB holes in first byte: 0xA8-0xAF. */
			if((*phz >= 0xA8 && *phz <= 0xAF) && ((*(phz+1) >= 0x40 && *(phz+1) <= 0x7E ) || (*(phz+1) >= 0xA1 && *(phz+1) <= 0xFE ))){

				return BIG5_CODE;
			}

			/* Valid GB and BIG5? see which one is more likely. */
			if ((*phz >= 0xA1 && *phz <= 0xF7) && (* (phz+1) >= 0xA1 && *(phz+1) <= 0xFE)){

				if(temphz = inGB(phz, 2))
					c_gb += temphz->percent;
				if(temphz = inBig5(phz, 2))
					c_b5 += temphz->percent;

				/* Every valid GB and BIG5 codes
				 * are tested. we jump 2 here.
				 */
				phz++;
			}
		}
		/* Anything else are either ASCII or screwed
		 * text. We pass that one char. 
		 */
	} /* for loop end */
	
#ifdef DEBUG
printf("GB = %f, \t BIG5 = %f\n", c_gb, c_b5);
#endif

	/* Assume that == is GB code. Prove me wrong! :) */
	if( c_gb >= c_b5 )
		return GB_CODE;
	else
		return BIG5_CODE;
}


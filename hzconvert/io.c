/*
AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
Copyright (C) 1999  ÓÚ¹ã»Ô  Yu Guanghui <ygh@dlut.edu.cn>

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
 *hack from hztty-2.0(Yongguang ZHANG,1992 COPYRIGHT)
 *by Yu Guanghui <ygh@dlut.edu.cn>
 *1999.10
 */       
//$Header: /home/prog/autoconvert/hzconvert/io.c,v 1.1.1.1 2000/04/19 05:43:32 ygh Exp $
#include "hz.h"

char *hzconvert (s, plen, psaved, dbcvrt)
     char *s;
     int *plen;
     char *psaved;		/* unprocessed char buffer pointer */
     void (*dbcvrt)();		/* 2-byte conversion func for a hanzi */
{
  char *p, *pend;

	if (*plen == 0)
		return (s);
        if (*psaved) {			/* previous buffered char */
                *(--s) = *psaved;	/* put the unprocessed char down */
		(*plen) ++;
		*psaved = 0;		/* clean this char buffer */
        }
	p = s;  pend = s + (*plen);	/* begin/end of the buffer string */
	while (p < pend) {
		if ((*p) & 0x80)	/* hi-bit on: hanzi */
			if (p < pend-1) 	/* not the last one */
				dbcvrt (p++);
			else {			/* the end of string */
				*psaved = *p;	/* save the unprocessed char */
				(*plen) --;
				break;
			}
		p++;
        }
        return (s);
}



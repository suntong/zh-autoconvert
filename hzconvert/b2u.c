/*
AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
Copyright (C) 1999  �ڹ��  Yu Guanghui <ygh@dlut.edu.cn>

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
//$Header: /home/prog/autoconvert/hzconvert/b2u.c,v 1.6 2001/04/28 01:45:46 ygh Exp $
#include "hz.h"

#define	BtoU_bad1 0xa1
#define	BtoU_bad2 0xf5

extern unsigned char BtoU[], UtoB[];
extern int BtoU_count, UtoB_count;

#define	c1	(unsigned char)(s[0])
#define	c2	(unsigned char)(s[1])

static void b2u(s, t)
     register char *s, *t;
{
  register unsigned int i;
/*
	if ((c2 >= 0xa1) && (c2 <= 0xfe)) {
		if ((c1 >= 0xa1) && (c1 <= 0xa9)) {
			i = ((c1 - 0xa1) * 94 + (c2 - 0xa1)) * 2;
			t[0] = BtoU[i++];  t[1] = BtoU[i];
			return;
		} else if ((c1 >= 0xb0) && (c1 <= 0xf7)) {
			i = ((c1 - 0xb0 + 9) * 94 + (c2 - 0xa1)) * 2;
			t[0] = BtoU[i++];  t[1] = BtoU[i];
			return;
		}
	}
*/
	s[1] = c2 - (c2 >= 0xa1 ? (0xa1 - 63) : 0x40);
        i= ((c1 - 0xa1)*157 + c2) * 2;
	t[1] = BtoU[i++];  t[0] = BtoU[i];

//	t[0] = BtoU_bad1;  t[1] = BtoU_bad2;
}

static void u2b(s, t)
     register char *s, *t;
{
	register long int i = ((c1 << 8) + c2) * 2;

	t[0] = UtoB[i++];  t[1] = UtoB[i];
}

#undef c1
#undef c2

static char big2uni_savec[MAX_MODULE];
static char uni2big_savec[MAX_MODULE];

int big2uni_init (arg)
     char *arg;
{
/*
  static int big2uni_inst = 0;

	big2uni_savec[big2uni_inst] = '\0';
	return (big2uni_inst++);
*/

	int i;
	for(i=0;i<MAX_MODULE;i++)
                big2uni_savec[i] = '\0';
        return (i++);
}

int uni2big_init (arg)
     char *arg;
{
/*
  static int uni2big_inst = 0;

	uni2big_savec[uni2big_inst] = '\0';
	return (uni2big_inst++);
*/
	int i;
	for(i=0;i<MAX_MODULE;i++)
                uni2big_savec[i] = '\0';
        return (i++);
}

char *big2uni (s,plen,inst)
     char *s;
     int *plen;
     int inst;
{
  char buf[MAX_BUFFER*2];	/* big enough */
  char *s_start = s;
  register int len = *plen;
  register char *p = buf;

	if (*plen == 0)
		return (s);
	if (big2uni_savec[inst]) {
		*(--s) = big2uni_savec[inst];
		len ++;
		big2uni_savec[inst] = 0;	
	}
	memcpy(p,s, len);

	s = s_start;
	while (len > 0) {
		if (((*p) & 0x80) == 0) {	/* ASCII */
			*s++ = 0;
			*s++ = *p++;
			len --;
			continue;
		}
		if (len == 1) {		/* dangling */
			big2uni_savec[inst] = *p;
			break;
		}
		b2u (p, s);
		s++;  s++;
		p++;  p++; 
		len--;  len--;
	}
	*plen = s - s_start;
	return s_start;
}

char *uni2big (s,plen,inst)
     char *s;
     int *plen;
     int inst;
{
  char buf[MAX_BUFFER*2];	/* big enough */
  char *s_start = s;
  register int len = *plen;
  register char *p = buf;

	if (len == 0)
		return (s);
	if (uni2big_savec[inst]) {
		*(--s) = uni2big_savec[inst];
		len++;
		uni2big_savec[inst] = 0;	
	}
	memcpy(p,s, len);

	s = s_start;
	while (len >= 2) {
		if ((p[0] == 0) && ((p[1] & 0x80) == 0)) {	/* ASCII */
			*s++ = p[1] & 0x7f;
		} else {
			u2b (p, s);
			s++;  s++;
		}
		p++;  p++;
		len--;  len--;
	}
	if (len == 1)		/* dangling */
		uni2big_savec[inst] = *p;	
	*plen = s - s_start;
	return s_start;
}


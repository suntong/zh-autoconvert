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
//$Header: /home/prog/autoconvert/hzconvert/g2u.c,v 1.4 2001/04/28 01:45:46 ygh Exp $

#include "hz.h"

#define	GtoU_bad1 0xa1
#define	GtoU_bad2 0xbc

extern unsigned char GtoU[], UtoG[];
extern int GtoU_count, UtoG_count;

#define	c1	(unsigned char)(s[0])
#define	c2	(unsigned char)(s[1])

static void g2u(s, t)
     register char *s, *t;
{
  register unsigned int i;

	if ((c2 >= 0xa1) && (c2 <= 0xfe)) {
		if ((c1 >= 0xa1) && (c1 <= 0xa9)) {
			i = ((c1 - 0xa1) * 94 + (c2 - 0xa1)) * 2;
			t[0] = GtoU[i++];  t[1] = GtoU[i];
			return;
		} else if ((c1 >= 0xb0) && (c1 <= 0xf7)) {
			i = ((c1 - 0xb0 + 9) * 94 + (c2 - 0xa1)) * 2;
			t[0] = GtoU[i++];  t[1] = GtoU[i];
			return;
		}
	}
	t[0] = GtoU_bad1;  t[1] = GtoU_bad2;
}

static void u2g(s, t)
     register char *s, *t;
{
  register long int i = ((c1 << 8) + c2) * 2;

	t[0] = UtoG[i++];  t[1] = UtoG[i];
}

#undef c1
#undef c2

static char gb2uni_savec[MAX_MODULE];
static char uni2gb_savec[MAX_MODULE];

int gb2uni_init (arg)
     char *arg;
{
/*
  static int gb2uni_inst = 0;

	gb2uni_savec[gb2uni_inst] = '\0';
	return (gb2uni_inst++);
*/

	int i;
	int gb2uni_inst=0;
	for(i=0;i<MAX_MODULE;i++)
		gb2uni_savec[gb2uni_inst] = '\0';
        return (gb2uni_inst++);
}

int uni2gb_init (arg)
     char *arg;
{
/*
  static int uni2gb_inst = 0;

	uni2gb_savec[uni2gb_inst] = '\0';
	return (uni2gb_inst++);
*/
	int i;
	int uni2gb_inst=0;
	for(i=0;i<MAX_MODULE;i++)
		uni2gb_savec[uni2gb_inst] = '\0';
        return (uni2gb_inst++);
}

char *gb2uni (s,plen,inst)
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
	if (gb2uni_savec[inst]) {
		*(--s) = gb2uni_savec[inst];
		len ++;
		gb2uni_savec[inst] = 0;	
	}
	memcpy(p, s, len);

	s = s_start;
	while (len > 0) {
		if (((*p) & 0x80) == 0) {	/* ASCII */
			*s++ = 0;
			*s++ = *p++;
			len --;
			continue;
		}
		if (len == 1) {		/* dangling */
			gb2uni_savec[inst] = *p;
			break;
		}
		g2u (p, s);
		s++;  s++;
		p++;  p++; 
		len--;  len--;
	}
	*plen = s - s_start;
	return s_start;
}

char *uni2gb (s,plen,inst)
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
	if (uni2gb_savec[inst]) {
		*(--s) = uni2gb_savec[inst];
		len++;
		uni2gb_savec[inst] = 0;	
	}
	memcpy(p,s, len);

	s = s_start;
	while (len >= 2) {
		if ((p[0] == 0) && ((p[1] & 0x80) == 0)) {	/* ASCII */
			*s++ = p[1] & 0x7f;
		} else {
			u2g (p, s);
			s++;  s++;
		}
		p++;  p++;
		len--;  len--;
	}
	if (len == 1)		/* dangling */
		uni2gb_savec[inst] = *p;	
	*plen = s - s_start;
	return s_start;
}


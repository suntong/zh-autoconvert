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
//$Header: /home/prog/autoconvert/hzconvert/hz2gb.c,v 1.2 2001/04/28 01:45:46 ygh Exp $

#include "hz.h"

#define	hzb(c)	((((c) & 0x7f) >= 0x21) && (((c) & 0x7f) <= 0x7e))

#define	S_ASC	0
#define	S_PHZ	1
#define	S_HZ	2
#define	S_HZ2	3
#define	S_HZA	4
#define	S_HZT	5
#define	S_PZW	6
#define	S_ZW	7
#define	S_ZW2	8
#define	S_ZWA	9

static int state[MAX_MODULE];
static int eoln[MAX_MODULE];

static char hzdecode(c, inst)
     char c;
     int inst;	/* instant number */
{
	switch (state[inst]) {
	  case S_ASC:
		switch (c) {
		  case '~':	state[inst] = S_PHZ;  break;
		  case 'z':	state[inst] = (eoln[inst] ? S_PZW : S_ASC);
				break;
		  default:	state[inst] = S_ASC;  break;
		}
		break;

	  case S_PHZ:
		switch (c) {
		  case '{':	state[inst] = S_HZ;   break;
		  default:	state[inst] = S_ASC;  break;
		}
		break;
	  case S_HZ:
		switch (c) {
		  case '~':	state[inst] = S_HZT;   break;
		  case ' ':	state[inst] = S_HZA;   break;
		  case '\b':	state[inst] = S_HZ2;   break;
		  default:	if (hzb(c)) {
					c |= 0x80;  state[inst] = S_HZ2;
				} else  state[inst] = S_ASC;
				break;
		}
		break;
	  case S_HZA:
		state[inst] = S_HZ;
		break;
	  case S_HZ2:
		switch (c) {
		  case ' ':
		  case '\b':	state[inst] = S_HZ;   break;
		  default:	if (hzb(c)) {
					c |= 0x80;  state[inst] = S_HZ;
				} else  state[inst] = S_ASC;
				break;
		}
		break;
	  case S_HZT:
		switch (c) {
		  case '}':	state[inst] = S_ASC;  break;
		  default:	state[inst] = S_HZ;   break;
		}
		break;

	  case S_PZW:
		switch (c) {
		  case 'W':	state[inst] = S_ZW;   break;
		  default:	state[inst] = S_ASC;  break;
		}
		break;
	  case S_ZW:
		switch (c) {
		  case ' ':	state[inst] = S_ZWA;   break;
		  case '\n':	state[inst] = S_ASC;   break;
		  case '\r':	state[inst] = S_ASC;   break;
		  default:	if (hzb(c)) {
					c |= 0x80;  state[inst] = S_ZW2;
				} else  state[inst] = S_ASC;
				break;
		}
		break;
	  case S_ZWA:	state[inst] = S_ZW;  break;
	  case S_ZW2:
		switch (c) {
		  case ' ':
		  case '\b':	state[inst] = S_ZW;   break;
		  default:	if (hzb(c)) {
					c |= 0x80;  state[inst] = S_ZW;
				} else  state[inst] = S_ASC;
				break;
		}
		break;
	}
	return (c);
}



char *hz2gb (s,plen,inst)
     char *s;
     int *plen;
     int inst;
{
  int i;

	for (i = 0; i < *plen; i++) {
		s[i] = hzdecode (s[i], inst);
		eoln[inst] = (s[i] == '\n' || s[i] == '\r') ? 1 : 0;
	}
	return (s);
}

char *gb2hz (s,plen,inst)
     char *s;
     int *plen;
     int inst;
{
  register int i;

	for (i = 0; i < *plen; i++)
		s[i] &= 0x7f;
	return (s);
}

int hz2gb_init (arg)
     char *arg;
{
/*
  static hz2gb_inst = 0;

	eoln[hz2gb_inst] = 0;
	state[hz2gb_inst] = S_ASC;
	return (hz2gb_inst++);
*/
    
	int i;
        for(i=0;i<MAX_MODULE;i++){
		eoln[i] = 0;
		state[i] = S_ASC;
	}
	return (i);
}

int gb2hz_init (arg)
     char *arg;
{
	return (0);
}

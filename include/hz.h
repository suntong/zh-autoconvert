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
/*author: Yu Guanghui
  1999.5
*/
//$Header: /home/prog/autoconvert/include/hz.h,v 1.8 2001/04/28 01:45:46 ygh Exp $

#define MAX_MODULE 16

/*j_code return type*/
/*0x0 means set code by hand*/
#define GB_CODE		0x0001
#define BIG5_CODE	0x0002
#define HZ_CODE		0x0004
#define UNI_CODE	0x0010
#define UTF7_CODE	0x0020
#define UTF8_CODE	0x0040
#define OTHER_CODE	0x8000

#define MAX_BUFFER	8192

char *hz2gb(),*gb2hz();				/* hz2gb.c */
int   hz2gb_init(),gb2hz_init();

char *gb2big(), *big2gb();		/* b2g.c */
int gb2big_init(), big2gb_init();	/* b2g.c */

char *uni_utf8(), *utf8_uni();		/* unicode.c */
int uni_utf8_init(), utf8_uni_init();	/* unicode.c */
char *uni_utf7(), *utf7_uni();		/* unicode.c */
int uni_utf7_init(), utf7_uni_init();	/* unicode.c */

char *gb2uni(), *uni2gb();		/* g2u.c */
int gb2uni_init(), uni2gb_init();	/* g2u.c */

char *big2uni(), *uni2big();		/* b2u.c */
int big2uni_init(), uni2big_init();	/* b2u.c */

int j_code(char * buff,int count); 	/* lib.c */

void hz_setup(void);/*search.c*/
void hz_init(void);/*search.c*/
int hz_search(int,int,int); /* search.c*/
char *hz_convert(char *,int*,int);	/*search.c*/

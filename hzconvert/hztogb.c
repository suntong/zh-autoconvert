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
/*Rewrite hztogb, Now it is work as a function,not a program
 * Yu Guanghui<ygh@rose.dlut.edu.cn>
 * 1999/10/22 01:13:30 ygh
 */
/* 
* Author: Yu Guanghui <ygh@rose.dlut.edu.cn>
* Network Center
* Dalian Univ. of Tech.
* 1999/5/8 05:47:17 ygh Exp 
*/
//$Header: /home/prog/autoconvert/hzconvert/hztogb.c,v 1.1.1.1 2000/04/19 05:43:32 ygh Exp $
/*
#include <stdio.h>
#include "hz.h"

char *hz2gb(char*s,int *plen,int inst,char *out)
{
	int ishz=0;
	int flag=0;
	int oinst=0;
	
	while(inst <= *plen){
		if(*(s+inst)==0x7E) {
			flag=1;
			inst++;
			continue;
		}
		if(flag){
			switch(*(s+inst)){
				case 0x7B:
					ishz=1;
					flag=0;
					inst++;
					continue;
				case 0x7D:
					ishz=0;
					flag=0;
					inst++;
					continue;
				default:
					flag=0;
					*(out+oinst)='~';
					oinst++;
					break;
			}
		}
		if(ishz){
			*(out+oinst)=*(s+inst)|0x80;
			oinst++;
		}else{
			*(out+oinst)=*(s+inst);
			oinst++;
		}
		inst++;
	}
	*plen=--oinst;
	return out;
}
*/

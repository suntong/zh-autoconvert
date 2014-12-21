/*
AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
Copyright (C) 1999  于广辉  Yu Guanghui <ygh@debian.org>

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
* author: Yu Guanghui <ygh@debian.org>
*	  Network Center
* 	  Dalian Univ. of Tech.	
*	  1999.5
*/

//$Header: /home/prog/autoconvert/hzconvert/judge.c,v 1.2 2000/08/31 03:11:08 ygh Exp $
#include <stdio.h>

#include "hz.h"

int j_code(char * buff,int count)
{
	unsigned char * phz;
	int c_gb=0;
	int c_big5=0;

	char HZ_START[]="~{";
	char HZ_END[]="~}";


/* first we look up "我"  and "的" ,both gb and big5
 * in the text.
 */
	for(phz=buff;phz<(buff+count);phz++){
		if(*phz & 0x80){
			if((*phz==0xB5&&*(phz+1)==0xC4) || ((*phz==0xCE)&&*(phz+1)==0xD2)){
				c_gb++;
				phz++;
				continue;
			}else if((*phz==0xAA&&*(phz+1)==0xBA)|| ((*phz==0xA7)&&*(phz+1)==0xDA)){
				c_big5++;
				phz++;
				continue;
			}
			phz++;
		}
	}


	if(c_gb > c_big5){
		return GB_CODE;
	}else if (c_gb ==  c_big5){ //c_gb == 0,c_big5==0
		if(strstr(buff,HZ_START)!=NULL && strstr(buff,HZ_END)!=NULL){
			return HZ_CODE;
		}
		/*There is not "我" and "的" in the text
		 *So we test the text with a 400 words table.
		 */
		return j_code3(buff,count);		
	}else{
		return BIG5_CODE;
	}
}

/*
 *This function replaced by j_code3
 *2000.8.31
 *Yu Guanghui <ygh@dlut.edu.cn>
 */
/*
int j_code2(char * buff,int count)
{
	unsigned char * phz;
	int c_gb=0;
	
	for(phz=buff;phz<(buff+count);phz++){
		if(*phz & 0x80){
			if ((*phz >= 0xF8 && *phz <= 0xF9) && ( (*(phz+1) >= 0x40 && *(phz+1) <= 0x7E) || (*(phz+1) >= 0xA1 && *(phz+1) <= 0xFE ))){
				return BIG5_CODE;
			}
			if ((*phz >= 0xA1 && *phz <= 0xF7) && (* (phz+1) >= 0x40 && *(phz+1) <= 0x7E)){
				return BIG5_CODE;
			}
			if ((*phz >= 0xA1 && *phz <= 0xF7) && (* (phz+1) >= 0xA1 && *(phz+1) <= 0xFE)){
				c_gb++;
			}
			phz++;
			continue;
		}
	}

	if( c_gb ==0 ){
		return OTHER_CODE;
	}else {
		return GB_CODE;
	}

}
*/

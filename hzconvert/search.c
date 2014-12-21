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
#include <stdio.h>
#include <string.h>
#include "hz.h"

#define MAX 100

struct c_modules{
	int from;
	int to;
	char* (*pfun_convert)();
	int (*pfun_init)();
	int  distance;
	int  skip;
};

struct stack {
	char from;
	char to;
	int dist;
	int index;
};

struct stack bt_stack[MAX];

int num_modules=0;
int find_pos=0;
int tos=0;

struct c_modules clist[MAX];

void assert_module(int from,int to,char* (*convert)(),int (*init)(),int dist)
{
	if(num_modules<MAX){
		clist[num_modules].from=from;
		clist[num_modules].to=to;
		clist[num_modules].distance=dist;
		clist[num_modules].pfun_convert=convert;
		clist[num_modules].pfun_init=init;
		clist[num_modules].skip=0;
		num_modules++;
	}else{
		printf("Module database full.\n");
	}
}

void hz_setup()
{
	assert_module(HZ_CODE,GB_CODE,hz2gb,hz2gb_init,1);
	assert_module(GB_CODE,HZ_CODE,gb2hz,gb2hz_init,1);
	assert_module(GB_CODE,BIG5_CODE,gb2big,gb2big_init,1);
	assert_module(BIG5_CODE,GB_CODE,big2gb,big2gb_init,1);
	assert_module(GB_CODE,UNI_CODE,gb2uni,gb2uni_init,1);
	assert_module(UNI_CODE,GB_CODE,uni2gb,uni2gb_init,1);
	assert_module(BIG5_CODE,UNI_CODE,big2uni,big2uni_init,1);
	assert_module(UNI_CODE,BIG5_CODE,uni2big,uni2big_init,1);
	assert_module(UNI_CODE,UTF8_CODE,uni_utf8,uni_utf8_init,1);
	assert_module(UTF8_CODE,UNI_CODE,utf8_uni,utf8_uni_init,1);
	assert_module(UNI_CODE,UTF7_CODE,uni_utf7,uni_utf7_init,1);
	assert_module(UTF7_CODE,UNI_CODE,utf7_uni,utf7_uni_init,1);
}

void hz_init()
{
	int i;

	find_pos=0;
	tos=0;

	for(i=0;i<num_modules;i++){
		clist[i].pfun_init();
		clist[i].skip=0;
	}
	
}

char* hz_convert(char *text,int* count,int inst)
{
	int t;
	char buf1[MAX_BUFFER*2];
	char buf2[MAX_BUFFER*2];
	char *ps;
	char *pbuf;

	pbuf=&buf1[1];

	t=0;

	memcpy(pbuf,text,*count);
	while(t<tos){
	//	printf("%s to ",bt_stack[t].from);
	//	FIX ME: init pconv
		ps=clist[bt_stack[t].index].pfun_convert(pbuf,count,inst);
		memcpy(&buf2[1],ps,*count);
		memcpy(&buf1[1],&buf2[1],*count);
		pbuf=&buf1[1];
		t++;
	}
	memcpy(text,pbuf,*count);
	return  text; 
}

void push(int from,int to,int dist,int index)
{
	if(tos<num_modules){
		bt_stack[tos].from=from;
		bt_stack[tos].to=to;
		bt_stack[tos].dist=dist;
		bt_stack[tos].index=index;
		tos++;
	}else{
		printf("Stack full.\n");
	}
}

void pop(int* from, int *to,int *dist,int *index)
{
	if(tos>0){
		tos--;
		*from=bt_stack[tos].from;
		*to=bt_stack[tos].to;
		*dist=bt_stack[tos].dist;
		*index=bt_stack[tos].index;
	}else{
		printf("Stack underflow.\n");
	}
}

int match(int from,int to,int *index)
{
	register int t;

	for(t=num_modules-1;t>-1;t--){
		if( (clist[t].from == from) && (clist[t].to == to)){
			*index=t;
			return clist[t].distance;
		}
	}
	return 0;
}

int find(int from,int * anywhere,int *index)
{
	find_pos=0;
	while(find_pos<num_modules){
		if((clist[find_pos].from == from) && !clist[find_pos].skip){
			*anywhere=clist[find_pos].to;
			clist[find_pos].skip=1;
			*index=find_pos;
			return clist[find_pos].distance;
		}
		find_pos++;
	}
	return 0;
}

int ismodule(int from,int to)
{
	int d,dist,index,index_any;
	int anywhere;

	
	if( (d=match(from,to,&index))){
		push(from,to,d,index);
		return 0;
	}

	while((dist=find(from,&anywhere,&index))){
		if((d=match(anywhere,to,&index_any))){
			push(from,to,dist,index);
			push(anywhere,to,d,index_any);
			return 0;
		}
	}

	if((dist=match(from,anywhere,&index))){
		push(from,to,dist,index);
		ismodule(anywhere,to);
	}else if(tos>0){
		pop(&from,&to,&dist,&index);
		ismodule(from,to);
	}
	return 0;
}


int hz_search(int from,int to,int distance)
{
	int dist=0, t=0;
	hz_init();
	ismodule(from,to);

	while(t<tos){
		dist+=bt_stack[t].dist;
		t++;
	}
	return dist;
}

int conv_str(char* str_code)
{
	if(!strcmp("gb",str_code)){
		return GB_CODE;
	}else if(!strcmp("big5",str_code)){
		return BIG5_CODE;
	}else if(!strcmp("hz",str_code)){
		return HZ_CODE;
	}else if(!strcmp("uni",str_code)){
		return UNI_CODE;
	}else if(!strcmp("utf7",str_code)){
		return UTF7_CODE;
	}else if(!strcmp("utf8",str_code)){
		return UTF8_CODE;
	}else{
		return OTHER_CODE;
	}
}

/*
 * Old Function , buggy , we don't need it anymore.
int search(char *from,char *to,struct converter *pconv,char *start,int deep)
{

	int i;
	extern int num_modules;

	deep++;
	if(deep> 4) return 0;

	for(i=0;i<=num_modules;i++){
		if(strcmp(from,clist[i].from)==0 && strcmp(to,clist[i].to)==0){
			pconv->convert=clist[i].pfun_convert;
			pconv->init=clist[i].pfun_init;
			return 1;
		}
	}

	for(i=0;i<=num_modules;i++){
		if(strcmp(from,clist[i].from)==0){
			clist[i].skip=1;
			pconv->convert=clist[i].pfun_convert;
			pconv->init=clist[i].pfun_init;
			if(strcmp(to,clist[i].to)==0){
				return 1;
			}else if(clist[1].skip==0 && strcmp(start,clist[i].to)!=0){
				(struct covernter*)pconv->next=malloc(sizeof(struct converter));
				if(!search(clist[i].to,to,pconv->next,start,deep)){
				free(pconv->next);
				pconv->next=NULL;
					
				}else{
			 		return 1;
				}

			}
			clist[i].skip=0;
			pconv->convert=NULL;
		}
	}
	return 0;
}
*/

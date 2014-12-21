/*********************************************************************** 
 * AutoConvert, a Chinese HZ/GB/Big5 encodings auto-converter
 * Copyright (C) 1999  ÓÚ¹ã»Ô  Yu Guanghui <ygh@debian.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or any
 * later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 * ********************************************************************/
/*Auther: Ha Shao <hashao@china.com>
 *Date: 2000.8.31
 */

/*$Header: /home/prog/autoconvert/include/zhstatis.h,v 1.1 2000/08/31 03:11:08 ygh Exp $*/
#define GBTOPPER        3.500849        /* Top GB frequency. */
#define B5TOPPER        3.803567        /* Top Big5 frequency. */


struct charHz { char *name; double percent; };
const struct charHz *
	inGB (register const char *str, register unsigned int len);
const struct charHz *
	inBig5 (register const char *str, register unsigned int len);


/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2010 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Gasol Wu <gasol.wu@gmail.com>                                |
  +----------------------------------------------------------------------+
*/
#include "php.h"

#define YYCTYPE         unsigned char
#define YYCURSOR        cql
#define YYLIMIT         cql
#define YYMARKER		q
#define YYFILL(n)

char *tokendup(char *str)
{
	int len = 0;
	char *begin = str;
    char ch = NULL;
	while (ch = *str) {
        if (ch == ' ') {
            break;
        }
        ++len;
		++str;
	}
	if (len > 0) {
		char *ret = emalloc(len+1);
		strncpy(ret, begin, len);
        ret[len] = '\0';
		return ret;
	} else {
		return NULL;
	}
}

char *scan_keyspace(char *cql)
{
    char *q;
/*!re2c
	[ ]*'use'[ ]+        {return tokendup(YYCURSOR);}
	[^]				{return (char*) 0;}
 */
}

char *scan_columnfamily(char *cql)
{
	char *q;
/*!re2c
	[ ]*'select'[ ]+.+[ ]+'from'[ ]+        {return tokendup(YYCURSOR);}
	[^]				{return (char*) 0;}
 */
}

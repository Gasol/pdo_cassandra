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

char* ltoa(long N, char *str)
{
    const size_t BUFSIZE = sizeof(int64_t) * 8 + 1;
    register int i = 2;
    long uarg;
    char *tail, *head = str, buf[BUFSIZE];

    tail = &buf[BUFSIZE - 1];           /* last character position      */
    *tail-- = '\0';

    if (N < 0L) {
        *head++ = '-';
        uarg    = -N;
    } else {
        uarg = N;
    }

    if (uarg) {
        for (i = 1; uarg; ++i) {
            register ldiv_t r;
            r = ldiv(uarg, 10);
            *tail-- = (char)(r.rem + ((9L < r.rem) ? ('A' - 10L) : '0'));
            uarg = r.quot;
        }
    } else {
        *tail-- = '0';
    }

    memcpy(head, ++tail, i);
    return str;
}

int64_t deserializeLong(char *str)
{
    int64_t ret = 0;
    int64_t tmp = 0;
    unsigned char *raw_array = (unsigned char *)str;
    ret |= raw_array[7];
    tmp = raw_array[6];
    ret |= (tmp << 8);
    tmp = raw_array[5];
    ret |= (tmp << 16);
    tmp = raw_array[4];
    ret |= (tmp << 24);
    tmp = raw_array[3];
    ret |= (tmp << 32);
    tmp = raw_array[2];
    ret |= (tmp << 40);
    tmp = raw_array[1];
    ret |= (tmp << 48);
    tmp = raw_array[0];
    ret |= (tmp << 56);
    return ret;
}

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
#include<string>

int64_t deserializeLong(std::string &str)
{
    int64_t ret = 0;
    int64_t tmp = 0;
    unsigned char *raw_array= reinterpret_cast<unsigned char *>(const_cast<char *>(str.c_str()));
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

char* stringToHex(char *str)
{
	int len = strlen(str);
	char *buf = (char*) malloc(len * 2 + 1);
	char *ret = buf;
	int i;
	char msb = 0, lsb = 0;
	for (; *str != 0; ++str) {
		msb = *str >> 4 & 15;
		lsb = *str & 15;

		sprintf(buf, "%x", msb);
		sprintf(buf + 1, "%x", lsb);
		buf += 2;
	}
	return ret;
}

char htoi(char ch)
{
	if (ch >= '0' && ch <= '9') {
		return ch - 0x30;
	} else if (ch >= 'a' && ch <= 'f') {
		return ch - 0x61 + 10;
	} else if (ch >= 'A' && ch <= 'F') {
		return ch - 0x41 + 10;
	}

	return -1;
}

char* hexToString(char *hex)
{
	size_t len = strlen(hex);
	if (len & 1) return NULL;

	char *buf = (char*) malloc(len / 2 + 1);
	char *ret = buf;
	int ch = 0, tmp = 0;
	int i;
	for (i = 0; i < len / 2; i++) {
		char msb = hex[i*2];
		char lsb = hex[i*2+1];
		tmp = htoi(msb);
		if (tmp == -1) {
			goto clean;
		}
		ch = tmp << 4;
		tmp = htoi(lsb);
		if (tmp == -1) {
			goto clean;
		}
		ch |= tmp;
		buf[i] = ch;
	}

	return ret;
clean:
	free(buf);
	return NULL;
}

/*============================================
# Filename: savekit.h
# Ver 1.0 2014-06-09
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: 
=============================================*/
#ifndef _SAVEKIT
#define _SAVEKIT
#include<stdio.h>
#include<stdlib.h>
#include"BaseClass.h"
#include<fstream>
#include<iostream>
using namespace std;
class savekit
{
	public:
		savekit(const char * file);
		~savekit();

		integer writei64(i64 value);
		integer writeu64(u64 value);
		integer writeinteger(integer value);
	    integer writeu32(u32 value);
		integer writei16(i16 value);
		integer writeu16(u16 value);

		integer writei64array(i64 * value,integer len);
		integer writeu64array(u64 * value,integer len);
		integer writeintegerarray(integer * value,integer len);
		integer writeu32array(u32 * value,integer len);
		integer writei16array(i16 * value,integer len);
		integer writeu16array(u16 * value,integer len);
	    void close();
	private:
		FILE *w;
};
#endif

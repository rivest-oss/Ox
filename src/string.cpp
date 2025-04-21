/* Ox: a general-purpose library.
** Copyright (C) 2024-2025  Rivest Osz
** 
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "nuclei.hpp"
#include "string.hpp"

namespace Ox {
	ulong strlen(const char *str) {
		if(str == nullptr)
			return 0;

		ulong i = 0;
		while(*str != '\0')
			{ str++; i++; };
		
		return i;
	};

	int String::from(const char *source, Error &err) {
		if(err != nullptr)
			return -1;

		if(source == nullptr) {
			err = "'source' is NULL";
			return -1;
		}

		char *s = (char *)implptr;

		if(s != nullptr) {
			exhale(s);
			implptr = nullptr;
		}

		ulong len = static_cast<ulong>(strlen(source));

		s = inhale<char>(len + 1, err);
		if(s == nullptr)
			return -1;

		for(ulong i = 0; i < len; i++)
			s[i] = source[i];

		implptr = s;
		return 0;
	};

	const char *String::c_str(void) {
		char *s = (char *)implptr;
		return s;
	};

	String String::concat(const char *right) {
		String str;
		Error err;

		if(right == nullptr)
			return str;

		char *left = (char *)implptr;
		if(left == nullptr) {
			str.from(right, err);
			return str;
		}

		if(right == nullptr) {
			str.from(left, err);
			return str;
		}

		ulong len_left = strlen(left);
		ulong len_right = strlen(right);

		char *b = inhale<char>(len_left + len_right + 1, err);
		if(b == nullptr)
			return str;

		ulong i;
		for(i = 0; *left != '\0'; i++)
			b[i] = *left++;
		for(; *right != '\0'; i++)
			b[i] = *right++;

		str.from(b, err);
		return str;
	};

	String String::operator+(const char *right) {
		return concat(right);
	};

	String String::operator+(String &right) {
		return concat(right.c_str());
	};

	String::~String(void) {
		char *s = (char *)implptr;

		if(s != nullptr)
			exhale(s);
	};
};

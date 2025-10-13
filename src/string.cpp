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

#include "../nuclei.hpp"
#include "string.hpp"
#include <cstring>
#include <cerrno>
#include <cstdarg>
#include <cstdio>

namespace Ox {
	ulong strlen(const char *str) {
		if(str == nullptr)
			return 0;

		ulong i = 0;
		while(*str != '\0')
			{ str++; i++; };
		
		return i;
	};

	String::String(void) {};

	String::String(const char *s) {
		from_c(s, err);
	};

	String::String(char *s) {
		from_c(s, err);
	};

	int String::from_fmt(Error &err, const char *format, ...) {
		if(err != nullptr)
			return -1;

		std::va_list args;

		va_start(args, format);
		int length = std::vsnprintf(nullptr, 0, format, args);
		va_end(args);

		if(length < 0) {
			err.from_fmt("Couldn't determine the necessary buffer length: %s", std::strerror(errno));
			err.from_c("Couldn't determine the necessary buffer length");
			return -1;
		}

		char *s = inhale<char>(length + 1, err);
		if(s == nullptr)
			return -1;

		va_start(args, format);
		int c = std::vsnprintf(s, length, format, args);
		va_end(args);

		if(c < 0) {
			exhale(s);
			err.from_fmt("Couldn't format the string: %s", std::strerror(errno));
			err.from_c("Couldn't format the string");
			return -1;
		}

		s[length] = '\0';

		if(implptr != nullptr)
			exhale(implptr);

		implptr = s;
		return 0;
	};

	int String::from_c(const char *source, Error &err) {
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

	Ox::ulong __string_strlen_wchar2utf8(const wchar_t *s) {
		Ox::ulong n = 0;

		while(*s != '\0') {
			if((*s & 0xf800) > 0) n += 3;
			else if((*s & 0x780) > 0) n += 2;
			else n++;

			s++;
		};

		return n;
	};

	void __string_strconv_wchar2utf8(const wchar_t *src, char *dst, ulong max_len) {
		u32 codepoint;
		ulong dst_i = 0;

		if(*src == 0xfeff) src++;
		ox_assert(*src != 0xfffe, "[TODO] : Add byte-swapping support");	// [TODO] : Add byte-swapping support

		while(dst_i < max_len) {
			codepoint = *src;
			if(codepoint == 0)
				break;

			src++;

			if(codepoint >= 0xd800 && codepoint <= 0xdfff) {
				codepoint = (codepoint & 0xff) << 10;

				ox_assert(*src >= 0xd800 && *src <= 0xdfff, "Invalid UTF-16 codepoint");

				codepoint |= (char)*src;

				src++;
			}

			if(codepoint < 0x80) {
				dst[dst_i] = (char)codepoint;
				dst_i++;
				continue;
			}

			if(codepoint < 0x800) {
				dst[dst_i] = 0b110'00000 | (char)(codepoint >> 6);
				dst_i++;

				if(dst_i >= max_len)
					break;

				dst[dst_i] = 0b1000'0000 | (char)(codepoint & 0b111111);
				dst_i++;

				continue;
			}

			if(codepoint < 0x10000) {
				dst[dst_i] = 0b1110'0000 | (char)(codepoint >> 12);
				dst_i++;

				if(dst_i >= max_len)
					break;

				dst[dst_i] = 0b1000'0000 | (char)(codepoint >> 6);
				dst_i++;

				if(dst_i >= max_len)
					break;

				dst[dst_i] = 0b1000'0000 | (char)(codepoint >> 6);
				dst_i++;

				continue;
			}

			if(codepoint < 0x200000) {
				dst[dst_i] = 0b1111'0000 | (char)(codepoint >> 18);
				dst_i++;

				if(dst_i >= max_len)
					break;

				dst[dst_i] = 0b1000'0000 | (char)(codepoint >> 12);
				dst_i++;

				if(dst_i >= max_len)
					break;

				dst[dst_i] = 0b1000'0000 | (char)(codepoint >> 6);
				dst_i++;

				if(dst_i >= max_len)
					break;
				
				dst[dst_i] = 0b1000'0000 | (char)codepoint;
				dst_i++;
	
				continue;
			}

			ox_assert(false, "huh? out of bonds?");
		};

		dst[dst_i] = '\0';
	};

	int String::from_c(const wchar_t *source, Error &err) {
		if(err != nullptr)
			return -1;
		
		if(source == nullptr) {
			err = "'source' is NULL";
			return -1;
		}

		ox_assert(sizeof(wchar_t) == 2, "Unexpected wchar_t size");
		
		char *s = (char *)implptr;

		if(s != nullptr) {
			exhale(s);
			implptr = nullptr;
		}

		ulong len = __string_strlen_wchar2utf8(source);

		s = inhale<char>(len + 1, err);
		if(s == nullptr)
			return -1;

		__string_strconv_wchar2utf8(source, s, len);

		implptr = s;

		return 0;
	};

	const char *String::c_str(void) {
		char *s = (char *)implptr;
		return s;
	};

	String String::concat(const char *right) {
		String str;

		if(right == nullptr)
			return str;

		char *left = (char *)implptr;
		if(left == nullptr) {
			str.from_c(right, err);
			return str;
		}

		if(right == nullptr) {
			str.from_c(left, err);
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

		str.from_c(b, err);
		exhale(b);
		
		return str;
	};

	String String::operator+(const char *right) {
		return concat(right);
	};

	String String::operator+(String &right) {
		return concat(right.c_str());
	};

	String String::operator=(const char *s) {
		from_c(s, err);
		return *this;
	};

	String String::operator=(char *s) {
		from_c(s, err);
		return *this;
	};

	String::~String(void) {
		char *s = (char *)implptr;

		if(s != nullptr)
			exhale(s);
	};
};

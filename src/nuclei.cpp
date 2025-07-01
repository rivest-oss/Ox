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
#include <cstdio>
#include <cstdarg>

#if __has_include(<execinfo.h>) && __has_include(<cxxabi.h>)
	// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
	// published under the WTFPL v2.0
	#include <execinfo.h>
	#include <cxxabi.h>
	#define OX_DEBUG_HASINCL_LINUXTRACE
#endif

namespace Ox {
	void __ox_assert__(const char *file, int line, const char *fn, const char *comment) {
		// Always print to terminal.
		if(comment == nullptr) {
			std::fprintf(stderr, "%s(\x1b[1m%i\x1b[0m): \x1b[1m\x1b[5m\x1b[31mAssertion failed\x1b[0m in function \x1b[4m%s\x1b[0m.\r\n", file, line, fn);
		} else {
			std::fprintf(stderr, "%s(\x1b[1m%i\x1b[0m): \x1b[1m\x1b[5m\x1b[31mAssertion failed\x1b[0m in function \x1b[4m%s\x1b[0m: \x1b[3m%s\r\n", file, line, fn, comment);
		}

		#ifdef OX_DEBUG_HASINCL_LINUXTRACE
			// stacktrace.h (c) 2008, Timo Bingmann from http://idlebox.net/
			// published under the WTFPL v2.0

			void *addrlist[16+1];
			int addrlist_len = backtrace(addrlist, 16+1);

			if(addrlist_len == 0)
				return;

			char **symlist = backtrace_symbols(addrlist, addrlist_len);

			size_t fn_name_len = 256;
			char *fn_name = (char *)calloc(fn_name_len, sizeof(char));

			for(int i = 1; i < addrlist_len && fn_name != nullptr; i++) {
				char *begin_name = nullptr, *begin_off = nullptr, *end_off = nullptr;
				for(char *p = symlist[i]; *p; ++p) {
					if(*p == '(')
						begin_name = p;
					else if(*p == '*')
						begin_off = p;
					else if(*p == ')' && begin_off) {
						end_off = p;
						break;
					}
				};

				if(begin_name && begin_off && end_off) {
					*begin_name++ = '\0';
					*begin_off++ = '\0';
					*end_off = '\0';
					// mangled name is now in [begin_name, begin_off] and caller
					// offset in [begin_off, end_offx], now apply
					// __cxa_demange():
					int status = -1;
					char *ret = abi::__cxa_demangle(begin_name, fn_name, &fn_name_len, &status);

					if(status == 0) {
						fn_name = ret;
						std::fprintf(stderr, "\x1b[2m\t%s: %s+%s\x1b[0m\n", symlist[i], fn_name, begin_off);
					} else {
						// demangling failed, then output function name as a C
						// function with no arguments.
						std::fprintf(stderr, "\x1b[2m\t%s (mangled): %s+%s\x1b[0m\n", symlist[i], begin_name, begin_off);
					}
				} else {
					std::fprintf(stderr, "\x1b[2m\t%s\x1b[0m\n", symlist[i]);
				}
			};

			free(fn_name);
			free(symlist);
		#endif

		std::exit(1);
	};

	void *__ox_alloc(ulong n, const char **err) {
		if(err == nullptr)
			return nullptr;
		if(*err != nullptr)
			return nullptr;
		
		void *p = std::calloc(n, 1);
		if(p == nullptr)
			*err = "Couldn't allocate enough memory";
		
		return p;
	};
	
	void *__ox_realloc(void *src, ulong n, const char **err) {
		if(err == nullptr)
			return nullptr;
		if(*err != nullptr)
			return nullptr;
		
		void *p = std::realloc(src, n);
		if(p == nullptr)
			*err = "Couldn't allocate enough memory";
		
		return p;
	};

	void exhale(void *p) {
		if(p == nullptr)
			return;
		
		std::free(p);
	};

	void Error::clear(void) {
		if(src != nullptr)
			exhale((void *)src);

		src = nullptr;
	};

	int Error::from_fmt(const char *format, ...) {
		if(src != nullptr)
			return 1;

		if(format == nullptr) {
			src = "'format' is NULL";
			return -1;
		}

		std::va_list args;

		va_start(args, format);
		ulong len = std::vsnprintf(nullptr, 0, format, args);
		va_end(args);

		char *buff = (char *)__ox_alloc(sizeof(char) * (len + 1), &src);

		if(buff == nullptr) {
			var = false;
			return -1;
		}

		var = true;
		src = (const char *)buff;

		va_start(args, format);
		(void)std::vsnprintf(buff, len, format, args);
		va_end(args);

		return 0;
	};

	int Error::from_c(const char *str) {
		if(src != nullptr)
			return 1;
		
		if(str == nullptr) {
			src = "'str' is NULL";
			return -1;
		}

		var = false;
		src = str;

		return 0;
	};
};

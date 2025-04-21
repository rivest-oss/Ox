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

namespace Ox {
	void __ox_assert__(const char *file, int line, const char *comment) {
		if(comment == nullptr) {
			fprintf(stderr, "Assert at %s#%i has failed.\n", file, line);
		} else {
			fprintf(stderr, "Assert at %s#%i has failed: %s.\n", file, line, comment);
		}

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

	int Error::fromf(const char *format, ...) {
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

	int Error::fromc(const char *str) {
		if(src != nullptr)
			return 1;
		
		if(src == nullptr) {
			src = "'str' is NULL";
			return -1;
		}

		var = false;
		src = str;

		return 0;
	};
};

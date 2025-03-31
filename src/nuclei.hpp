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

#pragma once
#include <cstdint>
#include <cstdlib>

namespace Ox {
	typedef uint8_t u8;
	typedef uint16_t u16;
	typedef uint32_t u32;
	typedef uint64_t u64;
	
	typedef int8_t i8;
	typedef int16_t i16;
	typedef int32_t i32;
	typedef int64_t i64;
	
	typedef float f32;
	typedef double f64;

	typedef unsigned long ulong;

	void __ox_assert__(const char *file, int line, const char *comment);

	#ifndef NDEBUG
		#define xassert(x,y) if((x) == false) __ox_assert__(__FILE__, __LINE__, y);
	#else
		#define xassert(x,y) (void)0;
	#endif

	template<typename T>
	T *xalloc(u64 count, const char **err) {
		if(*err != nullptr)
			return nullptr;

		void *p = std::calloc(count, sizeof(T));
		if(p == nullptr) {
			*err = "Couldn't allocate enough memory";
			return nullptr;
		}

		return (T *)p;
	};
	
	template<typename T>
	T *xalloc(const char **err) {
		return xalloc<T>(1, err);
	};

	void xfree(void *p);
};

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

#if(defined(_WIN16) || defined(_WIN32) || defined(_WIN64)) && !defined(__WINDOWS__)
	#define __WINDOWS__
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define OX_ENDIANNESS_LE // 321
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define OX_ENDIANNESS_BE // 123
#else
	#error "Unsupported endianness"
#endif

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

	typedef unsigned int uint;
	typedef unsigned long ulong;

	void __ox_assert__(const char *file, int line, const char *comment);

	#ifndef NDEBUG
		#define xassert(x,y) if((x) == false) __ox_assert__(__FILE__, __LINE__, y);
	#else
		#define xassert(x,y) (void)0;
	#endif

	// Implementation of 'alloc'.
	void *__ox_alloc(ulong n, const char **err);

	typedef enum {
		in = 1 << 0,
		out = 1 << 1,
	} openmode;

	typedef enum {
		beg = 0,
		cur = 1 << 2,
		end = 1 << 3,
	} seekdir;

	// Big-Endian <-> Host <-> Little-Endian functions.
	template<typename T>
	T __ox_byteswap(T o) {
		T r;
		
		u8 *op = (u8 *)&o;
		u8 *tp = (u8 *)&r + (sizeof(T) - 1);
		
		for(uint i = 0; i < sizeof(T); i++)
			*tp-- = *op++;
		
		return r;
	};

	template<typename T>
	T htobe(T x) {
		#ifdef OX_ENDIANNESS_BE
			return x;
		#elif defined(OX_ENDIANNESS_LE)
			return __ox_byteswap<T>(x);
		#endif
	};

	template<typename T>
	T htole(T x) {
		#ifdef OX_ENDIANNESS_BE
			return __ox_byteswap<T>(x);
		#elif defined(OX_ENDIANNESS_LE)
			return x;
		#endif
	};

	template<typename T>
	T betoh(T x) {
		#ifdef OX_ENDIANNESS_BE
			return x;
		#elif defined(OX_ENDIANNESS_LE)
			return __ox_byteswap<T>(x);
		#endif
	};

	template<typename T>
	T letoh(T x) {
		#ifdef OX_ENDIANNESS_BE
			return __ox_byteswap<T>(x);
		#elif defined(OX_ENDIANNESS_LE)
			return x;
		#endif
	};

	class Error {
		private:
			bool var = false;
			const char *src = nullptr;
		
		public:
			~Error(void) {
				clear();
			};

			void clear(void);

			const char *c_str(void) {
				return src;
			};

			// From "format".
			int from_fmt(const char *format, ...);

			// From constant.
			int from_c(const char *str);

			Error &operator=(const char *str) {
				clear();
				from_c(str);

				return *this;
			};

			bool operator==(void *p) {
				return src == p;
			};

			bool operator!=(void *p) {
				return src != p;
			};
	};

	// Allocates memory.
	template<typename T>
	T *inhale(ulong n, Error &err) {
		if(err != nullptr)
			return nullptr;

		const char *e = nullptr;
		T *p = (T *)__ox_alloc(n * sizeof(T), &e);
		if(p == nullptr)
			err = e;

		return p;
	};
	
	template<typename T>
	T *inhale(Error &err) {
		return inhale<T>(1, err);
	};
	
	// Frees previously allocated memory.
	void exhale(void *p);
};

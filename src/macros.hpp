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

/*
#define OX_TODO_STR2(x) #x
#define OX_TODO_STR1(x) OX_TODO_STR2(x)
#define OX_TODO(msg) _Pragma("message \"[TODO] " __FILE __ ":" OX_TODO_STR1(__LINE__) ": " msg "\"");
*/

// Sources:
// * https://github.com/cpredef/predef
#if(defined(__linux__))
	#define OX_OS_LINUX 1
#elif defined(_WIN32) || defined(_WIN64) || defined(__WIN32__) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#define OX_OS_WINDOWS 32

	#if defined(_WIN64)
		#define OX_OS_WINDOWS 64
	#endif
#else
	#define OX_OS_UNKNOWN 1
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	#define OX_ENDIANNESS_LE 4321
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define OX_ENDIANNESS_BE 1234
#else
	#error "Unsupported endianness"
#endif

#if !defined(NDEBUG)
	#define OX_DEBUG 1
#endif

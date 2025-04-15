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
#include "../nuclei.hpp"

namespace Ox {
	class BasicIOStream {
		public:
			virtual ulong tellg(void) = 0;
			virtual void seekg(ulong pos) = 0;
			virtual void seekg(i64 off, seekdir dir) = 0;
			
			virtual ulong tellp(void) = 0;
			virtual void seekp(ulong pos) = 0;
			virtual void seekp(i64 off, seekdir dir) = 0;
			
			virtual int read(u8 *s, ulong n, const char **err) = 0;
			virtual int write(u8 *s, ulong n, const char **err) = 0;
			
			u8 readU8(const char **err);

			u16 readU16BE(const char **err);
			u32 readU32BE(const char **err);
			u64 readU64BE(const char **err);
			
			u16 readU16LE(const char **err);
			u32 readU32LE(const char **err);
			u64 readU64LE(const char **err);

			i8 readI8(const char **err);

			i16 readI16BE(const char **err);
			i32 readI32BE(const char **err);
			i64 readI64BE(const char **err);
			
			i16 readI16LE(const char **err);
			i32 readI32LE(const char **err);
			i64 readI64LE(const char **err);

			f32 readF32BE(const char **err);
			f64 readF64BE(const char **err);

			f32 readF32LE(const char **err);
			f64 readF64LE(const char **err);
			
			int writeU8(u8 value, const char **err);

			int writeU16BE(u16 value, const char **err);
			int writeU32BE(u32 value, const char **err);
			int writeU64BE(u64 value, const char **err);
			
			int writeU16LE(u16 value, const char **err);
			int writeU32LE(u32 value, const char **err);
			int writeU64LE(u64 value, const char **err);
			
			int writeI8(i8 value, const char **err);
			
			int writeI16BE(i16 value, const char **err);
			int writeI32BE(i32 value, const char **err);
			int writeI64BE(i64 value, const char **err);
			
			int writeI16LE(i16 value, const char **err);
			int writeI32LE(i32 value, const char **err);
			int writeI64LE(i64 value, const char **err);
			
			int writeF32BE(float value, const char **err);
			int writeF64BE(double value, const char **err);

			int writeF32LE(float value, const char **err);
			int writeF64LE(double value, const char **err);
	};
};

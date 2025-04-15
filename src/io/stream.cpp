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

#include "stream.hpp"

namespace Ox {
	u8 BasicIOStream::readU8(const char **err) {
		u8 b[1];
		read(b, 1, err);
		if(*err != nullptr) return -1;
		return b[0];
	};

	u16 BasicIOStream::readU16BE(const char **err) {
		u8 b[2];
		read(b, 2, err);
		if(*err != nullptr) return -1;
		return betoh<u16>(*(u16 *)b);
	};

	u32 BasicIOStream::readU32BE(const char **err) {
		u8 b[4];
		read(b, 4, err);
		if(*err != nullptr) return -1;
		return betoh<u32>(*(u32 *)b);
	};

	u64 BasicIOStream::readU64BE(const char **err) {
		u8 b[8];
		read(b, 8, err);
		if(*err != nullptr) return -1;
		return betoh<u64>(*(u64 *)b);
	};

	u16 BasicIOStream::readU16LE(const char **err) {
		u8 b[2];
		read(b, 2, err);
		if(*err != nullptr) return -1;
		return letoh<u16>(*(u16 *)b);
	};

	u32 BasicIOStream::readU32LE(const char **err) {
		u8 b[4];
		read(b, 4, err);
		if(*err != nullptr) return -1;
		return letoh<u32>(*(u32 *)b);
	};

	u64 BasicIOStream::readU64LE(const char **err) {
		u8 b[8];
		read(b, 8, err);
		if(*err != nullptr) return -1;
		return letoh<u64>(*(u64 *)b);
	};

	i8 BasicIOStream::readI8(const char **err) {
		return static_cast<i8>(readU8(err));
	};

	i16 BasicIOStream::readI16BE(const char **err) {
		return static_cast<i16>(readU16BE(err));
	};

	i32 BasicIOStream::readI32BE(const char **err) {
		return static_cast<i32>(readU32BE(err));
	};

	i64 BasicIOStream::readI64BE(const char **err) {
		return static_cast<i64>(readU64BE(err));
	};

	f32 BasicIOStream::readF32BE(const char **err) {
		return static_cast<f32>(readU32BE(err));
	};

	f64 BasicIOStream::readF64BE(const char **err) {
		return static_cast<f64>(readU64BE(err));
	};

	i16 BasicIOStream::readI16LE(const char **err) {
		return static_cast<i16>(readU16LE(err));
	};

	i32 BasicIOStream::readI32LE(const char **err) {
		return static_cast<i32>(readU32LE(err));
	};

	i64 BasicIOStream::readI64LE(const char **err) {
		return static_cast<i64>(readU64LE(err));
	};

	f32 BasicIOStream::readF32LE(const char **err) {
		return static_cast<f32>(readU32LE(err));
	};

	f64 BasicIOStream::readF64LE(const char **err) {
		return static_cast<f64>(readU64LE(err));
	};

	int BasicIOStream::writeU8(u8 v, const char **err) {
		u8 b[1] = {
			(u8)v
		};
		return write(b, 1, err);
	};

	int BasicIOStream::writeU16BE(u16 v, const char **err) {
		v = htobe(v);
		u8 *b = (u8 *)&v;
		return write(b, 2, err);
	};

	int BasicIOStream::writeU32BE(u32 v, const char **err) {
		v = htobe(v);
		u8 *b = (u8 *)&v;
		return write(b, 4, err);
	};

	int BasicIOStream::writeU64BE(u64 v, const char **err) {
		v = htobe(v);
		u8 *b = (u8 *)&v;
		return write(b, 8, err);
	};

	int BasicIOStream::writeI8(i8 v, const char **err) {
		return writeU8(static_cast<u8>(v), err);
	};

	int BasicIOStream::writeI16BE(i16 v, const char **err) {
		return writeU16BE(static_cast<u16>(v), err);
	};

	int BasicIOStream::writeI32BE(i32 v, const char **err) {
		return writeU32BE(static_cast<u32>(v), err);
	};

	int BasicIOStream::writeI64BE(i64 v, const char **err) {
		return writeU64BE(static_cast<u64>(v), err);
	};

	int BasicIOStream::writeF32BE(f32 v, const char **err) {
		return writeU32BE(static_cast<u32>(v), err);
	};

	int BasicIOStream::writeF64BE(f64 v, const char **err) {
		return writeU64BE(static_cast<u64>(v), err);
	};

	int BasicIOStream::writeU16LE(u16 v, const char **err) {
		v = htole(v);
		u8 *b = (u8 *)&v;
		return write(b, 2, err);
	};

	int BasicIOStream::writeU32LE(u32 v, const char **err) {
		v = htole(v);
		u8 *b = (u8 *)&v;
		return write(b, 4, err);
	};

	int BasicIOStream::writeU64LE(u64 v, const char **err) {
		v = htole(v);
		u8 *b = (u8 *)&v;
		return write(b, 8, err);
	};

	int BasicIOStream::writeI16LE(i16 v, const char **err) {
		return writeU16LE(static_cast<u16>(v), err);
	};

	int BasicIOStream::writeI32LE(i32 v, const char **err) {
		return writeU32LE(static_cast<u32>(v), err);
	};

	int BasicIOStream::writeI64LE(i64 v, const char **err) {
		return writeU64LE(static_cast<u64>(v), err);
	};

	int BasicIOStream::writeF32LE(f32 v, const char **err) {
		return writeU32LE(static_cast<u32>(v), err);
	};

	int BasicIOStream::writeF64LE(f64 v, const char **err) {
		return writeU64LE(static_cast<u64>(v), err);
	};
};

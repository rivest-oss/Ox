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
#include "../crypto/crc.hpp"
#include <cstring>
#include <cstdio>

void test_crc32(void) {
	const char *str = "The quick brown fox jumps over the lazy dog";

	Ox::CRC32 crc;
	crc.update((Ox::u8 *)str, std::strlen(str));
	Ox::u32 digest = crc.digest();

	if(digest != 0x414fa339) {
		std::fprintf(stderr, "[Crypto/CRC32] ❌ BAD: %08x\n", digest);
		std::exit(1);
	}

	std::printf("[Crypto/CRC32] ✅ OK\n");
};

void test_endian(void) {
	Ox::u8 arr_le[4] = { 0x67, 0x45, 0x23, 0x01 };
	Ox::u8 arr_be[4] = { 0x83, 0x5a, 0x87, 0x6f };

	Ox::u32 endian_le = Ox::letoh<Ox::u32>(*(Ox::u32 *)arr_le);
	Ox::u32 endian_be = Ox::betoh<Ox::u32>(*(Ox::u32 *)arr_be);

	if((endian_le ^ endian_be) != 0x8279c208) {
		std::fprintf(stderr, "[Nuclei/Endian] ❌ BAD: %08x (%08x ^ %08x)\n", endian_le ^ endian_be, endian_le, endian_be);
		std::exit(1);
	}

	std::printf("[Nuclei/Endian] ✅ OK\n");
};

int main(void) {
	test_crc32();
	test_endian();

	return 0;
};

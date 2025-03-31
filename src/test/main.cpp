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

int main(void) {
	const char *str = "The quick brown fox jumps over the lazy dog";

	Ox::CRC32 crc;
	crc.update((Ox::u8 *)str, std::strlen(str));
	Ox::u32 digest = crc.digest();

	if(digest != 0x414fa339) {
		std::fprintf(stderr, "[Crypto/CRC32] ❌ BAD: %08x\n", digest);
		return 1;
	}

	std::printf("[Crypto/CRC32] ✅ OK\n");

	return 0;
};

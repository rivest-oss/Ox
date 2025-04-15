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
#include "../io/filesystem.hpp"
#include "../io/fstream.hpp"
#include <cstring>
#include <cstdio>

void test_endian(void) {
	Ox::u8 arr[8] = {
		0x67, 0x45, 0x23, 0x01,
		0x83, 0x5a, 0x87, 0x6f
	};

	Ox::u32 endian_le = Ox::letoh<Ox::u32>(*(Ox::u32 *)arr);
	Ox::u32 endian_be = Ox::betoh<Ox::u32>(*(Ox::u32 *)(arr + 4));

	if((endian_le ^ endian_be) != 0x8279c208) {
		std::fprintf(stderr, "[Nuclei/Endian] ❌ BAD: %08x (%08x ^ %08x)\n", endian_le ^ endian_be, endian_le, endian_be);
		std::exit(1);
	}

	std::printf("[Nuclei/Endian] ✅ OK\n");
};

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

void test_file_write(void) {
	const char *text = "Oxygen lives!\n";

	const char *err = nullptr;
	Ox::String temp = Ox::FS::temp_path(&err);

	if(err != nullptr) {
		std::fprintf(stderr, "[Filesystem/Temp Path] ❌ BAD: %s\n", err);
		std::exit(1);
	}

	Ox::String path = temp + "/ox-test.txt";
	Ox::FileStream fs = Ox::FS::open(path.c_str(), Ox::out, &err);

	if(err != nullptr) {
		std::fprintf(stderr, "[Filesystem/Write File] ❌ BAD: Open: %s\n", err);
		std::exit(1);
	}

	if(fs.write((Ox::u8 *)text, 14, &err) < 0) {
		std::fprintf(stderr, "[Filesystem/Write File] ❌ BAD: %s\n", err);
		std::exit(1);
	}

	fs.close();

	std::printf("[Filesystem/Write File] ✅ OK\n");
};

void test_file_read(void) {
	const char *text = "Oxygen lives!\n";

	const char *err = nullptr;
	Ox::String temp = Ox::FS::temp_path(&err);

	if(err != nullptr) {
		std::fprintf(stderr, "[Filesystem/Temp Path] ❌ BAD: %s\n", err);
		std::exit(1);
	}

	Ox::String path = temp + "/ox-test.txt";
	Ox::FileStream fs = Ox::FS::open(path.c_str(), Ox::in, &err);

	if(err != nullptr) {
		std::fprintf(stderr, "[Filesystem/Read File] ❌ BAD: Open: %s\n", err);
		std::exit(1);
	}

	char b[14];

	if(fs.read((Ox::u8 *)b, 14, &err) < 0) {
		std::fprintf(stderr, "[Filesystem/Read File] ❌ BAD: %s\n", err);
		std::exit(1);
	}

	fs.close();

	for(int i = 0; i < 14; i++) {
		if(b[i] == text[i])
			continue;

		std::fprintf(stderr, "[Filesystem/Read File] ❌ BAD: Failed comparison %i (%x Vs. %x)\n", i, text[i], b[i]);
		std::exit(1);
	};

	std::printf("[Filesystem/Read File] ✅ OK\n");
};

int main(void) {
	test_endian();

	test_crc32();

	test_file_write();
	test_file_read();

	return 0;
};

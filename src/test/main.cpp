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
#include <cstdarg>
#include <cstring>
#include <cstdio>

#define SUPERVISE(test_name)	\
	\
	auto ENFORCE = [](bool check, const char *format, ...) -> void {	\
		if(check == false) {	\
			std::fprintf(stderr, "\x1b[1m[%s] ❌ BAD: ", test_name);	\
			\
			va_list args;	\
			va_start(args, format);	\
			(void)std::vfprintf(stderr, format, args);	\
			va_end(args);	\
			\
			std::fprintf(stderr, "\x1b[0m\n");	\
			\
			std::exit(1);	\
		}	\
	};	\
	\
	auto OK = [](void) -> void {	\
		printf("[%s] ✅ OK\n", test_name);	\
	};	\
	\
	(void)ENFORCE;	\
	(void)OK;
//

void test_endian(void) {
	SUPERVISE("Nuclei/Endian");

	Ox::u8 arr[8] = {
		0x67, 0x45, 0x23, 0x01,
		0x83, 0x5a, 0x87, 0x6f
	};

	Ox::u32 endian_le = Ox::letoh<Ox::u32>(*(Ox::u32 *)arr);
	Ox::u32 endian_be = Ox::betoh<Ox::u32>(*(Ox::u32 *)(arr + 4));

	ENFORCE((endian_be ^ endian_le) == 0x8279c208, "Check returned 0x%08x and 0x%08x", endian_be, endian_le);
	OK();
};

void test_crc32(void) {
	SUPERVISE("Crypto/CRC32");
	
	const char *str = "The quick brown fox jumps over the lazy dog";
	
	Ox::CRC32 crc;
	crc.update((Ox::u8 *)str, std::strlen(str));
	Ox::u32 digest = crc.digest();

	ENFORCE(digest == 0x414fa339, "Expecting digest 0x414fa339, computed digest 0x%08x", digest);

	OK();
};

void test_file_write(void) {
	SUPERVISE("File system/Write file");

	const char *text = "Oxygen lives!\r\n";

	const char *err = nullptr;
	Ox::String temp = Ox::FS::temp_path(&err);

	ENFORCE(err == nullptr, "GetTemporalPath did not succeed: %s", err);

	Ox::String path = temp + "/ox-test.txt";
	Ox::FileStream fs = Ox::FS::open(path.c_str(), Ox::out, &err);

	ENFORCE(err == nullptr, "Couldn't open the file: %s", err);
	ENFORCE(fs.write((Ox::u8 *)text, 15, &err) == 0, "Couldn't write the file: %s", err);

	fs.close();

	OK();
};

void test_file_read(void) {
	SUPERVISE("File system/Read file");

	const char *text = "Oxygen lives!\r\n";

	const char *err = nullptr;
	Ox::String temp = Ox::FS::temp_path(&err);

	ENFORCE(err == nullptr, "GetTemporalPath did not succeed: %s", err);

	Ox::String path = temp + "/ox-test.txt";
	Ox::FileStream fs = Ox::FS::open(path.c_str(), Ox::in, &err);

	ENFORCE(err == nullptr, "Couldn't open the file: %s", err);

	char buff[15];

	ENFORCE(fs.read((Ox::u8 *)buff, 15, &err) == 0, "Couldn't read the file: %s", err);

	fs.close();

	for(int i = 0; i < 14; i++)
		ENFORCE(buff[i] == text[i], "Failed comparison %i (read = %i, actual = %i)", text[i], buff[i]);

	OK();
};


int main(void) {
	std::printf("\x1b[0m");

	test_endian();

	test_crc32();

	test_file_write();
	test_file_read();

	return 0;
};

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
#include "../string.hpp"
#include "fstream.hpp"

namespace Ox {
	namespace FS {
		typedef enum {
			null = 0,
			all = (1 << 9) - 1,

			owner_read = 1 << 0,
			owner_write = 1 << 1,
			owner_exec = 1 << 2,

			group_read = 1 << 3,
			group_write = 1 << 4,
			group_exec = 1 << 5,

			others_read = 1 << 6,
			others_write = 1 << 7,
			others_exec = 1 << 8,
		} file_perms;

		typedef enum {
			none = 0,
			not_found = 1,
			regular = 2,
			directory = 3,
			symlink = 4,
			block = 5,
			character = 6,
			fifo = 7,
			socket = 8,
			unknown = 9,
		} file_type;

		typedef struct status_t {
			u16 perms = file_perms::null;
			u8 type = file_type::unknown;
			ulong size = 0;
		} status_t;

		typedef struct space_t {
			ulong capacity = -1;
			ulong free = -1;
			ulong available = -1;
		} space_t;

		class Directory {
			private:
				void *implptr = nullptr;
				bool end = true;
			
			public:
				~Directory(void);

				int open(const char *p, Error &err);
				bool is_open(void);
				void close(void);

				String current(Error &err);
				String next(Error &err);
		};

		String abs(const char *p, Error &err);
		int cp(const char *from, const char *to, bool force, Error &err);
		// cp -r
		int cp_all(const char *from, const char *to, bool force, Error &err);
		int mv(const char *from, const char *to, Error &err);
		// mkdir -p
		int mkdir(const char *p, Error &err);
		// ln -s {to} {t}
		int ln(const char *p, const char *to, Error &err);
		int cd(const char *p, Error &err);
		String cwd(Error &err);
		bool exists(const char *p, Error &err);
		int rm(const char *p, Error &err);
		// rm -r
		int rm_all(const char *p, Error &err);
		String temp_path(Error &err);
		signed long file_size(const char *p, Error &err);
		status_t status(const char *p, Error &err);
		space_t space(const char *p, Error &err);

		FileStream open(const char *p, openmode mode, Error &err);
		Directory opendir(const char *p, Error &err);
	};
};
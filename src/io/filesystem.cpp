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

#include "filesystem.hpp"
#include <cstdio>
#include <filesystem>

namespace Ox {
	namespace FS {
		String abs(const char *p, const char **err) {
			String str;

			if(*err != nullptr)
				return str;

			std::filesystem::path path = std::filesystem::absolute(p);

			str.from(path.c_str(), err);
			return str;
		};

		int cp(const char *from, const char *to, bool force, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				if(force)
					std::filesystem::copy(from, to, std::filesystem::copy_options::overwrite_existing);
				else
					std::filesystem::copy(from, to);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		int cp_all(const char *from, const char *to, bool force, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				if(force)
					std::filesystem::copy(from, to, 
						std::filesystem::copy_options::recursive
						| std::filesystem::copy_options::overwrite_existing);
				else
					std::filesystem::copy(from, to,
						std::filesystem::copy_options::recursive);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		int mv(const char *from, const char *to, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				std::filesystem::rename(from, to);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		int mkdir(const char *p, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				std::filesystem::create_directories(p);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		int ln(const char *p, const char *to, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				std::filesystem::create_symlink(to, p);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		int cd(const char *p, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				(void)std::filesystem::current_path(p);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};

			return 0;
		};

		String cwd(const char **err) {
			String str;

			if(*err != nullptr)
				return str;

			try {
				std::filesystem::path path = std::filesystem::current_path();
				str.from(path.c_str(), err);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
			};

			return str;
		};

		bool exists(const char *p, const char **err) {
			if(*err != nullptr)
				return false;

			try {
				return std::filesystem::exists(p);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return false;
			};
		};

		int rm(const char *p, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				(void)std::filesystem::remove(p);
				return 0;
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};
		};

		int rm_all(const char *p, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				(void)std::filesystem::remove_all(p);
				return 0;
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};
		};

		String temp_path(const char **err) {
			String str;

			if(*err != nullptr)
				return str;

			try {
				std::filesystem::path path = std::filesystem::temp_directory_path();
				str.from(path.c_str(), err);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
			};

			return str;
		};

		signed long file_size(const char *p, const char **err) {
			if(*err != nullptr)
				return -1;

			try {
				return static_cast<signed long>(std::filesystem::file_size(p));
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
				return -1;
			};
		};

		status_t status(const char *path, const char **err) {
			status_t stat;

			if(*err != nullptr)
				return stat;

			try {
				std::filesystem::file_status s = std::filesystem::status(path);
				std::filesystem::perms p = s.permissions();
				u16 perms = file_perms::null;

				// The art: Eyehurter.
				// The artist: Rivest Osz.

				// [FIXME] Incorrect file type (unknown).
				switch(s.type()) {
					case std::filesystem::file_type::none: stat.type = file_type::none;
					case std::filesystem::file_type::not_found: stat.type = file_type::not_found;
					case std::filesystem::file_type::regular: stat.type = file_type::regular;
					case std::filesystem::file_type::directory: stat.type = file_type::directory;
					case std::filesystem::file_type::symlink: stat.type = file_type::symlink;
					case std::filesystem::file_type::block: stat.type = file_type::block;
					case std::filesystem::file_type::character: stat.type = file_type::character;
					case std::filesystem::file_type::fifo: stat.type = file_type::fifo;
					case std::filesystem::file_type::socket: stat.type = file_type::socket;
					case std::filesystem::file_type::unknown: stat.type = file_type::unknown;
				};

				if((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none) perms |= file_perms::owner_read;
				if((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none) perms |= file_perms::owner_write;
				if((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) perms |= file_perms::owner_exec;

				if((p & std::filesystem::perms::group_read) != std::filesystem::perms::none) perms |= file_perms::group_read;
				if((p & std::filesystem::perms::group_write) != std::filesystem::perms::none) perms |= file_perms::group_write;
				if((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none) perms |= file_perms::group_exec;

				if((p & std::filesystem::perms::others_read) != std::filesystem::perms::none) perms |= file_perms::others_read;
				if((p & std::filesystem::perms::others_write) != std::filesystem::perms::none) perms |= file_perms::others_write;
				if((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none) perms |= file_perms::others_exec;

				stat.perms = perms;

				try {
					stat.size = static_cast<ulong>(std::filesystem::file_size(path));
				} catch(const std::filesystem::filesystem_error &e) {
					(void)e;
					stat.size = 0;
				};
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
			};

			return stat;
		};

		space_t space(const char *p, const char **err) {
			space_t s;

			if(*err != nullptr)
				return s;

			try {
				std::filesystem::space_info nfo = std::filesystem::space(p);

				s.capacity = static_cast<ulong>(nfo.capacity);
				s.free = static_cast<ulong>(nfo.free);
				s.available = static_cast<ulong>(nfo.available);
			} catch(const std::filesystem::filesystem_error &e) {
				*err = e.what();
			};

			return s;
		};

		FileStream open(const char *p, openmode mode, const char **err) {
			FileStream fs;
			(void)fs.open(p, mode, err);
			return fs;
		};
	};
};
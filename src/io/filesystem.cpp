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
#include <filesystem>

namespace Ox {
	namespace FS {
		String abs(const char *p, Error &err) {
			String str;

			if(err != nullptr)
				return str;

			if(p == nullptr) {
				err = "'p' is NULL";
				return str;
			}

			std::filesystem::path path = std::filesystem::absolute(p);

			str.from_c(path.c_str(), err);
			return str;
		};

		int cp(const char *from, const char *to, bool force, Error &err) {
			if(err != nullptr)
				return -1;

			if(from == nullptr) {
				err = "'from' is NULL";
				return -1;
			}

			if(to == nullptr) {
				err = "'to' is NULL";
				return -1;
			}

			try {
				if(force)
					std::filesystem::copy(from, to, std::filesystem::copy_options::overwrite_existing);
				else
					std::filesystem::copy(from, to);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		int cp_all(const char *from, const char *to, bool force, Error &err) {
			if(err != nullptr)
				return -1;

			if(from == nullptr) {
				err = "'from' is NULL";
				return -1;
			}

			if(to == nullptr) {
				err = "'to' is NULL";
				return -1;
			}

			try {
				if(force)
					std::filesystem::copy(from, to, 
						std::filesystem::copy_options::recursive
						| std::filesystem::copy_options::overwrite_existing);
				else
					std::filesystem::copy(from, to,
						std::filesystem::copy_options::recursive);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		int mv(const char *from, const char *to, Error &err) {
			if(err != nullptr)
				return -1;

			if(from == nullptr) {
				err = "'from' is NULL";
				return -1;
			}

			if(to == nullptr) {
				err = "'to' is NULL";
				return -1;
			}

			try {
				std::filesystem::rename(from, to);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		int mkdir(const char *p, Error &err) {
			if(err != nullptr)
				return -1;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				std::filesystem::create_directories(p);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		int ln(const char *p, const char *to, Error &err) {
			if(err != nullptr)
				return -1;
			
			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			if(to == nullptr) {
				err = "'to' is NULL";
				return -1;
			}

			try {
				std::filesystem::create_symlink(to, p);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		int cd(const char *p, Error &err) {
			if(err != nullptr)
				return -1;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				(void)std::filesystem::current_path(p);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};

			return 0;
		};

		String cwd(Error &err) {
			String str;

			if(err != nullptr)
				return str;

			try {
				std::filesystem::path path = std::filesystem::current_path();
				str.from_c(path.c_str(), err);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
			};

			return str;
		};

		bool exists(const char *p, Error &err) {
			if(err != nullptr)
				return false;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				return std::filesystem::exists(p);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return false;
			};
		};

		int rm(const char *p, Error &err) {
			if(err != nullptr)
				return -1;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				(void)std::filesystem::remove(p);
				return 0;
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};
		};

		int rm_all(const char *p, Error &err) {
			if(err != nullptr)
				return -1;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				(void)std::filesystem::remove_all(p);
				return 0;
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};
		};

		String temp_path(Error &err) {
			String str;

			if(err != nullptr)
				return str;

			try {
				std::filesystem::path path = std::filesystem::temp_directory_path();
				str.from_c(path.c_str(), err);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
			};

			return str;
		};

		signed long file_size(const char *p, Error &err) {
			if(err != nullptr)
				return -1;

			if(p == nullptr) {
				err = "'p' is NULL";
				return -1;
			}

			try {
				return static_cast<signed long>(std::filesystem::file_size(p));
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
				return -1;
			};
		};

		status_t status(const char *path, Error &err) {
			status_t stat;

			if(err != nullptr)
				return stat;

			if(path == nullptr) {
				err = "'path' is NULL";
				return stat;
			}

			namespace stdfs = std::filesystem;

			try {
				stdfs::file_status s = stdfs::status(path);
				stdfs::perms p = s.permissions();
				u16 perms = file_perms::null;

				// The art: Eyehurter.
				// The artist: Rivest Osz.

				// [FIXME] Incorrect file type (unknown).
				switch(s.type()) {
					case stdfs::file_type::none: stat.type = file_type::none;
					case stdfs::file_type::not_found: stat.type = file_type::not_found;
					case stdfs::file_type::regular: stat.type = file_type::regular;
					case stdfs::file_type::directory: stat.type = file_type::directory;
					case stdfs::file_type::symlink: stat.type = file_type::symlink;
					case stdfs::file_type::block: stat.type = file_type::block;
					case stdfs::file_type::character: stat.type = file_type::character;
					case stdfs::file_type::fifo: stat.type = file_type::fifo;
					case stdfs::file_type::socket: stat.type = file_type::socket;
					case stdfs::file_type::unknown: stat.type = file_type::unknown;
				};

				if((p & stdfs::perms::owner_read) != stdfs::perms::none) perms |= file_perms::owner_read;
				if((p & stdfs::perms::owner_write) != stdfs::perms::none) perms |= file_perms::owner_write;
				if((p & stdfs::perms::owner_exec) != stdfs::perms::none) perms |= file_perms::owner_exec;

				if((p & stdfs::perms::group_read) != stdfs::perms::none) perms |= file_perms::group_read;
				if((p & stdfs::perms::group_write) != stdfs::perms::none) perms |= file_perms::group_write;
				if((p & stdfs::perms::group_exec) != stdfs::perms::none) perms |= file_perms::group_exec;

				if((p & stdfs::perms::others_read) != stdfs::perms::none) perms |= file_perms::others_read;
				if((p & stdfs::perms::others_write) != stdfs::perms::none) perms |= file_perms::others_write;
				if((p & stdfs::perms::others_exec) != stdfs::perms::none) perms |= file_perms::others_exec;

				stat.perms = perms;

				try {
					stat.size = static_cast<ulong>(stdfs::file_size(path));
				} catch(const stdfs::filesystem_error &e) {
					(void)e;
					stat.size = 0;
				};
			} catch(const stdfs::filesystem_error &e) {
				err = e.what();
			};

			return stat;
		};

		space_t space(const char *p, Error &err) {
			space_t s;

			if(err != nullptr)
				return s;

			if(p == nullptr) {
				err = "'p' is NULL";
				return s;
			}

			try {
				std::filesystem::space_info nfo = std::filesystem::space(p);

				s.capacity = static_cast<ulong>(nfo.capacity);
				s.free = static_cast<ulong>(nfo.free);
				s.available = static_cast<ulong>(nfo.available);
			} catch(const std::filesystem::filesystem_error &e) {
				err = e.what();
			};

			return s;
		};

		FileStream open(const char *p, openmode mode, Error &err) {
			FileStream fs;

			(void)fs.open(p, mode, err);

			return fs;
		};

		Directory::~Directory(void) {
			close();
		};

		int Directory::open(const char *p, Error &err) {
			close();
			
			using namespace std::filesystem;

			directory_iterator *ip = inhale<directory_iterator>(err);
			if(ip == nullptr)
				return -1;

			new (ip) directory_iterator(p);
			implptr = ip;

			end = false;

			return 0;
		};

		bool Directory::is_open(void) {
			using namespace std::filesystem;
			directory_iterator *ip = (directory_iterator *)implptr;

			return ip != nullptr;
		};

		void Directory::close(void) {
			using namespace std::filesystem;
			directory_iterator *ip = (directory_iterator *)implptr;

			if(ip != nullptr) {
				ip->~directory_iterator();
				exhale(ip);
			}

			implptr = nullptr;
		};

		String Directory::current(Error &err) {
			String s;

			if(err != nullptr)
				return s;
			if(end)
				return s;

			using namespace std::filesystem;
			directory_iterator *ip = (directory_iterator *)implptr;
			directory_iterator &di = *ip;
	
			if(ip == nullptr) {
				err = "Unitialized FileSystem::Directory implementation";
				return s;
			}

			path p = di->path();
			s.from_c(p.c_str(), err);

			return s;
		};

		String Directory::next(Error &err) {
			String s = current(err);
			if(err != nullptr)
				return s;

			using namespace std::filesystem;
			directory_iterator *ip = (directory_iterator *)implptr;
			directory_iterator &di = *ip;
	
			if(ip == nullptr) {
				err = "Unitialized FileSystem::Directory implementation";
				return s;
			}

			try {
				std::error_code ec;
				di.increment(ec);
			} catch(const filesystem_error &e) {
				end = true;
				err = e.what();
			};

			if(di == directory_iterator{})
				end = true;

			return s;
		};

		Directory opendir(const char *p, Error &err) {
			Directory dir;

			(void)dir.open(p, err);

			return dir;
		};
	};
};
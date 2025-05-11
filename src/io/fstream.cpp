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

#include "fstream.hpp"
#include <fstream>
#include <cerrno>
#include <cstring>

namespace Ox {
	std::ios::openmode __ox_impl_filestream_openmode(openmode mode) {
		std::ios::openmode smode = std::ios::binary;

		if(mode & Ox::openmode::in) smode |= std::ios::in;
		if(mode & Ox::openmode::out) smode |= std::ios::out;

		return smode;
	};

	std::ios::seekdir __ox_impl_filestream_seekdir(seekdir dir) {
		if(dir == Ox::seekdir::cur) return std::ios::cur;
		if(dir == Ox::seekdir::end) return std::ios::end;

		return std::ios::beg;
	};

	int FileStream::open(const char *path, openmode mode, Error &err) {
		std::fstream *f = (std::fstream *)__ox_implptr;

		if(err != nullptr)
			return -1;

		if(path == nullptr) {
			err = "'path' is NULL";
			return -1;
		}

		if(f != nullptr && is_open()) {
			err = "File stream is already open";
			return -1;
		}

		if(f == nullptr) {
			f = inhale<std::fstream>(err);
			if(f == nullptr)
				return -1;

			new (f) std::fstream();
		}

		__ox_implptr = f;

		f->clear();
		f->close();
		f->open(path, __ox_impl_filestream_openmode(mode));
		if(f->fail() && errno != 0) {
			err = std::strerror(errno);
			close();
			return -1;
		}

		return 0;
	};

	bool FileStream::is_open(void) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return false;
		
		return f->is_open();
	};

	void FileStream::close(void) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return;

		if(is_open())
			f->close();

		f->~basic_fstream();
		exhale(f);
		__ox_implptr = nullptr;
	};

	ulong FileStream::tellg(void) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return -1;
		
		return static_cast<ulong>(f->tellg());
	};

	void FileStream::seekg(ulong pos) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return;

		f->seekg(pos);
	};

	void FileStream::seekg(long off, seekdir dir) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return;

		f->seekg(off, __ox_impl_filestream_seekdir(dir));
	};

	ulong FileStream::tellp(void) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return -1;
		
		return static_cast<ulong>(f->tellp());
	};

	void FileStream::seekp(ulong pos) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return;

		f->seekp(pos);
	};

	void FileStream::seekp(long off, seekdir dir) {
		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr)
			return;

		f->seekp(off, __ox_impl_filestream_seekdir(dir));
	};

	long FileStream::ignore(ulong n, Error &err) {
		if(err != nullptr)
			return -1;

		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr) {
			err = "Unitialized FileStream implementation";
			return -1;
		}

		(void)f->ignore(n);

		if(f->eof())
			return f->gcount();

		if(f->fail() && errno != 0) {
			err = std::strerror(errno);
			return -1;
		}

		return f->gcount();
	};

	long FileStream::ignore(ulong n, char delimitator, Error &err) {
		if(err != nullptr)
			return -1;

		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr) {
			err = "Unitialized FileStream implementation";
			return -1;
		}

		(void)f->ignore(n, delimitator);

		if(f->eof())
			return f->gcount();

		if(f->fail() && errno != 0) {
			err = std::strerror(errno);
			return -1;
		}

		return f->gcount();
	};

	long FileStream::read(u8 *s, ulong n, Error &err) {
		if(err != nullptr)
			return -1;

		std::fstream *f = (std::fstream *)__ox_implptr;
		if(f == nullptr) {
			err = "Unitialized FileStream implementation";
			return -1;
		}

		if(s == nullptr) {
			err = "'s' is NULL";
			return -1;
		}

		(void)f->read((char *)s, n);

		if(f->eof())
			return f->gcount();

		if(f->fail() && errno != 0) {
			err = std::strerror(errno);
			return -1;
		}

		return f->gcount();
	};

	bool FileStream::eof(Error &err) {
		if(err != nullptr)
			return -1;

		std::fstream *f = (std::fstream *)__ox_implptr;

		if(f == nullptr) {
			err = "Unitialized FileStream implementation";
			return -1;
		}

		(void)f->peek();
		return f->eof();
	};

	int FileStream::write(u8 *s, ulong n, Error &err) {
		if(err != nullptr)
			return -1;

		std::fstream *f = (std::fstream *)__ox_implptr;

		if(f == nullptr) {
			err = "Unitialized FileStream implementation";
			return -1;
		}

		if(s == nullptr) {
			err = "'s' is NULL";
			return -1;
		}

		(void)f->write((char *)s, n);

		if(f->eof())
			return f->gcount();

		if(f->fail() && errno != 0) {
			err = std::strerror(errno);
			return -1;
		}

		return 0;
	};
};

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
#include "stream.hpp"

namespace Ox {
	class FileStream : public BasicIOStream {
		private:
			void *__ox_implptr = nullptr;
		
		public:
			~FileStream(void) {
				close();
			};

			int open(const char *path, openmode mode, Error &err);
			bool is_open(void);
			void close(void);

			ulong tellg(void);
			void seekg(ulong pos);
			void seekg(long off, seekdir dir);

			ulong tellp(void);
			void seekp(ulong pos);
			void seekp(long off, seekdir dir);

			int read(u8 *s, ulong n, Error &err);
			int write(u8 *s, ulong n, Error &err);
	};
};

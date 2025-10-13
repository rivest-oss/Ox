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

namespace Ox {
	namespace Net {
		class Socket {
			private:
				void *implptr = nullptr;

			public:
				~Socket(void) {
					clear();
				};

				void clear(void);

				int from(const char *addr, u16 port, Error &err);

				int get_ip(Error &err);
				const char *get_addr(Error &err);
				u16 get_port(Error &err);

				void *get_handle(Error &err);
		};
	};
};

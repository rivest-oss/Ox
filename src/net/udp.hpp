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
#include "socket.hpp"

namespace Ox {
	namespace Net {
		typedef struct udp_packet_t {
			Socket from;
			Socket to;
			ulong length = 0;
			u8 *data = nullptr;

			bool empty = true;
		} udp_packet_t;

		class UDP {
			private:
				void *implptr = nullptr;
			
			public:
				~UDP(void) {};

				int init(Error &err){(void)err;return 0;};
				int bind(Socket socket, Error &err){(void)socket;(void)err;return 0;};
				void close(void) {};

				udp_packet_t read(Error &err) {(void)err;udp_packet_t p;return p;};
				int send(udp_packet_t packet, Error &err) {(void)packet;(void)err;return 0;};
		};
	};
};

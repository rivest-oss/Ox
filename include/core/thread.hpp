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
	class Thread {
		private:
			void *handle = nullptr;

		public:
			typedef void (*callable_t)(void *user);

			~Thread(void);
			
			// automatically detached, shared memory.
			int init(Ox::Error &err, callable_t f, void *user);
			void join(void);

			// assume nothing
			static Ox::pointer_t get_id(void);
			static int hint_hardware_concurrency(void);
	};

	class Mutex {
		private:
			void *handle = nullptr;
			Ox::pointer_t owner = -1;
		
		public:
			Mutex(void);
			~Mutex(void);

			int lock(Ox::Error &err);
			bool try_lock(Ox::Error &err);
			int unlock(Ox::Error &err);
	};
};

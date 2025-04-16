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
#include "nuclei.hpp"

namespace Ox {
	ulong strlen(const char *str);

	// Placeholder.
	class String {
		private:
			void *implptr = nullptr;

		public:
			int from(const char *source, const char **err);
			const char *c_str(void);

			String concat(const char *with);
			String operator+(const char *right);
			String operator+(String &right);

			~String(void);
	};
};

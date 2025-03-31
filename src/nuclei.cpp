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

#include "nuclei.hpp"
#include <cstdio>

namespace Ox {
	void __ox_assert__(const char *file, int line, const char *comment) {
		if(comment == nullptr) {
			fprintf(stderr, "Assert at %s#%i has failed.\n", file, line);
		} else {
			fprintf(stderr, "Assert at %s#%i has failed: %s.\n", file, line, comment);
		}

		std::exit(1);
	};

	void vfree(void *p) {
		if(p == nullptr)
			return;
		
		std::free(p);
	};
};

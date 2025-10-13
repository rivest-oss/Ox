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
	namespace Env {
		bool is_var_set(const char *name);

		const char *get_var(const char *name);
	
		// accepts NULL value for unset
		int set_var(const char *name, const char *value, bool overwrite, Ox::Error err);
		// accepts NULL value for unset
		void set_var(const char *name, const char *value, bool overwrite);
		// accepts NULL value for unset
	
		void unset_var(const char *name);

		void unset_everything(void);
	};
};

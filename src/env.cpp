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

#include "env.hpp"

#ifdef OX_DISABLE_ENV
	#warning "Flag OX_DISABLE_ENV is set"
#endif

#ifndef OX_DISABLE_ENV
	#include <cstdio>

	#if ox_has_include(<cstdlib>)
		#include <cstdlib>
	#elif ox_has_include(<stdlib.h>)
		#include <stdlib.h>
	#else
		#error "Well, this is awkward..."
	#endif

	#if ox_has_include(<cerrno>)
		#include <cerrno>
	#elif ox_has_include(<errno.h>)
		#include <errno.h>
	#else
		#error "Well, this is awkward..."
	#endif

	#if ox_has_include(<cstring>)
		#include <cstring>
	#elif ox_has_include(<string.h>)
		#include <string.h>
	#else
		#error "Well, this is awkward..."
	#endif

	#if !defined(OX_DISABLE_ENVIRON) && ox_has_include(<unistd.h>)
		#define OX_USE_ENVIRON 1
		#include <unistd.h>
	#else
		#warning "'environ' unavailable, Ox::Env::get_everything() will always fail"
	#endif

	#ifdef OX_OS_WINDOWS
		extern "C" int setenv(const char *, const char *, bool);
		extern "C" int unsetenv(const char *);
		extern "C" int clearenv(void);
		#define OX_DISABLE_ENV 1
	#endif
#endif

namespace Ox {
	namespace Env {
		bool is_var_set(const char *name) {
			#ifdef OX_DISABLE_ENV
				(void)name;
				return false;
			#else
				using namespace std;
				const char *v = getenv(name);
				return v != nullptr && *v != '\0';
			#endif
		};

		const char *get_var(const char *name) {
			#ifdef OX_DISABLE_ENV
				(void)name;
				return nullptr;
			#else
				using namespace std;
				return getenv(name);
			#endif
		};

		int set_var(const char *name, const char *value, bool overwrite, Ox::Error err) {
			if(err != nullptr) {
				return -1;
			}

			#ifdef OX_DISABLE_ENV
				(void)name; (void)value; (void)overwrite;
				err = "Flag OX_DISABLE_ENV is set";
				return -1;
			#else
				using namespace std;

				if(value == nullptr) {
					unset_var(name);
					return 0;
				}

				if(name == nullptr) {
					err = "'name' is NULL";
					return -1;
				}

				if(*name == '\0') {
					err = "'name' is a string of length 0";
					return -1;
				}
				
				if(setenv(name, value, overwrite) < 0) {
					if(errno == EINVAL) {
						err = "Invalid '=' character";
						return -1;
					}

					if(errno == ENOMEM) {
						err = "Insufficient memory to add a new variable to the environment";
						return -1;
					}

					err.from_fmt("%s", strerror(errno));
					err.from_c(strerror(errno));
					return -1;
				}

				return 0;
			#endif
		};

		void set_var(const char *name, const char *value, bool overwrite) {
			#ifdef OX_DISABLE_ENV
				(void)name; (void)value; (void)overwrite;
				return;
			#else
				Ox::Error e;
				(void)set_var(name, value, overwrite, e);
			#endif
		};

		void unset_var(const char *name) {
			#ifdef OX_DISABLE_ENV
				(void)name;
				return;
			#else
				using namespace std;
				unsetenv(name);
			#endif
		};
		
		void unset_everything(void) {
			#ifndef OX_DISABLE_ENV
				using namespace std;
				clearenv();
			#endif
		};
	};
};

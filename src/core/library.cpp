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

#include "library.hpp"
#include "../macros.hpp"

#if !defined(OX_DISABLE_DLFCN) && ox_has_include(<dlfcn.h>)
	#define OX_USE_DLIB_DLFCN
#else
	#error "No <dlfcn.h> support"
#endif

#ifdef OX_USE_DLIB_DLFCN
	#include <dlfcn.h>
#endif

namespace Ox {
	DynamicLibrary::~DynamicLibrary(void) {
		close();
	};

	bool DynamicLibrary::is_open(void) {
		if(handle == nullptr)
			return false;

		#ifdef OX_USE_DLIB_DLFCN
			return true;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	int DynamicLibrary::open(const char *p, Ox::Error &err) {
		close();

		if(err != nullptr)
			return -1;

		if(p == nullptr || *p == '\0') {
			err = "Invalid path";
			return -1;
		}

		#ifdef OX_USE_DLIB_DLFCN
			handle = dlopen(p, RTLD_LAZY);
			if(handle == nullptr) {
				char *s = dlerror();
				if(s == nullptr) {
					err = "Unknown Dynamic Library error";
					return -1;
				}

				err.from_fmt("%s", s);
				err.from_c("Non-allocated Dynamic Library error");
				return -1;
			}

			return 0;
		#else
			#error "Well, this is awkward..."
		#endif
	};

	void DynamicLibrary::close(void) {
		#ifdef OX_USE_DLIB_DLFCN
			if(is_open())
				(void)dlclose(handle);
		#else
			#error "Well, this is awkward..."
		#endif

		handle = nullptr;
	};

	bool DynamicLibrary::has_symbol(const char *name, Ox::Error &err) {
		if(err != nullptr)
			return -1;

		if(name == nullptr || *name == '\0') {
			err = "Invalid path";
			return -1;
		}

		#ifdef OX_USE_DLIB_DLFCN
		#else
			#error "Well, this is awkward..."
		#endif

		Ox::Error e;
		void *addr = cast_symbol<void *>(name, e);
		if(addr == nullptr || e != nullptr)
			return false;

		return true;
	};

	template<typename T>
	T DynamicLibrary::cast_symbol(const char *name, Ox::Error &err) {
		if(err != nullptr)
			return nullptr;

		if(name == nullptr || *name == '\0') {
			err = "Invalid path";
			return nullptr;
		}

		#ifdef OX_USE_DLIB_DLFCN
			void *addr = dlsym(handle, name);
			if(addr == nullptr) {
				char *s = dlerror();
				if(s == nullptr) {
					err = "Unknown Dynamic Library error";
					return nullptr;
				}

				err.from_fmt("%s", s);
				err.from_c("Non-allocated Dynamic Library error");
				return nullptr;
			}

			return reinterpret_cast<T>(addr);
		#else
			#error "Well, this is awkward..."
		#endif
		
		// [TODO]
	};

	void *DynamicLibrary::cast_symbol(const char *name, Ox::Error &err) {
		return cast_symbol<void *>(name, err);
	};

	DynamicLibrary open_dynamic_library(const char *p, Ox::Error &err) {
		DynamicLibrary l;
		(void)l.open(p, err);
		return l;
	};
};

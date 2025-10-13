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
	// Dynamic array.
	template<typename T>
	class Elastic {
		private:
			T *handle_data = nullptr;
			T handle_invalid;
			long handle_size = 0;
			long handle_capacity = 0;

			long suggest_grow(void) {
				long s	= handle_capacity
						+ (handle_capacity >> 2)
						+ (handle_capacity >> 4)
						+ (handle_capacity >> 6)
						+ (handle_capacity >> 8)
						+ 3;

				if(s < 0) s = 0;
				return s;
			};

			long suggest_shrink(void) {
				long s	= handle_capacity
						-((handle_capacity >> 2)
						+ (handle_capacity >> 4)
						+ (handle_capacity >> 6)
						+ (handle_capacity >> 8));

				if(s < 0) s = 0;
				return s;
			};

		public:
			Ox::Error error;

			Elastic(void) {
				clear();
			};

			Elastic(T default_invalid) {
				clear();
				handle_invalid = default_invalid;
			};

			~Elastic(void) {
				clear();
			};

			void clear(void) {
				if(handle_data != nullptr)
					exhale(handle_data);

				handle_data = nullptr;
				handle_size = handle_capacity = 0;
			};

			long fill(T item) {
				for(long i = 0; i < handle_capacity; i++)
					handle_data[i] = item;

				return handle_capacity;
			};

			long push_end(T item, Ox::Error &err) {
				if(err != nullptr)
					return -1;

				if(handle_size >= handle_capacity)
					if(resize(suggest_grow(), err) < 0)
						return -1;

				handle_data[handle_size] = item;
				handle_size++;
				return handle_size;
			};

			T pop_end(Ox::Error &err) {
				if(handle_size <= 0) {
					err = "Out of bonds";
					return handle_invalid;
				}

				T item = handle_data[handle_size];
				handle_data[handle_size] = handle_invalid;
				handle_size--;

				if(handle_size < (handle_capacity >> 1)) {
					Ox::Error meh;
					(void)resize(suggest_shrink(), meh);
					(void)meh;
				}

				return item;
			};

			T &at(long n, Ox::Error &err) {
				if(err != nullptr)
					return handle_invalid;
				if(exists_at(n))
					return handle_data[n];

				err = "Out of bonds";
				return handle_invalid;
			};

			T &at_end(Ox::Error &err) {
				return at(handle_size - 1, err);
			};

			bool exists_at(long n) {
				return n >= 0 && n < handle_size;
			};

			int resize(long n, Ox::Error &err) {
				if(err != nullptr)
					return -1;
				if(n < 0) {
					err = "'n' should be greater or equal to zero";
					return -1;
				}

				if(n == handle_capacity)
					return 0;

				T *dest = handle_data == nullptr
					? inhale<T>(n, err)
					: respire<T>(handle_data, n, err);

				if(dest == nullptr)
					return -1;

				handle_data = dest;
				handle_capacity = n;

				if(handle_capacity < handle_size)
					handle_size = handle_capacity;

				return 0;
			};

			int reserve(long n, Ox::Error &err) {
				if(handle_capacity >= n)
					return 0;

				return resize(n, err);
			};

			// Sets this->error
			T &operator[](long n) { return at(n, error); };

			bool is_empty(void) {
				return handle_size <= 0;
			};

			long size(void) {
				return handle_size;
			};

			long capacity(void) {
				return handle_capacity;
			};

			T *data(Ox::Error &err) {
				if(err != nullptr)
					return nullptr;
				if(handle_data == nullptr)
					err = "Unitialized Elastic";

				return handle_data;
			};
	};
};

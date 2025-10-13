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
#include "../io/stream.hpp"

namespace Ox {
	namespace Media {
		class Matroska {
			private:
				void *handle = nullptr;
			
			public:
				Matroska(void);
				~Matroska(void);

				void init(void);
				int parse(Ox::BasicIOStream &rs, Ox::Error &err);
				void deinit(void);
		};
	};
};

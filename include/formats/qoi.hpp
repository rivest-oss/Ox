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
		class QOI {
			public:
				typedef enum : Ox::u8 {
					sRGB = 0,		// sRGB with linear alpha
					linearRGB = 1,	// linear RGBA
				} Colorspace;

				typedef struct params_t {
					int width = -1, height = -1;
					Colorspace colorspace = sRGB;
					Ox::u8 num_of_channels = 0;

					rgba32p_t *pixels = nullptr;
				} params_t;

				static params_t decode(Ox::BasicIOStream &rs, Ox::Error &err);
				static int encode(Ox::BasicIOStream &os, Ox::Error &err, params_t params);
		};
	};
};

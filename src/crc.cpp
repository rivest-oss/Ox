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

#include "../include/crypto/crc.hpp"

namespace Ox {
	CRC32::CRC32(void) {
		for(u32 i = 0; i < 256; i++) {
			u32 rem = i;
			
			for(u32 j = 0; j < 8; j++) {
				if(rem & 0x1) {
					rem >>= 1;
					rem ^= i_poly;
				} else {
					rem >>= 1;
				}
			};
			
			i_lookup_table[i] = rem;
		};
		
		init();
	};

	void CRC32::init(void) {
		i_state = 0xffffffff;
	};
			
	void CRC32::update(u8 *data, ulong length) {
		if(data == nullptr || length == 0)
			return;

		for(ulong i = 0; i < length; i++)
			i_state = (i_state >> 8) ^ i_lookup_table[(u8)i_state ^ data[i]];
	};
			
	u32 CRC32::digest(void) {
		return ~i_state;
	};
};

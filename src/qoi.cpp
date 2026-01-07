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

#include "../include/formats/qoi.hpp"

namespace Ox {
	namespace Media {
		inline Ox::u8 _qoi_hash(rgba32p_t v) {
			return (v.r * 3 + v.g * 5 + v.b * 7 + v.a * 11) & 0x3f;
		};

		QOI::params_t QOI::decode(Ox::BasicIOStream &rs, Ox::Error &err, bool allow_partial) {
			params_t params {
				-1, -1,
				sRGB, 0,
				nullptr,
			};

			if(err != nullptr)
				return params;

			char magic[4];
			if(rs.read((Ox::u8 *)magic, 4, err) != 4)
				return params;

			if(
				magic[0] != 'q'
				|| magic[1] != 'o'
				|| magic[2] != 'i'
				|| magic[3] != 'f'
			) {
				err = "Invalid file header";
				return params;
			}

			Ox::u32 width = rs.readU32BE(err);
			Ox::u32 height = rs.readU32BE(err);
			Ox::u8 channels = rs.readU8(err);
			Ox::u8 colorspace = rs.readU8(err);

			if(width > 65535 || height > 65535) {
				err = "Unsupported resolution";
				return params;
			}

			if(width < 1 || height < 1) {
				err = "Invalid resolution";
				return params;
			}

			if(channels != 3 && channels != 4) {
				err = "Invalid channels number";
				return params;
			}

			if(colorspace != 0 && colorspace != 1) {
				err = "Invalid colorspace";
				return params;
			}

			Ox::ulong fb_len = width * height;
			Ox::rgba32p_t *fb = Ox::inhale<Ox::rgba32p_t>(fb_len, err);

			if(fb == nullptr)
				return params;

			rgba32p_t index[64];
			for(int i = 0; i < 64; i++)
				index[i] = rgba32p_t { 0x00, 0x00, 0x00, 0x00 };

			rgba32p_t curr_px { 0x00, 0x00, 0x00, 0xff };

			bool is_partial = false;

			Ox::rgba32p_t *pixels = fb;
			Ox::u8 run = 0;
			for(Ox::ulong px_i = 0; px_i < fb_len; px_i++, pixels++) {
				if(run > 0) {
					run--;
				} else {
					if(rs.eof(err) && allow_partial) {
						params.progress = px_i;
						is_partial = true;
						break;
					}

					Ox::u8 op = rs.readU8(err);

					if((op & 0xc0) == 0x00) {
						curr_px = index[op];
					} else if((op & 0xc0) == 0x40) {
						Ox::i8 dr = -2 + Ox::i8((op >> 4) & 0x3);
						Ox::i8 dg = -2 + Ox::i8((op >> 2) & 0x3);
						Ox::i8 db = -2 + Ox::i8(op & 0x3);

						curr_px.r += dr;
						curr_px.g += dg;
						curr_px.b += db;
						
						index[_qoi_hash(curr_px)] = curr_px;
					} else if((op & 0xc0) == 0x80) {
						Ox::u8 op_x = rs.readU8(err);

						Ox::i8 dg = -32 + Ox::i8(op & 0x3f);
						Ox::i8 dg_dr = -8 + Ox::i8((op_x >> 4) & 0xf);
						Ox::i8 dg_db = -8 + Ox::i8(op_x & 0xf);

						dg_dr = dg + dg_dr;
						dg_db = dg + dg_db;

						curr_px.r += dg_dr;
						curr_px.g += dg;
						curr_px.b += dg_db;
						
						index[_qoi_hash(curr_px)] = curr_px;
					} else if((op & 0xc0) == 0xc0) {
						if(op == 0xfe) {
							curr_px.r = rs.readU8(err);
							curr_px.g = rs.readU8(err);
							curr_px.b = rs.readU8(err);
							index[_qoi_hash(curr_px)] = curr_px;
						} else if(op == 0xff) {
							curr_px.r = rs.readU8(err);
							curr_px.g = rs.readU8(err);
							curr_px.b = rs.readU8(err);
							curr_px.a = rs.readU8(err);
							index[_qoi_hash(curr_px)] = curr_px;
						} else {
							run = (op & 0x3f);
						}
					}

					if(err != nullptr) {
						Ox::exhale(fb);
						return params;
					}
				}

				pixels->r = curr_px.r;
				pixels->g = curr_px.g;
				pixels->b = curr_px.b;

				if(channels == 4)
					pixels->a = curr_px.a;
			};

			if(err == nullptr && is_partial && allow_partial == false)
				err = "Reached end of file yet image is still fully loaded";

			if(err != nullptr) {
				Ox::exhale(fb);
				return params;
			}

			if(is_partial == false)
				params.progress = fb_len;

			params.width = width;
			params.height = height;
			params.colorspace = (Colorspace)colorspace;
			params.num_of_channels = channels;
			params.pixels = fb;

			return params;
		};

		int QOI::encode(Ox::BasicIOStream &os, Ox::Error &err, params_t params) {
			if(err != nullptr)
				return -1;

			if(params.width == 0 || params.height == 0) {
				err = "Invalid resolution";
				return -1;
			}

			if(params.width > 65'535 || params.height > 65'535) {
				err = "Unsupported resolution";
				return -1;
			}

			if(params.pixels == nullptr) {
				err = "Invalid pixels pointer";
				return -1;
			}

			if(params.colorspace != 0 && params.colorspace != 1) {
				err = "Invalid colorspace";
				return -1;
			}

			if(params.num_of_channels != 3 && params.num_of_channels != 4) {
				err = "Invalid number of channels";
				return -1;
			}

			Ox::ulong px_len = params.width * params.height;

			os.write((Ox::u8 *)"qoif", 4, err);
			os.writeU32BE(params.width, err);
			os.writeU32BE(params.height, err);
			os.writeU8(params.num_of_channels, err);
			os.writeU8(params.colorspace, err);

			rgba32p_t index[64];
			for(int i = 0; i < 64; i++)
				index[i] = rgba32p_t { 0x00, 0x00, 0x00, 0x00 };

			rgba32p_t prev_px { 0x00, 0x00, 0x00, 0xff };
			rgba32p_t curr_px { 0x00, 0x00, 0x00, 0xff };
			Ox::u8 run = 0;

			rgba32p_t *pixels = params.pixels;

			for(Ox::ulong px_i = 0; px_i < px_len; px_i++, pixels++, prev_px = curr_px) {
				curr_px = *pixels;

				if(
					curr_px.r == prev_px.r
					&& curr_px.g == prev_px.g
					&& curr_px.b == prev_px.b
					&& curr_px.a == prev_px.a
				) {
					run++;
					if(run == 62) {
						os.writeU8(0xc0 | (run - 1), err);
						run = 0;

						if(err != nullptr)
							return -1;
					}

					continue;
				}

				if(run > 0) {
					os.writeU8(0xc0 | (run - 1), err);
					run = 0;

					if(err != nullptr)
						return -1;
				}

				Ox::u8 hash = _qoi_hash(curr_px);

				rgba32p_t indexed_px = index[hash];
				if(
					curr_px.r == indexed_px.r
					&& curr_px.g == indexed_px.g
					&& curr_px.b == indexed_px.b
					&& curr_px.a == indexed_px.a
				) {
					os.writeU8(hash, err);
					if(err != nullptr)
						return -1;

					continue;
				}

				if(curr_px.a != prev_px.a) {
					os.writeU8(0xff, err);
					os.writeU8(curr_px.r, err);
					os.writeU8(curr_px.g, err);
					os.writeU8(curr_px.b, err);
					os.writeU8(curr_px.a, err);

					if(err != nullptr)
						return -1;

					continue;
				}

				Ox::i8 dr = curr_px.r - prev_px.r;
				Ox::i8 dg = curr_px.g - prev_px.g;
				Ox::i8 db = curr_px.b - prev_px.b;

				Ox::i8 dg_dr = dr - dg;
				Ox::i8 dg_db = db - dg;

				if(
					dr > -3 && dr < 2
					&& dg > -3 && dg < 2
					&& db > -3 && db < 2
				) {
					Ox::u8 op = 0x40;
					op |= (2 + dr) << 4;
					op |= (2 + dg) << 2;
					op |=  2 + db;

					os.writeU8(0x40 | op, err);
					if(err != nullptr)
						return -1;

					continue;
				}

				if(
					dg_dr > -9 && dg_dr < 8
					&& dg > -33 && dg < 32
					&& dg_db > -9 && dg_db < 8
				) {
					os.writeU8(0x80 | (32 + dg), err);
					os.writeU8(((8 + dg_dr) << 4) | (8 + dg_db), err);

					if(err != nullptr)
						return -1;

					continue;
				}

				os.writeU8(0xfe, err);
				os.writeU8(curr_px.r, err);
				os.writeU8(curr_px.g, err);
				os.writeU8(curr_px.b, err);

				if(err != nullptr)
					return -1;
			};

			Ox::u8 bottom[8] = {
				0,0,0,0,0,0,0,
				0x01
			};

			if(run > 0)
				os.writeU8(0xc0 | (run - 1), err);

			os.write(bottom, 8, err);
			
			if(err != nullptr)
				return -1;

			return 0;
		};
	};
};

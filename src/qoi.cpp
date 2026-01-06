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

		typedef struct _qoi_t {
			int width, height;
			Ox::u8 channels, colorspace;
			rgba32p_t *framebuff = nullptr;
		} _qoi_t;

		QOI::QOI(void) {
			handle = nullptr;
		};

		QOI::~QOI(void) {
			deinit();
		};

		void QOI::init(void) {
			deinit();
		};

		int QOI::parse(Ox::BasicIOStream &rs, Ox::Error &err) {
			deinit();

			if(err != nullptr)
				return -1;

			if(handle != nullptr) {
				err = "QOI already decoded or in progress";
				return -1;
			}

			char magic[4];
			if(rs.read((Ox::u8 *)magic, 4, err) != 4)
				return -1;

			if(
				magic[0] != 'q'
				|| magic[1] != 'o'
				|| magic[2] != 'i'
				|| magic[3] != 'f'
			) {
				err = "Invalid file header";
				return -1;
			}

			Ox::u32 width = rs.readU32BE(err);
			Ox::u32 height = rs.readU32BE(err);
			Ox::u8 channels = rs.readU8(err);
			Ox::u8 colorspace = rs.readU8(err);

			if(width > 65535 || height > 65535) {
				err = "Unsupported resolution";
				return -1;
			}

			if(width < 1 || height < 1) {
				err = "Invalid resolution";
				return -1;
			}

			if(channels != 3 && channels != 4) {
				err = "Invalid channels number";
				return -1;
			}

			if(colorspace != 0 && colorspace != 1) {
				err = "Invalid colorspace";
				return -1;
			}

			Ox::ulong fb_len = width * height;
			Ox::rgba32p_t *fb = Ox::inhale<Ox::rgba32p_t>(fb_len, err);

			if(fb == nullptr)
				return -1;

			rgba32p_t index[64];
			for(int i = 0; i < 64; i++)
				index[i] = rgba32p_t { 0x00, 0x00, 0x00, 0x00 };

			rgba32p_t curr_px { 0x00, 0x00, 0x00, 0xff };

			Ox::rgba32p_t *pixels = fb;
			Ox::u8 run = 0;
			for(Ox::ulong px_i = 0; px_i < fb_len; px_i++, pixels++) {
				if(run > 0) {
					run--;
				} else {
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
						return -1;
					}
				}

				pixels->r = curr_px.r;
				pixels->g = curr_px.g;
				pixels->b = curr_px.b;

				if(channels == 4)
					pixels->a = curr_px.a;
			};

			if(err != nullptr) {
				Ox::exhale(fb);
				return -1;
			}

			_qoi_t *qoi = Ox::inhale<_qoi_t>(err);
			qoi->width = width;
			qoi->height = height;
			qoi->channels = channels;
			qoi->colorspace = colorspace;
			qoi->framebuff = fb;

			handle = qoi;

			return 0;
		};

		void QOI::deinit(void) {
			if(handle != nullptr)
				Ox::exhale(handle);
		};

		Ox::u8 QOI::channels(Ox::Error &err) {
			if(err != nullptr)
				return -1;

			if(handle == nullptr) {
				err = "Unitialized QOI implementation";
				return -1;
			}

			_qoi_t *qoi = (_qoi_t *)handle;
			return qoi->channels;
		};
		Ox::u8 QOI::colorspace(Ox::Error &err) {
			if(err != nullptr)
				return -1;

			if(handle == nullptr) {
				err = "Unitialized QOI implementation";
				return -1;
			}

			_qoi_t *qoi = (_qoi_t *)handle;
			return qoi->colorspace;
		};

		int QOI::width(Ox::Error &err) {
			if(err != nullptr)
				return -1;

			if(handle == nullptr) {
				err = "Unitialized QOI implementation";
				return -1;
			}

			_qoi_t *qoi = (_qoi_t *)handle;
			return qoi->width;
		};

		int QOI::height(Ox::Error &err) {
			if(err != nullptr)
				return -1;

			if(handle == nullptr) {
				err = "Unitialized QOI implementation";
				return -1;
			}

			_qoi_t *qoi = (_qoi_t *)handle;
			return qoi->height;
		};

		rgba32p_t *QOI::pixels(Ox::Error &err) {
			if(err != nullptr)
				return nullptr;

			if(handle == nullptr) {
				err = "Unitialized QOI implementation";
				return nullptr;
			}

			_qoi_t *qoi = (_qoi_t *)handle;
			return qoi->framebuff;
		};
	};
};

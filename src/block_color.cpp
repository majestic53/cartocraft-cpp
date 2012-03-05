/*
 * block_color.cpp
 * Copyright (C) 2012 David Jolly
 * ----------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "block_color.hpp"

/*
 * Block colors
 */
const unsigned int block_color::COLOR[block_color::MAX_BLOCK + 1] = {
	0x0, 		0x747474ff, 0x58864cff, 0xb9855cff, 0xa9a9a9ff, 0xbc9862ff, 0x49cc25ff, 0x575757ff,
	0x77a9ffff, 0x77a9ffff, 0xc63004ff, 0xd5570eff, 0xe2dba3ff, 0x9b9092ff, 0xfcee4bff, 0xd8af93ff,
	0x3f3f3fff, 0x624e30ff, 0x399013ff, 0xc1c139ff, 0xc0f5feff, 0x1b4aa1ff, 0x2a4f8cff, 0x797979ff,
	0xcdc592ff, 0x915840ff, 0xa92b2fff, 0xa92b2fff, 0xa92b2fff, 0x6c6439ff, 0xedededff, 0x265e0dff,
	0x946428ff, 0xb4905aff, 0x9f844dff, 0xf1f1f1ff, 0x959595ff, 0xb6bc01ff, 0x910205ff, 0xcc9978ff,
	0xfe2a2aff, 0xfcd637ff, 0xe0e0e0ff, 0xa8a8a8ff, 0xa8a8a8ff, 0x894c3bff, 0x6b210dff, 0xb4905aff,
	0x3a623aff, 0x3c3056ff, 0xffd800ff, 0xc56502ff, 0x172631ff, 0xb4905aff, 0xab792dff, 0xd9d9d9ff,
	0x77cefbff, 0xa5ece9ff, 0xa26b3eff, 0x00ad17ff, 0x633f24ff, 0x787878ff, 0x787878ff, 0xbc9862ff,
	0x916d38ff, 0xac8852ff, 0xa4a4a4ff, 0x818181ff, 0xbc9862ff, 0x808080ff, 0x747474ff, 0xb7b7b7ff,
	0x9f844dff, 0xaa0404ff, 0xaa0404ff, 0x560000ff, 0xfd0000ff, 0x8f8f8fff, 0xe3f3f3ff, 0xb8d2ffff,
	0xe3f3f3ff, 0x11831fff, 0xa1a5b6ff, 0xaadb74ff, 0x945f44ff, 0x9f844dff, 0xe3901dff, 0x884f4fff,
	0x6a5243ff, 0xffbc5eff, 0xae5ae8ff, 0xe3901dff, 0xeae9ebff, 0xb0b0b0ff, 0xb0b0b0ff, 0xab792dff,
	0x8f6b35ff, 0x747474ff, 0x797979ff, 0xceae7bff, 0xceae7bff, 0xaaa49bff, 0xb3d6dbff, 0xb1af27ff,
	0xc8c8c8ff, 0xc8c8c8ff, 0x1e4c0aff, 0x9f844dff, 0x9a5542ff, 0x797979ff, 0x6f6264ff, 0x134b1cff,
	0x3e1e24ff, 0x3e1e24ff, 0x3e1e24ff, 0x93283cff, 0x93283cff, 0x4d4d4dff, 0x4f4f4fff, 0x909090ff,
	0x3a675fff, 0xebf8b6ff, 0x1d0120ff, 0xa47c51ff, 0xd39b48ff,
};

/*
 * Returns true if a given id is transparent
 */
bool block_color::is_transparent(unsigned int id) {
	return (id == AIR
			|| id == WATER_1
			|| id == WATER_2
			|| id == LAVA_1
			|| id == LAVA_2
			|| id == GLASS
			|| id == ICE);
}

/*
 * block_color.hpp
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

#ifndef BLOCK_COLOR_HPP_
#define BLOCK_COLOR_HPP_

class block_color {
public:

	/*
	 * Transparent block type
	 */
	enum TRANSPARENT { AIR = 0, WATER_1 = 8, WATER_2 = 9, LAVA_1 = 10, LAVA_2 = 11, GLASS = 20, ICE = 79, };

	static const unsigned int BLACK = 0x000000ff;

	/*
	 * Maximum block
	 */
	static const unsigned int MAX_BLOCK = 124;

	/*
	 * Block colors
	 */
	static const unsigned int COLOR[];

	/*
	 * Returns true if a given id is transparent
	 */
	static bool is_transparent(unsigned int id);
};

#endif

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

#include <set>

class block_color {
public:

	/*
	 * Common block types
	 */
	enum BLOCK { AIR = 0, WATER_1 = 8, WATER_2 = 9, LAVA_1 = 10, LAVA_2 = 11, GLASS = 20, ICE = 79,
				TORCH = 50, FIRE = 51, RED_STONE_TORCH = 76, GLOW_STONE = 89, JACK_O_LANTERN = 91,
				RED_STONE_LAMP = 124, };

	/*
	 * Transparent block types
	 */
	static const unsigned int TRANS_COUNT = 7;
	static const unsigned int TRANS[TRANS_COUNT];
	static const std::set<unsigned int> TRANSPARENT;

	/*
	 * Emitter block types
	 */
	static const unsigned int EMIT_COUNT = 8;
	static const unsigned int EMIT[EMIT_COUNT];
	static const std::set<unsigned int> EMITTER;

	/*
	 * Fill color
	 */
	static const unsigned int FILL = 0x000000ff;

	/*
	 * Maximum block
	 */
	static const unsigned int MAX_BLOCK = 124;

	/*
	 * Block colors
	 */
	static const unsigned int COLOR[MAX_BLOCK + 1];

	/*
	 * Returns true if a given block id is an emitter
	 */
	static bool is_emitter(unsigned int id) { return EMITTER.find(id) != EMITTER.end(); }

	/*
	 * Returns true if a given block id is transparent
	 */
	static bool is_transparent(unsigned int id) { return TRANSPARENT.find(id) != TRANSPARENT.end(); }
};

#endif

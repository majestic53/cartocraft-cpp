/*
 * biome_color.hpp
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

#ifndef BIOME_COLOR_HPP_
#define BIOME_COLOR_HPP_

class biome_color {
public:

	/*
	 * Biome types
	 */
	enum BIOME { OCEAN, PLAINS, DESERT, MOUNTAINS, FOREST, TAIGA, SWAMPLAND, RIVER, HELL, SKY, FROZEN_OCEAN,
				FROZEN_RIVER, ICE_PLAINS, ICE_MOUNTAINS, MUSHROOM_ISLAND, MUSHROOM_ISLAND_SHORE, BEACH, DESERT_HILLS,
				FOREST_HILLS, TAIGA_HILLS, MOUNTAINS_EDGE, JUNGLE, JUNGLE_HILLS };

	/*
	 * Maximum biome
	 */
	static const unsigned int MAX_BIOME = 22;

	/*
	 * Biome color scales
	 */
	static const float COLOR_SCALE[MAX_BIOME + 1];
};

#endif

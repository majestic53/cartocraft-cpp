/*
 * carto.hpp
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

#ifndef CARTO_HPP_
#define CARTO_HPP_

#include <string>
#include <vector>
#include "image_buffer.hpp"
#include "../lib/headers/region_file_exc.hpp"
#include "../lib/headers/region_file_reader.hpp"

class carto {
private:

	/*
	 * Image buffer (stored with byte-order: RGBA)
	 */
	image_buffer terrain;

	/*
	 * Image buffer region filled
	 */
	bool *region_filled;

	/*
	 * Image buffer width, height, region count & heightmap
	 */
	unsigned int offset_x, offset_z, region_count, *heightmap;

	/*
	 * Blend a foreground color with a given pixel at x, z coord
	 */
	bool apply_alpha_blend(unsigned int px_x, unsigned int px_z, unsigned int fore);

	/*
	 * Scale a color at a given pixel at x, z coord
	 */
	bool apply_scale(unsigned int px_x, unsigned int px_z, float amount);

	/*
	 * Render a region file
	 */
	int render_region(const std::string &reg_file, unsigned int ren_height);

	/*
	 * Render screen-space ambient occlusion (SSAO) at a given region
	 */
	void render_region_occlusion(unsigned int off_x, unsigned int off_z);

public:

	/*
	 * Cartocraft exit codes
	 */
	static const int SUCCESS = 0;
	static const int VERSION = -1;
	static const int USAGE = -2;
	static const int MALFORMED_FLAG = -3;
	static const int NOT_A_DIRECTORY = -4;
	static const int FILES_NOT_FOUND = -5;
	static const int ALLOC_FAILED = -6;
	static const int REGION_FILE_EXC = -7;

	/*
	 * Cartocraft info
	 */
	static const std::string COPYRIGHT;
	static const std::string USE;
	static const std::string VER_NUM;
	static const std::string WARRANTY;

	/*
	 * Cartocraft render constants
	 */
	static const unsigned int SAMPLE_RADII_COUNT = 9;
	static const int SAMPLE_RADII[SAMPLE_RADII_COUNT];

	/*
	 * Cartocraft defaults
	 */
	static const unsigned int BLOCK_WIDTH_PER_REGION = 512;
	static const unsigned int DEF_HEIGHT = 256;
	static const std::string DEF_FILE_DIR, DEF_OUT_PATH;

	/*
	 * Cartocraft flags
	 */
	enum FLAGS { NOT_FLAG = -1, REGION_FILE_DIR, RENDER_HEIGHT, OUTPUT_PATH, DISP_USAGE, DISP_VERSION };
	static const std::string FLAG[];
	static const unsigned int FLAG_COUNT = 5;

	/*
	 * Cartocraft constructor
	 */
	carto(void);

	/*
	 * Cartocraft destructor
	 */
	virtual ~carto(void);

	/*
	 * Returns a maps height
	 */
	unsigned int get_height(void) { return terrain.get_height(); }

	/*
	 * Returns a maps raw pixel buffer, (channel order: RGBA)
	 */
	std::vector<unsigned char> &get_pixel_buffer(void) { return terrain.get_raw(); }

	/*
	 * Returns a maps width
	 */
	unsigned int get_width(void) { return terrain.get_width(); }

	/*
	 * Returns true if a region in the image buffer is filled
	 */
	bool is_filled(unsigned int x, unsigned int z);

	/*
	 * Returns an integer corresponding to a flag, or NOT_FLAG
	 */
	static int is_flag(const std::string &arg);

	/*
	 * Render a series of regions and output image at specified path
	 */
	int render_map(const std::string &reg_dir, unsigned int ren_height, bool ren_occlusion);

	/*
	 * Write rendered regions to file as a png
	 */
	void write(const std::string &path) { terrain.write(path); }
};

#endif

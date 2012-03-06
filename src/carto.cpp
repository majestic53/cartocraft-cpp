/*
 * carto.cpp
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

#include <boost/filesystem.hpp>
#include <cstdint>
#include <iostream>
#include "block_color.hpp"
#include "carto.hpp"
#include "region_dim.hpp"
#include "region_file.hpp"

/*
 * Cartocraft info
 */
const std::string carto::COPYRIGHT("Copyright (C) 2012 David Jolly");
const std::string carto::USE("carto [-v | -h] [-p REGION_FILE_DIR] [-r RENDER_HEIGHT] [-o OUTPUT_PATH]");
const std::string carto::VER_NUM("Cartocraft 0.1.1");
const std::string carto::WARRANTY("This is free software. There is NO warranty.");

/*
 * Cartocraft render constants
 */
const int carto::RADII[RADII_COUNT] = { 1, 1, 1, 1, 1, 2, 4, 8, 16 };

/*
 * Cartocraft defaults
 */
const std::string carto::DEF_FILE_DIR("."), carto::DEF_OUT_PATH("out.png");

/*
 * Cartocraft flags
 */
const std::string carto::FLAG[carto::FLAG_COUNT] = { "-p", "-r", "-o", "-h", "-v" };

/*
 * Cartocraft constructor
 */
carto::carto(void) {
	center_x = 0;
	center_z = 0;
	region_count = 0;
	region_filled = NULL;
	heightmap = NULL;
}

/*
 * Cartocraft destructor
 */
carto::~carto(void) {
	delete[] region_filled;
	delete[] heightmap;
}

/*
 * Apply occlusion to a given pixel at x, z coord
 */
bool carto::apply_occlusion(unsigned int px_x, unsigned int px_z, float amount) {
	unsigned int col;
	unsigned char n_col[image_buffer::CHANNELS];
	if(amount == 0)
		return true;
	col = img.at(px_x, px_z);

	// iterate through all color channels
	for(unsigned int i = 0; i < image_buffer::CHANNELS; ++i) {
		n_col[i] = (unsigned char) (col >> (24 - (i * 8)));

		// do not apply occlusion to alpha channel
		if(i != image_buffer::ALPHA)
			n_col[i] *= amount;
	}

	// apply the new color
	return img.set(px_x, px_z, ((n_col[0] << 24) | (n_col[1] << 16) | (n_col[2] << 8) | n_col[3]));
}

/*
 * Returns true if a region in the image buffer is filled
 */
bool carto::is_filled(unsigned int x, unsigned int z) {
	unsigned int pos = z * (img.get_width() / BLOCK_WIDTH_PER_REGION) + x;

	// check if position is valid
	if(pos >= region_count)
		return false;
	return region_filled[pos];
}

/*
 * Returns an integer corresponding to a flag, or NOT_FLAG
 */
int carto::is_flag(const std::string &arg) {
	if(arg == FLAG[REGION_FILE_DIR])
		return REGION_FILE_DIR;
	if(arg == FLAG[RENDER_HEIGHT])
		return RENDER_HEIGHT;
	if(arg == FLAG[OUTPUT_PATH])
		return OUTPUT_PATH;
	if(arg == FLAG[DISP_USAGE])
		return DISP_USAGE;
	if(arg == FLAG[DISP_VERSION])
		return DISP_VERSION;
	return NOT_FLAG;
}

/*
 * Render a series of regions and output image at specified path
 */
int carto::render_map(const std::string &reg_dir, unsigned int ren_height) {
	std::string file;
	std::vector<std::string> reg_files;
	std::vector<std::string>::iterator reg_file;
	int x, z, x_min = 0, z_min = 0, x_max = 0, z_max = 0, res;

	// check if region directory exists
	if(!boost::filesystem::exists(reg_dir)) {
		std::cerr << "Exception: Directory does not exist: " << reg_dir << std::endl;
		return NOT_A_DIRECTORY;
	}

	// iterate through all files in region directory
	boost::filesystem::directory_iterator end, iter(reg_dir);
	for(; iter != end; ++iter) {
		file = iter->path().native_file_string();

		// skip directories
		if(boost::filesystem::is_directory(*iter))
			continue;

		// parse for region files
		if(region_file::is_region_file(file, x, z)) {
			reg_files.push_back(file);

			// record min/max region coord
			if(x < x_min)
				x_min = x;
			if(z < z_min)
				z_min = z;
			if(x > x_max)
				x_max = x;
			if(z > z_max)
				z_max = z;
		}
	}

	// check if region files exist
	if(reg_files.empty())
		return FILES_NOT_FOUND;

	// calculate image size, allocate pixel buffer & filled buffer
	img = image_buffer(BLOCK_WIDTH_PER_REGION * (abs(x_min) + x_max) + BLOCK_WIDTH_PER_REGION,
				BLOCK_WIDTH_PER_REGION * (abs(z_min) + z_max) + BLOCK_WIDTH_PER_REGION);
	img.fill(block_color::BLACK);
	center_x = img.get_width() / 2 - (BLOCK_WIDTH_PER_REGION / 2);
	center_z = img.get_height() / 2 - (BLOCK_WIDTH_PER_REGION / 2);
	std::cout << "Rendering map: size: (" << img.get_width() << ", " << img.get_height() << "), center: (" << center_x << ", " << center_z << ")..." << std::endl;

	region_count = (img.get_width() / BLOCK_WIDTH_PER_REGION) * (img.get_height() / BLOCK_WIDTH_PER_REGION);
	region_filled = new bool[region_count];
	heightmap = new unsigned int[img.get_width() * img.get_height()];
	if(!region_filled
			|| !heightmap)
		return ALLOC_FAILED;
	memset(region_filled, false, region_count);
	memset(heightmap, 0, img.get_width() * img.get_height());

	// render image (by region)
	for(reg_file = reg_files.begin(); reg_file != reg_files.end(); ++reg_file)
		if((res = render_region(*reg_file, ren_height)) != SUCCESS)
			return res;

	// reader occlusion
	for(int z = 0; z < (abs(z_min) + z_max) + 1; ++z)
		for(int x = 0; x < (abs(x_min) + x_max) + 1; ++x)
			if(is_filled(x, z))
				render_region_occlusion(x, z);
	return SUCCESS;
}

/*
 * Render a region file
 */
int carto::render_region(const std::string &reg_file, unsigned int ren_height) {
	region_file_reader reader;
	std::vector<int32_t> blocks, heights;
	unsigned int block_height, block_id, pos;
	int reg_x, reg_z, ch_x, ch_z, b_x, b_z;

	// open region file and collect data
	try {
		reader = region_file_reader(reg_file);
		std::cout << "Processing region: " << reader.to_string() << "..." << std::endl;

		// calculate region offsets
		reg_x = center_x + (reader.get_x_coord() * BLOCK_WIDTH_PER_REGION) + BLOCK_WIDTH_PER_REGION;
		reg_z = center_z + (reader.get_z_coord() * BLOCK_WIDTH_PER_REGION);

		// set filled region to true
		if(!is_filled(reg_x / BLOCK_WIDTH_PER_REGION, reg_z / BLOCK_WIDTH_PER_REGION))
			region_filled[((reg_z / BLOCK_WIDTH_PER_REGION) * (img.get_width() / BLOCK_WIDTH_PER_REGION)) + (reg_x / BLOCK_WIDTH_PER_REGION)] = true;

		// render region chunk-by-chunk
		for(unsigned int chunk_z = 0; chunk_z < region_dim::REGION_Z; ++chunk_z)
			for(unsigned int chunk_x = 0; chunk_x < region_dim::REGION_X; ++chunk_x) {

				// check if chunk exists
				if(!reader.is_filled(chunk_x, chunk_z))
					continue;

				// collect chunk heightmap data
				blocks = reader.get_blocks_at(chunk_x, chunk_z);
				heights = reader.get_heightmap_at(chunk_x, chunk_z);

				// skip over empty chunks
				if(blocks.empty()
						|| heights.empty()) {
					std::cerr << "Warning: Chunk missing data at (" << chunk_x << ", " << chunk_z << "). Skipping." << std::endl;
					continue;
				}

				// calculate chunk offsets
				ch_x = reg_x + (chunk_x * region_dim::CHUNK_X);
				ch_z = reg_z + (chunk_z * region_dim::CHUNK_Z);

				// process each chunk block-by-block
				for(unsigned int block_z = 0; block_z < region_dim::CHUNK_Z; ++block_z)
					for(unsigned int block_x = 0; block_x < region_dim::CHUNK_X; ++block_x) {
						block_height = heights.at(block_z * region_dim::CHUNK_Z + block_x);
						if((unsigned int) block_height > ren_height)
							block_height = ren_height;
						pos = (block_height * region_dim::SECTION_Y + block_z) * region_dim::SECTION_Z + block_x;
						if(pos >= blocks.size())
							block_id = blocks.at(pos - region_dim::CHUNK_Y);
						else {
							block_id = blocks.at(pos);

							// find the first block that is not an air block
							while(!block_id) {
								pos -= region_dim::CHUNK_Y;
								block_id = blocks.at(pos);
							}

							// decrease block height until reaching a non-transparent material
							while(block_color::is_transparent(blocks.at(pos))) {
								pos -= region_dim::CHUNK_Y;
								--block_height;
							}
						}

						// skip unknown block ids
						if(block_id > block_color::MAX_BLOCK) {
							std::cerr << "Warning: Unknown block id (" << block_id << "). Skipping." << std::endl;
							continue;
						}

						// calculate block offsets
						b_x = ch_x + block_x;
						b_z = ch_z + block_z;

						// Use block_id to set image buffer color at a x, z coord
						img.set(b_x, b_z, block_color::COLOR[block_id]);

						// add height to global heightmap (used by ssoa later)
						heightmap[(b_z * img.get_width()) + b_x] = block_height;
					}
			}

	// catch all reader exceptions
	} catch(region_file_exc &exc) {
		std::cerr << exc.to_string() << std::endl;
		return REGION_FILE_DIR;
	}
	return SUCCESS;
}

/*
 * Render screen-space ambient occlusion (SSAO) at a given region
 */
void carto::render_region_occlusion(unsigned int reg_x, unsigned int reg_z) {
	int br_x, br_z;
	float average, value = 0;
	unsigned int off_x = reg_x * BLOCK_WIDTH_PER_REGION, off_z = reg_z * BLOCK_WIDTH_PER_REGION, samples;
	std::cout << "Rendering occlusion: (" << off_x << ", " << off_z << ")..." << std::endl;

	// iterate through each block and calculate occlusion value
	for(int z = 0; z < (int) BLOCK_WIDTH_PER_REGION; ++z)
		for(int x = 0; x < (int) BLOCK_WIDTH_PER_REGION; ++x) {

			// skip all blocks of height zero
			if(!heightmap[((off_z + z) * img.get_width()) + (off_x + x)])
				continue;

			// iterate through the various sampling radii
			for(unsigned int r = 0; r < RADII_COUNT; ++r) {
				average = 0;
				samples = 0;

				// iterate through surrounding block heights
				for(int i = -RADII[r]; i <= RADII[r]; ++i)
					for(int j = -RADII[r]; j <= RADII[r]; ++j) {
						br_x = off_x + x + i;
						br_z = off_z + z + j;
						if((!i && !j)
								|| br_x < 0 || (unsigned int) br_x >= img.get_width()
								|| br_z < 0 || (unsigned int) br_z >= img.get_height()
								|| (heightmap[(br_z * img.get_width()) + br_x] - heightmap[((off_z + z) * img.get_width()) + (off_x + x)] < 0))
							continue;
						average += heightmap[(br_z * img.get_width()) + br_x];
						samples++;
					}

				// average samples
				if(!samples)
					samples = 1;
				average /= samples;
				value = heightmap[((off_z + z) * img.get_width()) + (off_x + x)] - average;
				if(value >= 0)
					continue;

				// apply occlusion value to block
				value = fabs(((region_dim::CHUNK_Y - 1) + value) / (float) (region_dim::CHUNK_Y - 1));
				apply_occlusion(off_x + x, off_z + z, value);
			}
		}
}

int main(int argc, char *argv[]) {
	int flag, res;
	unsigned int height = carto::DEF_HEIGHT;
	std::string reg_dir = carto::DEF_FILE_DIR, out = carto::DEF_OUT_PATH;
	carto::carto map;

	// parse user input
	for(int i = 1; i < argc; ++i) {
		if((flag = carto::is_flag(argv[i])) != carto::NOT_FLAG) {

			// unary flags
			switch(flag) {

				// display usage info
				case carto::DISP_USAGE:
					std::cout << "Usage: " << carto::USE << std::endl;
					return carto::USAGE;
					break;

				// display version info
				case carto::DISP_VERSION:
					std::cout << carto::VER_NUM << " -- " << carto::COPYRIGHT << std::endl << carto::WARRANTY << std::endl;
					return carto::VERSION;
					break;
			}

			// varify flag has associated argument
			if((i + 1) == argc) {
				std::cerr << "Exception: Flag is missing argument: " << argv[i] << std::endl;
				return carto::MALFORMED_FLAG;
			}

			// binary flags
			switch(flag) {

				// collect region file directory
				case carto::REGION_FILE_DIR:
					reg_dir = argv[++i];
					break;

				// collect render height
				case carto::RENDER_HEIGHT:
					height = atoi(argv[++i]);
					if(height < 1
							|| height > carto::DEF_HEIGHT) {
						std::cerr << "Exception: Height must be non-negative and no greater than " << carto::DEF_HEIGHT << std::endl;
						return carto::MALFORMED_FLAG;
					}
					break;

				// collect output path
				case carto::OUTPUT_PATH:
					out = argv[++i];
					break;
				default:
					std::cerr << "Exception: Unsupported flag: " << argv[i] << std::endl;
					return carto::MALFORMED_FLAG;
					break;
			}
		} else {
			std::cerr << "Exception: Unknown flag: " << argv[i] << std::endl;
			return carto::MALFORMED_FLAG;
		}
	}

	// pass in user inputs to render
	if((res = map.render_map(reg_dir, height)))
		return res;

	std::cout << "Writing to file: " << out << "..." << std::endl;

	// write rendered map to file
	map.write(out);

	std::cout << "DONE." << std::endl;

	return carto::SUCCESS;
}

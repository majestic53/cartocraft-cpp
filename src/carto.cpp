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
#include <iostream>
#include <stdexcept>
#include "block_color.hpp"
#include "biome_color.hpp"
#include "carto.hpp"
#include "region_dim.hpp"
#include "region_file.hpp"

/*
 * Cartocraft info
 */
const std::string carto::COPYRIGHT("Copyright (C) 2012 David Jolly");
const std::string carto::USE("carto [-v | -h] [-p REGION_FILE_DIR] [-r RENDER_HEIGHT] [-o OUTPUT_PATH]");
const std::string carto::VER_NUM("Cartocraft 0.2.0");
const std::string carto::WARRANTY("This is free software. There is NO warranty.");

/*
 * Cartocraft render constants
 */
const int carto::SAMPLE_RADII[SAMPLE_RADII_COUNT] = { 1, 1, 1, 1, 1, 2, 4, 8, 16 };

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
	offset_x = 0;
	offset_z = 0;
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
 * Blend a foreground color with a given pixel at x, z coord
 */
bool carto::apply_alpha_blend(unsigned int px_x, unsigned int px_z, unsigned int fore) {
	unsigned int back;
	unsigned char c_back, c_fore, alpha = (unsigned char) fore;
	unsigned n_back[image_buffer::CHANNELS - 1];
	back = terrain.at(px_x, px_z);

	// iterate through all color channels
	for(unsigned int i = 0; i < image_buffer::CHANNELS - 1; ++i) {
		c_back = (unsigned char) (back >> (24 - (i * 8)));
		c_fore = (unsigned char) (fore >> (24 - (i * 8)));
		n_back[i] = c_fore * ((float) alpha / 0xff) + c_back * ((float) (0xff - alpha) / 0xff);
	}

	// apply alpha-blended color
	return terrain.set(px_x, px_z, ((n_back[0] << 24) | (n_back[1] << 16) | (n_back[2] << 8) | 0xff));
}

/*
 * Scale a color at a given pixel at x, z coord
 */
bool carto::apply_scale(unsigned int px_x, unsigned int px_z, float amount) {
	unsigned int col;
	unsigned char n_col[image_buffer::CHANNELS];
	if(!amount)
		return true;
	col = terrain.at(px_x, px_z);

	// iterate through all color channels
	for(unsigned int i = 0; i < image_buffer::CHANNELS; ++i) {
		n_col[i] = (unsigned char) (col >> (24 - (i * 8)));

		// do not apply scaling to alpha channel
		if(i != image_buffer::ALPHA)
			n_col[i] *= amount;
	}

	// apply the new color
	return terrain.set(px_x, px_z, ((n_col[0] << 24) | (n_col[1] << 16) | (n_col[2] << 8) | n_col[3]));
}

/*
 * Returns true if a region in the image buffer is filled
 */
bool carto::is_filled(unsigned int x, unsigned int z) {
	unsigned int pos = z * (terrain.get_width() / BLOCK_WIDTH_PER_REGION) + x;

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
int carto::render_map(const std::string &reg_dir, unsigned int ren_height, bool ren_occlusion) {
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
	terrain = image_buffer(BLOCK_WIDTH_PER_REGION * (abs(x_min) + x_max) + BLOCK_WIDTH_PER_REGION,
				BLOCK_WIDTH_PER_REGION * (abs(z_min) + z_max) + BLOCK_WIDTH_PER_REGION);
	terrain.fill(block_color::FILL);
	std::cout << "Rendering map: size: (" << terrain.get_width() << ", " << terrain.get_height() << ")..." << std::endl;

	// set offset and regions filled
	offset_x = abs(x_min);
	offset_z = abs(z_min);
	region_count = (terrain.get_width() / BLOCK_WIDTH_PER_REGION) * (terrain.get_height() / BLOCK_WIDTH_PER_REGION);
	region_filled = new bool[region_count];
	heightmap = new unsigned int[terrain.get_width() * terrain.get_height()];
	if(!region_filled
			|| !heightmap)
		return ALLOC_FAILED;
	memset(region_filled, false, region_count);
	memset(heightmap, 0, terrain.get_width() * terrain.get_height());

	// render image (by region)
	for(reg_file = reg_files.begin(); reg_file != reg_files.end(); ++reg_file)
		if((res = render_region(*reg_file, ren_height)) != SUCCESS)
			return res;

	// render occlusion & illumination (by region)
	if(ren_occlusion)
		for(int z = 0; z < (abs(z_min) + z_max) + 1; ++z)
			for(int x = 0; x < (abs(x_min) + x_max) + 1; ++x)
				if(is_filled(x, z))
					render_region_occlusion(x * BLOCK_WIDTH_PER_REGION, z * BLOCK_WIDTH_PER_REGION);
	return SUCCESS;
}

/*
 * Render a region file
 */
int carto::render_region(const std::string &reg_file, unsigned int ren_height) {
	bool below_ground;
	region_file_reader reader;
	std::vector<char> biomes;
	std::vector<int> blocks, heights;
	unsigned int block_height, block_id, blend_color, pos;
	int reg_x, reg_z, ch_x, ch_z, b_x, b_z;

	// open region file and collect data
	try {
		reader = region_file_reader(reg_file);
		reader.read();
		std::cout << "Processing region: " << reader.get_region().get_header().to_string() << "..." << std::endl;

		// calculate region offsets
		reg_x = ((abs(reader.get_x_coord() + offset_x)) * BLOCK_WIDTH_PER_REGION);
		reg_z = ((abs(reader.get_z_coord() + offset_z)) * BLOCK_WIDTH_PER_REGION);

		// set filled region to true
		if(!is_filled(reg_x / BLOCK_WIDTH_PER_REGION, reg_z / BLOCK_WIDTH_PER_REGION))
			region_filled[((reg_z / BLOCK_WIDTH_PER_REGION) * (terrain.get_width() / BLOCK_WIDTH_PER_REGION) + (reg_x / BLOCK_WIDTH_PER_REGION))] = true;

		// render region chunk-by-chunk
		for(unsigned int chunk_z = 0; chunk_z < region_dim::CHUNK_WIDTH; ++chunk_z)
			for(unsigned int chunk_x = 0; chunk_x < region_dim::CHUNK_WIDTH; ++chunk_x) {

				// check if chunk exists
				if(!reader.is_filled(chunk_x, chunk_z))
					continue;

				// collect chunk biome, block & heightmap data
				biomes = reader.get_biomes_at(chunk_x, chunk_z);
				blocks = reader.get_blocks_at(chunk_x, chunk_z);
				heights = reader.get_heightmap_at(chunk_x, chunk_z);

				// skip over empty chunks
				if(biomes.empty()
						|| blocks.empty()
						|| heights.empty()) {
					std::cerr << "Warning: Chunk missing data at (" << chunk_x << ", " << chunk_z << "). Skipping." << std::endl;
					continue;
				}

				// calculate chunk offsets
				ch_x = reg_x + (chunk_x * region_dim::BLOCK_WIDTH);
				ch_z = reg_z + (chunk_z * region_dim::BLOCK_WIDTH);

				// process each chunk block-by-block
				for(unsigned int block_z = 0; block_z < region_dim::BLOCK_WIDTH; ++block_z)
					for(unsigned int block_x = 0; block_x < region_dim::BLOCK_WIDTH; ++block_x) {
						block_height = heights.at(block_z * region_dim::BLOCK_WIDTH + block_x);
						if((unsigned int) block_height > ren_height) {
							block_height = ren_height;
							below_ground = true;
						} else
							below_ground = false;
						pos = (block_height * region_dim::BLOCK_WIDTH + block_z) * region_dim::BLOCK_WIDTH + block_x;
						if(pos >= blocks.size()) {
							while(pos >= blocks.size())
								pos -= region_dim::BLOCK_HEIGHT;
							block_id = blocks.at(pos);
						} else {
							block_id = blocks.at(pos);

							// find the first block that is not an air block
							while(!block_id) {
								pos -= region_dim::BLOCK_HEIGHT;
								block_id = blocks.at(pos);
							}

							// decrease block height until reaching a non-transparent material
							while(block_color::is_transparent(blocks.at(pos))) {
								pos -= region_dim::BLOCK_HEIGHT;
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

						// add height to global heightmap (used by SSOA later)
						heightmap[(b_z * terrain.get_width()) + b_x] = block_height;

						// use block_id to set terrain buffer color at an x, z coord
						terrain.set(b_x, b_z, block_color::COLOR[block_id]);

						// scale colors based off biome type
						blend_color = biome_color::BLEND_COLOR[(int) biomes.at((block_z * region_dim::BLOCK_WIDTH) + block_x)];
						if(blend_color
								&& !below_ground)
							apply_alpha_blend(b_x, b_z, blend_color);
					}
			}

	// catch all reader exceptions
	} catch(std::exception &exc) {
		std::cerr << "Exception: " << exc.what() << std::endl;
		return REGION_FILE_DIR;
	}
	return SUCCESS;
}

/*
 * Render screen-space ambient occlusion (SSAO) at a given region
 */
void carto::render_region_occlusion(unsigned int off_x, unsigned int off_z) {
	int br_x, br_z;
	unsigned int samples;
	float average, value = 0;
	std::cout << "Rendering occlusion: (" << off_x << ", " << off_z << ")..." << std::endl;

	// iterate through each block and calculate occlusion value
	for(unsigned int z = 0; z < BLOCK_WIDTH_PER_REGION; ++z)
		for(unsigned int x = 0; x < BLOCK_WIDTH_PER_REGION; ++x) {

			// skip all blocks of height zero
			if(!heightmap[((off_z + z) * terrain.get_width()) + (off_x + x)])
				continue;

			// iterate through the various sampling radii
			for(unsigned int r = 0; r < SAMPLE_RADII_COUNT; ++r) {
				average = 0;
				samples = 0;

				// iterate through surrounding block heights
				for(int i = -SAMPLE_RADII[r]; i <= SAMPLE_RADII[r]; ++i)
					for(int j = -SAMPLE_RADII[r]; j <= SAMPLE_RADII[r]; ++j) {
						br_x = off_x + x + i;
						br_z = off_z + z + j;
						if((!i && !j)
								|| br_x < 0 || (unsigned int) br_x >= terrain.get_width()
								|| br_z < 0 || (unsigned int) br_z >= terrain.get_height()
								|| (heightmap[(br_z * terrain.get_width()) + br_x] - heightmap[((off_z + z) * terrain.get_width()) + (off_x + x)] < 0))
							continue;
						average += heightmap[(br_z * terrain.get_width()) + br_x];
						samples++;
					}

				// average samples
				if(!samples)
					samples = 1;
				average /= samples;
				value = heightmap[((off_z + z) * terrain.get_width()) + (off_x + x)] - average;
				if(value >= 0)
					continue;

				// apply occlusion value to block
				value = fabs(((region_dim::BLOCK_HEIGHT - 1) + 4 * value) / (float) (region_dim::BLOCK_HEIGHT - 1));
				apply_scale(off_x + x, off_z + z, value);
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
	if((res = map.render_map(reg_dir, height, true)))
		return res;

	// write rendered map to file
	std::cout << "Writing to file: " << out << "..." << std::endl;
	map.write(out);

	std::cout << "DONE." << std::endl;

	return carto::SUCCESS;
}

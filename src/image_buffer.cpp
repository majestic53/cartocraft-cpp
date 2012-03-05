/*
 * image_buffer.cpp
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

#include "image_buffer.hpp"
#include "lode/lodepng.hpp"

/*
 * Image buffer assignment
 */
image_buffer &image_buffer::operator=(const image_buffer &other) {

	// check for self
	if(this == &other)
		return *this;

	// set attributes
	count = other.count;
	px = other.px;
	width = other.width;
	height = other.height;
	return *this;
}

/*
 * Image buffer equals
 */
bool image_buffer::operator==(const image_buffer &other) {

	// check for self
	if(this == &other)
		return true;

	// check attributes
	if(count != other.count
			|| width != other.width
			|| height != other.height)
		return false;
	for(unsigned int i = 0; i < count; ++i)
		if(px.at(i) != other.px.at(i))
			return false;
	return true;
}

/*
 * Retrieve a pixel at a given coord x, z
 */
unsigned int image_buffer::at(unsigned int x, unsigned int z) {
	unsigned int pos = (z * width + x) * CHANNELS;

	// check if valid x, z coord
	if(pos >= count)
		return 0;
	return (px.at(pos) << 24) | (px.at(pos + 1) << 16) | (px.at(pos + 2) << 8) | px.at(pos + 3);
}

/*
 * Retrieve a pixel channel at a given coord x, z
 */
unsigned char image_buffer::at(unsigned int x, unsigned int z, unsigned int chan) {
	unsigned int pos = (z * width + x) * CHANNELS;

	// check if valid x, z coord
	if(pos >= count)
		return 0;
	switch(chan) {
		case RED: return px.at(pos);
			break;
		case GREEN: return px.at(pos + 1);
			break;
		case BLUE: return px.at(pos + 2);
			break;
		case ALPHA: return px.at(pos + 3);
			break;
	}
	return 0;
}

/*
 * Clears an image buffers pixel at a given coord x, z
 */
bool image_buffer::clear(unsigned int x, unsigned int z) {
	unsigned int pos = (z * width + x) * CHANNELS;

	// check if valid x, z coord
	if(pos >= count)
		return false;
	px.at(pos) = 0;
	px.at(pos + 1) = 0;
	px.at(pos + 2) = 0;
	px.at(pos + 3) = 0;
	return true;
}

/*
 * Clears all pixels in an image buffer
 */
void image_buffer::clear_all(void) {

	// clear all pixels
	for(unsigned int i = 0; i < count; ++i)
		px.at(i) = 0;
}

/*
 * Fill an image buffer with a solid color
 */
void image_buffer::fill(unsigned int col) {

	// fill all pixels
	for(unsigned int i = 0; i < count; i += 4) {
		px.at(i) = (unsigned char) (col >> 24);
		px.at(i + 1) = (unsigned char) (col >> 16);
		px.at(i + 2) = (unsigned char) (col >> 8);
		px.at(i + 3) = (unsigned char) col;
	}
}

/*
 * Set a pixel at a given coord x, z
 */
bool image_buffer::set(unsigned int x, unsigned int z, unsigned int col) {
	unsigned int pos = (z * width + x) * CHANNELS;

	// check if valid x, z coord
	if(pos >= count)
		return false;
	px.at(pos) = (unsigned char) (col >> 24);
	px.at(pos + 1) = (unsigned char) (col >> 16);
	px.at(pos + 2) = (unsigned char) (col >> 8);
	px.at(pos + 3) = (unsigned char) col;
	return true;
}

/*
 * Write image buffer to file as a png
 */
void image_buffer::write(const std::string &path) {

	// instantiate encoder
	LodePNG::Encoder encoder;
	std::vector<unsigned char> output;
	encoder.infoRaw.color.colorType = LCT_RGBA;

	// write byte vector to file
	encoder.encode(output, &px[0], width, height);
	LodePNG::saveFile(output, path.c_str());
}

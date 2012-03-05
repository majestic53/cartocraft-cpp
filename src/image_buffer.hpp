/*
 * image_buffer.hpp
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

#ifndef IMAGE_BUFFER_HPP_
#define IMAGE_BUFFER_HPP_

#include <string>
#include <vector>

class image_buffer {
private:

	/*
	 * Image buffer pixel count
	 */
	size_t count;

	/*
	 * Image buffer data
	 */
	std::vector<unsigned char> px;

	/*
	 * Image buffer width, height
	 */
	unsigned int width, height;

public:

	/*
	 * Color channels
	 */
	enum CHANNEL { RED, GREEN, BLUE, ALPHA };
	static const unsigned int CHANNELS = 4;

	/*
	 * Image buffer constructor
	 */
	image_buffer(void) : count(0), width(0), height(0) { return; };

	/*
	 * Image buffer constructor
	 */
	image_buffer(const image_buffer &other) : count(other.count), px(other.px), width(other.width), height(other.height) { return; };

	/*
	 * Image buffer constructor
	 */
	image_buffer(unsigned int width, unsigned int height) : count(width * height * CHANNELS), width(width), height(height) { px.resize(width * height * CHANNELS); };

	/*
	 * Image buffer destructor
	 */
	virtual ~image_buffer(void) { return; }

	/*
	 * Image buffer assignment
	 */
	image_buffer &operator=(const image_buffer &other);

	/*
	 * Image buffer equals
	 */
	bool operator==(const image_buffer &other);

	/*
	 * Image buffer equals
	 */
	bool operator!=(const image_buffer &other) { return !(*this == other); }

	/*
	 * Retrieve a pixel at a given coord x, z
	 */
	unsigned int at(unsigned int x, unsigned int z);

	/*
	 * Retrieve a pixel channel at a given coord x, z
	 */
	unsigned char at(unsigned int x, unsigned int z, unsigned int chan);

	/*
	 * Clears an image buffers pixel at a given coord x, z
	 */
	bool clear(unsigned int x, unsigned int z);

	/*
	 * Clears all pixels in an image buffer
	 */
	void clear_all(void);

	/*
	 * Fill an image buffer with a solid color
	 */
	void fill(unsigned int col);

	/*
	 * Returns an image buffers pixel count
	 */
	size_t get_count(void) { return count; }

	/*
	 * Returns an image buffers height
	 */
	unsigned int get_height(void) { return height; }

	/*
	 * Returns an image buffers raw data
	 */
	std::vector<unsigned char> &get_raw(void) { return px; }

	/*
	 * Returns an image buffers width
	 */
	unsigned int get_width(void) { return width; }

	/*
	 * Set a pixel at a given coord x, z
	 */
	bool set(unsigned int x, unsigned int z, unsigned int col);

	/*
	 * Write image buffer to file as a png
	 */
	void write(const std::string &path);
};

#endif

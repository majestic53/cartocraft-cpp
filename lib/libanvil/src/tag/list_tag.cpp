/*
 * list_tag.cpp
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

#include <sstream>
#include "../byte_stream.hpp"
#include "list_tag.hpp"

/*
 * List tag assignment operator
 */
list_tag &list_tag::operator=(const list_tag &other) {

	// check for self
	if(this == &other)
		return *this;

	// assign attributes
	name = other.name;
	type = other.type;
	value = other.value;
	ele_type = other.ele_type;
	return *this;
}

/*
 * List tag equals operator
 */
bool list_tag::operator==(const generic_tag &other) {

	// check for self
	if(this == &other)
		return true;

	// convert into same type
	const list_tag *other_tag = dynamic_cast<const list_tag *>(&other);
	if(!other_tag)
		return false;

	// check attributes
	if(name != other.name
			|| type != other.type
			|| value.size() != other_tag->value.size())
		return false;
	for(unsigned int i = 0; i < value.size(); ++i)
		if(value.at(i) != other_tag->value.at(i))
			return false;
	return true;
}

/*
 * Return a list tag's data
 */
std::vector<char> list_tag::get_data(bool list_ele)  {
	byte_stream stream(byte_stream::SWAP_ENDIAN);

	// form data representation
	if(!list_ele) {
		stream << (char) type;
		stream << (short) name.size();
		stream << name;
	}
	stream << (char) ele_type;
	stream << (int) value.size();
	for(unsigned int i = 0; i < value.size(); ++i)
		stream << value.at(i)->get_data(true);
	return stream.vbuf();
}

/*
 * Insert a tag into a list tag at a given index
 */
bool list_tag::insert(generic_tag *value, unsigned int index) {

	// check for valid type
	if(value->get_type() != ele_type)
		return false;
	this->value.insert(this->value.begin() + index, value);
	return true;
}

/*
 * Insert a tag onto the tail of a list tag
 */
bool list_tag::push_back(generic_tag *value) {

	// check for valid type
	if(value->get_type() != ele_type)
		return false;
	this->value.push_back(value);
	return true;
}

/*
 * Return a string representation of a list tag
 */
std::string list_tag::to_string(unsigned int tab) {
	std::stringstream ss;

	// form string representation
	ss << generic_tag::to_string(tab) << " (" << value.size() << ") {";
	if(!value.empty()) {
		ss << std::endl;
		for(unsigned int i = 0; i < value.size(); ++i)
			ss << value.at(i)->to_string(tab + 1) << std::endl;
		generic_tag::append_tabs(tab, ss);
	}
	ss << "}";
	return ss.str();
}

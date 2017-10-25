/* Copyright (C) 2017 Marco Scarpetta
 *
 * This file is part of Liber.
 *
 * Liber is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Liber is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Liber. If not, see <http://www.gnu.org/licenses/>.
 */

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <float.h>
#include "csscolorparser.h"

#ifndef BASECLASSES_H
#define BASECLASSES_H

const double mm = 360.0/127.0;

struct Font {
    Font() : name("Arial"), size(12.0), bold(false), italic(false) {}
    std::string name;
    double size;
    bool bold;
    bool italic;
};

struct Rectangle {
    Rectangle() : left(0), top(0), width(0), height(0) {}
    double left;
    double top;
    double width;
    double height;
};

struct PageInfo {
    int number;
    double left;
    double top;
    double width;
    double height;
    double current_top;
    std::vector<Rectangle> reserved;
};

double parse_length(const std::string &str);

void clean_string(std::string &str);

std::string find_replaceable(std::string &str);

void replace(std::string &str, const std::string &key, const std::string &value);

std::string get_numbering_number(int number, int start);
std::string get_numbering_letter(int number, int start, bool uppercase);
std::string get_numbering_roman(int number, int start);

#endif

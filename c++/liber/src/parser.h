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

#include <fstream>
#include <sstream>
#include <vector>
#include <math.h>
#include "figure.h"
#include "flowable.h"
#include "blocks.h"
#include "allocator.h"

#ifndef PERSER_H
#define PERSER_H

const int max_iterations = 100;

const char allowed_attr_name_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-";
const char allowed_macro_name_start_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
const char allowed_macro_name_chars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";

std::string &read_file(std::string name);

void resolve_escapes(std::string &str);

void resolve_includes(std::string *str);

int replace_macro(std::string &text, std::string &macro_name, std::string &macro_value);

void replace_macros(std::string *str);

void add_toc_blocks(Allocator *allocator, Numbering *numbering,
                    std::vector<Flowable *> *prototype,
                    int i,
                    std::vector <Group *> *groups);

Block *get_next_block(std::string *str,
                          Allocator *allocator,
                          size_t &block_start,
                          size_t from=0);

void parse(std::string *str,
      Allocator *allocator,
      std::vector<Group *> *groups);

#endif // PERSER_H

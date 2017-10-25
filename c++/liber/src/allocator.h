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

#include "blocks.h"
#include "page.h"

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

class Flowable;

struct HeaderFooter {
    HeaderFooter() : header_left(NULL), header_center(NULL), header_right(NULL),
        footer_left(NULL), footer_center(NULL), footer_right(NULL) {}
    Flowable *header_left;
    Flowable *header_center;
    Flowable *header_right;
    Flowable *footer_left;
    Flowable *footer_center;
    Flowable *footer_right;
};

bool must_add_new_page(PageBlock &old_block, PageBlock &new_block);
std::string page_number_string(PageBlock &block, int number);

class Allocator
{
public:
    Allocator(Writer *writer);

    ~Allocator();

    Writer *get_writer();

    Page *get_current_page();

    Page *add_new_page();

    int get_current_page_number();

    std::string get_page_number_string(int page_number);

    void set_page_block(PageBlock *block);

    void set_header_footer(Flowable *flowable);

    void reset();

    void write();

private:
    Writer *writer;
    std::vector<Page *> pages;
    std::vector<std::string *> page_numbers;

    Page *current_page;

    std::map<int, PageBlock *> page_blocks;
    PageBlock current_page_block;
    int last_added_block_page_number;

    HeaderFooter hf;
    bool page_size_changed;
};

#endif // ALLOCATOR_H

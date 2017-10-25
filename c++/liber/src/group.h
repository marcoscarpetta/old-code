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

#include "numbering.h"
#include "blocks.h"
#include "allocator.h"

#ifndef GROUP_H
#define GROUP_H

class Group
{
public:
    Group(Allocator *allocator, Numbering *root_numbering);

    virtual bool add_block(Block *block);

    virtual bool allocate_area(Page *forced_page=NULL);

    virtual double get_width();

    virtual double get_height();

protected:
    Allocator *allocator;
    Numbering *root_numbering;

    std::vector<Block *> children;

    double start_position;
    int start_page;
    double end_position;
    int end_page;
    double width;
    double height;
};


#endif // GROUP_H

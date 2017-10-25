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

#include "vector"
#include "flowable.h"

#ifndef FIGURE_H
#define FIGURE_H

struct SubFigure {
    SubFigure() : image(NULL), caption(NULL), page(0), top(0), left(0) {}
    Image *image;
    Flowable *caption;
    int page;
    double top;
    double left;
};

class Figure : public Group
{
public:
    Figure(Allocator *allocator, Numbering *root_numbering);

    bool add_block(Block *block);

    bool allocate_area(Page *forced_page=NULL);

private:
    std::vector <SubFigure *> subfigures;
    Flowable *total_caption;
};

#endif // FIGURE_H

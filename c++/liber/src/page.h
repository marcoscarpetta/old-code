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

#include "operation.h"

#ifndef PAGE_H
#define PAGE_H

class Page: public Operation
{
public:
    Page(Writer *writer, double width, double height, Operation *parent=NULL);

    ~Page();

    double get_current_top();

    double get_width();

    double get_height();

    void set_font(const Font &font);

    void set_fill_color(const CSSColorParser::Color &color);

    void draw_string(const std::string& str, double left, double top, double word_spacing);

    void draw_image(const std::string& file_name,
                    double left,
                    double top,
                    double scale_x,
                    double scale_y);

    void draw_page(Page *page, double left, double top);

    void add_operation(Operation *operation);

    void reserve_space(double height);

    void run();

private:
    double current_top;

    double width;
    double height;

    std::vector<Operation *> operations;
};

#endif // PAGE_H

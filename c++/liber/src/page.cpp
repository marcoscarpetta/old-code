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

#include "page.h"

Page::Page(Writer *writer, double width, double height, Operation *parent) :
    Operation(writer, parent),
    current_top(0),
    width(width), height(height)
{

}

Page::~Page()
{
    for (Operation *operation : this->operations)
        delete operation;
}

double Page::get_current_top()
{
    return this->current_top;
}

double Page::get_width()
{
    return this->width;
}

double Page::get_height()
{
    return this->height;
}

void Page::set_font(const Font &font)
{
    this->operations.push_back(new SetFont(this->writer, font, this));
}

void Page::set_fill_color(const CSSColorParser::Color& color)
{
    this->operations.push_back(new SetColor(this->writer, color, this));
}

void Page::draw_string(const std::string& str, double left, double top, double word_spacing)
{
    Operation *operation = new DrawString(this->writer, str, this);
    operation->set_left(left);
    operation->set_top(top);
    this->operations.push_back(operation);
}

void Page::draw_image(const std::string& file_name,
                double left,
                double top,
                double scale_x,
                double scale_y)
{
    Operation *operation = new DrawImage(this->writer, file_name, scale_x, scale_y, this);
    operation->set_left(left);
    operation->set_top(top);
    this->operations.push_back(operation);
}

void Page::draw_page(Page *page, double left, double top)
{
    page->set_parent(this);
    page->set_left(left);
    page->set_top(top);
    this->operations.push_back(page);

    this->current_top += page->get_current_top();
}

void Page::add_operation(Operation *operation)
{
    operation->set_parent(this);

    this->operations.push_back(operation);
}

void Page::reserve_space(double height)
{
    this->current_top += height;
}

void Page::run()
{
    for (Operation *operation : this->operations)
        operation->run();
}

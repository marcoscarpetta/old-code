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

Operation::Operation(Writer *writer, Operation *parent) :
    writer(writer), parent(parent),
    left(0), top(0)
{

}

double Operation::get_left()
{
    if (this->parent != NULL)
        return this->left + this->parent->get_left();
    else
        return this->left;
}

double Operation::get_top()
{
    if (this->parent != NULL)
        return this->top + this->parent->get_top();
    else
        return this->top;
}

void Operation::set_left(double left)
{
    this->left = left;
}

void Operation::set_top(double top)
{
    this->top = top;
}

void Operation::set_parent(Operation *parent)
{
    this->parent = parent;
}

// SetFont operation
SetFont::SetFont(Writer *writer, const Font &font, Operation *parent) :
    Operation(writer, parent),
    font(font)
{

}

void SetFont::run()
{
    writer->set_font(this->font);
}

// SetColor operation
SetColor::SetColor(Writer *writer, const CSSColorParser::Color &color, Operation *parent) :
    Operation(writer, parent),
    color(color)
{

}

void SetColor::run()
{
    writer->set_fill_color(this->color);
}

// DrawString operation
DrawString::DrawString(Writer *writer, const std::string &str, Operation *parent) :
    Operation(writer, parent),
    str(str)
{

}

void DrawString::run()
{
    this->writer->draw_string(this->str, this->get_left(), this->get_top());
}

// DrawImage operation
DrawImage::DrawImage(Writer *writer, const std::string &file_name, double scale_x, double scale_y, Operation *parent) :
    Operation(writer, parent),
    file_name(file_name),
    scale_x(scale_x), scale_y(scale_y)
{

}

void DrawImage::run()
{
    this->writer->draw_image(this->file_name, this->get_left(), this->get_top(), this->scale_x, this->scale_y);
}

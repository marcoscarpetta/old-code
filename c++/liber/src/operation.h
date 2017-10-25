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

#include "writer.h"

#ifndef OPERATION_H
#define OPERATION_H

class Operation
{
public:
    Operation(Writer *writer, Operation *parent=NULL);

    virtual void run() = 0;

    double get_left();

    double get_top();

    void set_left(double left);

    void set_top(double top);

    void set_parent(Operation *parent);

protected:
    Operation *parent;
    Writer *writer;

    double left;
    double top;
};

// SetFont operation
class SetFont : public Operation
{
public:
    SetFont(Writer *writer, const Font &font, Operation *parent=NULL);

    void run();

private:
    Font font;
};

// SetColor operation
class SetColor : public Operation
{
public:
    SetColor(Writer *writer, const CSSColorParser::Color &color, Operation *parent=NULL);

    void run();

private:
    CSSColorParser::Color color;
};

// DrawString operation
class DrawString : public Operation
{
public:
    DrawString(Writer *writer, const std::string &str, Operation *parent=NULL);

    void run();

private:
    std::string str;
};

// DrawImage operation
class DrawImage : public Operation
{
public:
    DrawImage(Writer *writer, const std::string &file_name, double scale_x, double scale_y, Operation *parent=NULL);

    void run();

private:
    std::string file_name;
    double scale_x;
    double scale_y;
};

#endif // OPERATION_H

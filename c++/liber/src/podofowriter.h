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

#include "podofo/podofo.h"
#include "writer.h"

#ifndef PODOFOWRITER_H
#define PODOFOWRITER_H

class PoDoFoWriter : public Writer
{
public:
    PoDoFoWriter(const std::string &file_name);

    ~PoDoFoWriter();

    double get_string_width(const std::string& str, const Font &font);

    Rectangle get_image_size(const std::string &file_name);

    void set_font(const Font& font);

    void set_fill_color(const CSSColorParser::Color& color);

    void draw_string(const std::string &str, double left, double top);

    void draw_image(const std::string &file_name,
                    double left,
                    double top,
                    double scale_x,
                    double scale_y);

    void create_new_page(double width, double height);

    void close();

private:
    PoDoFo::PdfStreamedDocument* pdf_doc;
    PoDoFo::PdfPage* page;
    PoDoFo::PdfPainter painter;
};

#endif // PODOFOWRITER_H

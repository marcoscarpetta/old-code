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

#include "podofowriter.h"

PoDoFoWriter::PoDoFoWriter(const std::string &file_name) :
    Writer(file_name),
    page(NULL)
{
    this->pdf_doc = new PoDoFo::PdfStreamedDocument(file_name.c_str());
}

PoDoFoWriter::~PoDoFoWriter()
{
    delete this->pdf_doc;
}

double PoDoFoWriter::get_string_width(const std::string &str, const Font &font)
{
    PoDoFo::PdfFont* pdf_font = this->pdf_doc->CreateFont(
                font.name.c_str(),
                font.bold,
                font.italic,
                false,
                PoDoFo::PdfEncodingFactory::GlobalWin1250EncodingInstance()
                );

    pdf_font->SetFontSize(font.size);

    PoDoFo::PdfString s( reinterpret_cast<const PoDoFo::pdf_utf8*> (str.c_str()) );

    const PoDoFo::PdfFontMetrics* fm = pdf_font->GetFontMetrics();
    return fm->StringWidth(s);
}

Rectangle PoDoFoWriter::get_image_size(const std::string &file_name)
{
    PoDoFo::PdfImage *image = new PoDoFo::PdfImage(this->pdf_doc);
    image->LoadFromFile(file_name.c_str());

    Rectangle size;

    size.width = image->GetWidth();
    size.height = image->GetHeight();

    return size;
}

void PoDoFoWriter::set_font(const Font &font)
{
    if (this->page == NULL)
        return; //Error!

    PoDoFo::PdfFont* pdf_font = this->pdf_doc->CreateFont(
                font.name.c_str(),
                font.bold,
                font.italic,
                false,
                PoDoFo::PdfEncodingFactory::GlobalWin1250EncodingInstance()
                );

    pdf_font->SetFontSize(font.size);

    this->painter.SetFont(pdf_font);
}

void PoDoFoWriter::set_fill_color(const CSSColorParser::Color &color)
{
    if (this->page == NULL)
        return; //Error!

    this->painter.SetColor(1.0*color.r/255, 1.0*color.g/255, 1.0*color.b/255);
}

void PoDoFoWriter::draw_string(const std::string &str, double left, double top)
{
    if (this->page == NULL)
        return; //Error!

    PoDoFo::PdfString podofo_string( reinterpret_cast<const PoDoFo::pdf_utf8*> (str.c_str()) );

    painter.DrawText(
                left,
                this->page->GetPageSize().GetHeight() - top - painter.GetFont()->GetFontSize(),
                podofo_string
                );
}

void PoDoFoWriter::draw_image(const std::string &file_name,
                              double left,
                              double top,
                              double scale_x,
                              double scale_y)
{
    if (this->page == NULL)
        return; //Error!

    PoDoFo::PdfImage *image = new PoDoFo::PdfImage(this->pdf_doc);
    image->LoadFromFile(file_name.c_str());

    this->painter.DrawImage(
                left,
                this->page->GetPageSize().GetHeight() - top - image->GetHeight()*scale_y,
                image,
                scale_x,
                scale_y
                );
}

void PoDoFoWriter::create_new_page(double width, double height)
{
    //finish previous page, if any
    if (this->page != NULL)
        this->painter.FinishPage();

    this->page = this->pdf_doc->CreatePage(PoDoFo::PdfRect(0, 0, width, height));

    this->painter.SetPage(this->page);
}

void PoDoFoWriter::close()
{
    if (this->page != NULL)
        this->painter.FinishPage();

    try
    {
        this->pdf_doc->Close();
    }
    catch(PoDoFo::PdfError err)
    {
        err.PrintErrorMsg();
    }
}

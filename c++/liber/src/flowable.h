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
#include "blocks.h"
#include "numbering.h"
#include "group.h"

#ifndef FLOWABLE_H
#define FLOWABLE_H

class Text : public TextBlock
{
public:
    Text(TextBlock &block);

    void set_content(const std::string &value);

    std::vector<std::string> *get_words();

private:
    void next_word(std::string& str, int from, bool& found, int& start, int& length);

    std::vector<std::string> words;
};

class Image : public ImageBlock
{
public:
    Image(Allocator *allocator, ImageBlock &block);

    double get_width();
    double get_height();

    double get_scale_x();
    double get_scale_y();

    void write_at(double left, double top);

private:
    Allocator *allocator;
    std::string image_file;
    double image_width;
    double image_height;
    double scale_x;
    double scale_y;
    double occupied_width;
    double occupied_height;
};


struct Word
{
    Word() : left(0), top(0), width(0), height(0) {}
    Operation *operation;
    double left;
    double top;
    double width;
    double height;
    Keyword vertical_align;
};

class Flowable : public Group
{
public:
    Flowable(Allocator *allocator, Numbering *root_numbering);

    Flowable(const Flowable &base);

    bool add_block(Block *block);

    TextBlock *get_root_block();

    bool allocate_area(Page *forced_page=NULL);

private:
    std::vector <Block *> build_block(Text *prototype,
                                      Numbering *target_numbering,
                                      bool number_only=false);

    void build();

    void add_word(Word *word, double spacing, bool new_page);

    void write_line(bool new_page, bool last_line=false);

    std::vector<Block *> prototype;
    std::vector<Block *> blocks;
    std::vector<Block *> built_blocks;

    Text *root_block;

    Page *current_page;
    std::vector<Word *> line;
    double line_height, line_width, clean_line_width;
    int words_count, lines_on_page;

    Font last_font;
    Font current_font;

    CSSColorParser::Color last_color;
    CSSColorParser::Color current_color;

    bool write_header_footer;
};

#endif // FLOWABLE_H

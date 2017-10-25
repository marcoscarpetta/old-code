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

#ifndef NUMBERING_H
#define NUMBERING_H

class Numbering
{
public:
    Numbering(TextBlock *block);

    void add_child(Numbering *child);

    std::vector <Numbering *> *get_children();

    void calculate_numbering_number();

    Numbering *get_numbering(std::string id);

    TextBlock *get_text_block();

    std::string get_own_number();

    std::string get_total_number();

    void set_content(std::vector<Block *> content);

    std::vector<Block *> *get_content();

    void set_page_number(int page_number);

    int get_page_number();

    void set_top(double top);

    double get_top();

private:
    TextBlock *block;
    int number;
    int page_number;
    double top;
    Numbering *parent;
    std::vector<Numbering *> children;
    std::vector<Block *> content;
};

#endif // NUMBERING_H

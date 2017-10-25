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

Numbering::Numbering(TextBlock *block) :
    block(block),
    number(-1),
    page_number(0),
    top(0),
    parent(NULL)
{

}

void Numbering::add_child(Numbering *child)
{
    this->children.push_back(child);
    child->parent = this;
}

void Numbering::calculate_numbering_number()
{
    std::map<std::string, int> current_numbers;

    for (Numbering *child : this->children)
    {
        std::string name = child->block->numbering_name();
        if (name.size() > 0)
        {
            if (current_numbers.count(name) > 0)
                child->number = ++(current_numbers[name]);
            else
            {
                child->number = 0;
                current_numbers[name] = 0;
            }
        }

        child->calculate_numbering_number();
    }
}

std::vector <Numbering *> *Numbering::get_children()
{
    return &(this->children);
}

Numbering *Numbering::get_numbering(std::string id)
{
    Numbering *numbering = NULL;

    for (Numbering *child : this->children)
    {
        if (child->block->id() == id)
        {
            numbering = child;
            break;
        }
        else
        {
            numbering = child->get_numbering(id);
            if (numbering != NULL)
                break;
        }
    }

    return numbering;
}

TextBlock *Numbering::get_text_block()
{
    return this->block;
}

std::string Numbering::get_own_number()
{
    switch (this->block->numbering_type())
    {
    case Keyword::capital_letter:
        return get_numbering_letter(this->number, this->block->numbering_start(), true);
        break;
    case Keyword::letter:
        return get_numbering_letter(this->number, this->block->numbering_start(), false);
        break;
    case Keyword::roman:
        return get_numbering_roman(this->number, this->block->numbering_start());
        break;
    default:
        return get_numbering_number(this->number, this->block->numbering_start());
        break;
    }
}

std::string Numbering::get_total_number()
{
    std::string total_number = this->block->numbering_number();
    std::string name = find_replaceable(total_number);

    while (name.size() > 0)
    {
        Numbering *parent = this;
        while (parent != NULL)
        {
            if (parent->block->numbering_name() == name)
            {
                replace(total_number, name, parent->get_own_number());
                break;
            }
            parent = parent->parent;
        }
        name = find_replaceable(total_number);
    }

    return total_number;
}

void Numbering::set_content(std::vector<Block *> content)
{
    this->content = content;
}

std::vector<Block *> *Numbering::get_content()
{
    return &(this->content);
}

void Numbering::set_page_number(int page_number)
{
    this->page_number = page_number;
}

int Numbering::get_page_number()
{
    return this->page_number;
}

void Numbering::set_top(double top)
{
    this->top = top;
}

double Numbering::get_top()
{
    return this->top;
}

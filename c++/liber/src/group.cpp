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

#include "group.h"

Group::Group(Allocator *allocator, Numbering *root_numbering) :
    allocator(allocator),
    root_numbering(root_numbering),
    start_position(0),
    start_page(0),
    end_position(0),
    end_page(0),
    width(0),
    height(0)
{

}

bool Group::add_block(Block *block)
{
    if (block->block_type() == BlockType::page || block->block_type() == BlockType::space)
    {
        this->children.push_back(block);
        return true;
    }

    return false;
}

bool Group::allocate_area(Page *forced_page)
{
    for (Block *block : this->children)
    {
        if (block->block_type() == BlockType::page)
            this->allocator->set_page_block(static_cast<PageBlock *>(block));

        else if (block->block_type() == BlockType::space)
        {
            SpaceBlock *space = static_cast<SpaceBlock *>(block);
            if (space->type() == Keyword::new_page)
                this->allocator->add_new_page();
            else if (space->type() == Keyword::vertical)
                this->allocator->get_current_page()->reserve_space(space->height());
        }
    }

    return false;
}

double Group::get_width()
{
    return this->width;
}

double Group::get_height()
{
    return this->height;
}

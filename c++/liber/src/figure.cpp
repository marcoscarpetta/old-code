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

#include "figure.h"

Figure::Figure(Allocator *allocator, Numbering *root_numbering) :
    Group(allocator, root_numbering),
    total_caption(NULL)
{

}

bool Figure::add_block(Block *block)
{
    if (block->block_type() == BlockType::image)
    {
        SubFigure *subfigure = new SubFigure;
        subfigure->image = new Image(this->allocator, *(static_cast<ImageBlock *>(block)));
        subfigure->caption = NULL;

        //add subfigure to subfigures list
        this->subfigures.push_back(subfigure);
        return true;
    }
    else if (block->block_type() == BlockType::text)
    {
        TextBlock *text_block = static_cast<TextBlock *>(block);

        if (this->subfigures.size() > 0)
        {
            // If block is an inline, try adding it to the last subfigure caption
            // or to the total caption
            if (text_block->is_inline())
            {
                if (this->total_caption != NULL)
                    return this->total_caption->add_block(block);
                if (this->subfigures.back()->caption != NULL)
                    return this->subfigures.back()->caption->add_block(block);
            }
            else if (text_block->caption())
            {
                if (this->subfigures.back()->caption == NULL)
                {
                    Flowable *flowable = new Flowable(this->allocator, this->root_numbering);
                    this->subfigures.back()->caption = flowable;
                    flowable->add_block(block);
                    return true;
                }
                else if (total_caption == NULL)
                {
                    this->total_caption = new Flowable(this->allocator, this->root_numbering);
                    this->total_caption->add_block(block);
                    return true;
                }
            }
        }
    }

    return false;
}

bool Figure::allocate_area(Page *forced_page)
{
    Page *page = this->allocator->get_current_page();

    if (forced_page != NULL)
    {
        page = forced_page;
    }

    bool changed = false;

    if (this->start_position != page->get_current_top() ||
            this->start_page != this->allocator->get_current_page_number())
        changed = true;

    this->start_position = page->get_current_top();
    this->start_page = this->allocator->get_current_page_number();

    double reserve_height = 0;

    for (SubFigure *subfigure : this->subfigures)
    {
        double subfigure_height = subfigure->image->get_height();
        double subfigure_width = subfigure->image->get_width();

        reserve_height = subfigure_height;

        Page *caption_page = new Page(this->allocator->get_writer(), subfigure_width, 500000);
        if (subfigure->caption != NULL)
        {
            subfigure->caption->allocate_area(caption_page);
            reserve_height += subfigure->caption->get_height();
        }

        //if the figure doesn't fit the page, go to next page
        if (reserve_height > page->get_height() - page->get_current_top() &&
                forced_page == NULL)
            page = this->allocator->add_new_page();

        //allocate image and caption
        subfigure->image->write_at(0, page->get_current_top());
        if (subfigure->caption != NULL)
        {
            caption_page->set_parent(page);
            page->draw_page(caption_page, 0, page->get_current_top());
        }
    }

    if (page->get_current_top() != this->end_position ||
            this->allocator->get_current_page_number() != this->end_page)
        changed = true;

    this->end_position = page->get_current_top();
    this->end_page = this->allocator->get_current_page_number();

    return changed;
}

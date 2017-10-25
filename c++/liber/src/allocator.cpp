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

#include "allocator.h"
#include "flowable.h"

bool must_add_new_page(PageBlock &old_block, PageBlock &new_block)
{
    if (
            old_block.width() != new_block.width() ||
            old_block.height() != new_block.height() ||

            old_block.margin_top() != new_block.margin_top() ||
            old_block.margin_bottom() != new_block.margin_bottom() ||
            old_block.margin_left() != new_block.margin_left() ||
            old_block.margin_right() != new_block.margin_right() ||

            old_block.padding_top() != new_block.padding_top() ||
            old_block.padding_bottom() != new_block.padding_bottom() ||
            old_block.padding_left() != new_block.padding_left() ||
            old_block.padding_right() != new_block.padding_right() ||

            old_block.numbering_type() != new_block.numbering_type() ||
            old_block.numbering_start() != new_block.numbering_start() ||

            old_block.column_fill() != new_block.column_fill() ||
            old_block.column_gap() != new_block.column_gap() ||

            !new_block.numbering_start_is_default() ||
            !new_block.numbering_type_is_default()
            )
        return true;

    return false;
}

std::string page_number_string(PageBlock &block, int number)
{
    switch (block.numbering_type())
    {
    case Keyword::capital_letter:
        return get_numbering_letter(number, block.numbering_start(), true);
        break;
    case Keyword::letter:
        return get_numbering_letter(number, block.numbering_start(), false);
        break;
    case Keyword::roman:
        return get_numbering_roman(number, block.numbering_start());
        break;
    default:
        return get_numbering_number(number, block.numbering_start());
        break;
    }
}

Allocator::Allocator(Writer *writer) :
    writer(writer),
    page_size_changed(true),
    current_page(NULL),
    last_added_block_page_number(-1)
{

}

Allocator::~Allocator()
{
    for (Page *page : this->pages)
        delete page;

    for (std::pair<int, PageBlock *> pair: this->page_blocks)
        delete pair.second;

    for (std::string *page_number : this->page_numbers)
        delete page_number;
}

Writer *Allocator::get_writer()
{
    return this->writer;
}

Page *Allocator::get_current_page()
{
    if (this->page_size_changed)
        return this->add_new_page();

    return this->current_page;
}

Page *Allocator::add_new_page()
{
    this->page_size_changed = false;

    Page *page = new Page(this->writer,
                          this->current_page_block.width(),
                          this->current_page_block.height());

    this->pages.push_back(page);

    double page_width = this->current_page_block.width() -
            this->current_page_block.margin_left() - this->current_page_block.padding_left() -
            this->current_page_block.padding_right() - this->current_page_block.margin_right();
    double page_height = this->current_page_block.height() -
            this->current_page_block.margin_top() - this->current_page_block.padding_top() -
            this->current_page_block.padding_bottom() - this->current_page_block.margin_bottom();

    double left = this->current_page_block.margin_left() + this->current_page_block.padding_left();
    double top = this->current_page_block.margin_top() + this->current_page_block.padding_top();
    double bottom = this->current_page_block.height() -
            this->current_page_block.padding_bottom() - this->current_page_block.margin_bottom();

    double header_height = 0, footer_height = 0;

    if (this->hf.header_left != NULL)
    {
        Page *hl_page;
        if (this->hf.header_center != NULL)
            hl_page = new Page(this->writer, page_width/3, page_height, page);
        else if (this->hf.header_right != NULL)
            hl_page = new Page(this->writer, page_width/2, page_height, page);
        else
            hl_page = new Page(this->writer, page_width, page_height, page);

        page->draw_page(hl_page, left, top);

        this->hf.header_left->allocate_area(hl_page);

        header_height = std::max(this->hf.header_left->get_height(), header_height);
    }

    if (this->hf.header_center != NULL)
    {
        Page *hc_page;
        if (this->hf.header_left != NULL || this->hf.header_right != NULL)
        {
            hc_page = new Page(this->writer, page_width/3, page_height, page);
            page->draw_page(hc_page, left+page_width/3, top);
        }
        else
        {
            hc_page = new Page(this->writer, page_width, page_height, page);
            page->draw_page(hc_page, left, top);
        }

        this->hf.header_center->allocate_area(hc_page);

        header_height = std::max(this->hf.header_center->get_height(), header_height);
    }

    if (this->hf.header_right != NULL)
    {
        Page *hr_page;
        if (this->hf.header_center != NULL)
        {
            hr_page = new Page(this->writer, page_width/3, page_height, page);
            page->draw_page(hr_page, left+2*page_width/3, top);
        }
        else if (this->hf.header_right != NULL)
        {
            hr_page = new Page(this->writer, page_width/2, page_height, page);
            page->draw_page(hr_page, left+page_width/2, top);
        }
        else
        {
            hr_page = new Page(this->writer, page_width, page_height, page);
            page->draw_page(hr_page, left, top);
        }

        this->hf.header_right->allocate_area(hr_page);

        header_height = std::max(this->hf.header_right->get_height(), header_height);
    }

    if (this->hf.footer_left != NULL)
    {
        Page *fl_page;
        if (this->hf.footer_center != NULL)
            fl_page = new Page(this->writer, page_width/3, page_height, page);
        else if (this->hf.footer_right != NULL)
            fl_page = new Page(this->writer, page_width/2, page_height, page);
        else
            fl_page = new Page(this->writer, page_width, page_height, page);

        this->hf.footer_left->allocate_area(fl_page);

        page->draw_page(fl_page, left, bottom-this->hf.footer_left->get_height());

        footer_height = std::max(this->hf.footer_left->get_height(), footer_height);
    }

    if (this->hf.footer_center != NULL)
    {
        Page *fc_page;
        double left_position;
        if (this->hf.footer_left != NULL || this->hf.footer_right != NULL)
        {
            fc_page = new Page(this->writer, page_width/3, page_height, page);
            left_position = page_width/3;
        }
        else
        {
            fc_page = new Page(this->writer, page_width, page_height, page);
            left_position = 0;
        }

        this->hf.footer_center->allocate_area(fc_page);

        page->draw_page(fc_page, left+left_position, bottom-this->hf.footer_center->get_height());

        footer_height = std::max(this->hf.footer_center->get_height(), footer_height);
    }

    if (this->hf.footer_right != NULL)
    {
        Page *fr_page;
        double left_position;
        if (this->hf.footer_center != NULL)
        {
            fr_page = new Page(this->writer, page_width/3, page_height, page);
            left_position = 2*page_width/3;
        }
        else if (this->hf.footer_right != NULL)
        {
            fr_page = new Page(this->writer, page_width/2, page_height, page);
            left_position = page_width/2;
        }
        else
        {
            fr_page = new Page(this->writer, page_width, page_height, page);
            left_position = 0;
        }

        this->hf.footer_right->allocate_area(fr_page);

        page->draw_page(fr_page, left+left_position, bottom-this->hf.footer_right->get_height());

        footer_height = std::max(this->hf.footer_right->get_height(), footer_height);
    }

    this->current_page = new Page(this->writer, page_width, page_height-header_height-footer_height, page);
    page->draw_page(this->current_page, left, top + header_height);

    return this->current_page;
}

int Allocator::get_current_page_number()
{
    return this->pages.size() - 1;
}

std::string Allocator::get_page_number_string(int page_number)
{
    std::map<int, PageBlock *>::const_iterator it = this->page_blocks.begin();

    //find the valid PageBlock
    while (it != this->page_blocks.end() && page_number >= (*it).first)
    {
        std::map<int, PageBlock *>::const_iterator next = it;
        next++;

        //PageBlock found
        if ((*it).first == page_number || next == this->page_blocks.end() ||
                next != this->page_blocks.end() && (*next).first > page_number)
            return page_number_string(*(*it).second, page_number-(*it).first);

        it++;
    }

    //PageBlock not found, return a simple page number
    return get_numbering_number(page_number, 1);
}

void Allocator::set_page_block(PageBlock *block)
{
    if (must_add_new_page(this->current_page_block, *block))
    {
        //reset header/footer if there is a page size change
        if (this->page_size_changed == false)
            this->hf = HeaderFooter();


        int block_page_nember = this->pages.size();

        //remove old, no more valid PageBlocks
        std::map<int, PageBlock *>::const_iterator it = this->page_blocks.begin();
        while (it != this->page_blocks.end() && block_page_nember <= (*it).first)
        {
            if ((*it).first > this->last_added_block_page_number)
            {
                delete (*it).second;
                this->page_blocks.erase(it);
            }
                it++;
        }

        //insert the new PageBlock
        this->page_blocks[block_page_nember] = new PageBlock(*block);
        this->last_added_block_page_number = block_page_nember;

        this->page_size_changed = true;
    }

    this->current_page_block = *block;
}

void Allocator::set_header_footer(Flowable *flowable)
{
    //reset header/footer if the is header/footer change
    if (this->page_size_changed == false)
        this->hf = HeaderFooter();

    this->page_size_changed = true;

    switch (flowable->get_root_block()->special())
    {
    case Keyword::header_left:
        this->hf.header_left = flowable;
        break;
    case Keyword::header_center:
        this->hf.header_center = flowable;
        flowable->get_root_block()->set_text_align(Keyword::center);
        break;
    case Keyword::header_right:
        this->hf.header_right = flowable;
        flowable->get_root_block()->set_text_align(Keyword::right);
        break;
    case Keyword::footer_left:
        this->hf.footer_left = flowable;
        break;
    case Keyword::footer_center:
        this->hf.footer_center = flowable;
        flowable->get_root_block()->set_text_align(Keyword::center);
        break;
    case Keyword::footer_right:
        this->hf.footer_right = flowable;
        flowable->get_root_block()->set_text_align(Keyword::right);
        break;
    default:
        break;
    }
}

void Allocator::reset()
{
    for (Page *page : this->pages)
        delete page;

    this->page_size_changed = true;
    this->last_added_block_page_number = -1;

    this->pages.clear();

    this->hf = HeaderFooter();
}

void Allocator::write()
{
    for (Page *page : this->pages)
    {
        this->writer->create_new_page(page->get_width(), page->get_height());
        page->run();   
    }

    this->writer->close();
}

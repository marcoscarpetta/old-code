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

#include "flowable.h"

Text::Text(TextBlock &block) :
    TextBlock(block)
{
    //fill this->words
    this->Text::set_content(block.content());
}

void Text::set_content(const std::string &value)
{
    this->m_content = value;

    std::string clean_content = value;
    clean_string(clean_content);

    this->words.clear();

    //split content in words
    bool found;
    int start, length;

    this->next_word(clean_content, 0, found, start, length);

    while (found)
    {
        std::string word_str = clean_content.substr(start, length);
        this->words.push_back(word_str);

        this->next_word(clean_content, start+length, found, start, length);
    }
}

std::vector<std::string> *Text::get_words()
{
    return &(this->words);
}

void Text::next_word(std::string& str, int from, bool& found, int& start, int& length)
{
    found = false;

    start = str.find_first_not_of(" \n", from);

    if (start == std::string::npos)
      return;

    found = true;

    int end = str.find_first_of(" \n", start+1);

    if (end == std::string::npos)
        end == str.length()-1;

    length = end-start;
}

Image::Image(Allocator *allocator, ImageBlock &block) :
    allocator(allocator),
    ImageBlock(block)
{
    this->image_file = this->content();

    //get image size
    Rectangle image_size = this->allocator->get_writer()->get_image_size(this->image_file);
    this->image_width = image_size.width;
    this->image_height = image_size.height;

    //calculate scaling factors
    if (this->width() > 0)
    {
        this->scale_x = this->width()/this->image_width;
        if (this->height() > 0)
            this->scale_y = this->height()/this->image_height;
        else
            this->scale_y = this->scale_x;
    }
    else if (this->height() > 0)
    {
        this->scale_y = this->height()/this->image_height;
        this->scale_x = this->scale_y;
    }

    //calculate image size occupation
    this->occupied_height = this->image_height*this->scale_y +
            this->margin_top() + this->padding_top() +
            this->padding_bottom() + this->margin_bottom();

    this->occupied_width = this->image_width*this->scale_x +
            this->margin_left() + this->padding_left() +
            this->padding_right() + this->margin_right();
}

double Image::get_width()
{
    return this->occupied_width;
}

double Image::get_height()
{
    return this->occupied_height;
}

double Image::get_scale_x()
{
    return this->scale_x;
}

double Image::get_scale_y()
{
    return this->scale_y;
}

void Image::write_at(double left, double top)
{
    this->allocator->get_current_page()->draw_image(this->image_file,
                             left + this->margin_left() + this->padding_left(),
                             top + this->margin_top() + this->padding_top(),
                             this->scale_x, this->scale_y);

    this->allocator->get_current_page()->reserve_space(this->occupied_height);
}

Flowable::Flowable(Allocator *allocator, Numbering *root_numbering) :
    Group(allocator, root_numbering),
    root_block(NULL),
    write_header_footer(true)
{

}

Flowable::Flowable(const Flowable &base) :
    Group(base.allocator, base.root_numbering),
    prototype(base.prototype),
    root_block(NULL)
{
    if (base.root_block != NULL)
    {
        this->root_block = new Text(*base.root_block);
    }
}


bool Flowable::add_block(Block *block)
{
    if (block->block_type() == BlockType::text)
    {
        TextBlock *textblock = static_cast<TextBlock *>(block);
        Text *text = new Text(*textblock);

        //add block if it is an inline block
        if (text->is_inline() && this->root_block != NULL)
        {
            //if root_block is numbered, don't allow numbering on other
            //blocks of the group
            if (this->root_block->numbering_name().size() > 0)
                text->set_numbering_name("");

            text->add_properties_from(this->root_block);
            this->prototype.push_back(text);
            return true;
        }
        //add block if it is the root block
        else if (this->root_block == NULL)
        {
            this->prototype.push_back(text);
            this->root_block = text;
            return true;
        }
    }
    //add image block if it is inline
    else if (block->block_type() == BlockType::image)
    {
        ImageBlock *imageblock = static_cast<ImageBlock *>(block);
        Image *image = new Image(this->allocator, *imageblock);

        if (image->is_inline() && this->root_block != NULL)
        {
            this->prototype.push_back(image);
            return true;
        }
    }

    return false;
}

TextBlock *Flowable::get_root_block()
{
    return static_cast<TextBlock *>(this->root_block);
}

bool Flowable::allocate_area(Page *forced_page)
{
    if (this->root_block->special() != Keyword::none && forced_page == NULL)
    {
        this->allocator->set_header_footer(this);
        return false;
    }

    //rebuild the group from prototype
    this->build();

    if (forced_page != NULL)
        this->current_page = forced_page;
    else
        this->current_page = this->allocator->get_current_page();

    bool changed = false;

    if (this->start_position != this->current_page->get_current_top() ||
            this->start_page != this->allocator->get_current_page_number())
        changed = true;

    this->start_position = this->current_page->get_current_top();
    this->start_page = this->allocator->get_current_page_number();

    current_font.name = this->root_block->font_name();
    current_font.size = this->root_block->font_size();
    current_font.bold = this->root_block->bold();
    current_font.italic = this->root_block->italic();

    current_color = this->root_block->color();

    double space_width = this->allocator->get_writer()->get_string_width(" ", current_font);

    this->current_page->reserve_space(this->root_block->margin_top() + this->root_block->padding_top());

    if (this->current_page->get_current_top() > this->current_page->get_height() && forced_page == NULL)
        this->current_page = this->allocator->add_new_page();

    this->words_count = this->lines_on_page = 0;
    this->line_height = 0, this->line_width = 0, this->clean_line_width = 0;

    for (int i=0; i<this->blocks.size(); i++)
    {
        if (this->blocks[i]->block_type() == BlockType::text)
        {
            Text *text = static_cast<Text *>(this->blocks[i]);

            //update numbering page number and position
            if (text->id().size() > 0)
            {
                Numbering *numbering = this->root_numbering->get_numbering(text->id());
                numbering->set_page_number(this->allocator->get_current_page_number());
                numbering->set_top(this->current_page->get_current_top());
            }

            current_font.name = text->font_name();
            current_font.size = text->font_size();
            current_font.bold = text->bold();
            current_font.italic = text->italic();

            current_color = text->color();

            //Set font and color
            Word *font_color_word = new Word;
            font_color_word->operation = new SetFont(this->allocator->get_writer(), current_font);
            this->add_word(font_color_word, space_width, forced_page == NULL);
            font_color_word = new Word;
            font_color_word->operation = new SetColor(this->allocator->get_writer(), current_color);
            this->add_word(font_color_word, space_width, forced_page == NULL);

            if (this->root_block->preformatted())
            {
                size_t line_start = 0;
                size_t cursor = text->content().find('\n', 0);

                while (line_start != std::string::npos)
                {
                    std::string text_content = text->content().substr(line_start, cursor-line_start);
                    Word *word = new Word;
                    word->width = this->allocator->get_writer()->get_string_width(text_content, current_font);
                    word->height = current_font.size;
                    word->operation = new DrawString(
                                this->allocator->get_writer(),
                                text_content);

                    this->add_word(word, space_width, forced_page == NULL);

                    if (cursor != std::string::npos)
                        this->write_line(forced_page == NULL);
                    else
                        break;

                    line_start = cursor + 1;
                    cursor = text->content().find('\n', cursor+1);
                }
            }
            else
            {
                //this text block has the "fill" option
                std::string fill_text = text->fill();
                if (fill_text.size() > 0)
                {
                    double available_width = this->current_page->get_width() -
                            this->root_block->margin_left() -
                            this->root_block->padding_left() -
                            this->root_block->padding_right() -
                            this->root_block->margin_right();

                    std::string content = text->content();
                    double fill_width = this->allocator->get_writer()->get_string_width(fill_text, current_font);
                    double text_width = this->allocator->get_writer()->get_string_width(content, current_font);

                    //Save some variable of the current line, then write it
                    double current_top = this->current_page->get_current_top();
                    double current_line_height = this->line_height;
                    double current_line_width = this->line_width;
                    int page_number = this->allocator->get_current_page_number();

                    if (this->lines_on_page > 0)
                        current_top += this->root_block->font_size()*(this->root_block->line_height() - 1);

                    this->write_line(forced_page == NULL);

                    if (this->allocator->get_current_page_number() != page_number)
                        current_top = this->current_page->get_current_top() - current_line_height;

                    //The fill text will be on a new void line
                    if (current_line_width + text_width + fill_width > available_width)
                    {
                        current_line_height = current_font.size;
                        current_line_width = 0;
                        current_top = this->current_page->get_current_top();
                        this->current_page->reserve_space(current_font.size + this->root_block->font_size()*(this->root_block->line_height() - 1));
                    }

                    //Calculate the required replications of "fill_text"
                    int N = (available_width - current_line_width - text_width)/fill_width;

                    std::string new_content;
                    for (int i = 0; i < N; i++) new_content += fill_text;
                    new_content += text->content();

                    this->write_line(forced_page == NULL);

                    Operation *operation = new DrawString(this->allocator->get_writer(), new_content);
                    operation->set_left(this->current_page->get_width() -
                                        this->root_block->margin_right() -
                                        this->root_block->padding_right() -
                                        text_width - N*fill_width);

                    operation->set_top(current_top + (current_line_height - current_font.size)/2);

                    this->current_page->add_operation(operation);

                    continue;
                }

                for (int j=0; j < text->get_words()->size(); j++)
                {
                    Word *word = new Word;
                    word->width = this->allocator->get_writer()->get_string_width(text->get_words()->at(j), current_font);
                    word->height = current_font.size;
                    word->operation = new DrawString(
                                this->allocator->get_writer(),
                                text->get_words()->at(j));

                    this->add_word(word, space_width, forced_page == NULL);
                }
            }
        }
        else if (this->blocks[i]->block_type() == BlockType::image)
        {
            Image *image = static_cast<Image *>(this->blocks[i]);

            Word *word = new Word;
            word->left = image->margin_left() + image->padding_left();
            word->top = image->margin_top() + image->padding_top();
            word->width = image->get_width();
            word->height = image->get_height();
            word->operation = new DrawImage(
                        this->allocator->get_writer(),
                        image->content(), image->get_scale_x(), image->get_scale_y());

            this->add_word(word, space_width, forced_page == NULL);
        }
    }

    //write the last line, if any
    this->write_line(forced_page == NULL, true);

    this->current_page->reserve_space(this->root_block->padding_bottom() + this->root_block->margin_bottom());

    if (this->current_page->get_current_top() != this->end_position || this->allocator->get_current_page_number() != this->end_page)
        changed = true;

    this->end_position = this->current_page->get_current_top();
    this->end_page = this->allocator->get_current_page_number();

    if (forced_page != NULL)
    {
        this->width = forced_page->get_width();
        this->height = this->end_position - this->start_position;
    }

    return changed;
}

std::vector<Block *> Flowable::build_block(
        Text *prototype,
        Numbering *target_numbering,
        bool number_only)
{
    std::vector<Block *> built;

    std::string content = prototype->content();

    //replace {{number}}
    replace(content, "number", target_numbering->get_total_number());

    if (number_only)
    {
        Text *textblock = new Text(*prototype);
        textblock->set_content(content);
        textblock->set_id("");
        built.push_back(textblock);
    }
    else
    {
        //replace {{page}}
        replace(content, "page",
                this->allocator->get_page_number_string(target_numbering->get_page_number()));

        //replace {{content}}
        std::size_t from = 0;
        std::size_t cursor = content.find("{{content}}");
        while (cursor != std::string::npos)
        {
            std::string text_content = content.substr(from, cursor - from);

            Text *text = new Text(*prototype);
            text->set_content(text_content);
            built.push_back(text);

            for (Block *block : *(target_numbering->get_content()))
            {
                //copy block and add prototype attributes
                if (block->block_type() == BlockType::text)
                {
                    Text *textblock = new Text(*(static_cast<Text *>(block)));
                    textblock->set_id("");
                    textblock->add_properties_from(prototype, true);
                    built.push_back(textblock);
                }
                else if (block->block_type() == BlockType::image)
                {
                    Image *image = new Image(*(static_cast<Image *>(block)));
                    image->set_id("");
                    built.push_back(image);
                }
            }

            from = cursor + 11;
            cursor = content.find("{{content}}", from);
        }

        std::string text_content = content.substr(from, content.size() - from);

        Text *text = new Text(*prototype);
        text->set_id("");
        text->set_content(text_content);
        built.push_back(text);
    }

    return built;
}

void Flowable::build()
{
    //clean previous build
    for (Block *child : this->built_blocks)
        delete child;
    this->built_blocks.clear();
    this->blocks.clear();

    std::string id = this->root_block->id();
    std::string name = this->root_block->numbering_name();
    std::string target_id = this->root_block->target_id();

    //the flowable is a header/footer
    if (this->root_block->special() != Keyword::none)
    {
        for (Block *block : this->prototype)
        {
            if (block->block_type() == BlockType::image)
            {
                //copy image block
                Image *image = new Image(*(static_cast<Image *>(block)));
                //add copied block to built_content
                this->built_blocks.push_back(image);
                this->blocks.push_back(image);
            }
            else if (block->block_type() == BlockType::text)
            {
                std::string content = block->content();

                //replace {{page}}
                replace(content, "page",
                        this->allocator->get_page_number_string(this->allocator->get_current_page_number()));

                //add new content to built_content
                Text *textblock = new Text(*static_cast<Text *>(block));
                textblock->set_content(content);
                this->built_blocks.push_back(textblock);
                this->blocks.push_back(textblock);
            }
        }

        return;
    }

    //the whole group is a reference
    if (target_id.size() > 0)
    {
        Numbering *target_numbering = this->root_numbering->get_numbering(target_id);

        //if this group has some content, build it according to target_numbering
        if (this->prototype.size() > 1 || this->root_block->content().size() > 0)
        {
            for (Block *block : this->prototype)
            {
                if (block->block_type() == BlockType::image)
                {
                    //copy image block
                    Image *image = new Image(*(static_cast<Image *>(block)));
                    //add copied block to built_content
                    this->built_blocks.push_back(image);
                    this->blocks.push_back(image);
                }
                else if (block->block_type() == BlockType::text)
                {
                    //build new content
                    std::vector<Block *> built = this->build_block(
                                static_cast<Text *>(block),
                                target_numbering
                                );

                    //add new content to built_content
                    this->built_blocks.insert(this->built_blocks.end(), built.begin(), built.end());
                    this->blocks.insert(this->blocks.end(), built.begin(), built.end());
                }
            }
        }
        //use reference_string of the target block as content for this group
        //and build it
        else
        {
            Text *content = new Text(*(this->root_block));

            content->set_content(target_numbering->get_text_block()->numbering_reference());

            //build new content
            std::vector<Block *> built = this->build_block(content, target_numbering);

            //add new content to built_content
            this->built_blocks.insert(this->built_blocks.end(), built.begin(), built.end());
            this->blocks.insert(this->blocks.end(), built.begin(), built.end());
        }
    }
    //look for single references inside blocks
    else
    {
        for (Block *block : this->prototype)
        {
            bool add_block = true;

            if (block->block_type() == BlockType::text)
            {
                Text *textblock = static_cast<Text *>(block);

                std::string block_target_id = textblock->target_id();

                if (block_target_id.size() > 0)
                {
                    add_block = false;

                    Numbering *target_numbering = this->root_numbering->get_numbering(
                                block_target_id);

                    if (textblock->content().size() > 0)
                    {
                        //build new content
                        std::vector<Block *> built = this->build_block(
                                    textblock,
                                    target_numbering
                                    );

                        //add new content to built_content
                        this->built_blocks.insert(this->built_blocks.end(), built.begin(), built.end());
                        this->blocks.insert(this->blocks.end(), built.begin(), built.end());
                    }
                    else
                    {
                        Text *content = new Text(*(textblock));

                        content->set_content(target_numbering->get_text_block()->numbering_reference());

                        //build new content
                        std::vector<Block *> built = this->build_block(content, target_numbering);

                        //add new content to built_content
                        this->built_blocks.insert(this->built_blocks.end(), built.begin(), built.end());
                        this->blocks.insert(this->blocks.end(), built.begin(), built.end());
                    }
                }
            }

            //add the block from prototype to blocks if it hasn't a reference
            if (add_block)
                this->blocks.push_back(block);
        }
    }

    //the whole group has an id
    if (target_id.size() == 0 && id.size() > 0)
    {
        Numbering *numbering = this->root_numbering->get_numbering(id);

        //set content on numbering object
        numbering->set_content(this->blocks);

        //the whole group has a name
        if (name.size() > 0)
        {
            Text *numbering_block = new Text(*(this->root_block));
            numbering_block->set_content(numbering->get_text_block()->numbering_string());

            std::vector<Block *> built = this->build_block(numbering_block, numbering, true);
            this->built_blocks.insert(this->built_blocks.end(), built.begin(), built.end());
            //insert built block at the beginning
            this->blocks.insert(this->blocks.begin(), built.begin(), built.end());
        }
    }
    //look for single blocks with an id
    else if (target_id.size() == 0 && id.size() == 0)
    {
        for (int i=0; i<this->blocks.size(); i++)
        {
            if (this->blocks.at(i)->block_type() == BlockType::text)
            {
                Text *textblock = static_cast<Text *>(this->blocks.at(i));

                std::string id = textblock->id();
                std::string name = textblock->numbering_name();

                //this block has an id
                if (id.size() > 0)
                {
                    Numbering *numbering = this->root_numbering->get_numbering(id);

                    //set content on numbering object
                    std::vector<Block *> content;
                    content.push_back(textblock);
                    numbering->set_content(content);

                    //this block has a name
                    if (name.size() > 0)
                    {
                        Text *numbering_block = new Text(*(textblock));
                        numbering_block->set_content(numbering->get_text_block()->numbering_string());

                        Text * built = static_cast<Text *>(this->build_block(numbering_block, numbering, true).at(0));
                        std::string built_block_content = built->content();
                        built_block_content.append(" ").append(textblock->content());
                        built->set_content(built_block_content);
                        this->built_blocks.push_back(built);
                        //insert built block before this one
                        this->blocks[i] = built;
                    }
                }
            }
        }
    }
}

void Flowable::add_word(Word *word, double spacing, bool new_page)
{
    double available_width = this->current_page->get_width() -
            this->root_block->margin_left() -
            this->root_block->padding_left() -
            this->root_block->padding_right() -
            this->root_block->margin_right();

    //if this word doesn't fit current line, write it and create a void one
    if (this->line_width + word->width + spacing > available_width && !this->root_block->preformatted())
        this->write_line(new_page);

    //add word to line
    this->line.push_back(word);

    //update line dimensions
    this->line_width += word->width;
    this->clean_line_width += word->width;
    this->line_height = std::max(this->line_height, word->height);

    if (word->width > 0)
    {
        this->words_count += 1;

        if (this->words_count > 0)
            this->line_width += spacing;
    }
}

void Flowable::write_line(bool new_page, bool last_line)
{
    if (this->line.size() > 0)
    {
        //add a new page if the line doesn't fit the current one
        if (new_page &&
                this->current_page->get_current_top() + this->line_height >
                this->current_page->get_height())
        {
            this->current_page = this->allocator->add_new_page();

            //set font and color to that used on last page
            this->current_page->set_font(last_font);
            this->current_page->set_fill_color(this->last_color);
            this->last_font = this->current_font;
            this->last_color = this->current_color;

            this->lines_on_page = 0;
        }

        //add lines spacing
        if (this->lines_on_page > 0)
            this->current_page->reserve_space(this->root_block->font_size()*(this->root_block->line_height() - 1));

        int n = 0;

        //calculate left position and spacing of words according to this->root_block properties
        double left = this->root_block->margin_left() + this->root_block->padding_left();
        double word_spacing = 0;
        if (this->words_count > 1)
            word_spacing = (this->line_width-this->clean_line_width)/(this->words_count-1);

        double available_width = this->current_page->get_width() -
                this->root_block->margin_left() -
                this->root_block->padding_left() -
                this->root_block->padding_right() -
                this->root_block->margin_right();

        switch (this->root_block->text_align())
        {
        case Keyword::left:
            break;

        case Keyword::right:
            left += available_width - this->line_width;
            break;

        case Keyword::center:
            left += (available_width - this->line_width)/2;
            break;

        case Keyword::justify_all:
            if (words_count > 1)
                word_spacing = (available_width - this->clean_line_width)/(words_count-1);
            break;

        case Keyword::justify:
            if (words_count > 1 && !last_line)
                word_spacing = (available_width - this->clean_line_width)/(words_count-1);

        default:
            break;
        }


        for (Word *word : this->line)
        {
            if (word->width > 0)
            {
                word->operation->set_left(left + word->left + n*word_spacing);
                word->operation->set_top(
                            this->current_page->get_current_top() + word->top +
                            (line_height - word->height)/2);

                n += 1;
                left += word->width;
            }

            this->current_page->add_operation(word->operation);

            delete word;
        }

        //reserve space for the written line
        this->current_page->reserve_space(this->line_height);

        this->lines_on_page += 1;

        this->line.clear();

        this->clean_line_width = this->line_height = this->line_width = 0;
        this->words_count = 0;
    }
}

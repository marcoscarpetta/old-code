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

#include "parser.h"

std::string &read_file(std::string name)
{
    std::ifstream file(name);
    std::stringstream file_string;
    file_string << file.rdbuf();
    std::string *str = new std::string;
    str->append(file_string.str());
    return *str;
}

void resolve_escapes(std::string &str)
{
    std::size_t escape = str.find('\\');

    while (escape != std::string::npos)
    {
        str.erase(escape, 1);
        escape = str.find('\\', escape + 1);
    }
}

void resolve_includes(std::string *str)
{
    int occurrences = 1, i = 0;

    while (occurrences > 0 && i < max_iterations)
    {

        occurrences = 0;
        std::size_t include_start = str->find("#include ", 0);
        i++;

        while (include_start != std::string::npos)
        {
            //escaped
            if (include_start != 0)
                if (str->at(include_start - 1) == '\\')
                {
                    include_start = str->find("#include ", include_start + 9);
                    continue;
                }

            std::size_t line_end = str->find('\n', include_start);

            //no filename
            if (str->find_first_not_of(' ', include_start + 9) == line_end)
            {
                str->replace(include_start, line_end - include_start, "");
                include_start = str->find("#include ", include_start + 9);
                continue;
            }

            if (line_end == std::string::npos)
                line_end = str->size();
            else
                line_end += 1;

            occurrences += 1;

            std::size_t name_start = include_start + 9;

            std::string file_name = str->substr(name_start, line_end - name_start);

            clean_string(file_name);

            std::string file_content = read_file(file_name);

            str->replace(include_start, line_end - include_start, file_content);
            include_start = str->find("#include ", include_start + file_content.size());
        }
    }
}

int replace_macro(std::string &text, std::string &macro_name, std::string &macro_value)
{
    int occurrences = 0;
    std::string name = "$";
    name.append(macro_name);

    size_t macro_start = text.find(name);

    while(macro_start != std::string::npos)
    {
        if (text.find_first_not_of(allowed_macro_name_chars, macro_start + 1) == macro_start + name.size() ||
                macro_start + name.size() == text.size() + 1)
        {
            occurrences += 1;
            text.replace(macro_start, name.size(), macro_value);
            macro_start = text.find(name, macro_start + macro_value.size());
        }
        else
        {
            macro_start = text.find(name, macro_start + name.size());
        }
    }

    return occurrences;
}

void replace_macros(std::string *str)
{
    std::map <std::string, std::string *> macros;

    //find macro definitions
    std::size_t define_start = str->find("#define ", 0);

    while (define_start != std::string::npos)
    {
        //escaped
        if (define_start != 0)
            if (str->at(define_start - 1) == '\\')
            {
                define_start = str->find("#define ", define_start + 8);
                continue;
            }

        std::size_t line_end = str->find('\n', define_start + 8);
        if (line_end == std::string::npos)
            break;

        //macro name
        std::string macro_name = str->substr(define_start + 8, line_end - define_start - 8);
        clean_string(macro_name);

        //macro content
        std::string *macro_content = new std::string();
        std::size_t line_start;

        bool continued = false;
        do {
            line_start = line_end + 1;
            line_end = str->find('\n', line_start);

            if (line_end == std::string::npos)
                line_end = str->size() - 1;
            else
                line_end;

            continued = false;
            std::size_t backslash = str->find_last_of('\\', line_end);
            if (backslash != std::string::npos &&
                    str->find_first_not_of(" \n", backslash + 1) >= line_end &&
                    str->at(backslash - 1) != '\\')
            {
                str->erase(backslash, 1);
                line_end -= 1;
                continued = true;
            }

            macro_content->append(str->substr(line_start, line_end - line_start + 1));
        }
        while (line_end < str->size() && continued);


        //add macro to std::map object only if it has a valid name
        if (macro_name.find_first_of(allowed_macro_name_start_chars) == 0 &&
                macro_name.find_first_not_of(allowed_macro_name_chars) == std::string::npos)
        {
            if (macros.count(macro_name) > 0)
            {
                replace_macro(*macro_content, macro_name, *(macros.at(macro_name)));
                delete macros.at(macro_name);
            }

            macros[macro_name] = macro_content;
        }

        //delete macro definition
        str->replace(define_start, line_end - define_start + 1, "");

        define_start = str->find("#define ", define_start);
    }

    int i=0, occurrences=1;

    while (i < max_iterations && occurrences > 0)
    {
        occurrences = 0;
        for (std::pair <std::string, std::string *> name_value: macros)
        {
            occurrences += replace_macro(*str, name_value.first, *(name_value.second));
        }
    }
}

void add_toc_blocks(Allocator *allocator,
                    Numbering* numbering,
                    std::vector<Flowable *> *prototype,
                    int i,
                    std::vector<Group *> *groups)
{
    Flowable *prototype_flowable = prototype->at(i);
    TextBlock *root_block = prototype_flowable->get_root_block();
    std::string name = root_block->target_name();

    for (Numbering *child_numbering : *(numbering->get_children()))
    {
        if (child_numbering->get_text_block()->numbering_name() == name)
        {
            Flowable *flowable = new Flowable(*(prototype_flowable));
            flowable->get_root_block()->set_target_id(child_numbering->get_text_block()->id());
            groups->push_back(flowable);

            if (i+1 < prototype->size())
                add_toc_blocks(allocator,
                               child_numbering,
                               prototype,
                               i+1,
                               groups);
        }
    }
}

Block *get_next_block(std::string *str,
                          Allocator *allocator,
                          size_t &block_start,
                          size_t from)
{
    block_start = str->find('#', from);

    while (block_start != std::string::npos)
    {
        if (block_start != 0 && str->at(block_start - 1) == '\\')
        {
            block_start = str->find('#', block_start + 1);
            continue;
        }

        size_t block_name_end = str->find_first_of(" \n", block_start);

        std::string block_type = str->substr(block_start + 1, block_name_end - (block_start + 1));

        if (block_type == "text")
            return new TextBlock();
        if (block_type == "image")
            return new ImageBlock();
        if (block_type == "page")
            return new PageBlock();
        if (block_type == "space")
            return new SpaceBlock();

        block_start = str->find('#', block_start + 1);
    }

    block_start = std::string::npos;
    return NULL;
}

void parse(std::string *str,
      Allocator *allocator,
      std::vector<Group *> *groups)
{
    std::map<std::string, Numbering *> last_elements;
    Numbering *root_numbering = new Numbering(new TextBlock());

    size_t cursor;
    Block *block = get_next_block(str, allocator, cursor, 0);

    size_t block_end;
    Block *new_block = get_next_block(str, allocator, block_end, cursor + 1);

    std::vector<std::vector<Flowable *> *> toc_flowables;
    std::vector<std::vector<Group *> *> group_parts;

    group_parts.push_back(new std::vector<Group *>());

    bool add_toc_flowable = false;

    Group *group = NULL;

    while (cursor != std::string::npos)
    {
        //get block's attributes
        cursor = str->find_first_of(" \n", cursor);
        cursor = str->find_first_not_of(" \n", cursor);

        while (cursor < block_end)
        {
            size_t attr_name_end = str->find("=\"", cursor);

            if (attr_name_end >= block_end)
                break;

            if (str->find_first_not_of(allowed_attr_name_chars, cursor) < attr_name_end)
                break;

            size_t attr_value_end = str->find('"', attr_name_end+2);
            while (attr_value_end != std::string::npos && str->at(attr_value_end - 1) == '\\')
                attr_value_end = str->find('"', attr_value_end+1);

            if (attr_value_end >= block_end)
                break;

            //the attribute is good
            std::string attr_name = str->substr(cursor, attr_name_end-cursor);
            std::string attr_value =  str->substr(attr_name_end + 2,
                                                  attr_value_end - attr_name_end - 2);

            if (attr_name.compare("fill") != 0)
                clean_string(attr_value);

            resolve_escapes(attr_value);

            block->set(attr_name, attr_value);

            cursor = str->find_first_of(" \n", attr_value_end);
            cursor = str->find_first_not_of(" \n", cursor);
        }

        std::string content = str->substr(cursor, block_end - cursor);

        resolve_escapes(content);

        block->set_content(content);

        //add numbering
        if (block->block_type() == BlockType::text)
        {
            TextBlock *textblock = static_cast<TextBlock *>(block);

            std::string name = textblock->numbering_name();
            std::string block_id = textblock->id();

            if (name.size() != 0)
            {
                //assign a unique id if the block doesn't have one
                if (block_id.size() == 0)
                {
                    std::ostringstream oss;
                    oss << "unique_id_" << cursor;
                    textblock->set_id(oss.str());
                }

                Numbering *numbering = new Numbering(textblock);

                std::string parent_name = textblock->numbering_parent();

                //add numbering to root numbering, if it hasn't a parent
                if (parent_name.size() == 0)
                {
                    root_numbering->add_child(numbering);
                    last_elements[name] = numbering;
                }
                //add numbering to its parent, if it has one
                else if (last_elements.count(parent_name) > 0)
                {
                    Numbering *parent_numbering = last_elements[parent_name];
                    parent_numbering->add_child(numbering);
                    last_elements[name] = numbering;
                }
            }
            else if (block_id.size() > 0)
            {
                Numbering *numbering = new Numbering(textblock);
                root_numbering->add_child(numbering);
            }
        }

        //add block to the last group or if it is rejected add it to a newly created group
        if (group == NULL || !group->add_block(block))
        {
            bool add_to_groups = true;
            switch (block->block_type())
            {
            case BlockType::text:
            {
                Flowable *flowable = new Flowable(allocator, root_numbering);
                if (block->block_type() == BlockType::text)
                {
                    TextBlock *textblock = static_cast<TextBlock *>(block);
                    if (textblock->target_name().size() > 0)
                    {
                        add_to_groups = false;
                        if (add_toc_flowable)
                            toc_flowables.back()->push_back(flowable);
                        else
                        {
                            std::vector<Flowable *> *flowables = new std::vector<Flowable *>();
                            flowables->push_back(flowable);

                            group_parts.push_back(new std::vector<Group *>());
                            toc_flowables.push_back(flowables);
                        }
                        add_toc_flowable = true;
                    }
                    else
                        add_toc_flowable = false;
                }
                group = flowable;
                break;
            }
            case BlockType::image:
                group = new Figure(allocator, root_numbering);
                add_toc_flowable = false;
                break;
            default:
                group = new Group(allocator, root_numbering);
                add_toc_flowable = false;
            }

            group->add_block(block);
            if (add_to_groups)
                group_parts.back()->push_back(group);
        }

        block = new_block;
        cursor = block_end;

        new_block = get_next_block(str, allocator, block_end, cursor + 1);
    }

    //set numbering numbers
    root_numbering->calculate_numbering_number();

    //resolve toc
    int i = 0;
    while (i < group_parts.size()-1)
    {
        groups->insert(groups->end(), group_parts.at(i)->begin(), group_parts.at(i)->end());
        std::vector<Group *> built;
        add_toc_blocks(allocator, root_numbering, toc_flowables.at(i), 0, &built);
        groups->insert(groups->end(), built.begin(), built.end());
        i++;
    }

    groups->insert(groups->end(), group_parts.at(i)->begin(), group_parts.at(i)->end());
}

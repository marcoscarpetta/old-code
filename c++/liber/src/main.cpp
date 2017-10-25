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

#include <iostream>
#include "podofowriter.h"
#include "allocator.h"
#include "parser.h"

int main(int argc, char *argv[])
{
    std::string input_file_name;
    std::string output_file_name;

    //parse command line arguments
    if (argc < 2)
    {
        std::cout << "ERROR: no input file" << std::endl;
        return 1;
    }
    else
    {
        input_file_name.assign(argv[1]);
        if (argc > 2)
            output_file_name.assign(argv[2]);
        else
        {
            output_file_name.assign(input_file_name);
            output_file_name.append(".pdf");
        }
    }

    std::string file = read_file(input_file_name);

    PoDoFoWriter *writer = new PoDoFoWriter(output_file_name);
    Allocator *allocator = new Allocator(writer);

    resolve_includes(&file);
    replace_macros(&file);

    std::vector <Group *> groups;

    parse(&file, allocator, &groups);

    bool changed = true;

    while (changed)
    {
        changed = false;

        allocator->reset();

        for (Group * group : groups)
        {
            changed = group->allocate_area() || changed;
        }
    }

    allocator->write();

    return 0;
}

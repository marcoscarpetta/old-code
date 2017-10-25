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

#include "baseclasses.h"

char units[3][3] = {
    {'I', 'V', 'X'},
    {'X', 'L', 'C'},
    {'C', 'D', 'M'},
};

double parse_length(const std::string &str)
{
    size_t double_end;
    double value = std::stod(str, &double_end);

    std::string unit = str.substr(double_end);
    if (unit == "mm")
        value = value*mm;

    return value;
}

void clean_string(std::string &str)
{
    //replace newlines with spaces
    size_t cursor = str.find('\n');
    while (cursor != std::string::npos)
    {
        str.replace(cursor, 1, " ");
        cursor = str.find('\n', cursor+1);
    }

    //trim
    cursor = str.find_first_not_of(' ');
    str.erase(0, cursor);
    cursor = str.find_last_not_of(' ');
    if (cursor != std::string::npos)
        str.erase(cursor+1);

    //remove duplicate spaces
    cursor = str.find(' ');
    while (cursor != std::string::npos)
    {
        str.erase(cursor + 1, str.find_first_not_of(' ', cursor+1) - cursor - 1);
        cursor = str.find(' ', cursor+1);
    }
}

std::string find_replaceable(std::string &str)
{
    size_t cursor = str.find("{{");

    while (cursor != std::string::npos)
    {
        size_t key_end = str.find("}}", cursor);
        if (key_end != std::string::npos)
        {
            std::string key = str.substr(cursor + 2, key_end - cursor - 2);
            if (key.size() > 0)
                return key;
        }

        cursor = str.find("{{", cursor + 2);
    }

    return "";
}

void replace(std::string &str, const std::string &key, const std::string &value)
{
    std::string k = "{{";
    k += key + "}}";
    size_t start = str.find(k.c_str());

    if (start != std::string::npos)
        str.replace(start, 4+key.size(), value.c_str());
}

std::string get_numbering_number(int number, int start)
{
    std::ostringstream oss;
    //if number is 0 and start is 1, return 1
    oss << (number + start);
    return oss.str();
}

std::string get_numbering_letter(int number, int start, bool uppercase)
{
    if (start < 1)
        start = 1;

    number += start - 1;
    char starting_letter = uppercase ? 'A' : 'a';

    std::string str;

    int power = 1;
    //numbers representable with words of power letters +
    //words of (power-1) letters + ... + words of 1 letter
    int unique_strings = 26;
    while (unique_strings-1 < number)
    {
        power++;
        unique_strings += std::pow(26, power);
    }

    number -= (unique_strings - std::pow(26, power));

    while (power != 0)
    {
        int unit = std::pow(26, power-1);
        int letter = number / unit;

        str += (starting_letter + letter);

        number -= unit*letter;
        power--;
    }

    return str;
}

std::string get_numbering_roman(int number, int start)
{
    if (start < 1)
        start = 1;

    number += start;
    std::string str;

    if (number > 999)
        return "Crazy!!1!";

    std::ostringstream oss;
    for (int i = 2; i > -1; i--)
    {
        int min = std::pow(10, i);
        if (number >= min*9)
        {
            oss << units[i][0] << units[i][2];
            number -= min*9;
        }
        else if (number >= min*8)
        {
            oss << units[i][1] << units[i][0] << units[i][0] << units[i][0];
            number -= min*8;
        }
        else if (number >= min*7)
        {
            oss << units[i][1] << units[i][0] << units[i][0];
            number -= min*7;
        }
        else if (number >= min*6)
        {
            oss << units[i][1] << units[i][0];
            number -= min*6;
        }
        else if (number >= min*5)
        {
            oss << units[i][1];
            number -= min*5;
        }
        else if (number >= min*4)
        {
            oss << units[i][0] << units[i][1];
            number -= min*4;
        }
        else if (number >= min*3)
        {
            oss << units[i][0] << units[i][0] << units[i][0];
            number -= min*3;
        }
        else if (number >= min*2)
        {
            oss << units[i][0] << units[i][0];
            number -= min*2;
        }
        else if (number >= min)
        {
            oss << units[i][0];
            number -= min;
        }
    }

    return oss.str();
}

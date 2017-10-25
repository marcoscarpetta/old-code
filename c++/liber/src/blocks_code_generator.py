# Copyright (C) 2017 Marco Scarpetta
#
# This file is part of Liber.
#
# Liber is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Liber is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Liber. If not, see <http://www.gnu.org/licenses/>.

import jinja2
import ruamel.yaml as yaml
import argparse

h_template = jinja2.Template(open("blocks.h.template").read())
cpp_template = jinja2.Template(open("blocks.cpp.template").read())
docs_template = jinja2.Template(open("../docs/attributes.liber.template").read())

pass_types = {
    "bool": "bool",
    "int": "int",
    "float": "double",
    "length": "double",
    "string": "const std::string &",
    "color": "const CSSColorParser::Color &",
    "keyword": "Keyword"}

store_types = {
    "bool": "bool",
    "int": "int",
    "float": "double",
    "length": "double",
    "string": "std::string",
    "color": "CSSColorParser::Color",
    "keyword": "Keyword"
}

parsers = {
    "bool": "std::stoi",
    "int": "std::stoi",
    "float": "std::stod",
    "length": "parse_length",
    "string": "",
    "color": "CSSColorParser::parse",
    "keyword": "parse_keyword"
}

parser = argparse.ArgumentParser()
parser.add_argument("-d", "--docs", help="only generate documentation for Blocks' attributes",
                    action="store_true")
args = parser.parse_args()

data = yaml.safe_load(open("attributes.yaml").read())

if (args.docs):
    for block in data:
        f = open("../docs/{}_attributes.liber".format(block), "w")
        f.write(docs_template.render(attributes=data[block]))
        f.close()

else:
    keywords_list = []
    
    for block in data:
        for attr in data[block]:
            if attr["type"] == "keyword":
                for keyword in attr["allowed_values"]:
                    if keyword not in keywords_list:
                        keywords_list.append(keyword)
                
                attr["default_value"] = "Keyword::" + attr["default_value"].replace("-", "_")
            elif attr["type"] == "bool":
                attr["default_value"] = attr["default_value"]*"true" + (not attr["default_value"])*"false"
            elif attr["type"] == "string" or attr["type"] == "length" or attr["type"] == "color":
                attr["default_value"] = '"{}"'.format(attr["default_value"])
                
            if not "cpp_name" in attr:
                attr["cpp_name"] = attr["name"].replace("-", "_")

    f = open("blocks.h", "w")
    f.write(h_template.render(block_types=data, keywords=keywords_list, pass_types=pass_types, store_types=store_types))
    f.close()

    f = open("blocks.cpp", "w")
    f.write(cpp_template.render(block_types=data, keywords=keywords_list, pass_types=pass_types, store_types=store_types, parsers=parsers))
    f.close()

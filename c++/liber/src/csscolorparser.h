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
 *
 *
 * (c) Dean McNamee <dean@gmail.com>, 2012.
 * C++ port by Mapbox, Konstantin Käfer <mail@kkaefer.com>, 2014-2017.
 *
 * https://github.com/deanm/css-color-parser-js
 * https://github.com/kkaefer/css-color-parser-cpp
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef CSS_COLOR_PARSER_CPP
#define CSS_COLOR_PARSER_CPP

#include <string>
#include <cmath>

namespace CSSColorParser {

struct Color {
    inline Color() {
    }
    inline Color(unsigned char r_, unsigned char g_, unsigned char b_, double a_=1.0)
        : r(r_), g(g_), b(b_), a(a_ > 1 ? 1 : a_ < 0 ? 0 : a_) {
    }
    unsigned char r = 0, g = 0, b = 0;
    double a = 1.0;
};

inline bool operator==(const Color& lhs, const Color& rhs) {
    return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && ::fabs(lhs.a - lhs.b) < 0.0001;
}

inline bool operator!=(const Color& lhs, const Color& rhs) {
    return !(lhs == rhs);
}

Color parse(const std::string& css_str);

} // namespace CSSColorParser

#endif

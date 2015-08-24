/*****************************************************************************
 *
 * This file is part of Mapnik (c++ mapping toolkit)
 *
 * Copyright (C) 2014 Artem Pavlenko
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef MAPNIK_CVS_GRAMMAR_HPP
#define MAPNIK_CVS_GRAMMAR_HPP

//#define BOOST_SPIRIT_DEBUG

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace mapnik {

namespace qi = boost::spirit::qi;
using column  = std::string;
using columns = std::vector<column>;
using csv_line = columns;
using csv_data = std::vector<csv_line>;

template <typename Iterator>
struct csv_line_grammar : qi::grammar<Iterator, void(csv_line&, std::string const&), qi::blank_type>
{
    csv_line_grammar() : csv_line_grammar::base_type(line)
    {
        using namespace qi;
        qi::_a_type _a;
        qi::_r1_type _r1;
        qi::_r2_type _r2;
        qi::lit_type lit;
        //qi::eol_type eol;
        qi::_val_type _val;
        qi::_1_type _1;
        qi::char_type char_;
        qi::eps_type eps;
        qi::omit_type omit;
        unesc_char.add
            ("\\a", '\a')
            ("\\b", '\b')
            ("\\f", '\f')
            ("\\n", '\n')
            ("\\r", '\r')
            ("\\t", '\t')
            ("\\v", '\v')
            ("\\\\",'\\')
            ("\\\'", '\'')
            ("\\\"", '\"')
            ("\"\"", '\"') // double quote
            ;

        line = column(_r2)[boost::phoenix::push_back(_r1,_1)]  % char_(_r2)
            ;
        column = quoted | *(char_ - (lit(_r1) /*| eol*/))
            ;
        quoted = omit[char_("\"'")[_a = _1]] > text(_a)[boost::phoenix::swap(_val,_1)] > -lit(_a)
            ;
        text = *(unesc_char | (char_ - char_(_r1)))
            ;
        //BOOST_SPIRIT_DEBUG_NODES((line)(column)(quoted));
    }
  private:
    qi::rule<Iterator, void(csv_line&,std::string const&), qi::blank_type> line;
    qi::rule<Iterator, column(std::string const&)> column; // no-skip
    qi::rule<Iterator, std::string(char)> text;
    qi::rule<Iterator, qi::locals<char>, std::string()> quoted;
    qi::symbols<char const, char const> unesc_char;
};

template <typename Iterator>
struct csv_file_grammar : qi::grammar<Iterator, csv_data(std::string const&), qi::blank_type>
{
    csv_file_grammar() : csv_file_grammar::base_type(start)
    {
        using namespace qi;
        qi::eol_type eol;
        qi::_r1_type _r1;
        start  = -line(_r1) % eol
            ;
        BOOST_SPIRIT_DEBUG_NODES((start));
    }
  private:
    qi::rule<Iterator, csv_data(std::string const&), qi::blank_type> start;
    csv_line_grammar<Iterator> line;
};


}

#endif // MAPNIK_CVS_GRAMMAR_HPP

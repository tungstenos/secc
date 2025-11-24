/* secc - SELinux Compiler Collection
   Copyright (C) 2025  Rahul Sandhu <nvraxn@gmail.com>

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License, as published by
   the Free Software Foundation, either version 3 of the License, or (at your
   option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

#pragma once

#include <concepts>
#include <generator>
#include <istream>
#include <string>
#include <string_view>

#include "secc/source_location.hxx"

namespace secc
{

  struct token
  {
    enum class type
    {
      LPAREN,
      RPAREN,
      SYMBOL,
      STRING,
      END_OF_FILE,
      INVALID
    };

    type t;
    std::string lexeme;
    source_location loc;
  };

  struct lexer
  {
    lexer (std::istream &stream, std::string_view filename);

    std::generator<token &&>
    lex ();

  private:
    bool
    is_at_end () const;

    char
    peek () const;

    char
    advance ();

    template <typename Predicate>
    void
    skip_while (Predicate p)
      requires std::invocable<Predicate, char>;

    void
    skip_whitespace ();

    void
    skip_comment ();

    inline source_location
    current_location () const;

    token
    next_token ();

    token
    read_string (source_location loc);

    token
    read_symbol (source_location loc);

    /* TODO: an iterator, possibly a forward iterator would be nice.  Not sure
       how feasible this is; we want to peek ahead and then backtrack for the
       allocations in read_string () and read_symbol().  */
    std::istream &m_stream;
    std::string_view m_filename;
    size_t m_col{0};
    size_t m_pos{0};
    size_t m_line{1};
  };

} // namespace secc

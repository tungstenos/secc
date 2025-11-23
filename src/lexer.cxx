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

#include <cctype>
#include <concepts>
#include <generator>
#include <ios>
#include <istream>
#include <string>
#include <string_view>

#include "secc/lexer.hxx"
#include "secc/source_location.hxx"

namespace
{

  constexpr inline bool
  is_symbol (char c)
  {
    /* Symbols (any string not enclosed in double quotes) must only contain
       alphanumeric [a-z A-Z] [0-9] characters plus the following special
       characters: \.@=/-_$%@+!|&ˆ:  */
    return (std::isalnum (static_cast<unsigned char> (c))
            || c == '\\' || c == '.' || c == '@' || c == '=' || c == '/'
            || c == '-' || c == '_' || c == '$' || c == '%' || c == '+'
            || c == '!' || c == '|' || c == '&' || c == '^' || c == ':');
  }

} // anonymous namespace

using namespace secc;

lexer::lexer (std::istream &stream, std::string_view filename)
  : m_stream{stream}, m_filename{filename}
{
  m_stream.unsetf (std::ios::skipws);
}

std::generator<token &&>
lexer::lex ()
{
  while (!is_at_end ())
    {
      skip_whitespace ();

      if (is_at_end ())
        {
          break;
        }

      /* Comments start with a semicolon ';' and end when a new line is started.  */
      if (peek () == ';')
        {
          skip_comment ();
          continue;
        }

      token tok = next_token ();
      co_yield std::move (tok);

      if (tok.t == token::type::INVALID)
        {
          break;
        }
    }

  co_yield token{token::type::END_OF_FILE, "", current_location ()};
}

bool
lexer::is_at_end () const
{
  return (m_stream.eof ()
          || m_stream.peek () == std::char_traits<char>::eof ());
}

char
lexer::peek () const
{
  return m_stream.peek ();
}

char
lexer::advance ()
{
  char c = m_stream.get ();
  if (c == '\n')
    {
      ++m_line;
      m_col = 0;
    }
  else
    {
      ++m_col;
    }

  ++m_pos;
  return c;
}

template <typename Predicate>
void
lexer::skip_while (Predicate p)
  requires std::invocable<Predicate, char>
{
  while (!is_at_end ()
         && p (peek ()))
    {
      advance ();
    }
}

void
lexer::skip_whitespace ()
{
  skip_while ([] (char c)
    {
      return std::isspace (static_cast<unsigned char> (c));
    });
}

void
lexer::skip_comment ()
{
  skip_while ([] (char c)
    {
      return (c != '\n');
    });
}

inline source_location
lexer::current_location () const
{
  return source_location{m_line, m_col, m_filename};
}

token
lexer::next_token ()
{
  source_location loc = current_location ();
  char c = peek ();
  if (c == '(')
    {
      advance ();
      return token{token::type::LPAREN, "(", loc};
    }
  if (c == ')')
    {
      advance ();
      return token{token::type::RPAREN, ")", loc};
    }
  if (c == '"')
    {
      return read_string (loc);
    }
  if (std::isdigit (static_cast<unsigned char> (c)))
    {
      return read_number (loc);
    }
  if (is_symbol (c))
    {
      return read_symbol (loc);
    }

  /* error  */
  advance ();
  return token{token::type::INVALID, std::string (1, c), loc};
}

token
lexer::read_string (source_location loc)
{
  /* TODO: is it worth it to first iterate over the entire string and compute
     the length, then backtrack?  It means we can allocate in one go,
     allowing us to avoid constant reallocs.  */
  std::string str;

  /* consume opening "  */
  advance ();

  while (!is_at_end ()
         && peek () != '"')
    {
      str += advance ();
    }

  if (is_at_end ())
    {
      /* string literal was unterminated.  */
      return token
        {
          token::type::INVALID,
          "wtf do I actually want to return here?",
          loc
        };
    }

  /* consume closing "  */
  advance ();
  return token
    {
      token::type::STRING,
      str,
      loc
    };
}

token
lexer::read_number (source_location loc)
{
  /* TODO: same alloc question as read_string ().  */
  std::string num;

  /* hex digits  */
  if (peek () == '0'
      && !is_at_end ())
    {
      num += advance ();
      if (!is_at_end ()
          && (peek () == 'x'
              || peek () == 'X'))
        {
          num += advance ();
          while (!is_at_end ()
                 && std::isxdigit (static_cast<unsigned char> (peek ())))
            {
              num += advance ();
            }
          return token
            {
              token::type::NUMBER,
              num,
              loc
            };
        }
    }

  /* decimal/octal  */
  while (!is_at_end ()
         && std::isdigit (static_cast<unsigned char> (peek ())))
    {
      num += advance ();
    }

  /* ip addresses/ranges  */
  while (!is_at_end ()
         && (peek () == '.'
             || peek () == ':'))
    {
      num += advance ();
      while (!is_at_end ()
             && (std::isxdigit (static_cast<unsigned char> (peek ()))
                 || std::isdigit (static_cast<unsigned char> (peek ()))))
        {
          num += advance ();
        }
    }

  return token
    {
      token::type::NUMBER,
      num,
      loc
    };
}

token
lexer::read_symbol (source_location loc)
{
  /* TODO: same alloc question as read_string ().  */
  std::string sym;

  while (!is_at_end ()
         && is_symbol (peek ()))
    {
      sym += advance ();
    }

  return token{token::type::SYMBOL, sym, loc};
}

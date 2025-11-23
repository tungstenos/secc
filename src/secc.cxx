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

#include <fstream>
#include <print>

#include "secc/lexer.hxx"

int
main (int argc, char **argv)
{
  using namespace secc;

  if (argc != 2)
    {
      return 1;
    }

  std::ifstream stream{argv[1]};
  if (!stream)
    {
      return 1;
    }

  lexer l{stream, argv[1]};

  for (const auto &&token : l.lex ())
    {
      std::println ("token='{}' line={} col={}",
                    token.lexeme, token.loc.line, token.loc.col);
    }

  return 0;
}

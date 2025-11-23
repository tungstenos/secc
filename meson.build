# secc - SELinux Compiler Collection
# Copyright (C) 2025  Rahul Sandhu <nvraxn@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License, as published by
# the Free Software Foundation, either version 3 of the License, or (at your
# option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

project('secc', 'cpp',
  default_options: [
    'warning_level=3',
    'cpp_std=gnu++23',
    'b_ndebug=if-release'
  ],
  license: 'GPL-3.0-or-later',
  license_files: 'COPYING',
  version: '0.1'
)

secc = executable(
  'secc',
  'src/secc.cxx',
  'src/lexer.cxx',
  include_directories: include_directories('include'),
  install: true
)
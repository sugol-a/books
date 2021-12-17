# Copyright (C) 2021  Alister Sanders

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

project('books', 'cpp',
        version : '0.2.2',
        default_options : ['warning_level=3', 'cpp_std=c++17', 'c_std=c11'])

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language: 'cpp')
endif

deps = [
  dependency('opencv4'),
  dependency('gtkmm-4.0'),
  dependency('cairomm-1.16'),
  dependency('threads')
]

inc = include_directories('src')

gnome = import('gnome')

ui_res = gnome.compile_resources(
  'ui-resources',
  'data/ui-resources.xml',
  source_dir: 'data',
  c_name: 'ui_res')

sources = [ ui_res ]

subdir('src')

if host_machine.system() == 'windows' or host_machine.system() == 'cygwin'
  windows = import('windows')
  sources += [ windows.compile_resources('data/win32/books.rc') ]
endif

executable('books',
           sources,
           install : true,
           dependencies: deps,
	         include_directories: inc,
           gui_app: true)

project('books', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3', 'cpp_std=c++17'])

deps = [
  dependency('opencv4'),
  dependency('gtkmm-4.0'),
  dependency('cairomm-1.0')
]

inc = include_directories('.')

gnome = import('gnome')

ui_res = gnome.compile_resources(
  'ui-resources',
  'assets/ui-resources.xml',
  source_dir: 'assets',
  c_name: 'ui_res')

sources = [ 'books.cpp',
               'crop.cpp',
               'imageloader.cpp',
               'mainwindow.cpp',
               'croppreview.cpp',
               'filter/filter.cpp',
               'filter/filterchain.cpp',
               'filter/bgr2grey.cpp',
               'filter/medianblur.cpp',
               'filter/normalise.cpp',
               'filter/canny.cpp',
               ui_res ]

executable('books',
           sources,
           install : true,
           dependencies: deps,
	   include_directories: inc)

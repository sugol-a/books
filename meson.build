project('books', 'cpp',
        version : '0.2.1',
        default_options : ['warning_level=3', 'cpp_std=c++17', 'c_std=c11'])

if get_option('buildtype').startswith('debug')
  add_project_arguments('-DDEBUG', language: 'cpp')
endif

deps = [
  dependency('opencv4'),
  dependency('gtkmm-4.0'),
  dependency('cairomm-1.0'),
  dependency('threads')
]

inc = include_directories('.')

gnome = import('gnome')

ui_res = gnome.compile_resources(
  'ui-resources',
  'assets/ui-resources.xml',
  source_dir: 'assets',
  c_name: 'ui_res')

sources = [ 'books.cpp',
            'mainwindow.cpp',
            'progresswindow.cpp',
            'imagepreview.cpp',
            'imageloader.cpp',
            'imageloaderpool.cpp',
            'imagedata.cpp',
            'imageexporter.cpp',
            'imageexporterpool.cpp',
            'workerpool.cpp',
            'imagestore.cpp',
            'featuredetector.cpp',
            'featuredetectorpool.cpp',
            'fitnessmetrics.cpp',
            'util/box.cpp',
            ui_res ]

if build_machine.system() == 'windows' or build_machine.system() == 'cygwin'
  windows = import('windows')
  sources += [ windows.compile_resources('assets/win32/books.rc') ]
endif

executable('books',
           sources,
           install : true,
           dependencies: deps,
	         include_directories: inc,
           gui_app: true)

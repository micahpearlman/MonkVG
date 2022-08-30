# SPDX-License-Identifier: MIT
# Copyright © 2021 Intel Corporation

project(
  'glu',
  ['c', 'cpp'],
  version : '9.0.2',
  meson_version : '>= 0.52.0',
  license : 'SGI-B-1.1 AND SGI-B-2.0 AND MIT',
  default_options : ['default_library=both', 'b_ndebug=if-release']
)

if get_option('debug')
  add_project_arguments('-DDEBUG', language : ['c', 'cpp'])
endif

gl_provider = get_option('gl_provider')
if gl_provider == 'glvnd'
  gl_provider = 'opengl'
endif
dep_gl = dependency(gl_provider)

inc_include = include_directories('include')

subdir('src')

install_headers(
  'include/GL/glu.h',
  subdir : 'GL',
)

pkgconf = import('pkgconfig')
pkgconf.generate(
  libglu,
  name : 'glu',
  description : 'Mesa OpenGL Utility Library',
  requires: 'opengl'
)

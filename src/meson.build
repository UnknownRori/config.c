src = files('rconfig.c')
inc = include_directories('.')

rconfig_lib = library(meson.project_name(), src, include_directories: [inc, rstb_inc])
rconfig_dep = declare_dependency(
  link_with: rconfig_lib,
  include_directories: [inc],
)

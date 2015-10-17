add_executable(wator wator.cpp)

target_link_libraries( wator
  wator_static
  boost_program_options
  opencv_core
)

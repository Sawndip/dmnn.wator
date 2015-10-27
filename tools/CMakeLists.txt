add_executable(wator wator.cpp)

target_link_libraries( wator
  wator_static
  boost_program_options
  boost_filesystem
  boost_system
  boost_log_setup-mt
  boost_log-mt
  opencv_imgproc
  opencv_highgui
  opencv_core
)

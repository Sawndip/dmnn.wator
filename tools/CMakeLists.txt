add_executable(wator wator.cpp)

target_link_libraries( wator
  boost_log_setup
  boost_log
  wator_static
#  boost_program_options
  boost_filesystem
  boost_system
  boost_log_setup
  boost_log
  boost_program_options
  opencv_imgproc
  opencv_highgui
  opencv_core
)


add_executable(shotThough shotThough.cpp)


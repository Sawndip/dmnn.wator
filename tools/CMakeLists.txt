add_executable(wator wator.cpp)

target_link_libraries( wator
  wator_static
  boost_log_setup-mt
  boost_log-mt
  boost_program_options-mt
  boost_filesystem-mt
  boost_system-mt
  boost_log_setup-mt
  boost_log-mt
  boost_program_options-mt
  opencv_imgproc
  opencv_highgui
  opencv_core
)


add_executable(shotThough shotThough.cpp)


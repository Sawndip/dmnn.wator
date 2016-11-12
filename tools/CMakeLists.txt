add_executable(wator wator.cpp)

set(SUFFIX )
target_link_libraries( wator
  wator_static
  boost_log_setup${SUFFIX}
  boost_log${SUFFIX}
  boost_program_options${SUFFIX}
  boost_filesystem${SUFFIX}
  boost_system${SUFFIX}
  boost_log_setup${SUFFIX}
  boost_log${SUFFIX}
  boost_program_options${SUFFIX}
  opencv_imgproc
  opencv_highgui
  opencv_core
  pthread
)


add_executable(shotThough shotThough.cpp)


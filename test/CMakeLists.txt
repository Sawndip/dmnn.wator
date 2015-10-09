#add_definitions(-DBOOST_TEST_DYN_LINK)

file(GLOB __CXX *.cpp)

add_executable(test_wator ${__CXX})

target_link_libraries( test_wator
  boost_unit_test_framework
)

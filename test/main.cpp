#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#include <vector>
#include <boost/test/included/unit_test.hpp>
BOOST_AUTO_TEST_SUITE(MasterSuite)

BOOST_AUTO_TEST_CASE(TestFoo)
{
    BOOST_CHECK(0==0);
}

BOOST_AUTO_TEST_SUITE_END()


#define BOOST_TEST_ALTERNATIVE_INIT_API 1

#include <boost/test/unit_test.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include "test/inc/LogRedirector.h"

static LogRedirector logRedirector(getenv("TEST_LOG_OUTPUT"));


extern void reduction_test();

bool init_unit_test() {
  boost::unit_test::framework::master_test_suite().add( BOOST_TEST_CASE( &reduction_test ), 0, 12 );
  return true;
}

int main( int argc, char* argv[] )
{
  return ::boost::unit_test::unit_test_main( &init_unit_test, argc,
															argv );
} 

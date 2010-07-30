//#define BOOST_TEST_MODULE SComplexTestSuite

#define BOOST_AUTO_TEST_MAIN

#include <boost/test/included/unit_test.hpp>

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <cassert>
#include "LogRedirector.h"

static LogRedirector logRedirector(getenv("TEST_LOG_OUTPUT"));


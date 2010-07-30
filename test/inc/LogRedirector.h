#ifndef LOG_REDIRECTOR
#define LOG_REDIRECTOR

#include <fstream>
#include <boost/test/unit_test.hpp>

class LogRedirector
{
    std::ofstream out;

public:
  
  LogRedirector(const char* filename)
  {
	 if (filename) {
		out.open(filename);
		assert( out.is_open() );
		boost::unit_test::unit_test_log.set_stream(out);
    }
  }
  
};


#endif //  LOG_REDIRECTOR

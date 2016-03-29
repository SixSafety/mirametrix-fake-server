//
// mmServer.cpp
//

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

//Simple faked reading of the no eyes detected condition (with count)
std::string fake_reading()
{
  static int count = 0;
  std::ostringstream reading;

  reading << "<REC CNT=\""
          << ++count
          << "\">\r\n"
          << "<Attachment guid=\"ACBFE1C2-EF41-4B9F-B019-A3E2A16A2CD6\" LEYE_FATIGUE=\"-1.000000\" LEYE_FCONF=\"-2.000000\" REYE_FATIGUE=\"-1.000000\" REYE_FCONF=\"-2.000000\" />\r\n"
          << "</REC>\r\n";
  return (reading.str());
}

int main()
{
  try
  {
    boost::asio::io_service io_service;
    boost::system::error_code ec;
    tcp::iostream stream;

    tcp::endpoint endpoint(tcp::v4(), 4242);
    tcp::acceptor acceptor(io_service, endpoint);
 
    acceptor.accept(*stream.rdbuf(), ec);
    if(ec)
    {
      std::cerr << ec.message() << std::endl;
    }
    
    for (;;)
    {
      struct timespec foo;
      foo.tv_sec = 0;
      foo.tv_nsec = 33333333L; //30Hz in nanoseconds
      nanosleep(&foo,&foo);

      stream << fake_reading();
      if(!stream)
      {
        std::cerr << stream.error().message() << std::endl;
        break;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}

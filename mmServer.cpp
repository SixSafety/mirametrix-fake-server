//
// mmServer.cpp
//

#include <iostream>
#include <string>
#include <sstream>
#include <time.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

static int count = 0;
static float timestamp = 0.0;
#define MISSING 0
#define OPEN 1
#define CLOSED 2
static int mode = MISSING;

//Simple faked reading of the no eyes detected condition (with count)
std::string fake_missing()
{
  std::ostringstream reading;

  reading << "<REC CNT=\""
          << ++count
          << "\" TIME=\""
          << timestamp
          << "\">\r\n"
          << "<Attachment guid=\"ACBFE1C2-EF41-4B9F-B019-A3E2A16A2CD6\" LEYE_FATIGUE=\"-1.000000\" LEYE_FCONF=\"-2.000000\" REYE_FATIGUE=\"-1.000000\" REYE_FCONF=\"-2.000000\" />\r\n"
          << "</REC>\r\n";
  return (reading.str());
}

//Simple faked reading of the no eyes detected condition (with count)
std::string fake_closed()
{
  std::ostringstream reading;

  reading << "<REC CNT=\""
          << ++count
          << "\" TIME=\""
          << timestamp
          << "\">\r\n"
          << "<Attachment guid=\"ACBFE1C2-EF41-4B9F-B019-A3E2A16A2CD6\" LEYE_FATIGUE=\"0.070000\" LEYE_FCONF=\"1.000000\" REYE_FATIGUE=\"0.080000\" REYE_FCONF=\"1.000000\" />\r\n"
          << "</REC>\r\n";
  return (reading.str());
}

//Simple faked reading of the no eyes detected condition (with count)
std::string fake_open()
{
  std::ostringstream reading;

  reading << "<REC CNT=\""
          << ++count
          << "\" TIME=\""
          << timestamp
          << "\">\r\n"
          << "<Attachment guid=\"ACBFE1C2-EF41-4B9F-B019-A3E2A16A2CD6\" LEYE_FATIGUE=\"0.600000\" LEYE_FCONF=\"1.000000\" REYE_FATIGUE=\"0.500000\" REYE_FCONF=\"1.000000\" />\r\n"
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
      int lastmode;
      struct timespec foo;
      foo.tv_sec = 0;
      foo.tv_nsec = 16666667L; //60Hz in nanoseconds
      nanosleep(&foo,&foo);

      if( (count%10000) < 1500 )
        mode = OPEN;
      else if( (count%10000 < 1510) )
        mode = CLOSED;
      else if( (count%10000 < 2510) )
        mode = OPEN;
      else if( (count%10000 < 2538) )
        mode = CLOSED;
      else if( (count%10000 < 3030) )
        mode = OPEN;
      else if( (count%10000 < 3060) )
        mode = CLOSED;
      else if( (count%10000 < 3190) )
        mode = OPEN;
      else
        mode = MISSING;

      if(mode != lastmode)
      {
          std::cout << "Timestamp " << timestamp  << std::endl;
        if(mode == MISSING)
          std::cout << "Missing" << std::endl;
        else if(mode == OPEN)
          std::cout << "Open" << std::endl;
        else if(mode == CLOSED)
          std::cout << "Closed" << std::endl;
      }
      if(count > 4000)
        sleep(20);

      lastmode = mode;
      timestamp += 20;
      if(mode == MISSING)
        stream << fake_missing();
      else if(mode == OPEN)
        stream << fake_open();
      else if(mode == CLOSED)
        stream << fake_closed();

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

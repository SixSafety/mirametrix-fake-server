//
// mmServer.cpp
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <time.h>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
  if(argc != 2)
    exit(1);
  std::string logFile(argv[1]);
  std::cout << "Log File: " << logFile << std::endl;
  std::ifstream logDataFile(logFile);

  try
  {
    boost::asio::io_service io_service;
    boost::system::error_code ec;
    tcp::iostream tcpSocketStream;

    tcp::endpoint tcpEndpoint(tcp::v4(), 4242);
    tcp::acceptor tcpAcceptor(io_service, tcpEndpoint);
 
    tcpAcceptor.accept(*tcpSocketStream.rdbuf(), ec);
    if(ec)
      std::cerr << ec.message() << std::endl;
    
    for (;;)
    {
      std::string inputLine;
      std::string strTimeStamp;
      double fTimeStamp;
      static double fLastTimeStamp = 0.0f;
      double fElapsedTime;
      int i, flag;
      size_t firstQuote, lastQuote, szTime;
      struct timespec foo;

      if(!logDataFile.good())
        exit(1);

      std::getline(logDataFile,inputLine);
      if( (firstQuote = inputLine.find("TIME=")) != std::string::npos)
      {
        firstQuote += 6;
        lastQuote = inputLine.rfind('"');
        szTime = lastQuote - firstQuote;
        strTimeStamp = inputLine.substr(firstQuote,szTime);
        //std::cout << "FirstQuote:" << firstQuote << ":LastQuote:" << lastQuote << ":" << std::endl;
        fTimeStamp = stof(strTimeStamp);
        if(fTimeStamp != 0.0f)
        {
          fElapsedTime = fTimeStamp - fLastTimeStamp;
          long lElapsed = fElapsedTime * 833333333LL;
          //std::cout << "Now:" << fTimeStamp << ":Then:" << fLastTimeStamp << ":Elapsed:" << fElapsedTime
                    //<< ":Nano:" << lElapsed << std::endl;
          fLastTimeStamp = fTimeStamp;

          foo.tv_sec = 0;
          foo.tv_nsec = lElapsed;

          nanosleep(&foo,&foo);
        }
      }
      i = fLastTimeStamp;
      if(!(i%60))
      {
        if(flag == 0)
        {
          std::chrono::time_point<std::chrono::system_clock> nowtime = std::chrono::system_clock::now();
          std::time_t current_time = std::chrono::system_clock::to_time_t(nowtime);
          std::cout << "Time: " << std::ctime(&current_time) << "Last Stamp: " << fLastTimeStamp << std::endl;
          flag = 1;
        }
      }
      else
        flag = 0;

      tcpSocketStream << inputLine << std::endl;

      if(!tcpSocketStream)
      {
        std::cerr << tcpSocketStream.error().message() << std::endl;
        break;
      }
    }
  }
  catch (std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
  logDataFile.close();
  return 0;
}

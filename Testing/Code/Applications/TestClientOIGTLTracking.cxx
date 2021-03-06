/*=============================================================================
  NiftyLink:  A software library to facilitate communication over OpenIGTLink.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See LICENSE.txt in the top level directory for details.
=============================================================================*/
#include <NiftyLinkUtils.h>
#include <NiftyLinkMessageContainer.h>
#include <NiftyLinkTrackingDataMessageHelpers.h>

#include <iostream>

#include <igtlTrackingDataMessage.h>
#include <igtlStringMessage.h>
#include <igtlClientSocket.h>

int main(int argc, char* argv[])
{
  if (argc != 6)
  {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <host> <port> <fps> <channels> <total>"    << std::endl;
    std::cerr << "    <host>       : Host." << std::endl;
    std::cerr << "    <port>       : Port #." << std::endl;
    std::cerr << "    <fps>        : Frequency (fps) to send image." << std::endl;
    std::cerr << "    <channels>   : Number of channels (tracking matrices per message)." << std::endl;
    std::cerr << "    <total>      : Total number of messages." << std::endl;
    exit(0);
  }

  QString hostName      = argv[1];
  int    port           = atoi(argv[2]);
  int    fps            = atoi(argv[3]);
  int    channels       = atoi(argv[4]);
  int    totalMessages  = atoi(argv[5]);

  std::cout << "hostName=" << hostName.toStdString() << std::endl;
  std::cout << "port=" << port << std::endl;
  std::cout << "fps=" << fps << std::endl;
  std::cout << "channels=" << channels << std::endl;
  std::cout << "totalMessages=" << totalMessages << std::endl;

  // This is to make sure we have the best possible system timer.
#if defined(_WIN32) && !defined(__CYGWIN__)
  niftk::InitializeWinTimers();
#endif

  // Setup socket.
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostName.toStdString().c_str(), port);
  if (r != 0)
  {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
  }

  int numberMessagesSent = 0;
  int numberMessagesRequired = totalMessages;

  igtlUint64 nanosecondsBetweenMessages = 1000000000 / fps;

  igtl::TimeStamp::Pointer timeCreated = igtl::TimeStamp::New();
  timeCreated->GetTime();

  igtl::TimeStamp::Pointer timeNow = igtl::TimeStamp::New();
  timeNow->GetTime();

  igtl::TimeStamp::Pointer timeStarted = igtl::TimeStamp::New();
  timeStarted->SetTimeInNanoseconds(timeNow->GetTimeStampInNanoseconds());

  // This will occupy a lot of CPU, but we have multi-cpu machines, so assumed to be no problem.
  while(numberMessagesSent < numberMessagesRequired)
  {

    timeNow->GetTime();
    igtlUint64 diff = niftk::GetDifferenceInNanoSeconds(timeNow, timeStarted);

    if (diff >= nanosecondsBetweenMessages*numberMessagesSent)
    {
      niftk::NiftyLinkMessageContainer::Pointer m = niftk::CreateTrackingDataMessageWithRandomData(timeCreated, channels);
      r = socket->Send(m->GetMessage()->GetPackPointer(), m->GetMessage()->GetPackSize());
      if (r == 0)
      {
        std::cerr << "Failed to send message." << std::endl;
        exit(0);
      }
      numberMessagesSent++;
    }
  }

  igtl::StringMessage::Pointer msg = igtl::StringMessage::New();
  msg->SetDeviceName("TestClientOIGTLTracking");
  msg->SetString("STATS");
  msg->Pack();

  r = socket->Send(msg->GetPackPointer(), msg->GetPackSize());
  if (r == 0)
  {
    std::cerr << "Failed to send stats request." << std::endl;
    exit(0);
  }
}



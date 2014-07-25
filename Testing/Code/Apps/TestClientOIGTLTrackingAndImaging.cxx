/*=============================================================================
  NiftyLink:  A software library to facilitate communication over OpenIGTLink.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See LICENSE.txt in the top level directory for details.
=============================================================================*/
#include <NiftyLinkUtils.h>
#include <NiftyLinkImageMessageHelpers.h>

#include <iostream>

#include <igtlImageMessage.h>
#include <igtlStringMessage.h>
#include <igtlImageMessage.h>
#include <igtlTrackingDataMessage.h>
#include <igtlClientSocket.h>

int main(int argc, char* argv[])
{
  if (argc != 7)
  {
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0] << " <host> <port> <fps> <total> <filename>"    << std::endl;
    std::cerr << "    <host>       : Host." << std::endl;
    std::cerr << "    <port>       : Port #." << std::endl;
    std::cerr << "    <fps>        : Frequency (fps) to send image." << std::endl;
    std::cerr << "    <channels>   : Number of channels (tracking matrices per message)." << std::endl;
    std::cerr << "    <total>      : Total number of messages." << std::endl;
    std::cerr << "    <filename>   : filename of image (.pgm)." << std::endl;
    exit(0);
  }

  QString hostName      = argv[1];
  int    port           = atoi(argv[2]);
  int    fps            = atoi(argv[3]);
  int    channels       = atoi(argv[4]);
  int    totalMessages  = atoi(argv[5]);
  QString fileName      = argv[6];

  std::cout << "hostName=" << hostName.toStdString() << std::endl;
  std::cout << "port=" << port << std::endl;
  std::cout << "fps=" << fps << std::endl;
  std::cout << "channels=" << channels << std::endl;
  std::cout << "totalMessages=" << totalMessages << std::endl;
  std::cout << "fileName=" << fileName.toStdString() << std::endl;

  // Load Image.
  igtl::ImageMessage::Pointer imageMessage = igtl::ImageMessage::New();
  niftk::LoadImage(fileName, imageMessage);

  // Setup socket.
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostName.toStdString().c_str(), port);
  if (r != 0)
  {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
  }

  int imgSize[3];
  imageMessage->GetDimensions(imgSize);

  igtl::ImageMessage::Pointer localImage = igtl::ImageMessage::New();
  localImage->InitPack();
  localImage->SetDimensions(imgSize[0], imgSize[1], imgSize[2]);
  localImage->SetScalarType(igtl::ImageMessage::TYPE_UINT8);
  localImage->AllocateScalars();

  igtl::TimeStamp::Pointer timeLastImageMessage = igtl::TimeStamp::New();
  timeLastImageMessage->GetTime();

  igtl::TimeStamp::Pointer timeLastTrackingMessage = igtl::TimeStamp::New();
  timeLastTrackingMessage->GetTime();

  igtl::TimeStamp::Pointer timeNow = igtl::TimeStamp::New();
  timeNow->GetTime();

  int nanosecondsBetweenImageMessages = 1000000000 / fps;
  int nanosecondsBetweenTrackingMessages = 1000000000 / 100;  // i.e. tracking rate constant.

  int numberMessagesSent = 0;
  int numberMessagesRequired = totalMessages;

  while (numberMessagesSent < numberMessagesRequired)
  {
    timeNow->GetTime();

    // Do tracking first.
    if (niftk::GetDifferenceInNanoSeconds(timeNow, timeLastTrackingMessage) > nanosecondsBetweenTrackingMessages)
    {
      igtl::TimeStamp::Pointer timeCreated = igtl::TimeStamp::New();
      timeCreated->GetTime();

      niftk::NiftyLinkMessageContainer::Pointer m = niftk::CreateTestTrackingDataMessage(channels);
      r = socket->Send(m->GetMessage()->GetPackPointer(), m->GetMessage()->GetPackSize());
      if (r == 0)
      {
        std::cerr << "Failed to send message." << std::endl;
        exit(0);
      }
      timeLastTrackingMessage->SetTimeInNanoseconds(timeNow->GetTimeStampInNanoseconds());
    }

    // Do imaging second.
    if (niftk::GetDifferenceInNanoSeconds(timeNow, timeLastImageMessage) > nanosecondsBetweenImageMessages)
    {
      igtl::TimeStamp::Pointer timeCreated = igtl::TimeStamp::New();
      timeCreated->GetTime();
      localImage->SetTimeStamp(timeCreated);

      memcpy(localImage->GetScalarPointer(), imageMessage->GetScalarPointer(), imgSize[0]*imgSize[1]);
      localImage->Pack();

      r = socket->Send(localImage->GetPackPointer(), localImage->GetPackSize());
      if (r == 0)
      {
        std::cerr << "Failed to send message." << std::endl;
        exit(0);
      }

      numberMessagesSent++;
      timeLastImageMessage->SetTimeInNanoseconds(timeNow->GetTimeStampInNanoseconds());
    }
  }

  igtl::StringMessage::Pointer msg = igtl::StringMessage::New();
  msg->SetDeviceName("TestClientOIGTLTrackingAndImaging");
  msg->SetString("STATS");
  msg->Pack();

  r = socket->Send(msg->GetPackPointer(), msg->GetPackSize());
  if (r == 0)
  {
    std::cerr << "Failed to send stats request." << std::endl;
    exit(0);
  }
}



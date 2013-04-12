/*=============================================================================
NiftyLink: A software library to facilitate communication over OpenIGTLink.

Copyright (c) University College London (UCL). All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

See LICENSE.txt in the top level directory for details.
=============================================================================*/

#include "NiftyLinkMessage.h"

#include "QsLog.h"
#include "QsLogDest.h"
#include "NiftyLinkUtils.h"

//-----------------------------------------------------------------------------
NiftyLinkMessage::NiftyLinkMessage(void)
{
  m_TimeCreated = igtl::TimeStamp::New();
  m_TimeCreated->Update();
  m_SenderPort = -1;

  m_SenderHostName = QString("localhost");
  m_Id = GetTimeInNanoSeconds(m_TimeCreated);

  //m_TimeCreated = NULL;
  m_Resolution = 0;
  m_Processed = 0;
  m_OwnerName.append("!");
}


//-----------------------------------------------------------------------------
NiftyLinkMessage::~NiftyLinkMessage(void)
{
  //QLOG_INFO() <<"Message destructor (Base class): "<<m_OwnerName <<m_Id;

  m_Message.operator = (NULL);
  m_TimeReceived.operator = (NULL);
  m_TimeCreated.operator = (NULL);
}


//-----------------------------------------------------------------------------
NiftyLinkMessage::NiftyLinkMessage(const NiftyLinkMessage &other)
  : QSharedData(other)
{
  // Copy constructor
  m_Message.operator = (other.m_Message);
  m_TimeReceived.operator = (other.m_TimeReceived);

  m_SenderHostName = other.m_SenderHostName;
  m_MessageType = other.m_MessageType;
  m_SenderPort = other.m_SenderPort;

  m_Resolution = other.m_Resolution;
  m_Id = other.m_Id;

  m_OwnerName = other.m_OwnerName;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::SetMessagePointer(igtl::MessageBase::Pointer mp)
{
  // Embeds the OpenIGTLink message pointer into the message
  m_Message.operator = (mp);
  m_MessageType = QString(m_Message->GetDeviceType());
  m_Message->GetTimeStamp(m_TimeCreated);
  m_SenderHostName = QString(m_Message->GetDeviceName());
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::GetMessagePointer(igtl::MessageBase::Pointer &mp)
{
  mp.operator = (m_Message);
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::SetTimeReceived(igtl::TimeStamp::Pointer ts)
{
  m_TimeReceived.operator = (ts);
}


//-----------------------------------------------------------------------------
igtl::TimeStamp::Pointer NiftyLinkMessage::GetTimeReceived(void)
{
  return m_TimeReceived;
}


//-----------------------------------------------------------------------------
igtl::TimeStamp::Pointer NiftyLinkMessage::GetTimeCreated(void)
{
  return m_TimeCreated;
}


//-----------------------------------------------------------------------------
igtlUint64 NiftyLinkMessage::GetId(void)
{
  return m_Id;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::ChangeHostName(QString hname)
{
  if (m_Message.IsNull())
  {
    return;
  }

  m_SenderHostName = hname;

  m_Message->Unpack();
  m_Message->SetDeviceName(m_SenderHostName.toStdString().c_str());
  m_Message->Pack();
}


//-----------------------------------------------------------------------------
QString NiftyLinkMessage::GetHostName(void)
{
  if (m_Message.IsNull())
  {
    return QString();
  }

  m_Message->Unpack();
  m_SenderHostName = m_Message->GetDeviceName();
  m_Message->Pack();

  return m_SenderHostName;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::SetPort(int port)
{
  m_SenderPort = port;
}


//-----------------------------------------------------------------------------
int NiftyLinkMessage::GetPort(void)
{
  return m_SenderPort;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::ChangeMessageType(QString type)
{
  m_MessageType = type;
}


//-----------------------------------------------------------------------------
QString NiftyLinkMessage::GetMessageType(void)
{
  return m_MessageType;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::SetResolution(igtlUint64 res)
{
  // This is to set the streaming frequency
  m_Resolution = res;

  if (m_Message.IsNull())
  {
    return;
  }

  QString className(m_Message->GetNameOfClass());

  if (!className.contains("Start"))
  {
    return;
  }

  m_Message->Unpack();

  if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTransformMessage") == 0)
  {
    igtl::StartTransformMessage::Pointer pointer = static_cast<igtl::StartTransformMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTrajectoryMessage") == 0)
  {
    igtl::StartTrajectoryMessage::Pointer pointer = static_cast<igtl::StartTrajectoryMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTrackingDataMessage") == 0)
  {
    igtl::StartTrackingDataMessage::Pointer pointer = static_cast<igtl::StartTrackingDataMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartStringMessage") == 0)
  {
    igtl::StartStringMessage::Pointer pointer = static_cast<igtl::StartStringMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartStatusMessage") == 0)
  {
    igtl::StartStatusMessage::Pointer pointer = static_cast<igtl::StartStatusMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartQuaternionTrackingDataMessage") == 0)
  {
    igtl::StartQuaternionTrackingDataMessage::Pointer pointer = static_cast<igtl::StartQuaternionTrackingDataMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartPositionMessage") == 0)
  {
    igtl::StartPositionMessage::Pointer pointer = static_cast<igtl::StartPositionMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartPointMessage") == 0)
  {
    igtl::StartPointMessage::Pointer pointer = static_cast<igtl::StartPointMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartImageMessage") == 0)
  {
    igtl::StartImageMessage::Pointer pointer = static_cast<igtl::StartImageMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartBindMessage") == 0)
  {
    igtl::StartBindMessage::Pointer pointer = static_cast<igtl::StartBindMessage *>(m_Message.GetPointer());
    pointer->SetResolution(res);
  }

  m_Message->Pack();
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::GetResolution(igtlUint64 &res)
{
  if (m_Message.IsNull())
  {
    return;
  }

  QString className(m_Message->GetNameOfClass());

  if (!className.contains("Start"))
  {
    return;
  }

  m_Message->Unpack();

  if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTransformMessage") == 0)
  {
    igtl::StartTransformMessage::Pointer pointer = static_cast<igtl::StartTransformMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTrajectoryMessage") == 0)
  {
    igtl::StartTrajectoryMessage::Pointer pointer = static_cast<igtl::StartTrajectoryMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartTrackingDataMessage") == 0)
  {
    igtl::StartTrackingDataMessage::Pointer pointer = static_cast<igtl::StartTrackingDataMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartStringMessage") == 0)
  {
    igtl::StartStringMessage::Pointer pointer = static_cast<igtl::StartStringMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartStatusMessage") == 0)
  {
    igtl::StartStatusMessage::Pointer pointer = static_cast<igtl::StartStatusMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartQuaternionTrackingDataMessage") == 0)
  {
    igtl::StartQuaternionTrackingDataMessage::Pointer pointer = static_cast<igtl::StartQuaternionTrackingDataMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartPositionMessage") == 0)
  {
    igtl::StartPositionMessage::Pointer pointer = static_cast<igtl::StartPositionMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartPointMessage") == 0)
  {
    igtl::StartPointMessage::Pointer pointer = static_cast<igtl::StartPointMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartImageMessage") == 0)
  {
    igtl::StartImageMessage::Pointer pointer = static_cast<igtl::StartImageMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }
  else if (strcmp(m_Message->GetNameOfClass(), "igtl::StartBindMessage") == 0)
  {
    igtl::StartBindMessage::Pointer pointer = static_cast<igtl::StartBindMessage *>(m_Message.GetPointer());
    m_Resolution = pointer->GetResolution();
  }

  m_Message->Pack();

  res = m_Resolution;
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::Update(QString hostname, igtl::TimeStamp::Pointer ts)
{
  // Updates timestamp and hostname

  if (m_Message.IsNull())
  {
    return;
  }

  m_TimeCreated.operator = (ts);
  m_SenderHostName = hostname;

  m_Message->Unpack();
  m_Message->SetTimeStamp(ts);
  m_Message->SetDeviceName(m_SenderHostName.toStdString().c_str());
  m_Message->Pack();
}


//-----------------------------------------------------------------------------
void NiftyLinkMessage::Update(QString hostname)
{
  // Updates timestamp and hostname

  if (m_Message.IsNull())
  {
    return;
  }

  igtl::TimeStamp::Pointer ts;
  ts = igtl::TimeStamp::New();
  ts->Update();

  m_TimeCreated.operator = (ts);
  m_SenderHostName = hostname;

  m_Id = GetTimeInNanoSeconds(ts);

  m_Message->Unpack();
  m_Message->SetTimeStamp(ts);
  m_Message->SetDeviceName(m_SenderHostName.toStdString().c_str());
  m_Message->Pack();
}

//-----------------------------------------------------------------------------
void NiftyLinkMessage::SetOwnerName(QString str)
{
  if (!m_OwnerName.isEmpty() && !m_OwnerName.isNull())
  {
    m_OwnerName.clear();
  }
  m_OwnerName.append(str);
}

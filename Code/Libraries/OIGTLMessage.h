/*=============================================================================
  NiftyLink:  A software library to facilitate communication over OpenIGTLink.

  Copyright (c) University College London (UCL). All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

  See LICENSE.txt in the top level directory for details.
=============================================================================*/

#ifndef OIGTLMESSAGE_H
#define OIGTLMESSAGE_H

// Qt related issues
#include <QtCore>
#include <QString>
#include <QObject>
#include <QSharedData>
#include <QExplicitlySharedDataPointer>

// OpenIGTLink message types
#include "igtlBindMessage.h"
#include "igtlColorTableMessage.h"
#include "igtlImageMessage.h"
#include "igtlImageMetaMessage.h"
#include "igtlLabelMetaMessage.h"
#include "igtlPointMessage.h"
#include "igtlPositionMessage.h"
#include "igtlQuaternionTrackingDataMessage.h"
#include "igtlSensorMessage.h"
#include "igtlStatusMessage.h"
#include "igtlStringMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlTrajectoryMessage.h"
#include "igtlTransformMessage.h"

// OpenIGTLink utils
#include "igtlOSUtil.h"
#include "igtlTimeStamp.h"

#include "NiftyLinkCommonWin32ExportHeader.h"

/**
 * \class OIGTLMessage
 * \brief Base class for wrapping OpenIGTLink message types, but it can be instanciated on its own too.
 * 
 * OIGTLMessage is the base class for all OIGTL message types. The aim is to wrap OpenIGTLink
 * functionality into lightweight and easy to use QT-based message format. The OIGTLMessage is
 * based on QSharedData, used along with QExplicitlySharedDataPointer which is basically the Qt
 * version of the smartpointers. OIGTLMessage has all the required functions and data members to
 * easily create, destroy, initialize, identify and tag message data.
 */
class NIFTYLINKCOMMON_WINEXPORT OIGTLMessage : public QSharedData
{

public:

	// Typedefs that are common in all message types
	typedef OIGTLMessage				                       Self;
	typedef QExplicitlySharedDataPointer<Self>        Pointer;
	typedef QExplicitlySharedDataPointer<const Self>  ConstPointer;

  /// \brief Basic constructor which generates a timestamp and derives the message ID from it.
	OIGTLMessage(void);

  /// \brief Basic destructor
	~OIGTLMessage(void);

  /// \brief Basic copy constructor required for this data type
  OIGTLMessage(const OIGTLMessage &other); 

  /// \brief This function embeds an OpenIGTLink message pointer 
	void setMessagePointer(igtl::MessageBase::Pointer mp);

  /// \brief This function returns the embedded OpenIGTLink message pointer
	void getMessagePointer(igtl::MessageBase::Pointer &mp);

  /// \brief This functions sets the sender's host name
	void changeHostName(QString hname);

  /// \brief This functions returns the sender's host name
	QString getHostName(void);

  /// \brief Changes the message tag stored within the message (USE WITH CARE)
	void changeMessageType(QString type);

  /// \brief This function returns the currently set message tag
	QString getMessageType(void);

  /// \brief This function is the set the port number on which the message was received
	void setPort(int port);

  /// \brief This function returns the port number
	int getPort(void);

  /// \brief This function is for setting the timestamp when the message is received
	void setTimeReceived(igtl::TimeStamp::Pointer ts);

  /// \brief Returns the time when the message was received
	igtl::TimeStamp::Pointer getTimeReceived(void);

  /// \brief Returns the time when the message was created
	igtl::TimeStamp::Pointer getTimeCreated(void);

  /// \brief Returns the message ID
	igtlUint64 getId(void);

	//STT related functions
  /// \brief Sets the frequency of the stream (used in STT_ messages)
	void setResolution(igtlUint64 res);

  /// \brief Returns the frequency of the stream (used in STT_ messages)
	void getResolution(igtlUint64 &res);

  /// \brief Function base for initilizing the message with test data (defined in NiftyLink Utils)
	virtual void initializeWithTestData(void) {}

  /// \brief Function base for initilizing the message with random data (defined in NiftyLink Utils)
  virtual void initializeWithRandomData(void) {}

  /// \brief Function to update the timestamp and host name after changes
	void update(QString hostname, igtl::TimeStamp::Pointer ts);

  /// \brief Function to update the host name after changes
  void update(QString hostname);

  /// \brief Function set message processed and to check status
  inline void setProcessed(bool p) { m_processed = p; };

  /// \brief Function to check if a message was processed
  bool getProcessed() { return m_processed; };

  /// \brief Set the name of the owner class as the message is passed back and forth
  void setOwnerName(QString str); 

  /// \brief Method to check who owns the message at the moment
  QString getOwnerName(void) { return m_ownerName; }

protected:

	QString			                m_messageType;
	igtl::MessageBase::Pointer  m_message;
	igtl::TimeStamp::Pointer    m_timeReceived;
	igtl::TimeStamp::Pointer    m_timeCreated;

	QString			                m_senderHostName;
	int					                m_senderPort;

	igtlUint64	                m_id;
	igtlUint64	                m_resolution;

  bool                        m_processed;
  QString                     m_ownerName;
};

Q_DECLARE_METATYPE(OIGTLMessage::Pointer);

#endif

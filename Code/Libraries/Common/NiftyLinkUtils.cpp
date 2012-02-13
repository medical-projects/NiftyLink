/*=============================================================================

 NiftyLink:  A software library to facilitate communication over OpenIGTLink.

             http://cmic.cs.ucl.ac.uk/
             http://www.ucl.ac.uk/

 Copyright (c) UCL : See LICENSE.txt in the top level directory for details.

 Last Changed      : $Date: 2010-05-25 17:02:50 +0100 (Tue, 25 May 2010) $
 Revision          : $Revision: 3300 $
 Last modified by  : $Author: mjc $

 Original author   : m.clarkson@ucl.ac.uk

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.

 ============================================================================*/

#include "NiftyLinkUtils.h"

bool validateIp(const QString &inputIP)
{
	QStringList nums = inputIP.split(".");

	if (nums.count() !=4)
		return false;

	for (int i = 0; i < 4; i++)
	{
		bool ok = false;
		int frag = nums.at(i).toInt(&ok, 10);
		
		if (!ok) return false;
		else if (frag < 0 || frag > 255) return false;
	}
	
	return true;
}

QString resolveHostName(const QString &input)
{
	if (validateIp(input))
		return input;

	QHostInfo info = QHostInfo::fromName(input);
	QList<QHostAddress> addresses = info.addresses();

	for (int i=0; i< addresses.count(); i++)
	{
		if( validateIp(addresses.at(i).toString()) == true )
		{
			return addresses.at(i).toString();
		}
	}
	
	return QString("UNKNOWN");

}

QString getLocalHostAddress(void)
{
    QNetworkConfigurationManager mgr;
    QNetworkConfiguration nconfig = mgr.defaultConfiguration();
    QNetworkSession session ( nconfig );
    QNetworkInterface ninter = session.interface();

    // this provides more than one ip addresses
    QList<QNetworkAddressEntry> laddr = ninter.addressEntries();
    for ( QList<QNetworkAddressEntry> ::const_iterator it = laddr.begin(); it != laddr.end(); ++it )
    {
        QString ipstr = it->ip().toString();
        if (validateIp(ipstr))
        {
            // Need to return the one which is valid and not 'localhost'
            if (ipstr != QString("127.0.0.1"))
                return ipstr;
        }
    }

    return QString("UNKNOWN");
}

int NiftyLinkDummyFunction1()
{
  return 0;
}

void CreateRandomTransformMatrix(igtl::Matrix4x4& matrix)
{
  float position[3];
  float orientation[4];

  // random position
  static float phi = 0.0;
  position[0] = 50.0 * cos(phi);
  position[1] = 50.0 * sin(phi);
  position[2] = 50.0 * cos(phi);
  phi = phi + 0.2;

  // random orientation
  static float theta = 0.0;
  orientation[0]=0.0;
  orientation[1]=0.6666666666*cos(theta);
  orientation[2]=0.577350269189626;
  orientation[3]=0.6666666666*sin(theta);
  theta = theta + 0.1;

  //igtl::Matrix4x4 matrix;
  igtl::QuaternionToMatrix(orientation, matrix);

  matrix[0][3] = position[0];
  matrix[1][3] = position[1];
  matrix[2][3] = position[2];
  
  //igtl::PrintMatrix(matrix);
}

bool CompareMsgData(OIGTLMessage::Pointer &msg1, OIGTLMessage::Pointer &msg2)
{
	igtl::MessageBase::Pointer basePointer1;
	msg1->getMessagePointer(basePointer1);
	igtl::MessageBase::Pointer basePointer2;
	msg2->getMessagePointer(basePointer2);

	if (strcmp(basePointer1->GetNameOfClass(), basePointer2->GetNameOfClass()) != 0)
	{
		QLOG_ERROR() <<"CompareMsgData: Incompatible message types, cannot comapre them" <<endl;
		return false;
	}

    if (strcmp(basePointer1->GetNameOfClass(), "igtl::TransformMessage") == 0)
    {
        igtl::TransformMessage::Pointer msgPointer1 = static_cast<igtl::TransformMessage *>(basePointer1.GetPointer());
        msgPointer1->Unpack();

        igtl::TransformMessage::Pointer msgPointer2 = static_cast<igtl::TransformMessage *>(basePointer2.GetPointer());
        msgPointer2->Unpack();
		
		igtl::Matrix4x4 matrix1;
		igtl::Matrix4x4 matrix2;

        msgPointer1->GetMatrix(matrix1);
		msgPointer2->GetMatrix(matrix2);

        int r = memcmp((const void*)&matrix1, (const void*)matrix2, sizeof(igtl::Matrix4x4));

        if (r != 0)
		{
			std::cout <<"Data do not match!" <<std::endl;
			std::cout <<"Matrix1: " <<std::endl;
	        igtl::PrintMatrix(matrix1);
	        std::cout <<std::endl;

			std::cout <<"Matrix2: " <<std::endl;
			igtl::PrintMatrix(matrix2);
	        std::cout <<std::endl;

			QLOG_ERROR() <<"CompareMsgData: Data fields are not equal!" <<endl;
			return false;
		}
		else
		{
			QLOG_INFO() <<"CompareMsgData: Data fields are equal";
			return true;
		}
    }
	else if (strcmp(basePointer1->GetNameOfClass(), "igtl::StatusMessage") == 0)
	{
		igtl::StatusMessage::Pointer msgPointer1 = static_cast<igtl::StatusMessage *>(basePointer1.GetPointer());
        msgPointer1->Unpack();

        igtl::StatusMessage::Pointer msgPointer2 = static_cast<igtl::StatusMessage *>(basePointer2.GetPointer());
        msgPointer2->Unpack();
		
		if ( msgPointer1->GetCode() != msgPointer2->GetCode() ||
			msgPointer1->GetSubCode() != msgPointer2->GetSubCode() ||
			strcmp(msgPointer1->GetStatusString(), msgPointer2->GetStatusString()) != 0 ||
			strcmp(msgPointer1->GetErrorName(), msgPointer2->GetErrorName()) != 0 )
		{
			std::cout <<"Data do not match!" <<std::endl;
			std::cout <<"ErrorCode 1: " <<msgPointer1->GetCode() <<std::endl;
			std::cout <<"ErrorSubCode 1: " <<msgPointer1->GetSubCode() <<std::endl;
			std::cout <<"ErrorString 1: " <<msgPointer1->GetErrorName() <<std::endl;
			std::cout <<"StatusString 1: " <<msgPointer1->GetStatusString() <<std::endl;
			
			std::cout <<"ErrorCode 2: " <<msgPointer2->GetCode() <<std::endl;
			std::cout <<"ErrorSubCode 2: " <<msgPointer2->GetSubCode() <<std::endl;
			std::cout <<"ErrorString 2: " <<msgPointer2->GetErrorName() <<std::endl;
			std::cout <<"StatusString 2: " <<msgPointer2->GetStatusString() <<std::endl;
	        std::cout <<std::endl;

			QLOG_ERROR() <<"CompareMsgData: Data fields are not equal!" <<endl;
			return false;
		}
		else
		{
			QLOG_INFO() <<"CompareMsgData: Data fields are equal";
			return true;
		}
	}
	else
	{
		QLOG_ERROR() <<"CompareMsgData: Unknown message type, cannot compare" <<endl;
		return false;
	}
	
	//should never get here, really
	return false;
}

void InitMessageTypes(mapStrMsgType &types)
{
  types["NONDEFINED"] = NONDEFINED;
  types["BIND"] = BIND;
  types["GET_BIND"] = GET_BIND;
  types["STT_BIND"] = STT_BIND;
  types["STP_BIND"] = STP_BIND;
  types["RTS_BIND"] = RTS_BIND;
  types["COLORTABLE"] = COLORTABLE;
  types["GET_COLORT"] = GET_COLORT;
  types["STT_COLORT"] = STT_COLORT;
  types["STP_COLORT"] = STP_COLORT;
  types["RTS_COLORT"] = RTS_COLORT;
  types["IMAGE"] = IMAGE;
  types["GET_IMAGE"] = GET_IMAGE;
  types["STT_IMAGE"] = STT_IMAGE;
  types["STP_IMAGE"] = STP_IMAGE;
  types["RTS_IMAGE"] = RTS_IMAGE;
  types["IMGMETA"] = IMGMETA;
  types["GET_IMGMETA"] = GET_IMGMETA;
  types["STT_IMGMETA"] = STT_IMGMETA;
  types["STP_IMGMETA"] = STP_IMGMETA;
  types["RTS_IMGMETA"] = RTS_IMGMETA;
  types["LBMETA"] = LBMETA;
  types["GET_LBMETA"] = GET_LBMETA;
  types["STT_LBMETA"] = STT_LBMETA;
  types["STP_LBMETA"] = STP_LBMETA;
  types["RTS_LBMETA"] = RTS_LBMETA;
  types["NDARRAY"] = NDARRAY;
  types["GET_NDARRAY"] = GET_NDARRAY;
  types["STT_NDARRAY"] = STT_NDARRAY;
  types["STP_NDARRAY"] = STP_NDARRAY;
  types["RTS_NDARRAY"] = RTS_NDARRAY;
  types["POINTMSG"] = POINTMSG;
  types["GET_POINTMSG"] = GET_POINTMSG;
  types["STT_POINTMSG"] = STT_POINTMSG;
  types["STP_POINTMSG"] = STP_POINTMSG;
  types["RTS_POINTMSG"] = RTS_POINTMSG;
  types["POLYDATA"] = POLYDATA;
  types["GET_POLYDATA"] = GET_POLYDATA;
  types["STT_POLYDATA"] = STT_POLYDATA;
  types["STP_POLYDATA"] = STP_POLYDATA;
  types["RTS_POLYDATA"] = RTS_POLYDATA;
  types["POSITION"] = POSITION;
  types["GET_POSITION"] = GET_POSITION;
  types["STT_POSITION"] = STT_POSITION;
  types["STP_POSITION"] = STP_POSITION;
  types["RTS_POSITION"] = RTS_POSITION;
  types["QTDATA"] = QTDATA;
  types["GET_QTDATA"] = GET_QTDATA;
  types["STT_QTDATA"] = STT_QTDATA;
  types["STP_QTDATA"] = STP_QTDATA;
  types["RTS_QTDATA"] = RTS_QTDATA;
  types["SENSOR"] = SENSOR;
  types["GET_SENSOR"] = GET_SENSOR;
  types["STT_SENSOR"] = STT_SENSOR;
  types["STP_SENSOR"] = STP_SENSOR;
  types["RTS_SENSOR"] = RTS_SENSOR;
  types["STATUS"] = STATUS;
  types["GET_STATUS"] = GET_STATUS;
  types["STT_STATUS"] = STT_STATUS;
  types["STP_STATUS"] = STP_STATUS;
  types["RTS_STATUS"] = RTS_STATUS;
  types["STRING"] = STRING;
  types["GET_STRING"] = GET_STRING;
  types["STT_STRING"] = STT_STRING;
  types["STP_STRING"] = STP_STRING;
  types["RTS_STRING"] = RTS_STRING;
  types["TDATA"] = TDATA;
  types["GET_TDATA"] = GET_TDATA;
  types["STT_TDATA"] = STT_TDATA;
  types["STP_TDATA"] = STP_TDATA;
  types["RTS_TDATA"] = RTS_TDATA;
  types["TRAJ"] = TRAJ;
  types["GET_TRAJ"] = GET_TRAJ;
  types["STT_TRAJ"] = STT_TRAJ;
  types["STP_TRAJ"] = STP_TRAJ;
  types["RTS_TRAJ"] = RTS_TRAJ;
  types["TRANSFORM"] = TRANSFORM;
  types["GET_TRANS"] = GET_TRANS;
  types["STT_TRANS"] = STT_TRANS;
  types["STP_TRANS"] = STP_TRANS;
  types["RTS_TRANS"] = RTS_TRANS;
}
#ifndef OIGTLSOCKETOBJECT_H
#define OIGTLSOCKETOBJECT_H

#include "OIGTLSenderThread.h"
#include "OIGTLListenerThread.h"

#include "NiftyLinkCommonWin32ExportHeader.h"

class NIFTYLINKCOMMON_WINEXPORT OIGTLSocketObject : public QObject
{
	Q_OBJECT

signals:
	void messageReceived(OIGTLMessage::Pointer);
    void messageToSend(OIGTLMessage::Pointer);
	void sendingFinished();
	
	void connectedToRemoteSignal(void);
	void lostConnectionToRemoteSignal(void);
	void clientConnectedSignal(void);
	void clientDisconnectedSignal(void);

	void testSignal(void);

public:
	OIGTLSocketObject(QObject *parent = 0);
	~OIGTLSocketObject(void);

	bool listenOnPort(int port);
	bool connectToRemote(QUrl url);

	void closeSocket(void);

	inline int getPort() { return m_port; }
	inline bool isListening() { return m_listening; }
	inline bool isConnected() { return m_connectedToRemote; }
	inline bool isClientConnecting() { return m_clientConnected; }
	inline bool isAbleToSend() { return m_ableToSend; }

	void testSignals() {
		emit testSignal(); }

public slots:
	void sendMessage(OIGTLMessage::Pointer msg);
	void catchMsgSignal(OIGTLMessage::Pointer msg);
	void catchTestSignal();

private:
	void initThreads(void);

private slots:
	
	void connectedToRemote(void);
	void disconnectedFromRemote(void);

	void clientConnected(void);
	void clientDisconnected(void);



private:
	int      m_port;
	QMutex * m_mutex;
	
	OIGTLSenderThread   * m_sender;
	OIGTLListenerThread * m_listener;

    bool m_initialized;

	bool m_listening;
	bool m_connectedToRemote;
	
	bool m_clientConnected;
	bool m_ableToSend;

};

#endif


// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/


#include "MLPlatform.h"

#if ML_WINDOWS
	// TODO
#else

#include "OscTypes.h"
#include "OscException.h"
#include "OscPacketListener.h"
#include "UdpSocket.h"

#include "MLDebug.h"

#include "pthread.h"

class MLOSCListener : public osc::OscPacketListener
{
public:
	MLOSCListener();
	~MLOSCListener();
	
	// listen to the given port or, if port = 0, shut down listening gear
	void listenToOSC(int port);
	
	// start a listener thread, not returning until the thread is done.
	static void *startThread(void *arg);
		
protected:
    virtual void ProcessMessage(const osc::ReceivedMessage &, const IpEndpointName& ) = 0;
	virtual void ProcessBundle(const osc::ReceivedBundle& , const IpEndpointName& ){};

private:
	UdpListeningReceiveSocket* mpSocket;
	bool mSocketActive;
	pthread_t mListenerThread;
	int mPort;
};


#endif // ML_WINDOWS
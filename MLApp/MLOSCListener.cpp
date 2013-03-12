
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#include "MLPlatform.h"
#include "MLOSCListener.h"

#if ML_WINDOWS
	// TODO
#else


MLOSCListener::MLOSCListener() :
	mpSocket(0),
	mSocketActive(0),
	mPort(0)
{
}

MLOSCListener::~MLOSCListener()
{
	listenToOSC(0);
}

void *MLOSCListener::startThread(void *arg)
{
	MLOSCListener* pL = static_cast<MLOSCListener*>(arg);
	UdpListeningReceiveSocket* pSocket = pL->mpSocket;
	int port = pL->mPort;
			
	debug() << "MLOSCListener: starting listen thread for OSC on port " << port << "...\n";

	pSocket->Run();
	
	debug() << "MLOSCListener: listener thread on port " << port << " terminated.\n"; 

	pL->mSocketActive = false;
	return 0;
}

void MLOSCListener::listenToOSC(int port)
{
	if(port)
	{
		if(mpSocket)
		{
			listenToOSC(0);
		}
		try
		{
			mpSocket = new UdpListeningReceiveSocket(
				IpEndpointName( IpEndpointName::ANY_ADDRESS, port), 
				this);
		}
		catch( osc::Exception& e )
		{
			// TODO find another socket!  Integrate with zeroconf stuff.  
			mpSocket = 0;
			MLError() << "MLOSCListener::listenToOSC: couldn't bind to port " << port << ".\n";
			MLError() << "error: " << e.what() << "\n";
		}
		catch(...)
		{
			mpSocket = 0;
			MLError() << "MLOSCListener::listenToOSC: couldn't bind to port " << port << ".\n";
		}
		
		if(mpSocket)
		{
			mSocketActive = true;
			mPort = port;
			
			int err;
			pthread_attr_t attr;
			err = pthread_attr_init(&attr);
			err = pthread_create(&mListenerThread, &attr, &MLOSCListener::startThread, (void*)this);
		}
	}
	else
	{
		if(mpSocket)
		{
			mpSocket->AsynchronousBreak();
			while(mSocketActive)
			{
				usleep(100);
			}
			delete mpSocket;
			mpSocket = 0;
			mPort = 0;
		}
	}
}

#endif // ML_WINDOWS
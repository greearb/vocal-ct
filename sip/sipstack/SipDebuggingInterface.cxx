/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

static const char* const SipDebuggingInterface_cxx_Version =
    "$Id: SipDebuggingInterface.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "TCPServerSocket.hxx"
#include "ThreadIf.hxx"
#include "IPAddress.hxx"
#include "SipDebuggingInterface.hxx"
#include "Lock.hxx"
#include "Mutex.hxx"

using namespace Vocal;


SipClientDebuggingInterface::SipClientDebuggingInterface(Sptr<TCPClientSocket> sock, SipDebuggingInterface& parent)
    : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, 64 * 1024 /*VTHREAD_STACK_SIZE_DEFAULT*/),
      mySock(sock),
      myParent(parent)
{
}

void
SipClientDebuggingInterface::thread()
{
    char buffer[256];
    int pos = 0;
    while(!isShutdown())
    {
	int count = 0;
	char tmp;
	try
	{
	    count = mySock->receive(&tmp, 1);
	}
	catch (...)
	{
	    shutdown();
	    mySock = 0;
	    return;
	}
	if(count) 
	{
	    if(tmp == '\r' || tmp == '\n' || (pos > 250))
	    {
		// run command
		buffer[pos] = '\0';
		if(strcmp(buffer, "status") == 0)
		{
		    Vocal::Threads::Lock x(myParent.myMutex);
		    for(SipDebuggingInterface::StatusList::iterator i 
			    = myParent.myStatus.begin() ;
			i != myParent.myStatus.end() ; ++i)
		    {
			Data x = (*i)->status();
                        LocalScopeAllocator lo;
			mySock->send(x.getData(lo));
			mySock->send("\n");
		    }
		}
		if(strcmp(buffer, "quit") == 0)
		{
		    shutdown();
		    mySock = 0;
		    return;
		}
		pos = 0;
	    }
	    else if(tmp == '\x08') 
	    {
		if(pos > 0)
		{
		    pos--;
		}
	    }
	    else
	    {
		buffer[pos++] = tmp;
	    }
	}
    }
}



SipParentDebuggingInterface::SipParentDebuggingInterface(int port, 
							 SipDebuggingInterface& parent)
    : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, 64 * 1024 /*VTHREAD_STACK_SIZE_DEFAULT*/),
      mySock(IPAddress(port),1),
      myParent(parent)
{
}


void
SipParentDebuggingInterface::thread()
{
    while(!isShutdown())
    {
	// lame, but simple -- max 1 connection at a time
	Sptr<TCPClientSocket> s = mySock.accept();
	if(myInterface != 0)
	{
	    if(myInterface->isShutdown())
	    {
		myInterface->join();
		myInterface = 0;
	    }
	}

	if((s != 0) && (myInterface == 0))
	{
	    myInterface = new SipClientDebuggingInterface(s, myParent);
	    myInterface->run();
	}
	else if(s != 0)
	{
	    s->send("sorry, you cannot connect\r\n");
	    s = 0;
	}
    }
    if(myInterface != 0)
    {
	myInterface->shutdown();
	myInterface->join();
	myInterface = 0;
    }
}


SipParentDebuggingInterface::~SipParentDebuggingInterface()
{
    if(myInterface != 0)
    {
	myInterface->shutdown();
	myInterface->join();
	myInterface = 0;
    }
}


SipDebuggingInterface::SipDebuggingInterface(int port)
    :
    myInterface(port, *this)
{
    myInterface.run();
}


void
SipDebuggingInterface::add(Sptr<SipDebuggingStatus> item)
{
    Vocal::Threads::Lock x(myMutex);
    myStatus.push_back(item);
}


SipDebuggingInterface::~SipDebuggingInterface()
{
    myInterface.shutdown();
    myInterface.join();
}


SipDebuggingStatus::SipDebuggingStatus()
{
}


SipDebuggingStatus::~SipDebuggingStatus()
{
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

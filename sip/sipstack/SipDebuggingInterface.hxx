#ifndef SIP_DEBUGGING_INTERFACE_HXX_
#define SIP_DEBUGGING_INTERFACE_HXX_
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

static const char* const SipDebuggingInterface_hxx_Version =
    "$Id: SipDebuggingInterface.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <list>

#include "global.h"
#include "TransportCommon.hxx"
#include "TCPServerSocket.hxx"
#include "ThreadIf.hxx"
#include "IPAddress.hxx"
#include "TCPClientSocket.hxx"
#include "SipTransactionDB.hxx"
#include "SipUdpConnection.hxx"
#include "Mutex.hxx"
#include "Lock.hxx"

namespace Vocal
{

using Vocal::Transport::TCPServerSocket;
using Vocal::Transport::TCPClientSocket;
using Vocal::Transport::IPAddress;
using Vocal::Threads::Lock;

class SipDebuggingInterface;

class SipClientDebuggingInterface : public ThreadIf
{
    public:
	SipClientDebuggingInterface(Sptr<TCPClientSocket> sock, 
				    SipDebuggingInterface& parent);
	void thread();
	
    private:
	Sptr<TCPClientSocket> mySock;
	SipDebuggingInterface& myParent;
};


class SipParentDebuggingInterface : public ThreadIf
{
    public:
        SipParentDebuggingInterface(int port, SipDebuggingInterface& parent);
        virtual ~SipParentDebuggingInterface();

        void thread();
    private:
	TCPServerSocket mySock;
	Sptr<SipClientDebuggingInterface> myInterface;
	SipDebuggingInterface& myParent;
};


class SipDebuggingStatus
{
    public:
	SipDebuggingStatus();
        virtual ~SipDebuggingStatus();
        virtual Data status() = 0;
};


class SipDebuggingInterface
{
    public:
	SipDebuggingInterface(int port); 
	~SipDebuggingInterface();
	void add(Sptr<SipDebuggingStatus> item);

    public:
        typedef list < Sptr<SipDebuggingStatus> > StatusList;
        SipParentDebuggingInterface myInterface;
	StatusList myStatus;
        Vocal::Threads::Mutex myMutex;
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

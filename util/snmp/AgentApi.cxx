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


static const char* const AgentApi_cxx_Version =
    "$Id: AgentApi.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "AgentApi.hxx"
#include "UdpStack.hxx"
#include "cpLog.h"
#include "NetworkConfig.hxx"


AgentApi::AgentApi(ServerType serType /*Default Argument*/, string appName /*Default Argument*/ ) : ThreadIf(0, VTHREAD_PRIORITY_DEFAULT, VTHREAD_STACK_SIZE_DEFAULT)
{
    char hostname[MAXHOSTNAMELEN];
    myServerType = serType;
    myApplName = appName;

    memset(hostname, 0, sizeof(hostname));
    NetworkAddress na(""); //TODO:  Allow one to specify local IP.
    agentIpStr = na.getIpName().c_str();

}


///
AgentApi::~AgentApi()
{
    // close ports, deallocate memory
    cpLog( LOG_DEBUG, "entered agentApi destructor");
    shutdown();
    join();
}

/**
 * send a trap message to the trap agentTrapPort on the local machine
 * @param trapType enum representing trap type defined in mib
 * @param parameter text string to include in message
 * @return voReturnStatus
 */
voReturnStatus
AgentApi::sendTrap(int trapType, string parameter)
{
    try
    {
        NetworkAddress dest(agentIpStr, agentTrapPort);
        memset(&trapMessage, 0, sizeof(trapMessage));
        trapMessage.action = (actionT)Trap;
        trapMessage.mibVariable = (AgentApiMibVarT)trapType;
        if (parameter.length() < PARM1SIZE )
        {
            strcpy(trapMessage.parm1, parameter.c_str());
        }
        else
        {
            return voFailure;
        }
        udpStack->transmitTo((char *)&trapMessage, sizeof(trapMessage), &dest);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

/**
 * send an integer reaponse to another agentapi 
 * @param val integer value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(int val, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
	memcpy(message.parm2, &val,  sizeof(val));
	if (sender) 
	    cpLog( LOG_DEBUG, "send integer response: %d to %s:%d",
		   val, sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
    return voSuccess;
}

/**
 * send a ulong reaponse to another agentapi 
 * @param val ulong value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(unsigned long val, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
	memcpy(message.parm2, &val, sizeof(val));
        cpLog( LOG_DEBUG, "send ulong response: %d to %s:%d",
               val, sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
    return voSuccess;
}

/**
 * send a string reaponse to another agentapi 
 * @param parameter string value
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(string parameter, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
        if (parameter.length() < PARM2SIZE )
        {
            strcpy(message.parm2, parameter.c_str());
        }
        else
        {
            return voFailure;
        }
        cpLog( LOG_DEBUG, "send string response: %s to %s:%d",
               parameter.c_str(), sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

/**
 * send a reaponse to another agentapi 
 * @param inData data to send in parameter.  must be smaller than PARM2SIZE
 * @param sender destination of message
 */
voReturnStatus
AgentApi::sendResponse(void *inData, NetworkAddress *sender)
{
    try
    {
        message.action = (actionT)Response;
	memset(message.parm2, 0, sizeof(message.parm2));
        memcpy(message.parm2, inData, sizeof(message.parm2));
        cpLog( LOG_DEBUG, "send generic response to %s:%d",
               sender->getHostName().c_str(), sender->getPort());
        udpStack->transmitTo((char *)&message, sizeof(message), sender);
        return voSuccess;
    }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
    {
        return voFailure;
    }
}

voReturnStatus
sendRequest(string indexName, ServerType serType)
{
    cpLog( LOG_ERR, "Using function not yet implemented 2" );
    return voFailure;
}

voReturnStatus
sendRequest(string indexName, int setValue)
{
    cpLog( LOG_ERR, "Using function not yet implemented 2" );
    return voFailure;
}

/**
 * first it registers on ports based on the type of agent it is, then 
 * infinate loop that waits for messages from other agentApi's and processes them.
 */
void
AgentApi::thread()
{
    int bytesRead = 0;
    NetworkAddress sender("");
    NetworkAddress dest("0.0.0.0");
    if((NetworkConfig::instance().isDualStack()) ||
       (NetworkConfig::instance().getAddrFamily() == PF_INET))
    {
        dest.setHostName(registerMulticastIP);
        dest.setPort(registerMulticastPort );
    }
    else
    {
        dest.setHostName("ff14::1");
        dest.setPort(registerMulticastPort );
    }

    memset(&message1, 0, sizeof(message1));
    myLock.lock();

    switch (myServerType)
    {
        case SERVER_Agent:
        // register to receive the messages on the well known agent port
        // then send out the multicast message.
#ifdef __linux__
	    udpStack = new UdpStack("", "", 0, agentTrapPort, agentTrapPort, sendrecv, false);
#else
	    udpStack = new UdpStack("", "", 0, -1 , -1 , sendrecv, false);
#endif	
	    message1.action = (actionT)Register_Req;
	    udpStack->transmitTo((char *)&message1, sizeof(message1), &dest);
	    break;

        case SERVER_NetMgnt:
	    udpStack = new UdpStack("", "", 0, MANAGERTRAPPORT , MANAGERTRAPPORT, sendrecv, false);
	    break;

        default :

	    udpStack = new UdpStack("", "", 0, -1 , -1 , sendrecv, false);

	    memset(&message, 0, sizeof(message));
	    message1.action = (actionT)Register;
	    int *pi = (int *)message1.parm1;
	    int tmpPort = udpStack->getRxPort();
	    memcpy(&pi[0], &myServerType, sizeof(int));
	    memcpy(&pi[1], &tmpPort, sizeof(int));
	    if (sizeof(message1.parm2) < myApplName.length()) 
	    {
		memcpy(message1.parm2, (char*)myApplName.c_str(), 
		       sizeof(message1.parm2));
	    } 
	    else 
	    {
		memcpy(message1.parm2, (char*)myApplName.c_str(), 
		       myApplName.length());
	    }
	    agentRegister = new AgentRegister((void *) & message1, 
					      sizeof(message1));
	    agentRegister->run();
	    break;
    }

    myLock.unlock();

    while (true)
    {
        try
        {
            // Poll every second 

            memset((void *)&message, 0, sizeof(ipcMessage));
            bytesRead = udpStack->receiveTimeout((char *) & message, 
						 sizeof(ipcMessage), 
						 &sender, 
						 0, 100);
            if ( bytesRead )
            {
                cpLog( LOG_DEBUG, "bytes read=%d", bytesRead);
                processMessage(&message, &sender);
            }
        }
#ifdef PtW32CatchAll
    PtW32CatchAll
#else
    catch ( ... ) 
#endif
        {
            cpLog( LOG_ERR, "Exception from udpstack for AgentApiRx" );
        }

        if ( isShutdown() == true )
        {
            return ;
        }
    }
}


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


static const char* const AgentRegister_cxx_Version =
    "$Id: AgentRegister.cxx,v 1.2 2004/05/04 07:31:16 greear Exp $";


#include "global.h"
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include "AgentRegister.hxx"
#include "UdpStack.hxx"
#include "cpLog.h"
#include <errno.h>
#include "NetworkConfig.hxx"


/**
 * constructor.  This will send a register message to the snmpd from this agent.
 * @param msg the register message to send
 * @param msgLEN size of msg in bytes
 */
AgentRegister::AgentRegister(void *msg, int msgLEN)
      : dest("0.0.0.0")
{
    const int maxHostNameLen = 128;
    char hostname[maxHostNameLen];
    txMsgLen = msgLEN;
    txMessage = (char *)calloc(txMsgLen, sizeof(char));
    memcpy(txMessage, msg, txMsgLen);
    memset(hostname, 0, sizeof(hostname));
    if (gethostname(hostname, maxHostNameLen ) < 0)
    {
        cpLog(LOG_ERR, "gethostname failed: %s", strerror(errno));
        throw "big fat mess";
    }
    NetworkAddress mcAddr("0.0.0.0");
    if((NetworkConfig::instance().isDualStack()) ||
       (NetworkConfig::instance().getAddrFamily() == PF_INET))
    {
        mcAddr.setHostName(registerMulticastIP);
        mcAddr.setPort(registerMulticastPort );
    }
    else 
    {
        mcAddr.setHostName("ff14::1");
        mcAddr.setPort(registerMulticastPort );
    }
    dest.setHostName(hostname);
    dest.setPort(agentTrapPort);

    regUdpStack = new UdpStack("", "", &mcAddr, registerMulticastPort, registerMulticastPort, sendrecv, false, true);
    if (regUdpStack == 0)
    {
        cpLog(LOG_ERR, "can't register udp multicast port %d", registerMulticastPort);
        throw"another bad thing happened";
    }

    NetworkAddress iface(hostname);
    regUdpStack->joinMulticastGroup(mcAddr, &iface, 0);

    // A new stack has been created to do the transmission. Contact nismail@cisco.com
    regTrUdpStack = new UdpStack("", "", (const NetworkAddress *)&dest, -1, -1, sendonly, false, false);
    if (regTrUdpStack == 0)
    {
        cpLog(LOG_ERR, "can't create a register transmit UDP stack");
        throw"another bad thing happened";
    }

    // Transmission stack is connected as we always send to the same address. contact nismail@cisco.com
    //regTrUdpStack->connectPorts();


    // This is modified to send on the regTrUdpStack instead of the regUdpStack. Contact
    // nismail@cisco.com
    regTrUdpStack->transmitTo(txMessage, txMsgLen, &dest);
}

AgentRegister::~AgentRegister()
{
    if (txMessage)
       free (txMessage);
}


#warning "Not converted to non-threaded model."
#if 0

/**
 * periodically send the register message to the snmpd
 */
void
AgentRegister::thread()
{
    int bytesRead = 0;
    NetworkAddress sender(""); //TODO:  Allow to specify local IP.

    while (true)
    {
        try
        {
            memset((void *)&rxMessage, 0, sizeof(rxMessage));
            bytesRead = regUdpStack->receiveTimeout((char *) & rxMessage, sizeof(rxMessage), &sender, 1);

            // There are 3 cases that need to be take care of.
            // 1. Timeout - send the message to the trapport (0)
            // 2. received response - send message to the trapport (>0)
            // 3. an error occured ..... just report an error (-1)
            // If the message we get is either a 0 or greater than that then send a message with
            // the port number and type, name of app, to the snmpd at the port 33602 (trapport ) on which the
            // snmpd will be listening ....
            // Also I will have to construct the message in the constructor.

            if ( bytesRead >= 0)
            {
		// Send on the transmission stack. Contact nismail@cisco.com
                regTrUdpStack->transmitTo(txMessage, txMsgLen, &dest);

            }
            if ( bytesRead == -1)
            {
                cpLog( LOG_ERR, "Error in receiving on port %d", regUdpStack->getRxPort());
            }
        }
#ifdef PtW32CatchAll
        PtW32CatchAll
#else
        catch ( ... )
#endif
        {
            cpLog( LOG_ERR, "Exception from udpstack for AgentRegister" );
        }
        if ( isShutdown() == true )
        {
            return ;
        }
    }
}
#endif

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


static const char* const SipEvent_cxx_Version =
    "$Id: SipEvent.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";


#include "global.h"
#include "cpLog.h"
#include "SipEvent.hxx"
#include "InviteMsg.hxx"
#include "SipVia.hxx"
#include "CallContainer.hxx"
#include "SipTransceiver.hxx"
#include "SipMsg.hxx"
#include <sstream>

using namespace Vocal;

SipEvent::SipEvent( const Sptr < Fifo < Sptr < SipProxyEvent > > > outputFifo )
    :   SipProxyEvent(outputFifo),
        mySipMsg(0),
        mySipMsgQueue(0),
        mySipCallLeg(0)
{
}


SipEvent::~SipEvent()
{
}


void
SipEvent::setSipMsg( const Sptr < SipMsg > sipMsg )
{
    assert( sipMsg != 0 );

    mySipMsg = sipMsg;
}


const Sptr < SipMsg >
SipEvent::getSipMsg() const
{
    assert( mySipMsg != 0 );

    return ( mySipMsg );
}



void
SipEvent::setSipMsgQueue(const Sptr < SipMsgQueue > sipRcv)
{
    assert( sipRcv != 0 );

    mySipMsgQueue = sipRcv;

    setSipMsg(sipRcv->back());
}


const Sptr < SipMsgQueue >
SipEvent::getSipMsgQueue() const
{
    assert( mySipMsgQueue != 0 );

    return ( mySipMsgQueue );
}


void
SipEvent::setSipReceive( const Sptr < SipMsgQueue > sipRcv )
{
    setSipMsgQueue(sipRcv);
}


const Sptr < SipMsgQueue >
SipEvent::getSipReceive() const
{
    assert ( mySipMsgQueue != 0 );

    return ( mySipMsgQueue );
}


const Sptr < InviteMsg >
SipEvent::getInvite() const
{
    // iterate through all msgs associated with this event looking
    // for an INVITE, and return it if found
    Sptr < SipMsg > sipMsg;

    cpLog( LOG_DEBUG, "Search transaction for previous INVITE" );
    Sptr < SipMsgQueue > transaction = mySipMsgQueue;
    if ( transaction != 0 )
    {
        SipMsgQueue::iterator i;
        for (i = transaction->begin(); i != transaction->end(); ++i)
        {
            sipMsg = *i;
            if ( sipMsg == 0 )
            {
                // reach empty sip msg without finding INVITE, return 0
                cpLog( LOG_DEBUG, "no INVITE found, returning 0" );
                return 0;
            }

            if ( sipMsg->getType() == SIP_INVITE )
            {
	        Sptr < InviteMsg > invite;
                invite.dynamicCast( sipMsg );
                cpLog( LOG_DEBUG, "Found INVITE:" );
                cpLog( LOG_DEBUG, "%s", invite->encode().logData() );
                return invite;
            }
            cpLog( LOG_DEBUG, "Not an INVITE:" );
            cpLog( LOG_DEBUG, "%s", sipMsg->encode().logData() );
        }
    }

    // no Invite found
    cpLog( LOG_DEBUG, "no INVITE found, returning 0" );
    return 0;
}


const Sptr < SipCommand >
SipEvent::getCommand() const
{
    // iterate through all msgs associated with this event looking
    // for a command, and return it if found
    Sptr < SipMsg > sipMsg;

    cpLog( LOG_DEBUG, "Search transaction for previous command" );
    Sptr < SipMsgQueue > transaction = mySipMsgQueue;
    if ( transaction != 0 )
    {
        SipMsgQueue::iterator i;
        for (i = transaction->begin(); i != transaction->end(); ++i)
        {
            sipMsg = *i;
            if ( sipMsg == 0 )
            {
                // reach empty sip msg without finding INVITE, return 0
                cpLog( LOG_DEBUG, "no command found, returning 0" );
                return 0;
            }

	    Sptr < SipCommand > command;
            command.dynamicCast( sipMsg );
            if ( command != 0 )
            {
                cpLog( LOG_DEBUG, "Found command sent to %s:%d",
		                  command->getSendAddress().getIpName().c_str(),
		                  command->getSendAddress().getPort() );
                cpLog( LOG_DEBUG, "%s", command->encode().logData() );
                return command;
            }
            cpLog( LOG_DEBUG, "Not an INVITE:" );
            cpLog( LOG_DEBUG, "%s", sipMsg->encode().logData() );
        }
    }

    // no command found
    cpLog( LOG_DEBUG, "no command found, returning 0" );
    return 0;
}


const Sptr < SipCommand >
SipEvent::getPendingCommand( Sptr < SipCommand > sipCommand ) const
{
    // allocate vector to store previous msgs with same call leg
    CallLegVector callLegMsgs( mySipStack->getCallLegMsgs( sipCommand ) );

    // create iterator to search for matching commandw
    vector < Sptr < SipMsg > > ::iterator iter( callLegMsgs.begin() );

    // search through previous msgs, and return matching command if found;
    // otherwise return 0
    cpLog( LOG_INFO, "Check each matching command" );
    Sptr < SipCommand > pendingCommand;

    while ( iter != callLegMsgs.end() )
    {
        cpLog( LOG_INFO, "Check this command" );

        if ( *iter == 0 )
        {
            cpLog( LOG_ERR, "This command is empty" );
        }

        else if ( pendingCommand.dynamicCast( *iter ) == 0 )
        {
            cpLog( LOG_ERR, "This command is not a SIP command" );
        }

        else
        {
            // first Via of Command must match second Via of pending command
            if ( ( pendingCommand->getNumVia() >= 2 ) &&
                    ( sipCommand->getNumVia() >= 1 ) )
            {
                if ( pendingCommand->getVia( 1 ) ==
                        sipCommand->getVia( 0 ) )
                {
                    cpLog( LOG_DEBUG, "matching command found" );
                    return pendingCommand;
                }
                cpLog( LOG_DEBUG, "found command, but Via's don't match" );
            }
        }

        ++iter;
    }

    // no match found, return 0
    cpLog( LOG_ERR, "no matching command" );
    return 0;
}


Sptr < SipCallLeg >
SipEvent::getSipCallLeg() const
{
    assert( mySipCallLeg != 0 );

    return ( mySipCallLeg );
}


void
SipEvent::removeCall( )
{
    assert( myCallInfo != 0 );
    assert( myCallContainer != 0 );
    assert( mySipCallLeg != 0 );

    myCallContainer->removeCall(mySipCallLeg);
}


void 
SipEvent::setCallLeg()
{
    assert(mySipMsg != 0);
    mySipCallLeg = new SipCallLeg(mySipMsg->computeCallLeg());
    cpLog(LOG_DEBUG, "set SipCallLeg, From user: %s, To user: %s",
          mySipCallLeg->getFrom().getUser().logData(),
	  mySipCallLeg->getTo().getUser().logData());
}


string SipEvent::toString() const {
   ostringstream oss;
   if (mySipMsg.getPtr()) {
      oss << "mySipMsg: " << mySipMsg->toString() << endl;
   }
   else {
      oss << "mySipMsg: NULL\n";
   }

   if (mySipCallLeg.getPtr()) {
      oss << "mySipCallLeg: " << mySipCallLeg->toString() << endl;
   }
   else {
      oss << "mySipCallLeg:  NULL\n";
   }

   return oss.str();
}

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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtspTcpBuffer_cxx_Version =
    "$Id: RtspTcpBuffer.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspTcpBuffer.hxx"
#include "RtspTcpConnection.hxx"
#include "RtspMsgParser.hxx"
#include "RtspMsg.hxx"
#include "RtspTransceiver.hxx"
#include "cpLog.h"

//isAlive isn't being used by anyone right now

RtspTcpBuffer::RtspTcpBuffer( Connection conn, int keepAliveCounter,
                              Fifo< Sptr<RtspMsg> >& recvFifo, int bufferSize )
    : myMapIndex( conn.getConnId() ),
      myConnection( conn ),
      myMaxKeepAliveCounter( keepAliveCounter ),
      myKeepAliveCounter( keepAliveCounter ),
      myBufferSize( bufferSize ),
      mySavedRtspMsg( 0 ),
      myRecvFifo( recvFifo )
{
    myCharBuf = new char[myBufferSize];
}


RtspTcpBuffer::~RtspTcpBuffer()
{
    // delete data buffer
    delete[] myCharBuf;
}


bool
RtspTcpBuffer::processConnection( Sptr<RtspTcpBuffer> tcpBufferObj )
{
    bool result = true;

    // reset keepAliveCounter;
    myKeepAliveCounter = myMaxKeepAliveCounter;

    try
    {
        result = readTcpData( tcpBufferObj );
    }
    catch( VException& e )
    {
        cpLog( LOG_DEBUG, e.getDescription().c_str() );
        result = false;
    }

    return result;
}


bool
RtspTcpBuffer::readTcpData( Sptr<RtspTcpBuffer> tcpBufferObj )
throw( VNetworkException&, RtspBadDataException& )
{
    char* readStart = myCharBuf;
    u_int32_t charRead = myBufferSize;
    int charReadReal = 0;

    memset(readStart, 0, myBufferSize);
    RtspTransceiver::readData( myConnection,(void*)readStart,
                               charRead, charReadReal );
    if( charReadReal == 0 )
    {
        //cpLog( LOG_DEBUG, "Tcp connection closed" );
        return false;
    }
    else if( charReadReal < 0 )
    {
        throw VNetworkException( "Error reading connection",
                                 __FILE__, __LINE__, 0 );
    }
    cpLog( LOG_DEBUG, "Received:\n%s", readStart );

    int bytesDecoded = 0;
    int bytesNeedtoDecode = charReadReal;

    Sptr< RtspMsg > newMsg = 0;
    Sptr< RtspMsg > msg = 0;
    while( bytesDecoded < bytesNeedtoDecode )
    {
        msg = RtspMsgParser::preParse( myCharBuf + bytesDecoded,
                                       bytesNeedtoDecode,
                                       bytesDecoded,
                                       mySavedRtspMsg);
        assert(msg != 0);

        if( msg->getCompletionFlag() == RTSP_MSG_COMPLETE )
        {
            if (msg->getFirstWordUnknown())
            {
                //Here msg is still in base class, need to create derived class
                // and assign msg to the derived class
                LocalScopeAllocator lo;

                newMsg = RtspMsgParser::createRtspMsg(
                    (msg->getStartLine()).getData(lo), 
                    (msg->getStartLine()).length());
                if (newMsg != 0)
                   (*newMsg) = *msg;
                msg = newMsg;
                  
            }

            cpLog( LOG_DEBUG, "Adding RTSP msg to fifo" );
            if (msg != 0)
            {
                msg->setTransConnPtr( tcpBufferObj );
                myRecvFifo.add( msg );
            }
            mySavedRtspMsg = 0;
        }
        else
        {
            if( bytesDecoded != bytesNeedtoDecode )
            {
                // The incomplete msg is in the middle of the buffer
                // which should not happen
                throw RtspBadDataException( "Bad RTSP message",
                                            __FILE__, __LINE__, 0 );
            }
            else
            {
                // save this incomplete msg in this object
                cpLog( LOG_DEBUG_STACK, "Saved an incomplete msg" );
                mySavedRtspMsg = msg;
            }
        }
    }
    return true;
}


void RtspTcpBuffer::closeConnection()
{
    cpLog( LOG_DEBUG, "TCP Buffer connection closed for: %s",
           myConnection.getDescription().c_str() );
    myConnection.close();
}


bool RtspTcpBuffer::isAlive()
{
    return ( myKeepAliveCounter-- < 0 ) ? true : false;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

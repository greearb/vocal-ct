#ifndef RtspTcpBuffer_Hxx
#define RtspTcpBuffer_Hxx
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


static const char* const RtspTcpBuffer_hxx_Version =
    "$Id: RtspTcpBuffer.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspBadDataException.hxx"

#include "vtypes.h"
#include "Sptr.hxx"
#include "Fifo.h"
#include "Connection.hxx"
#include "VNetworkException.hxx"

using namespace Vocal;

class RtspMsg;

/** RtspTcpBuffer
    Receives a message from a tcp connection, and adds the rtsp messages into
    into a shraed recvFifo.
 */
class RtspTcpBuffer
{
    public:
        /** Constructor
         *  Process data on TCP connection
         *  @param conn TCP socket connection
         *  @param keepAliveCounter counter until self destruct
         *  @param recvFifo fifo to insert new RTSP messages
         *  @param bufferSize size of raw data buffer from socket
         */
        RtspTcpBuffer( Connection conn, int keepAliveCounter,
                       Fifo< Sptr<RtspMsg> >& recvFifo, int bufferSize = 1024 );
        /** Deconstructor */
        virtual ~RtspTcpBuffer();

        /** accessor function */
        int getMapIndex() const { return myMapIndex; }

        /** accessor function */
        Connection getConnection() const { return myConnection; }

        /** accessor function */
        string getDescription() const { return myConnection.getDescription(); }

        /** process data from TCP connect and add new RTSP message into recvFifo
         *  @return true if sucessful
         */
        bool processConnection( Sptr<RtspTcpBuffer> tcpBufferObj );

        /** closes the TCP connection */
        void closeConnection();
        
        /** true if this TCP connection has been active */
        bool isAlive();

    private:
        /** read TCP data into myCharBuf[0, bufferSize-1] 
         *  adds new RTSP message to fifo if one is complete;
         *  If it's not, save it in myRtspMsg and wait for future data to
         *  make it complete
         */
        bool readTcpData( Sptr<RtspTcpBuffer> tcpBufferObj )
        throw( VNetworkException&, RtspBadDataException& );

        /** map index this object is stored under */
        int myMapIndex;

        /** TCP socket connection */
        Connection myConnection;

        /** max keepa alive counter */
        int myMaxKeepAliveCounter;

        /** keep alive counter */
        int myKeepAliveCounter;

        /** receiving char buffer */
		char* myCharBuf;

        /** size of receiving char buffer */
		int myBufferSize;

        /** hold the incomplete msg */
        Sptr<RtspMsg> mySavedRtspMsg;

        /** recv fifo from RtspTransceiver */
        Fifo< Sptr<RtspMsg> >& myRecvFifo;

    protected:
        /** suppress copy constructor */
        RtspTcpBuffer( const RtspTcpBuffer& );
        /** suppress assignment operator */
        RtspTcpBuffer& operator=( const RtspTcpBuffer& );
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-labul . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif 


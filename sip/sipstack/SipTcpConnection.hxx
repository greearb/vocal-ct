#ifndef SIP_TCP_CONNECTION_HXX_
#define SIP_TCP_CONNECTION_HXX_

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

static const char* const SipTcpConnection_hxx_Version =
    "$Id: SipTcpConnection.hxx,v 1.5 2004/05/29 01:10:33 greear Exp $";

#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"
#include <Tcp_ServerSocket.hxx>
#include <list>
#include <queue>
#include <misc.hxx>
#include "RetransmitContents.hxx"

namespace Vocal
{

#define MAX_SIP_TCP_RCV_BUF 4096

class SipMsgContainer;

class NTcpStuff: public BugCatcher {
    public:
        ///
        NTcpStuff() : tcpConnection(0) { }

        ///
        virtual ~NTcpStuff() { }

        ///
        bool operator==(const  NTcpStuff& other) {
            return (tcpConnection->getConnId() ==
                    other.tcpConnection->getConnId());
        }

        bool needsToWrite();

        // This will consume all of d as long as we are within the limits
        // of the amount of data we will buffer.  If the socket is not immediately
        // writable, it will be buffered in this class, so calling code can be sure
        // that if the message is accepted, it will be transmitted if at all possible.
        int writeData(const Data& d);

        Sptr<Connection> getConnection() { return tcpConnection; }
        void setConnection(Sptr<Connection> c) { tcpConnection = c; }
        void setPeerPort(int pp) { peer_port = pp; }
        void setPeerIp(const char* pip) { peer_ip = pip; }
        void setPeerIp(const string& pip) { peer_ip = pip; }
        const string& getPeerIp() { return peer_ip; }

        bool isLive() { return tcpConnection->isLive(); }

        int tryWrite(fd_set* output_fds);

    protected:
        //  Connection holds all the needed send and receive buffers.
        Sptr < Connection > tcpConnection;

        // For receiving.
        string peer_ip;
        int peer_port;
};


class SipTcpConnection;

class NTcpConnInfo {
    public:
        ///
        NTcpConnInfo(SipTcpConnection* sc);
        ///
        virtual ~NTcpConnInfo();
        ///
        Sptr<NTcpStuff> setConnNPeerIp(int fd, Sptr < Connection > conn, const Data& ip);

        void createConnection(Sptr<Connection> conn);

        Sptr < NTcpStuff > createOrGetPersistentConnection(const NetworkAddress& nwaddr);

        // Adds a mapping to this destination.
        void notifyDestination(const string& dest_key, Sptr<NTcpStuff> connInfo);

        ///
        Sptr < NTcpStuff > getStatusMsgConn(Sptr < SipMsg > msg);
        ///
        void setStatusMsgConn(Sptr < SipMsg > msg, int fd);

        ///
        Sptr<NTcpStuff> getConnInfo(int fd);

        ///
        void doCleanup();

        void delConn(int fd);
        
        void delIdMapEntry(const SipTransactionId& id);
        
        int tcpReadOrAccept(int tcpfd, TcpServerSocket& tcpStack);

        virtual void writeTick(fd_set* output_fds);

        virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          uint64 now);

        virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           int& maxdesc, uint64& timeout, uint64 now);

    private:
        ///
        Sptr < NTcpStuff > getCurrent(int fd);
        
        // Keep various fast-lookup maps.
        map < int, Sptr < NTcpStuff > > myMap;
        map < SipTransactionId, Sptr < NTcpStuff > > idMap;
        map < string, Sptr < NTcpStuff > > myDestinationMap;


        list<int>  myCleanupList;

        Data nullData;

        SipTcpConnection* sip_conn; // Owner of this object, will process incomming msgs.
};


class SipTcpConnection: public BugCatcher
{
    public:
        /**
         * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
         */
        SipTcpConnection(const string& local_ip,
                         const string& local_dev_to_bind_to,
                         int port /* = SIP_PORT */, bool blocking);
        ///
        virtual ~SipTcpConnection();

        int send(Sptr<SipMsgContainer> msg, const Data& host,
                 const Data& port);

        const string getLocalIp() const;

        // May pull from fifo
        Sptr<SipMsgContainer> getNextMessage();


        virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                          uint64 now);

        virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                           int& maxdesc, uint64& timeout, uint64 now);

        TcpServerSocket& getTcpStack() { return mytcpStack; }

        void processMsgsIfReady(Sptr<NTcpStuff> conn);

        const string& getLocalDev() { return local_dev_to_bind_to; }
        const string& getLocalIp() { return local_ip_to_bind_to; }

    protected:
        // Read from socket.
        Sptr<SipMsgContainer> receiveMessage();

    private:

        Sptr < NTcpStuff > createRequestTransaction(Sptr<SipCommand> command);

        int prepareEvent(Sptr<SipMsgContainer> sipMsg);

        int sendMain(uint64& now);
        int processMain();

        TcpServerSocket mytcpStack;
        NTcpConnInfo tcpConnInfo;

        list < Sptr <SipMsgContainer> > sendQ;
        list <int > processFifo;
        list < Sptr <SipMsgContainer> > rcvFifo;

        string local_ip_to_bind_to;
        string local_dev_to_bind_to;
};


int isFullMsg(const string& str);
 
} // namespace Vocal

#endif

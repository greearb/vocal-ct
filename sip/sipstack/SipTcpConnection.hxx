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
    "$Id: SipTcpConnection.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "SipMsg.hxx"
#include "Sptr.hxx"
#include "TransceiverSymbols.hxx"

#include "SipTransactionLevels.hxx"
#include "SipTransactionGC.hxx"
#include <Tcp_ServerSocket.hxx>
#include <list>

namespace Vocal
{
    
class SipTcpConnection_impl_;
class SipMsgContainer;

class SipTcpConnection: public BugCatcher
{
    public:
        /**
         * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
         * TODO:  Who owns the 'fifo' memory?
         */
        SipTcpConnection(list < Sptr<SipMsgContainer> > * fifo,
                         const string& local_ip,
                         const string& local_dev_to_bind_to,
                         int port = SIP_PORT);
        ///
        virtual ~SipTcpConnection();

        ///
        void send(SipMsgContainer* msg, const Data& host="",
                             const Data& port="");

        const string getLocalIp() const;

    private:
        ///
        SipTcpConnection_impl_* impl_;
};


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

        ///
        Sptr < Connection > tcpConnection;
        ///
        Data tcpBuf;
        ///
        Data sender_ip;
        int sender_port;
};

typedef int TcpFd;


class NTcpConnInfo
{
    public:
        ///
        NTcpConnInfo();
        ///
        ~NTcpConnInfo();
        ///
        void setConnNSenderIp(int fd, Sptr < Connection > conn, const Data& ip);
        ///
        Sptr < Connection > getStatusMsgConn(Sptr < SipMsg > msg);
        ///
        void setStatusMsgConn(Sptr < SipMsg > msg, int fd);
        ///
        int setTCPFds(fd_set* fdSet);

        ///
        Sptr<NTcpStuff> getConnInfo(int fd);

        ///
        void doCleanup();

    private:
        ///
        Sptr < NTcpStuff > getCurrent(int fd);
        ///
        void delConn(int fd);
        ///
        void delIdMapEntry(const SipTransactionId& id);
        ///
        int tcpReadOrAccept(int tcpfd, TcpServerSocket* tcpStack);
        ///
        map < TcpFd, Sptr < NTcpStuff > > myMap;
        ///
        map < SipTransactionId, Sptr < NTcpStuff > > idMap;

        ///
        list<int>  myCleanupList;

        Data nullData;
        ///
        friend class SipTcpConnection_impl_;
};


class SipTcpConnection_impl_
{
    public:
        ///
        SipTcpConnection_impl_(list < Sptr <SipMsgContainer> > * fifo,
                               const string& local_ip,
                               const string& local_dev_to_bind_to,
			       int port = SIP_PORT);
        ///
        ~SipTcpConnection_impl_();
        ///
        void send(SipMsgContainer *msgPtr, const Data& host,
                  const Data& port);

        const string getLocalIp() const;

    private:
        ///
        void readOnFdSet(fd_set* fdSet, TcpServerSocket* tcpStack);
        ///
        Sptr < Connection > 
        createRequestTransaction(Sptr<SipCommand> command);

        ///
        Sptr < Connection > 
        createOrGetPersistentConnection(NetworkAddress);

        ///
        void processMsgsIfReady(int fd);
        ///
        void prepareEvent(SipMsgContainer* sipMsg);
        ///
        static void* receiveThreadWrapper(void* p);
        ///
        void* receiveMain();
        ///
        static void* sendThreadWrapper(void* p);
        ///
        void* sendMain();
        ///
        static void* processThreadWrapper(void* p);
        ///
        void* processMain();
        ///
        TcpServerSocket mytcpStack;
        ///
        NTcpConnInfo tcpConnInfo;
        ///
        list <SipMsgContainer*> sendFifo;
        list <int > processFifo;
        ///
        list < Sptr <SipMsgContainer> >* recFifo;
        ///
        bool shutdownNow;
        ///
        map <string, Sptr < Connection > > myDestinationMap;

        string local_ip_to_bind_to;
        string local_dev_to_bind_to;

};

int isFullMsg(const string& str);
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

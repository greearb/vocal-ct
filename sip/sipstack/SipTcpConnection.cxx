/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const SipTcpConnection_cxx_Version =
    "$Id: SipTcpConnection.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "global.h"
#include "InviteMsg.hxx"
#include "Lock.hxx"
#include "NetworkAddress.h"
#include "SipCommand.hxx"
#include "SipContact.hxx"
#include "SipTcpConnection.hxx"
#include "SipTransactionId.hxx"
#include "SipTransactionLevels.hxx"
#include "SipVia.hxx"
#include "SystemInfo.hxx"
#include "StatusMsg.hxx"
#include "Tcp_ClientSocket.hxx"
#include "Tcp_ServerSocket.hxx"
#include "ThreadIf.hxx"
#include "VFilter.hxx"
#include "VFunctor.hxx"
#include "VNetworkException.hxx"
#include "VThread.hxx"
#include "VThreadPool.hxx"
#include "support.hxx"
#include "symbols.hxx"

using namespace Vocal;
using namespace Vocal::Threads;

//Note: If client or server needs to keep a persistent TCP connection for
//all calls, define USE_PERSISTENT_TCP.If want to have Transaction based
//TCP connections, comment out the following line.
#define USE_PERSISTENT_TCP 1


Sptr < Connection >
NTcpConnInfo::getStatusMsgConn(Sptr < SipMsg > msg)
{
    //form the trans id.
    SipTransactionId newId(*msg);

    Lock lLock(mapMutex);

    map < SipTransactionId, Sptr < NTcpStuff > > ::iterator i;
    cpLog(LOG_DEBUG, "NTcpConnInfo - IDMap size:%d", idMap.size());
    i = idMap.find(newId);

    if (i != idMap.end())
    {
        // found it, do something
        return i->second->tcpConnection;
    }
    else
    {
        cpLog(LOG_WARNING,
              "could not find TCP connection for status msg (%s) reply", msg->encode().logData());
        return 0;
    }
}

void
NTcpConnInfo::doCleanup()
{
    Lock l(cleanupMutex);
    while(myCleanupList.size())
    {
        int id = myCleanupList.front();
        myCleanupList.pop_front();
        delConn(id);
    }
}


void
NTcpConnInfo::setStatusMsgConn(Sptr < SipMsg > msg, int fd)
{
    Sptr < NTcpStuff > t = getCurrent(fd);

    Lock lLock(mapMutex);
    if (t != 0)
    {
        SipTransactionId id(*msg);
        idMap[id] = t;
        cpLog(LOG_DEBUG, "NTcpConnInfo::setStatusMsgConn, idMapSize:%d", idMap.size() );
    }
    else
    {
        cpLog(LOG_WARNING, "could not find valid tcp connection for message");
    }
}



NTcpConnInfo::NTcpConnInfo()
   : nullData(TransNull)
{
}


NTcpConnInfo::~NTcpConnInfo()
{
    mutex.lock();
    myMap.clear();
    idMap.clear();
    mutex.unlock();
}


void
NTcpConnInfo::delConn(int fd)
{
    Lock l(mutex);
    map < TcpFd, Sptr < NTcpStuff > > ::iterator i = myMap.find( fd );
    Sptr < NTcpStuff > del;

    if ( i == myMap.end() )
    {
        return; 	// not found
    }

    del = i->second;
    myMap.erase(i);
    cpLog(LOG_DEBUG_STACK, "After del size:%d" ,myMap.size());
    if (del != 0)
    {
        //Cleanup the transactionMap for the closed connection
        Lock      l2(mapMutex);
        map <SipTransactionId, Sptr < NTcpStuff > > ::iterator j;
        map <SipTransactionId, Sptr < NTcpStuff > > ::iterator delItr;

        j = idMap.begin();

        while (j != idMap.end())
        {
            if (j->second == del) 
            {
                delItr= j++; 
                idMap.erase(delItr);
            }
            else j++;
        }
        del->tcpConnection->close();
    }
}

void
NTcpConnInfo::delIdMapEntry(const SipTransactionId&  id)
{
    cpLog(LOG_DEBUG, "Deleting IdMap entry, size:%d", idMap.size());
    // erase from the transactionId if needed
    Lock l2(mapMutex);
    map < SipTransactionId, Sptr < NTcpStuff > > ::iterator j;

    j = idMap.find(id);

    if (j != idMap.end())
    {
        idMap.erase(j);
    }
}


Sptr < NTcpStuff >
NTcpConnInfo::getCurrent(int fd)
{
    Lock lLock(mutex);
    Sptr < NTcpStuff > myObj;

    map < TcpFd, Sptr < NTcpStuff > > ::iterator i = myMap.find( fd );

    if ( i == myMap.end() )
    {
        myObj = 0;
    }
    else
    {
        myObj = i->second;
    }

    return myObj;
}


void
NTcpConnInfo::setConnNSenderIp(int fd, Sptr < Connection > conn, const Data& ip)
{
    Lock l(mutex);
    Sptr < NTcpStuff > myTcp;

    myTcp = new NTcpStuff;
    if (myTcp != 0)
    {
        myTcp->tcpConnection = conn;
        NetworkAddress na(ip);
        myTcp->sender_ip = na.getIpName();
        myTcp->sender_port = na.getPort();
    }
    myMap[fd] = myTcp;
    cpLog(LOG_DEBUG_STACK, "Map size %d after adding %d" ,myMap.size(), fd);
}


Sptr<NTcpStuff> 
NTcpConnInfo::getConnInfo(int fd)
{
    return(getCurrent(fd));
}


int
NTcpConnInfo::tcpReadOrAccept(int tcpfd, TcpServerSocket* tcpStack)
{
    if (!tcpfd)
    {
        return -1;
    }

    Sptr < Connection > newconn = 0;
    Sptr <NTcpStuff> connInfo = 0;

    cpLog(LOG_DEBUG_STACK, "Received tcp msg on fd: %d", tcpfd);

    if (tcpfd == tcpStack->getServerConn().getConnId())
    {
        //this has happened on the default server socket.
        //create a new connection and accept on it.
        newconn = new Connection;

        int clientId = 0;

        // catch exception thrown when we run out of fds
        try
        {
            clientId = tcpStack->accept(*newconn);
        }
        catch (VNetworkException &)
        {
            return -1;
        }
        
        cpLog(LOG_DEBUG_STACK,"*****  New Connection = %s *****\n", 
              newconn->getDescription().c_str());

        // set the sender ip   
        string ipName = newconn->getIp();

        //cache this in the tcpMap.
        setConnNSenderIp(clientId, newconn, ipName);

        //
        cpLog(LOG_DEBUG_STACK, "accepting new connection: ", clientId);

        // need to redo this conn object, etc.
        tcpfd = newconn->getConnId();
        connInfo = getConnInfo(tcpfd);
        assert(connInfo != 0);

        //overwrite the conn object.
    }
    else
    {
        connInfo = getConnInfo(tcpfd);
        assert(connInfo != 0);
        newconn = connInfo->tcpConnection;   //it already has the
                                             //correct connection.
    }

    int numBytes;

    if (newconn != 0)
    {
        char buf [1025];
        buf[0] = '\0';

        // read off some data, possibly break
        Lock l(connInfo->myMutex);
        Data& myBuf = connInfo->tcpBuf;

        numBytes = newconn->readn(buf, 1024);

        if (numBytes <= 0)
        {
            // probably should close here!
            cpLog(LOG_DEBUG_STACK, "closing fd");
            delConn(tcpfd);
            return -1;
        }

        buf[numBytes] = '\0';

        cpLog(LOG_DEBUG_STACK, "appending data: ");
        cpLog(LOG_DEBUG_STACK, "::::::::::" );
        cpLog(LOG_DEBUG_STACK, "%s", buf);
        cpLog(LOG_DEBUG_STACK, "::::::::::");

        myBuf += buf;
        cpLog(LOG_DEBUG_STACK, "has read data, will check");
        cpLog(LOG_DEBUG_STACK, "%s ", myBuf.logData());
    }
    return tcpfd;
}


int
NTcpConnInfo::setTCPFds(fd_set* fdSet)
{
    Lock lLock(mutex);

    map < TcpFd, Sptr < NTcpStuff > > ::iterator i;
    int maxfd = -1;

    i = myMap.begin();

    while (i != myMap.end())
    {
        int mapfd = i->first;
	if(mapfd <= 0) continue;

        FD_SET(mapfd, fdSet);

        if (mapfd > maxfd)
        {
            maxfd = mapfd;
        }
        ++i;
    }

    // special for connected but unaccepted connections
    return maxfd;
}


SipTcpConnection_impl_::SipTcpConnection_impl_(Fifo <SipMsgContainer*> * fifo,
                                               const string& local_ip,
                                               const string& _local_dev_to_bind_to,
                                               int port)
        :
        mytcpStack(local_ip, _local_dev_to_bind_to, port),
        sendFifo(),
        recFifo(fifo),
        shutdownNow(false),
        local_ip_to_bind_to(local_ip),
        local_dev_to_bind_to(_local_dev_to_bind_to)
{

    // put the tcpStack into the tcpConnInfo map

    int fd = mytcpStack.getServerConn().getConnId();
    string mh = local_ip;
    if (mh.size() == 0) {
        mh = Vocal::theSystem.gethostAddress(); //OK
    }
    Sptr < Connection > myConn = new Connection(mytcpStack.getServerConn());
    tcpConnInfo.setConnNSenderIp(fd, myConn, mh);

    sendThread.spawn(sendThreadWrapper, this);
    receiveThread.spawn(receiveThreadWrapper, this);
    processThread.spawn(processThreadWrapper, this);

}


SipTcpConnection_impl_::~SipTcpConnection_impl_()
{
    // also need to wait on the appropriate threads after they've been killed

    // wake the sender with an appropriate message

    shutdownNow = true;
    sendFifo.add(0);
    processFifo.add(0);

    sendThread.join();
    receiveThread.join();
    processThread.join();
}


const string SipTcpConnection_impl_::getLocalIp() const {
    string rv = mytcpStack.getSpecifiedLocalIp();
    if (rv.size()) {
        return rv;
    }
    else {
        return theSystem.gethostAddress(); //OK
    }
}




void
SipTcpConnection_impl_::send(SipMsgContainer* msg, const Data& host,
                             const Data& port)
{

    Data nhost;
    int nport=0;

    if(host.length())
    {
        nhost = host;
        nport = port.convertInt();
    }
    else
    {
        Sptr<SipCommand> command;
        if(command.dynamicCast(msg->msg.in)!=0)
        {
            Sptr<SipUrl> sipDest;
            sipDest.dynamicCast(command->getRequestLine().getUrl());
            if(sipDest != 0)
            {
                nhost = sipDest->getMaddrParam();
                if(!nhost.length()) nhost = sipDest->getHost();
                nport = sipDest->getPort().convertInt();
                cpLog( LOG_DEBUG_STACK,
                       "got host from request line: %s:%d",
                       nhost.logData(), nport);
                if ( nport == 0 )
                {
                    nport = SIP_PORT;
                    cpLog( LOG_DEBUG_STACK, "Changed port to %d", nport);
                }
            }
        }
    }
    if(nhost.length())
    {
        try
        {
            Lock lo(msg->myLock);
            msg->msg.netAddr = new NetworkAddress(nhost.convertString(),
                                              nport); 
        }
        catch(NetworkAddress::UnresolvedException& e)
        {
            cpLog(LOG_ERR, "Destination (%s) is not reachable, reason:%s.",
                  nhost.logData(), e.getDescription().c_str());
        }
    }
    sendFifo.add(msg);
}


void*
SipTcpConnection_impl_::receiveThreadWrapper(void* p)
{
    return static_cast < SipTcpConnection_impl_* > (p)->receiveMain();
}


void*
SipTcpConnection_impl_::processThreadWrapper(void* p)
{
    return static_cast < SipTcpConnection_impl_* > (p)->processMain();
}


void*
SipTcpConnection_impl_::sendThreadWrapper(void* p)
{
    return static_cast < SipTcpConnection_impl_* > (p)->sendMain();
}


void*
SipTcpConnection_impl_::sendMain()
{
    while (1)
    {
        SipMsgContainer* sipMsg = sendFifo.getNext();

        if ( !shutdownNow )
        {
            int type = sipMsg->msg.type;
            // If connection is persistent, try connecting atleast these times
            for(int i = 0; i < 3; i++)
            {
                try 
                {
                    prepareEvent(sipMsg);
                    break;
                }
                catch(VNetworkException& e)
                {
                    cpLog(LOG_ERR, "Network error, reason %s", e.getDescription().c_str());
                    if(sipMsg->msg.in->nextHopIsAProxy())
                    {
                       continue;
                    }
                    else
                    {
                        //TODO send error to application ??
                        break;
                    }
                }
            }
	    /// also inform the cleanup
	    if(type != SIP_INVITE)
            {
                SipTransactionGC::instance()->
                    collect(sipMsg, MESSAGE_CLEANUP_DELAY);
            }
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


void*
SipTcpConnection_impl_::processMain()
{
    while (1)
    {
        int fdToRead = processFifo.getNext();

        if ( !shutdownNow )
        {
            processMsgsIfReady(fdToRead);
        }
        else
        {
            return 0;
        }
    }
    return 0;
}


Sptr < Connection >
SipTcpConnection_impl_::createRequestTransaction(Sptr < SipCommand > command)
{
    //come here only if this is a command.
    assert (command != 0);
    Sptr<SipUrl> dest = command->postProcessRouteAndGetNextHop();
    assert (dest != 0);
    
    Data host = dest->getMaddrParam();
    if(host == "") host = dest->getHost();
    int port = dest->getPort().convertInt();
    cpLog( LOG_DEBUG_STACK,"Using destination: %s:%d", host.logData(), port);
    
    if ( port == 0 )
    {
        port = SIP_PORT;
        cpLog( LOG_DEBUG_STACK, "Changed port to %d", port);
    }

    NetworkAddress nwaddr(host.convertString(), port);
    TcpClientSocket clientSocket(nwaddr, local_dev_to_bind_to,
                                 local_ip_to_bind_to, false );
    try
    {
        clientSocket.connect();
    }
    catch (VNetworkException& exception)
    {
        cpLog(LOG_ERR, exception.getDescription().c_str());
    }
    Sptr<Connection> connection = new Connection(clientSocket.getConn());

    cpLog(LOG_DEBUG_STACK, "Adding non-persistent connection %d",connection->getConnId());

    char buf[56];
    ostrstream strm(buf,56);
    strm << nwaddr.getIpName() << ":" << nwaddr.getPort() << ends;

    tcpConnInfo.setConnNSenderIp(connection->getConnId(),connection,strm.str());
    return connection;
}



Sptr < Connection >
SipTcpConnection_impl_::createOrGetPersistentConnection(
    NetworkAddress nwaddr)
{
    //come here only if this is a command.
    //	NetworkAddress nwaddr(host.getData(lo), port);
    string ipName = nwaddr.getIpName().convertString();
    char buf[56];
    sprintf(buf, "%s:%d", ipName.c_str(), nwaddr.getPort());
    
    if(myDestinationMap.count(buf))
    {
        //Found an existing connection
        cpLog( LOG_DEBUG_STACK, "Found existing connection for %s", buf );
        if(myDestinationMap[buf]->isLive())
        {
            return myDestinationMap[buf];
        }
        else
        {
               //close the connection , remove it from the map that
            //would re-establish it
            cpLog(LOG_ERR, "Stale connection from (%s), re-connecting..", 
                  buf);
            myDestinationMap.erase(buf);
        }
    }
    
    cpLog( LOG_DEBUG_STACK, "Creating persistent connection for %s", buf );
    TcpClientSocket clientSocket(nwaddr, local_dev_to_bind_to,
                                 local_ip_to_bind_to, false );
    try
    {
        clientSocket.connect();
        Sptr < Connection > connection = new Connection(clientSocket.getConn());
	
        // insert the appropriate connection
        myDestinationMap[buf] = connection;	
        cpLog(LOG_DEBUG_STACK, "Adding persistent connection %d" , 
              connection->getConnId());
        tcpConnInfo.setConnNSenderIp(connection->getConnId(), connection, buf);
        return connection;
    }
    catch (VNetworkException& exception)
    {
        cpLog(LOG_ERR, exception.getDescription().c_str());
        myDestinationMap.erase(buf);
        return 0;
    }
}



void
SipTcpConnection_impl_::prepareEvent(SipMsgContainer* sipMsg)
{
    Sptr < Connection > conn;
    LocalScopeAllocator lo;

    // create or use the current TCP details as appropriate
    //If for a duplicate request a previous response is being 
    //sent, there in no information on transport to send to.
    if(sipMsg->msg.in == 0) 
    {
        cpLog( LOG_DEBUG_STACK, "retransmission of the message :%s being ignored", sipMsg->msg.out.logData() );
        return; 
    }

    int type = sipMsg->msg.in->getType();

    Sptr < SipCommand > sipCommand;
    sipCommand.dynamicCast(sipMsg->msg.in);

    Sptr < StatusMsg > statusMsg;
    statusMsg.dynamicCast(sipMsg->msg.in);

    if ((type == SIP_STATUS) && (statusMsg != 0))
    {
        conn = tcpConnInfo.getStatusMsgConn(sipMsg->msg.in);
    }
    else if (sipCommand != 0)
    {
        if (sipMsg->msg.netAddr == 0){
            cpLog(LOG_WARNING, "TCP Send is NULL");
            return;
        }

        // this is a request
#ifdef USE_PERSISTENT_TCP
////        conn = createOrGetPersistentConnection(sipCommand);
        conn = createOrGetPersistentConnection(*(sipMsg->msg.netAddr));
#else
        if(sipCommand->nextHopIsAProxy())
        {
	  
////            conn = createOrGetPersistentConnection(sipCommand);
            conn = createOrGetPersistentConnection(*(sipMsg->msg.netAddr));
        }
        else
        {
             conn = createRequestTransaction(sipMsg);
        }
#endif
    }
    else
    {
        // this is an error
        cpLog(LOG_ERR, "API violation: attempt to send null message");
        return;
    }
    // at this point, you need to transmit
    if (conn != 0)
    {
        sipMsg->msg.out =  sipMsg->msg.in->encode();
        string dataString = sipMsg->msg.out.getData(lo);
        cpLog(LOG_INFO, "sending TCP SIP msg:\n\n-> %s\n\n%s",
              conn->getDescription().c_str(), dataString.c_str());
        cpLog(LOG_DEBUG_STACK, "TCP fd: %d", conn->getConnId());

        conn->writeData(dataString);

#ifdef USE_PERSISTENT_TCP
        Sptr<StatusMsg> statusMsg;
        statusMsg.dynamicCast(sipMsg->msg.in);
        if((statusMsg != 0) && (statusMsg->getStatusLine().getStatusCode() >= 200))
        {
            //Transaction is over, remove the entry from the ID map
            SipTransactionId id(*statusMsg);
            tcpConnInfo.delIdMapEntry(id);
        }
#else
        if((type == SIP_ACK) && !(sipMsg->msg.in->nextHopIsAProxy()))
        {
            Lock l(tcpConnInfo.cleanupMutex);
            tcpConnInfo.myCleanupList.push_back(conn->getConnId());
        }
#endif
    }
}


string
get_next_line(const string& str, string::size_type* old_pos)
{
    string myStr;

    string::size_type pos = str.find('\n', *old_pos);
    if (pos != string::npos)
    {
        myStr = str.substr(*old_pos, pos - (*old_pos));
        *old_pos = pos + 1;
    }
    else
    {
        myStr = str.substr(*old_pos, str.length() - *old_pos);
        *old_pos = pos;
    }
    return myStr;
}


int
getContentLength(const string& str)
{
    LocalScopeAllocator lo;
    // given that this string is a content-length then do something
    static string contentLengthStr = str2lower(CONTENT_LENGTH.getData(lo));
    static string contentLengthStr2 = str2lower(CONTENT_LENGTH_SHORT.getData(lo));
   
    int pos = 0; 
    unsigned len = str.length();
    if(len > contentLengthStr.length())
         len = contentLengthStr.length();
    string x = str2lower(str.substr(0,len));

    if (x.find(contentLengthStr) == 0) 
    {
        // this is right -- scan
        pos = contentLengthStr.length();
    }
    else if (x.find(contentLengthStr2) == 0) 
    {
        pos = contentLengthStr2.length();
    }
    else
    {
        // not the content-length
        return -1;
    }
    string::size_type newPos = str.find_first_not_of("0123456789", pos + 1);
    if (newPos == string::npos)
    {
        newPos = str.length();
    }
    string contentLen = str.substr(pos + 1, newPos - (pos + 1));
    return (atoi(contentLen.c_str()));
}


bool
isBlankLine(const string& str)
{
    // a line which has only whitespace is NOT a blank line for the
    // purposes of separating the headers from the body in a SIP
    // message, only a line which has ONLY the CRLF.  we also accept
    // lines w/ LF only for robustness.

    if((str == "\r") || (str == ""))
    {
        return true;
    }
    else
    {
        return false;
    }

#if 0
    if (str.find_first_not_of(" \t\r\n") == string::npos)
    {
        return true;
    }
    else
    {
        return false;
    }
#endif
}


int 
Vocal::isFullMsg(const string& str)
{
    string::size_type pos = 0;

    int contentLen = -1;
    bool foundBlank = false;
    bool foundNonBlank = false;
    LocalScopeAllocator lo;
    pos = str.find(CONTENT_LENGTH.getData(lo));
    if(pos == string::npos)
    {
        pos = str.find(CONTENT_LENGTH_SHORT.getData(lo));
        if(pos == string::npos) pos = 0;
    }

    if(pos != 0)
    {
        foundNonBlank = true;
    }

    while (!foundBlank && (pos != string::npos))
    {
        string myLine = get_next_line(str, &pos);

        if(pos != string::npos)
        {
            if (contentLen == -1)
            {
                // check against content-length
                contentLen = getContentLength(myLine);
            }
            if(isBlankLine(myLine))
            {
                if(foundNonBlank)
                {
                    foundBlank = true;
                    break;
                }
            }
            else
            {
                foundNonBlank = true;
            }
        }
    } // While

    if (pos == string::npos)
    {
        pos = str.length();
    }

    if (foundBlank)
    {
        // then make sure that the remaining character count is the same
        int remainingBytes = str.length() - pos;

        // if there have been no blank lines, ignore these lines
        if (contentLen == -1 && foundNonBlank)
        {
            contentLen = 0;
        }
        if ((remainingBytes >= contentLen) && (str.length() > 0))
        {
            return (pos + contentLen);
        }
    }
    return -1;
}


void
SipTcpConnection_impl_::processMsgsIfReady(int fd)
{
    Data data;
    string ipName;

    // read off some data, possibly break
    Sptr<NTcpStuff> connInfo = tcpConnInfo.getConnInfo(fd);
    if(connInfo == 0)
    {
        // Too late , connection is closed already
        return;
    }
    Lock l(connInfo->myMutex);
    LocalScopeAllocator lo;
    Data& myBufD = connInfo->tcpBuf;
    ipName = connInfo->sender_ip.getData(lo);
    string myBuf = myBufD.getData(lo);
    int bufLen = isFullMsg(myBuf);
    cpLog(LOG_DEBUG_STACK, "Raw Msg read: [%s]" , myBuf.c_str());
    while (bufLen != -1)
    {
        string msgBuf = myBuf.substr(0, bufLen);
        string remainingBuf =
            myBuf.substr(bufLen, myBuf.length() - bufLen);
        myBufD = Data(remainingBuf);

        data = Data(msgBuf);

        SipMsgContainer* sipMsg = new SipMsgContainer;


	sipMsg->msg.out = data;
        sipMsg->msg.in = SipMsg::decode( data, getLocalIp());

        if (sipMsg->msg.in != 0)
        {
            //store the receivedIPName.
            cpLog(LOG_DEBUG_STACK, "*** getSenderIP = %s***", ipName.c_str());
            sipMsg->msg.in->setReceivedIPName( ipName );
            cpLog(LOG_INFO, "Received UDP Message :\n\n<- HOST[%s]\n\n%s",ipName.c_str(),  data.logData());

            Sptr<StatusMsg> statusMsg;
            statusMsg.dynamicCast(sipMsg->msg.in);
            if ((sipMsg->msg.in->getType() != SIP_STATUS) &&
                (sipMsg->msg.in->getType() != SIP_ACK))
            {
                // save in the map
                tcpConnInfo.setStatusMsgConn(sipMsg->msg.in, fd);
                if(sipMsg->msg.in->getType() == SIP_REGISTER)
                {
                   //If request is a REGISTER message, map the contact
                   //host port with TCP socket, so that future
                   //requests to the contact would follow the same
                   //TCP connection
                   Data rData = connInfo->tcpConnection->getDescription();
                   cpLog( LOG_DEBUG_STACK, "Received from %s", rData.logData() );
                   int numContacts = sipMsg->msg.in->getNumContact();
                   if( numContacts )
                   {
                       const SipContact& contact = sipMsg->msg.in->getContact( numContacts-1 );
                       Sptr<SipUrl> sUrl;
                       sUrl.dynamicCast( contact.getUrl() );
                       if( sUrl != 0 )
                       {
                           Data tmp = sUrl->getHost();
                           tmp += ":";
                           tmp += sUrl->getPort();
                           string cAddr(tmp.getData(lo));
                           cpLog( LOG_DEBUG_STACK, "fd %d for address %s", fd, cAddr.c_str() );
                           myDestinationMap[ cAddr ] = new Connection( fd );
                       }
                   }
                }
            }
            else if((statusMsg != 0) && 
                    statusMsg->getStatusLine().getStatusCode() >= 200)
            {
#ifndef USE_PERSISTENT_TCP
                string iName = connInfo->tcpConnection->getDescription();
                if(myDestinationMap.count(iName) == 0)
                {
                    cpLog(LOG_DEBUG_STACK, 
                          "Deleting non-persistent connection %d", fd);
                    //Non-persistent connection, close
                    tcpConnInfo.delConn(fd);
                }
                //Do the cleanup also
                tcpConnInfo.doCleanup();
#endif
             }
           
            
            recFifo->add(sipMsg);
        }
	else
	  {
	    delete sipMsg;
	  }
        myBuf = myBufD.getData(lo);
        bufLen = isFullMsg(myBuf);
    } // while
}


void
SipTcpConnection_impl_::readOnFdSet(fd_set* fdSet, TcpServerSocket* tcpStack)
{
    tcpConnInfo.mutex.lock();
    int tcpreadfd = -1;
    list<int> fdList;

    map < TcpFd, Sptr < NTcpStuff > > ::iterator i = tcpConnInfo.myMap.begin();
    while (i != tcpConnInfo.myMap.end())
    {
        int mapfd = i->first;

        if (FD_ISSET(mapfd, fdSet))
        {
            FD_CLR(mapfd, fdSet);
            fdList.push_back(mapfd);
        }
        ++i;
    }
    tcpConnInfo.mutex.unlock();
    for(list<int>::iterator itr = fdList.begin(); itr != fdList.end();
             itr++)
    {
        int mapfd = (*itr);
        cpLog(LOG_DEBUG_STACK, "reading tcp data on %d", mapfd);
        tcpreadfd =  tcpConnInfo.tcpReadOrAccept(mapfd, tcpStack);
        if(tcpreadfd > 0) processFifo.add(tcpreadfd);
    }
}


void* 
SipTcpConnection_impl_::receiveMain()
{
    fd_set netFD;

    while (1)
    {
        FD_ZERO (&netFD);
        int maxfd = tcpConnInfo.setTCPFds( &netFD );

        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        int selret = select(maxfd + 1, &netFD, 0, 0, &timeout);

        if (selret > 0)
        {
            readOnFdSet(&netFD, &mytcpStack);
        }
	else if (selret < 0)
        {
            // do something
            if ( errno != EINTR )
            {
	        // this should be an error
                cpLog(LOG_ALERT, "select() failed, reason: %s, %d", 
                      strerror(errno), errno);
            }
        }
	else if (selret == 0)
        {
            if (shutdownNow)
            {
                return 0;
            }
        }
    }
    return 0;
}


SipTcpConnection::SipTcpConnection( Fifo <SipMsgContainer* > * fifo,
                                    const string& local_ip,
                                    const string& local_dev_to_bind_to,
                                    int port)
{
    impl_ = new SipTcpConnection_impl_(fifo, local_ip, local_dev_to_bind_to, port);
}


SipTcpConnection::~SipTcpConnection()
{
    delete impl_;
}


void
SipTcpConnection::send(SipMsgContainer* msg, const Data& host,
                             const Data& port)
{
    impl_->send(msg, host, port);
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

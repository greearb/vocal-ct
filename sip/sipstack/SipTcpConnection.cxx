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
    "$Id: SipTcpConnection.cxx,v 1.4 2004/05/27 04:32:18 greear Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "global.h"
#include "InviteMsg.hxx"
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
#include "VFilter.hxx"
#include "VFunctor.hxx"
#include "VNetworkException.hxx"
#include "support.hxx"
#include "symbols.hxx"


using namespace Vocal;

//Note: If client or server needs to keep a persistent TCP connection for
//all calls, define USE_PERSISTENT_TCP.If want to have Transaction based
//TCP connections, comment out the following line.
#define USE_PERSISTENT_TCP 1


Sptr < NTcpStuff >
NTcpConnInfo::getStatusMsgConn(Sptr < SipMsg > msg)
{
    //form the trans id.
    SipTransactionId newId(*msg);

    map < SipTransactionId, Sptr < NTcpStuff > > ::iterator i;
    cpLog(LOG_DEBUG, "NTcpConnInfo - IDMap size:%d", idMap.size());
    i = idMap.find(newId);

    if (i != idMap.end()) {
        // found it, do something
        return i->second;
    }
    else {
        cpLog(LOG_WARNING,
              "could not find TCP connection for status msg (%s) reply",
              msg->encode().logData());
        return 0;
    }
}

void
NTcpConnInfo::doCleanup()
{
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



NTcpConnInfo::NTcpConnInfo(SipTcpConnection* tc)
   : nullData(TransNull)
{
    sip_conn = tc;
}


NTcpConnInfo::~NTcpConnInfo()
{
    myMap.clear();
    idMap.clear();
}


void
NTcpConnInfo::delConn(int fd)
{
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

void NTcpConnInfo::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        uint64 now) {
    int rv;
    map < int, Sptr < NTcpStuff > > ::iterator i = myMap.begin();
    while (i != myMap.end()) {
        int mapfd = i->first;
        if (FD_ISSET(mapfd, input_fds)) {
            cpLog(LOG_DEBUG_STACK, "reading tcp data on %d", mapfd);
            rv = tcpReadOrAccept(mapfd, sip_conn->getTcpStack());
            if (rv > 0) {
                sip_conn->processMsgsIfReady(i->second);
            }
        }
        ++i;
    }
}//tick


int NTcpConnInfo::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                         int& maxdesc, uint64& timeout, uint64 now) {

    map < int, Sptr < NTcpStuff > > ::iterator i;
    i = myMap.begin();
    while (i != myMap.end()) {
        int mapfd = i->first;
	if (mapfd <= 0)
            continue;

        Sptr<NTcpStuff> conn = i->second;

        FD_SET(mapfd, *input_fds); // always check for reads
        FD_SET(mapfd, *exc_fds);   // always check for exceptions

        if (conn->needsToWrite()) {
            FD_SET(mapfd, *output_fds); // Check for writes too
        }

        if (mapfd > maxdesc) {
            maxdesc = mapfd;
        }
        ++i;
    }
    return 0;
}





int NTcpConnInfo::tcpReadOrAccept(int tcpfd, TcpServerSocket& tcpStack) {

    Sptr < Connection > newconn = 0;
    Sptr <NTcpStuff> connInfo = 0;

    cpLog(LOG_DEBUG_STACK, "Received tcp msg on fd: %d", tcpfd);

    if (tcpfd == tcpStack->getServerConn()->getConnId()) {
        //this has happened on the default server socket.
        //create a new connection and accept on it.
        newconn = new Connection(tcpStack->getServerConn()->isBlocking());

        int clientId = 0;

        // catch exception thrown when we run out of fds
        try {
            clientId = tcpStack->accept(*newconn);
        }
        catch (VNetworkException &) {
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
    }
    else {
        connInfo = getConnInfo(tcpfd);
        newconn = connInfo->tcpConnection;   //it already has the
                                             //correct connection.
    }

    int numBytes = 0;

    if (newconn != 0) {
        // read off some data, possibly break
        Data& myBuf = connInfo->tcpBuf;

        numBytes = newconn->readn(connInfo->rcvBuf, MAX_SIP_TCP_RCV_BUF);

        if (numBytes < 0) {
            cpLog(LOG_DEBUG_STACK, "closing fd");
            delConn(tcpfd);
            return -1;
        }
        if (numBytes == 0) {
            // Nothing to read, will try again next time.
            return 0;
        }

        connInfo->rcvBuf[numBytes] = '\0';

        cpLog(LOG_DEBUG_STACK, "appending data: ::::::::::%s::::::::::",
              connInfo->rcvBuf);

        myBuf += rcvBuf;
        cpLog(LOG_DEBUG_STACK, "has read data, will check: %s",
              myBuf.c_str());
    }
    return numBytes;
}


SipTcpConnection::SipTcpConnection(const string& local_ip,
                                   const string& _local_dev_to_bind_to,
                                   int port, bool blocking)
    : mytcpStack(local_ip, _local_dev_to_bind_to, port, blocking),
      tcpConnInfo(this),
      local_ip_to_bind_to(local_ip),
      local_dev_to_bind_to(_local_dev_to_bind_to) {

    // put the tcpStack into the tcpConnInfo map

    int fd = mytcpStack.getServerConn()->getConnId();
    string mh = local_ip;
    if (mh.size() == 0) {
        mh = Vocal::theSystem.gethostAddress(); //OK
    }
    tcpConnInfo.setConnNSenderIp(fd, mytcpStack.getServerConn(), mh);
}


SipTcpConnection::~SipTcpConnection() {
    // Nothing to do
}


const string SipTcpConnection::getLocalIp() const {
    string rv = mytcpStack.getSpecifiedLocalIp();
    if (rv.size()) {
        return rv;
    }
    else {
        return theSystem.gethostAddress(); //OK
    }
}


Sptr<SipMsgContainer>
SipTcpConnection::getNextMessage() {
    // First, see if we can pull something off of the fifo
    if (rcvFifo.size()) {
        Sptr<SipMsgContainer> rv = rcvFifo.front();
        rcvFifo.pop_front();
        return rv;
    }
    else {
        // Maybe can read from the Udp Stack
        return receiveMessage();
    }
}

int SipTcpConnection::send(Sptr<SipMsgContainer> msg, const Data& host,
                           const Data& port) {
    Data nhost;
    int nport=0;

    if (host.length()) {
        nhost = host;
        nport = port.convertInt();
    }
    else {
        Sptr<SipCommand> command((SipCommand*)(msg->msg.in.getPtr()));
        if (command.getPtr() != 0) {
            Sptr<SipUrl> sipDest((SipUrl*)(command->getRequestLine().getUrl().getPtr()));
            if (sipDest != 0) {
                nhost = sipDest->getMaddrParam();
                if (!nhost.length())
                    nhost = sipDest->getHost();
                nport = sipDest->getPort().convertInt();
                cpLog( LOG_DEBUG_STACK,
                       "got host from request line: %s:%d",
                       nhost.logData(), nport);
                if ( nport == 0 ) {
                    nport = SIP_PORT;
                    cpLog( LOG_DEBUG_STACK, "Changed port to %d", nport);
                }
            }
        }
    }

    if (nhost.length()) {
        try {
            msg->msg.netAddr = new NetworkAddress(nhost.convertString(),
                                                  nport); 
        }
        catch(NetworkAddress::UnresolvedException& e) {
            cpLog(LOG_ERR, "Destination (%s) is not reachable, reason:%s.",
                  nhost.logData(), e.getDescription().c_str());
            return -1;
        }
    }
    sendQ.push(msg);
    return 0;
}


Sptr < Connection >
SipTcpConnection::createRequestTransaction(Sptr < SipCommand > command) {
    //come here only if this is a command.
    assert (command != 0);
    Sptr<SipUrl> dest = command->postProcessRouteAndGetNextHop();
    assert (dest != 0);
    
    Data host = dest->getMaddrParam();
    if(host == "") host = dest->getHost();
    int port = dest->getPort().convertInt();
    cpLog( LOG_DEBUG_STACK,"Using destination: %s:%d", host.logData(), port);
    
    if ( port == 0 ) {
        port = SIP_PORT;
        cpLog( LOG_DEBUG_STACK, "Changed port to %d", port);
    }

    NetworkAddress nwaddr(host.convertString(), port);
    TcpClientSocket clientSocket(nwaddr, local_dev_to_bind_to,
                                 local_ip_to_bind_to, true, false );
    try {
        clientSocket.connect();
    }
    catch (VNetworkException& exception) {
        cpLog(LOG_ERR, exception.getDescription().c_str());
    }

    Sptr<Connection> connection = clientSocket.getConn();

    cpLog(LOG_DEBUG_STACK, "Adding non-persistent connection %d",
          connection->getConnId());

    char buf[56];
    ostrstream strm(buf,56);
    strm << nwaddr.getIpName() << ":" << nwaddr.getPort() << ends;

    tcpConnInfo.setConnNSenderIp(connection->getConnId(), connection, strm.str());
    return connection;
}



Sptr < Connection >
SipTcpConnection::createOrGetPersistentConnection(NetworkAddress nwaddr) {
    //come here only if this is a command.
    //	NetworkAddress nwaddr(host.getData(lo), port);
    string ipName = nwaddr.getIpName().convertString();
    char buf[56];
    sprintf(buf, "%s:%d", ipName.c_str(), nwaddr.getPort());
    
    if (myDestinationMap.count(buf)) {
        //Found an existing connection
        cpLog( LOG_DEBUG_STACK, "Found existing connection for %s", buf );
        if(myDestinationMap[buf]->isLive()) {
            return myDestinationMap[buf];
        }
        else {
               //close the connection , remove it from the map that
            //would re-establish it
            cpLog(LOG_ERR, "Stale connection from (%s), re-connecting..", 
                  buf);
            myDestinationMap.erase(buf);
        }
    }
    
    cpLog( LOG_DEBUG_STACK, "Creating persistent connection for %s", buf );
    TcpClientSocket clientSocket(nwaddr, local_dev_to_bind_to,
                                 local_ip_to_bind_to, true, false);
    try {
        clientSocket.connect();
        Sptr < Connection > connection = clientSocket.getConn();
	
        // insert the appropriate connection
        myDestinationMap[buf] = connection;
        cpLog(LOG_DEBUG_STACK, "Adding persistent connection %d" , 
              connection->getConnId());
        tcpConnInfo.setConnNSenderIp(connection->getConnId(), connection, buf);
        return connection;
    }
    catch (VNetworkException& exception) {
        cpLog(LOG_ERR, exception.getDescription().c_str());
        myDestinationMap.erase(buf);
        return 0;
    }
}



int
SipTcpConnection::prepareEvent(Sptr<SipMsgContainer> sipMsg)
{
    Sptr < NTcpStuff > conn;

    // create or use the current TCP details as appropriate

    int type = sipMsg->msg.in->getType();

    if ((type == SIP_STATUS) && (sipMsg->msg.in.getPtr())) {
        conn = tcpConnInfo.getStatusMsgConn(sipMsg->msg.in);
    }
    else if (sipMsg->msg.in.getPtr()) {

        // Make sure the cast is sane.
        assert(sipMsg->msg.in->isSipCommand());

        Sptr < SipCommand > sipCommand((SipCommand*)(sipMsg->msg.in.getPtr()));

        if (sipMsg->msg.netAddr == 0){
            cpLog(LOG_WARNING, "TCP Send is NULL");
            return -EINVAL;
        }

        // this is a request
        Sptr<Connection> c
#ifdef USE_PERSISTENT_TCP
        c = createOrGetPersistentConnection(*(sipMsg->msg.netAddr));
#else
        if (sipCommand->nextHopIsAProxy()) {
            c = createOrGetPersistentConnection(*(sipMsg->msg.netAddr));
        }
        else {
            c = createRequestTransaction(sipMsg);
        }
#endif
        tcpConnInfo.createConnection(c);
        conn = getConnInfo(c->getConnId());
    }
    else {
        // this is an error
        cpLog(LOG_ERR, "API violation: attempt to send null message");
        assert(0);
    }

    // at this point, you need to transmit
    if (conn != 0) {
        sipMsg->msg.out =  sipMsg->msg.in->encode();
        string dataString = sipMsg->msg.out.c_str();
        cpLog(LOG_INFO, "sending TCP SIP msg:\n\n-> %s\n\n%s",
              conn->getDescription().c_str(), dataString.c_str());
        cpLog(LOG_DEBUG_STACK, "TCP fd: %d", conn->getConnId());

        // If this fails, we are out of buffer space..  the conn class will
        // try as hard as possible, so just ignore errors at this point.
        conn->writeData(dataString);

#ifdef USE_PERSISTENT_TCP
        assert(type == SIP_STATUS);
        Sptr<StatusMsg> statusMsg((StatusMsg*)(sipMsg->msg.in.getPtr()));
        if ((statusMsg != 0) && (statusMsg->getStatusLine().getStatusCode() >= 200)) {
            //Transaction is over, remove the entry from the ID map
            SipTransactionId id(*statusMsg);
            tcpConnInfo.delIdMapEntry(id);
        }
#else
        if ((type == SIP_ACK) && !(sipMsg->msg.in->nextHopIsAProxy())) {
            tcpConnInfo.myCleanupList.push_back(conn->tcpConnection->getConnId());
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
    // given that this string is a content-length then do something
    static string contentLengthStr = str2lower(CONTENT_LENGTH.c_str());
    static string contentLengthStr2 = str2lower(CONTENT_LENGTH_SHORT.c_str());
   
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

    if ((str == "\n\r") || (str == "\n") ||
        (str == "\r") || (str == "")) {
        return true;
    }
    else {
        return false;
    }
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
SipTcpConnection::processMsgsIfReady(Sptr<NTcpStuff> connInfo) {
    Data data;
    string ipName;

    // TODO:  Probably can get rid of some of these coppies of data.
    Data& myBufD = connInfo->tcpBuf;
    ipName = connInfo->sender_ip.c_str();
    string myBuf = myBufD.c_str();
    int bufLen = isFullMsg(myBuf);
    cpLog(LOG_DEBUG_STACK, "Raw Msg read: [%s]" , myBuf.c_str());
    while (bufLen != -1) {
        string msgBuf = myBuf.substr(0, bufLen);
        string remainingBuf =
            myBuf.substr(bufLen, myBuf.length() - bufLen);
        myBufD = Data(remainingBuf);

        data = Data(msgBuf);

        Sptr<SipMsgContainer> sipMsg = new SipMsgContainer();

	sipMsg->msg.out = data;
        sipMsg->msg.in = SipMsg::decode( data, getLocalIp());

        if (sipMsg->msg.in != 0) {
            //store the receivedIPName.
            cpLog(LOG_DEBUG_STACK, "*** getSenderIP = %s***", ipName.c_str());
            sipMsg->msg.in->setReceivedIPName( ipName );
            cpLog(LOG_INFO, "Received UDP Message :\n\n<- HOST[%s]\n\n%s",
                  ipName.c_str(),  data.logData());

            if ((sipMsg->msg.in->getType() != SIP_STATUS) &&
                (sipMsg->msg.in->getType() != SIP_ACK)) {
                // save in the map
                tcpConnInfo.setStatusMsgConn(sipMsg->msg.in, fd);
                if (sipMsg->msg.in->getType() == SIP_REGISTER) {
                   //If request is a REGISTER message, map the contact
                   //host port with TCP socket, so that future
                   //requests to the contact would follow the same
                   //TCP connection
                   Data rData = connInfo->tcpConnection->getDescription();
                   cpLog( LOG_DEBUG_STACK, "Received from %s", rData.logData() );
                   int numContacts = sipMsg->msg.in->getNumContact();
                   if ( numContacts ) {
                       const SipContact& contact = sipMsg->msg.in->getContact( numContacts-1 );
                       Sptr<SipUrl> sUrl((SipUrl*)(contact.getUrl().getPtr()));
                       if ( sUrl != 0 ) {
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
            else if (sipMsg->msg.in.getPtr()
                     && sipMsg->msg.in->isStatusMsg()) {
                // Now we know it's safe to cast.
                Sptr<StatusMsg> statusMsg((StatusMsg*)(sipMsg->msg.in.getPtr()));
                if (statusMsg->getStatusLine().getStatusCode() >= 200) {
#ifndef USE_PERSISTENT_TCP
                    string iName = connInfo->tcpConnection->getDescription();
                    if (myDestinationMap.count(iName) == 0) {
                        cpLog(LOG_DEBUG_STACK, 
                              "Deleting non-persistent connection %d", fd);
                        //Non-persistent connection, close
                        tcpConnInfo.delConn(fd);
                    }
                    //Do the cleanup also
                    tcpConnInfo.doCleanup();
#endif
                }
            }
            
            rcvFifo.push_back(sipMsg);
        }
        myBuf = myBufD.getData(lo);
        bufLen = isFullMsg(myBuf);
    } // while
}


int SipTcpConnection::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                             int& maxdesc, uint64& timeout, uint64 now) {
    tcpConnInfo.setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);

}

void SipTcpConnection::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            uint64 now) {
    tcpConnInfo.tick(input_fds, output_fds, exc_fds, now);

    while (sendQ.size()) {
        Sptr<SipMsgContainer> sipMsg = sendQ.top();

        int type = sipMsg->msg.type;

        if ((sipMsg->msg.in == 0) || (sipMsg->getPrepareCount() > 10)) {
            // Give up
            cpLog(LOG_ERR, "Network error, giving up on sipMsg: %s\n",
                  sipMsg->toString().c_str());
            sendQ.pop();
            retval++; //Made some progress
            continue;
        }

        if (prepareEvent(sipMsg) < 0) {
            // Something is un-fixably broken.  Toss this guy.
            sendQ.pop();
        }
        else {
            // Successfully sent it
            retval++; // Made progress
            sendQ.pop();
        }

        // also inform the cleanup
        if (type != SIP_INVITE) {
            SipTransactionGC::instance()->collect(sipMsg, MESSAGE_CLEANUP_DELAY);
        }
    }//while
    return retval;
}



}

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
"$Id: SipTcpConnection.cxx,v 1.12 2005/03/03 19:59:49 greear Exp $";

#include <sys/types.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "global.h"
#include "InviteMsg.hxx"
#include "NetworkAddress.h"
#include "SipCommand.hxx"
#include "SipContact.hxx"
#include "SipTcpConnection.hxx"
#include "SipTransactionId.hxx"
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


int NTcpStuff::tryWrite(fd_set* output_fds) {
   int written = tcpConnection->write();
   if (written < 0) {
      // We are hosed, close the connection.
      tcpConnection->close();
   }
   return -errno;
}

bool NTcpStuff::needsToWrite() {
   if (tcpConnection != 0) {
      return tcpConnection->needsToWrite();
   }
   return false;
}


// This will consume all of d as long as we are within the limits
// of the amount of data we will buffer.  If the socket is not immediately
// writable, it will be buffered in this class, so calling code can be sure
// that if the message is accepted, it will be transmitted if at all possible.
int NTcpStuff::writeData(const Data& d) {
   int rv = tcpConnection->queueSendData(d.c_str(), d.size());
   // Go ahead and try to flush the transport...it's non-blocking anyway.
   tcpConnection->write();
   return rv;
}


Sptr < NTcpStuff >
NTcpConnInfo::getStatusMsgConn(Sptr < SipMsg > msg) {
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

void NTcpConnInfo::doCleanup() {
   while(myCleanupList.size()) {
      int id = myCleanupList.front();
      myCleanupList.pop_front();
      delConn(id);
   }
}


void NTcpConnInfo::setStatusMsgConn(Sptr < SipMsg > msg, int fd) {
   Sptr < NTcpStuff > t = getCurrent(fd);
   
   if (t != 0) {
      SipTransactionId id(*msg);
      idMap[id] = t;
      cpLog(LOG_DEBUG, "NTcpConnInfo::setStatusMsgConn, idMapSize:%d", idMap.size() );
   }
   else {
      cpLog(LOG_WARNING, "could not find valid tcp connection for message");
   }
}



NTcpConnInfo::NTcpConnInfo(SipTcpConnection* tc)
      : nullData(TransNull) {
   sip_conn = tc;
}


NTcpConnInfo::~NTcpConnInfo() {
}


void NTcpConnInfo::delConn(int fd) {
   map < int, Sptr < NTcpStuff > > ::iterator i = myMap.find( fd );
   Sptr < NTcpStuff > del;

   if ( i == myMap.end() ) {
      return; 	// not found
   }

   del = i->second;
   myMap.erase(i);
   cpLog(LOG_DEBUG_STACK, "After del size:%d" ,myMap.size());

   if (del != 0) {
      {
         //Cleanup the transactionMap for the closed connection
         map <SipTransactionId, Sptr < NTcpStuff > > ::iterator j;
         map <SipTransactionId, Sptr < NTcpStuff > > ::iterator delItr;
          
         j = idMap.begin();
          
         while (j != idMap.end()) {
            if (j->second == del) {
               delItr = j;
               j++;
               idMap.erase(delItr);
            }
            else {
               j++;
            }
         }
      }

      {
         //Cleanup the transactionMap for the closed connection
         map <string, Sptr < NTcpStuff > > ::iterator j;
         map <string, Sptr < NTcpStuff > > ::iterator delItr;

         j = myDestinationMap.begin();

         while (j != myDestinationMap.end()) {
            if (j->second == del) {
               delItr = j;
               j++;
               myDestinationMap.erase(delItr);
            }
            else {
               j++;
            }
         }
      }

      del->getConnection()->close();
   }
}


void NTcpConnInfo::notifyDestination(const string& dest_key, Sptr<NTcpStuff> connInfo) {
   myDestinationMap[ dest_key ] = connInfo;
}

void
NTcpConnInfo::delIdMapEntry(const SipTransactionId&  id) {
   cpLog(LOG_DEBUG, "Deleting IdMap entry, size:%d", idMap.size());
   // erase from the transactionId if needed
   map < SipTransactionId, Sptr < NTcpStuff > > ::iterator j;

   j = idMap.find(id);

   if (j != idMap.end()) {
      idMap.erase(j);
   }
}


Sptr < NTcpStuff >
NTcpConnInfo::getCurrent(int fd) {
   Sptr < NTcpStuff > myObj;

   map < int, Sptr < NTcpStuff > > ::iterator i = myMap.find( fd );

   if ( i != myMap.end() ) {
      myObj = i->second;
   }

   return myObj;
}


Sptr<NTcpStuff>
NTcpConnInfo::setConnNPeerIp(int fd, Sptr < Connection > conn, const Data& peer) {
   Sptr < NTcpStuff > myTcp = new NTcpStuff();

   myTcp->setConnection(conn);
   NetworkAddress na(peer);
   myTcp->setPeerIp(na.getIpName().c_str());
   myTcp->setPeerPort(na.getPort());
    
   myMap[fd] = myTcp;
   myDestinationMap[peer.c_str()] = myTcp;

   cpLog(LOG_DEBUG_STACK, "Map size %d after adding %d",
         myMap.size(), fd);
   return myTcp;
}


Sptr<NTcpStuff> 
NTcpConnInfo::getConnInfo(int fd)
{
   return(getCurrent(fd));
}


void NTcpConnInfo::writeTick(fd_set* output_fds) {
   map < int, Sptr < NTcpStuff > > ::iterator i = myMap.begin();
   while (i != myMap.end()) {
      i->second->tryWrite(output_fds);
      i++;
   }
}

void NTcpConnInfo::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        uint64 now) {
   int rv;
   map < int, Sptr < NTcpStuff > > ::iterator i = myMap.begin();
   while (i != myMap.end()) {
      int mapfd = i->first;
      if (FD_ISSET(mapfd, input_fds)) {
         // Read
         cpLog(LOG_DEBUG_STACK, "reading tcp data on %d", mapfd);
         rv = tcpReadOrAccept(mapfd, sip_conn->getTcpStack());
         if (rv > 0) {
            sip_conn->processMsgsIfReady(i->second);
         }
      }

      // Write
      i->second->tryWrite(output_fds);
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

      FD_SET(mapfd, input_fds); // always check for reads
      FD_SET(mapfd, exc_fds);   // always check for exceptions

      if (conn->needsToWrite()) {
         FD_SET(mapfd, output_fds); // Check for writes too
      }

      if (mapfd > maxdesc) {
         maxdesc = mapfd;
      }
      ++i;
   }
   return 0;
}


Sptr < NTcpStuff >
NTcpConnInfo::createOrGetPersistentConnection(uint16 tos, uint32 priority,
                                              const NetworkAddress& nwaddr) {
   //come here only if this is a command.
   string ipName = nwaddr.getIpName().convertString();
   string key(ipName);
   key.append(":");
   char buf[56];
   snprintf(buf, 55, "%d", nwaddr.getPort());
   key.append(buf);
    
   if (myDestinationMap.count(key)) {
      //Found an existing connection
      cpLog( LOG_DEBUG_STACK, "Found existing connection for %s", key.c_str() );
      Sptr<NTcpStuff> conn = myDestinationMap[key];
      assert(conn->isLive());
   }
    
   cpLog( LOG_DEBUG_STACK, "Creating persistent connection for %s", key.c_str() );
   TcpClientSocket clientSocket(tos, priority, nwaddr, sip_conn->getLocalDev(),
                                sip_conn->getLocalIp(), true, false);

   clientSocket.connect();

   Sptr < Connection > connection = clientSocket.getConn();	
   // insert the appropriate connection
   return setConnNPeerIp(connection->getConnId(), connection, key);
}


int NTcpConnInfo::tcpReadOrAccept(int tcpfd, TcpServerSocket& tcpStack) {

   Sptr < Connection > newconn;
   Sptr <NTcpStuff> connInfo;

   cpLog(LOG_DEBUG_STACK, "Received tcp msg on fd: %d", tcpfd);

   if (tcpfd == tcpStack.getServerConn()->getConnId()) {
      //this has happened on the default server socket.
      //create a new connection and accept on it.
      newconn = new Connection(tcpStack.getServerConn()->isBlocking());

      int clientId = -1;

      // catch exception thrown when we run out of fds
      try {
         clientId = tcpStack.accept(*newconn);
      }
      catch (VNetworkException &) {
         return -1;
      }
        
      cpLog(LOG_DEBUG_STACK,"*****  New Connection = %s *****\n", 
            newconn->getDescription().c_str());

      // set the sender ip
      string ipName = newconn->getPeerIp();

      //cache this in the tcpMap.
      connInfo = setConnNPeerIp(clientId, newconn, ipName);

      //
      cpLog(LOG_DEBUG_STACK, "accepting new connection: ", clientId);

      // need to redo this conn object, etc.
      tcpfd = newconn->getConnId();
   }
   else {
      connInfo = getConnInfo(tcpfd);
      newconn = connInfo->getConnection();   /* it already has the
                                              * correct connection. */
   }

   int numBytes = newconn->read();
   if (numBytes < 0) {
      cpLog(LOG_DEBUG_STACK, "closing fd");
      delConn(tcpfd);
      return -1;
   }
   return numBytes;
}//tcpReadOrAccept


SipTcpConnection::SipTcpConnection(uint16 tos, uint32 priority, const string& local_ip,
                                   const string& _local_dev_to_bind_to,
                                   int port, bool blocking)
      : mytcpStack(tos, priority, local_ip, _local_dev_to_bind_to, port, blocking),
        tcpConnInfo(this),
        local_ip_to_bind_to(local_ip),
        local_dev_to_bind_to(_local_dev_to_bind_to) {

   _tos = tos;
   _skb_priority = priority;

   // put the tcpStack into the tcpConnInfo map

   int fd = mytcpStack.getServerConn()->getConnId();

   // TODO:  This is slightly weird.  At this point, we shouldn't
   // care about the IP for this guy... --Ben
   string mh = local_ip;
   if (mh.size() == 0) {
      mh = Vocal::theSystem.gethostAddress(); //OK
   }
   tcpConnInfo.setConnNPeerIp(fd, mytcpStack.getServerConn(), mh);
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
   return NULL;
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
      Sptr<SipCommand> command;
      command.dynamicCast(msg->getMsgIn());
      if (command.getPtr() != 0) {
         Sptr<SipUrl> sipDest;
         sipDest.dynamicCast(command->getRequestLine().getUrl());
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
         Sptr<NetworkAddress> na =  new NetworkAddress(nhost.convertString(), nport); 
         msg->setNetworkAddr(na);
      }
      catch(NetworkAddress::UnresolvedException& e) {
         cpLog(LOG_ERR, "Destination (%s) is not reachable, reason:%s.",
               nhost.logData(), e.getDescription().c_str());
         return -1;
      }
   }
   sendQ.push_back(msg);
   return 0;
}


Sptr < NTcpStuff >
SipTcpConnection::createRequestTransaction(Sptr < SipCommand > command) {
   //come here only if this is a command.
   assert (command != 0);
   Sptr<SipUrl> dest = command->postProcessRouteAndGetNextHop();
   assert (dest != 0);
    
   Data host = dest->getMaddrParam();
   if (host == "")
      host = dest->getHost();
   int port = dest->getPort().convertInt();
   cpLog( LOG_DEBUG_STACK,"Using destination: %s:%d", host.logData(), port);
    
   if ( port == 0 ) {
      port = SIP_PORT;
      cpLog( LOG_DEBUG_STACK, "Changed port to %d", port);
   }

   NetworkAddress nwaddr(host.convertString(), port);
   TcpClientSocket clientSocket(_tos, _skb_priority, nwaddr, local_dev_to_bind_to,
                                local_ip_to_bind_to, true, false );

   // Start the connection process.  Since we are non blocking, it won't
   // necessarily happen immediately...but select will tell us when the
   // connect is complete.
   clientSocket.connect();

   Sptr<Connection> connection = clientSocket.getConn();

   cpLog(LOG_DEBUG_STACK, "Adding non-persistent connection %d",
         connection->getConnId());

   char buf[56];
   ostrstream strm(buf,56);
   strm << nwaddr.getIpName() << ":" << nwaddr.getPort() << ends;

   return tcpConnInfo.setConnNPeerIp(connection->getConnId(), connection, strm.str());
}


int SipTcpConnection::prepareEvent(Sptr<SipMsgContainer> sipMsg) {
   Sptr < NTcpStuff > conn;

   // create or use the current TCP details as appropriate

   int type = sipMsg->getMsgIn()->getType();

   if ((type == SIP_STATUS) && (sipMsg->getMsgIn().getPtr())) {
      conn = tcpConnInfo.getStatusMsgConn(sipMsg->getMsgIn());
   }
   else if (sipMsg->getMsgIn().getPtr()) {

      // Make sure the cast is sane.
      assert(sipMsg->getMsgIn()->isSipCommand());

      Sptr < SipCommand > sipCommand;
      sipCommand.dynamicCast(sipMsg->getMsgIn());

      if (sipMsg->getNetworkAddr() == 0){
         cpLog(LOG_WARNING, "TCP Send is NULL");
         return -EINVAL;
      }

      // this is a request
#ifdef USE_PERSISTENT_TCP
      conn = tcpConnInfo.createOrGetPersistentConnection(_tos, _skb_priority,
                                                         *(sipMsg->getNetworkAddr()));
#else
      if (sipCommand->nextHopIsAProxy()) {
         conn = tcpConnInfo.createOrGetPersistentConnection(_tos, _skb_priority,
                                                            *(sipMsg->getNetworkAddr()));
      }
      else {
         conn = createRequestTransaction(sipMsg);
      }
#endif
   }
   else {
      // this is an error
      cpLog(LOG_ERR, "API violation: attempt to send null message");
      assert(0);
   }

   // at this point, you need to transmit
   sipMsg->cacheEncode();
   cpLog(LOG_INFO, "sending TCP SIP msg:\n\n-> %s\n\n%s",
         conn->getConnection()->getDescription().c_str(),
         sipMsg->getEncodedMsg().c_str());
   cpLog(LOG_DEBUG_STACK, "TCP fd: %d", conn->getConnection()->getConnId());
   
   // If this fails, we are out of buffer space..  the conn class will
   // try as hard as possible, so just ignore errors at this point.
   conn->writeData(sipMsg->getEncodedMsg());

#ifdef USE_PERSISTENT_TCP
   assert(type == SIP_STATUS);
   Sptr<StatusMsg> statusMsg((StatusMsg*)(sipMsg->getMsgIn().getPtr()));
   if ((statusMsg != 0) && (statusMsg->getStatusLine().getStatusCode() >= 200)) {
      //Transaction is over, remove the entry from the ID map
      SipTransactionId id(*statusMsg);
      tcpConnInfo.delIdMapEntry(id);
   }
#else
   if ((type == SIP_ACK) && !(sipMsg->getMsgIn()->nextHopIsAProxy())) {
      tcpConnInfo.myCleanupList.push_back(conn->tcpConnection->getConnId());
   }
#endif
   return 0;
}


string
get_next_line(const string& str, string::size_type* old_pos) {
   string myStr;
   
   string::size_type pos = str.find('\n', *old_pos);
   if (pos != string::npos) {
      myStr = str.substr(*old_pos, pos - (*old_pos));
      *old_pos = pos + 1;
   }
   else {
      myStr = str.substr(*old_pos, str.length() - *old_pos);
      *old_pos = pos;
   }
   return myStr;
}


int
getContentLength(const string& str) {
   // given that this string is a content-length then do something
   static string contentLengthStr = str2lower(CONTENT_LENGTH.c_str());
   static string contentLengthStr2 = str2lower(CONTENT_LENGTH_SHORT.c_str());
   
   int pos = 0; 
   unsigned len = str.length();
   if (len > contentLengthStr.length()) {
      len = contentLengthStr.length();
   }
   string x = str2lower(str.substr(0,len));

   if (x.find(contentLengthStr) == 0) {
      // this is right -- scan
      pos = contentLengthStr.length();
   }
   else if (x.find(contentLengthStr2) == 0) {
      pos = contentLengthStr2.length();
   }
   else {
      // not the content-length
      return -1;
   }
   string::size_type newPos = str.find_first_not_of("0123456789", pos + 1);
   if (newPos == string::npos) {
      newPos = str.length();
   }
   string contentLen = str.substr(pos + 1, newPos - (pos + 1));
   return (atoi(contentLen.c_str()));
}


bool
isBlankLine(const string& str) {
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


// TODO:  Could have it parse a char* buf and save some data coppies.
int 
Vocal::isFullMsg(const string& str) {
   string::size_type pos = 0;

   int contentLen = -1;
   bool foundBlank = false;
   bool foundNonBlank = false;
   pos = str.find(CONTENT_LENGTH.c_str());
   if (pos == string::npos) {
      pos = str.find(CONTENT_LENGTH_SHORT.c_str());
      if (pos == string::npos)
         pos = 0;
   }

   if (pos != 0) {
      foundNonBlank = true;
   }

   while (!foundBlank && (pos != string::npos)) {
      string myLine = get_next_line(str, &pos);
      
      if (pos != string::npos) {
         if (contentLen == -1) {
            // check against content-length
            contentLen = getContentLength(myLine);
         }
         if (isBlankLine(myLine)) {
            if(foundNonBlank) {
               foundBlank = true;
               break;
            }
         }
         else {
            foundNonBlank = true;
         }
      }
   } // While

   if (pos == string::npos) {
      pos = str.length();
   }

   if (foundBlank) {
      // then make sure that the remaining character count is the same
      int remainingBytes = str.length() - pos;

      // if there have been no blank lines, ignore these lines
      if (contentLen == -1 && foundNonBlank) {
         contentLen = 0;
      }
      if ((remainingBytes >= contentLen) && (str.length() > 0)) {
         return (pos + contentLen);
      }
   }
   return -1;
}


void
SipTcpConnection::processMsgsIfReady(Sptr<NTcpStuff> connInfo) {
   Data data;
   const string& ipName = connInfo->getPeerIp();
   int fd = connInfo->getConnection()->getConnId();
   // Assume that no message is greater than 64k.  TODO:  Verify
#define MX_SIP_MSG_LEN 65536
   unsigned char buf[MX_SIP_MSG_LEN];

   // TODO:  Probably can get rid of some of these coppies of data.
   //Data& myBufD = connInfo->tcpBuf;

   connInfo->getConnection()->peekRcvdBytes(buf, MX_SIP_MSG_LEN);
   string myBuf((char*)(buf));
   int bufLen = isFullMsg(myBuf);
   cpLog(LOG_DEBUG_STACK, "Raw Msg read: [%s]" , myBuf.c_str());
   while (bufLen != -1) {
      string msgBuf = myBuf.substr(0, bufLen);
      // Drop it from the received bytes buffer
      connInfo->getConnection()->consumeRcvdBytes(bufLen);

      Sptr<SipMsg> msg = SipMsg::decode(msgBuf, getLocalIp());
      if (msg != 0) {
         SipTransactionId id(*msg);
         Sptr<SipMsgContainer> sipMsg = new SipMsgContainer(id);

         //store the receivedIPName.
         cpLog(LOG_DEBUG_STACK, "*** getSenderIP = %s***", ipName.c_str());
         msg->setReceivedIPName( ipName );
         cpLog(LOG_INFO, "Received UDP Message :\n\n<- HOST[%s]\n\n%s",
               ipName.c_str(),  data.logData());

         sipMsg->setMsgIn(msg);

         if ((msg->getType() != SIP_STATUS) &&
             (msg->getType() != SIP_ACK)) {
            // save in the map
            tcpConnInfo.setStatusMsgConn(msg, fd);
            if (msg->getType() == SIP_REGISTER) {
               //If request is a REGISTER message, map the contact
               //host port with TCP socket, so that future
               //requests to the contact would follow the same
               //TCP connection
               string rData = connInfo->getConnection()->getDescription();
               cpLog( LOG_DEBUG_STACK, "Received from %s", rData.c_str());
               int numContacts = msg->getNumContact();
               if ( numContacts ) {
                  const SipContact& contact = msg->getContact( numContacts - 1 );
                  Sptr<SipUrl> sUrl;
                  sUrl.dynamicCast(contact.getUrl());
                  if ( sUrl != 0 ) {
                     Data tmp = sUrl->getHost();
                     tmp += ":";
                     tmp += sUrl->getPort();
                     string cAddr(tmp.c_str());
                     cpLog( LOG_DEBUG_STACK, "fd %d for address %s", fd, cAddr.c_str() );
                     tcpConnInfo.notifyDestination(cAddr, connInfo);
                  }
               }
            }
         }
         else if (msg->isStatusMsg()) {
            // Now we know it's safe to cast.
            Sptr<StatusMsg> statusMsg;
            statusMsg.dynamicCast(msg);
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
      connInfo->getConnection()->peekRcvdBytes(buf, MX_SIP_MSG_LEN);
      myBuf = (char*)(buf);
      bufLen = isFullMsg(myBuf);
   } // while
}


int SipTcpConnection::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                             int& maxdesc, uint64& timeout, uint64 now) {
   tcpConnInfo.setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   return 0;
}

void SipTcpConnection::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                            uint64 now) {
   tcpConnInfo.tick(input_fds, output_fds, exc_fds, now);

   while (sendQ.size()) {
      Sptr<SipMsgContainer> sipMsg = sendQ.front();

      if (sipMsg->getMsgIn() == 0) {
         // Give up
         cpLog(LOG_ERR, "msg.in == 0, giving up on sipMsg: %s\n",
               sipMsg->toString().c_str());
      }
      else {
         if (prepareEvent(sipMsg) < 0) {
            // Something is un-fixably broken.  Toss this guy.
            cpLog(LOG_ERR, "prepareEvent failed, giving up on sipMsg: %s\n",
                  sipMsg->toString().c_str());
         }
      }

      sendQ.pop_front();
   }//while

   tcpConnInfo.writeTick(output_fds);
}

/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 1999, 2000 Vovida Networks, Inc.  All rights reserved.
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
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by Vovida Networks,
 *        Inc. (http://www.vovida.org/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must 
 *    not be used to endorse or promote products derived from this 
 *    software without prior written permission. For written permission, 
 *    please contact vocal\@vovida.org.
 *
 * 5. Products derived from this software may not be called "VOCAL",
 *    nor may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL VOVIDA NETWORKS, INC. OR ITS 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software consists of voluntary contributions made by Vovida 
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see 
 * <http://www.vovida.org/>.
 *
 */


////////////////////////////////////////////////////////////////////////
//
//  Parts of this code were copied from the free distribution available at:
//
//  Livingston Enterprises, Inc.
//  6920 Koll Center Parkway
//  Pleasanton, CA   94566
//
//  Copyright 1992 Livingston Enterprises, Inc.
//
//  Permission to use, copy, modify, and distribute this software for any
//  purpose and without fee is hereby granted, provided that this
//  copyright and permission notice appear on all copies and supporting
//  documentation, the name of Livingston Enterprises, Inc. not be used
//  in advertising or publicity pertaining to distribution of the
//  program without specific prior permission, and notice be given
//  in supporting documentation that copying and distribution is by
//  permission of Livingston Enterprises, Inc.
//
//  Livingston Enterprises, Inc. makes no representations about
//  the suitability of this software for any purpose.  It is
//  provided "as is" without express or implied warranty.
//
////////////////////////////////////////////////////////////////////////


static const char* const RadiusStack_cxx_Version =
    "$Id: RadiusStack.cxx,v 1.3 2004/06/15 00:30:10 greear Exp $";


#include <sys/time.h>
#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <strings.h>

#include "RadiusStack.hxx"
#include "VRadiusException.hxx"
#include "NetworkAddress.h"
#include "UdpStack.hxx"
#include "vmd5.h"
#include "cpLog.h"
#include <sstream>


///************************************************************///
///**********************  RadiusMessage  *********************///
///************************************************************///

RadiusMessage::RadiusMessage(RadiusStack* r, uint8 id, int retries) {
   m_requestId = id;
   remainingRetries = retries;
   m_sendBufferLen = 0;
   stack = r; //used for processVsa callbacks, among other things
}

string RadiusMessage::toString() {
   ostringstream oss;
   oss << "msg-id: " << (int)(m_requestId) << " retries: " << remainingRetries
       << " sendLength: " << m_sendBufferLen;
   return oss.str();
}


bool RadiusMessage::handleResponse(BufType* msg, int ln) {
   BufType::AcctHdr *auth = &(msg->acctHdr);
   unsigned char *ptr = msg->buffer;
   int totalLen;
   unsigned char reply_digest[ACCT_VECTOR_LEN];
   unsigned char calc_digest[ACCT_VECTOR_LEN];

   BufType::AcctHdr requestHdr = m_sendBuffer.acctHdr;
    
   totalLen = ntohs(auth->length);

   if (totalLen != ln) {
      cpLog(LOG_ERR, "ERROR:  Received invalid replay length from server, want: %d, got: %d",
            totalLen, ln);
      return false;
   }

   // save the response auth vector, compare with calculated vector
   memcpy(reply_digest, auth->authVector, ACCT_VECTOR_LEN);
    
   // replace the response auth vector in the message with the
   // request auth vector in order to calculate the correct md5 sum
   memcpy(auth->authVector, requestHdr.authVector,
          ACCT_VECTOR_LEN);

   // Verify the reply digest
   memcpy(ptr + totalLen, stack->m_secretKey.c_str(), stack->m_secretKey.size());
   md5Calc(calc_digest, (unsigned char *)auth,
           totalLen + stack->m_secretKey.size());

   // Compare the request and response auth digests
   if (memcmp(reply_digest, calc_digest, ACCT_VECTOR_LEN) != 0) {
      cpLog(LOG_DEBUG_STACK, "Warning: Received invalid reply digest from server");
   }
   else {
      // Looks good.
   }

   // If here, we can evaluate the buffer...
   return evaluateRecvBuffer(msg);
}//handleResponse


bool RadiusMessage::evaluateRecvBuffer(BufType* msg) {
   // evaluate attribute list in m_receiveBuffer

   BufType::AcctHdr *auth = &(msg->acctHdr);
   unsigned char *ptr = msg->buffer;
   int totalLen;
   int count;
   unsigned char attribType;
   unsigned char attribLen;
   AttribType enumAttribType;
   unsigned char vsaLength;
   unsigned char vsaType;

   totalLen = ntohs(auth->length);
   count = sizeof(BufType::AcctHdr);

   cpLog(LOG_DEBUG_STACK, "Received reply from radius server %s code=%d id=%d length=%d",
         stack->m_serverName.c_str(), auth->code, auth->id, totalLen);

   // fast forward to the start of the attributes
   ptr += count;

   while (count < totalLen) {
      attribType = (unsigned char) * ptr++;
      attribLen = (unsigned char) * ptr++;
      count += attribLen;
      enumAttribType = (AttribType) attribType;

      // check if attribute value is empty
      if (attribLen < 2) {
         continue;
      }

      switch (enumAttribType) {
       case PW_ACCT_STATUS_TYPE: {
          stack->setAccountStatusType((AcctStatusType)ntohl((int) * ptr));
          cpLog(LOG_DEBUG_STACK, "Attribute PW_ACCT_STATUS_TYPE:%d",
                stack->m_acctStatusType);
          ptr += attribLen - 2;
          break;
       }
       case PW_VENDOR_SPECIFIC: {
          ptr += 4;
          vsaType = (unsigned char) * ptr++;
          vsaLength = (unsigned char) * ptr++;
          stack->processVsa(vsaType, ptr, vsaLength);
          ptr += vsaLength - 2;
          break;
       }
       default: {
          cpLog(LOG_DEBUG_STACK, "Ignored attribute type:%d, length:%d",
                (int)enumAttribType,
                (int)attribLen);
          ptr += attribLen - 2;
          break;
       }
      }//switch
   }
   return true;
}//evaluateRecvBuffer


int
RadiusMessage::addHeader( const PacketType type ) {
    BufType::AcctHdr *auth = &(m_sendBuffer.acctHdr);

    memset(&m_sendBuffer, 0, sizeof(m_sendBuffer));

    auth->code = type;
    auth->id = m_requestId;

    m_sendBufferLen = sizeof(BufType::AcctHdr);

    return m_sendBufferLen;
}

// Algorithm for Request/Response authenticator:
// MD5(code + id + length + authenticator + attributes + secretKey)
void
RadiusMessage::addMD5()
{
    BufType::AcctHdr *auth = &(m_sendBuffer.acctHdr);
    unsigned char *ptr = m_sendBuffer.buffer;

    ptr += m_sendBufferLen;

    // Secret key, used to create digest, do not send secret key
    strcpy((char*)ptr, stack->m_secretKey.c_str());
    int bufSizeWithKey = m_sendBufferLen + stack->m_secretKey.size();

    // Set the length field
    auth->length = (unsigned short int)htons(m_sendBufferLen);

    // set authenticator to zeros before calculating hash
    memset(auth->authVector, 0, ACCT_VECTOR_LEN);

    // Calculate the MD5 Digest of the entire message
    unsigned char digest[ACCT_VECTOR_LEN];
    md5Calc(digest, m_sendBuffer.buffer, bufSizeWithKey);

    memcpy(auth->authVector, digest, sizeof(digest));
}

int
RadiusMessage::addAttribute( const AttribType type,
                             const unsigned long int value ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeof(unsigned long int) + 2;

   ptr += m_sendBufferLen;
   *ptr++ = (unsigned char)type;
   *ptr++ = length;
   memcpy(ptr, &value, sizeof(unsigned long int));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addAttribute( const AttribType type,
                             const unsigned short int value ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeof(unsigned short int) + 2;

   ptr += m_sendBufferLen;
   *ptr++ = (unsigned char)type;
   *ptr++ = length;
   memcpy(ptr, &value, sizeof(unsigned short int));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addAttribute( const AttribType type,
                           const unsigned char value ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   char length = sizeof(unsigned char) + 2;

   ptr += m_sendBufferLen;
   *ptr++ = (unsigned char)type;
   *ptr++ = length;
   memcpy(ptr, &value, sizeof(unsigned char));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addAttribute( const AttribType type,
                             const unsigned char *value,
                             const int sizeOfValue ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeOfValue + 2;

   ptr += m_sendBufferLen;
   *ptr++ = (unsigned char)type;
   *ptr++ = length;
   if (sizeOfValue > 0 && ptr) {
      memcpy(ptr, value, sizeOfValue);
   }
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addVsaAttribute( const unsigned char vsaType,
                                const unsigned long int value,
                                const unsigned long int vendorId ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeof(unsigned long int) + 8;

   ptr += m_sendBufferLen;
   
   *ptr++ = (unsigned char)PW_VENDOR_SPECIFIC;      // RAD Type
   *ptr++ = length;                                 // RAD length
   memcpy( ptr, &vendorId, sizeof(unsigned long int) ); // Vendor ID
   ptr += sizeof(unsigned long int);
   *ptr++ = (unsigned char)vsaType;                 // Vendor Type
   *ptr++ = sizeof(unsigned long int) + 2;          // Vendor Length

   memcpy(ptr, &value, sizeof(unsigned long int));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addVsaAttribute( const unsigned char vsaType,
                              const unsigned short int value,
                              const unsigned long int vendorId ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeof(unsigned short int) + 8;
   
   ptr += m_sendBufferLen;

   *ptr++ = (unsigned char)PW_VENDOR_SPECIFIC;      // RAD Type
   *ptr++ = length;                                 // RAD length
   memcpy( ptr, &vendorId, sizeof(unsigned long int) ); // Vendor ID
   ptr += sizeof(unsigned long int);
   *ptr++ = (unsigned char)vsaType;                 // Vendor Type
   *ptr++ = sizeof(unsigned short int) + 2;         // Vendor Length

   memcpy(ptr, &value, sizeof(unsigned short int));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addVsaAttribute( const unsigned char vsaType,
                                const unsigned char value,
                                const unsigned long int vendorId ) {
   unsigned char *ptr = m_sendBuffer.buffer;
   char length = sizeof(unsigned char) + 8;

   ptr += m_sendBufferLen;
   
   *ptr++ = (unsigned char)PW_VENDOR_SPECIFIC;      // RAD Type
   *ptr++ = length;                                 // RAD length
   memcpy( ptr, &vendorId, sizeof(unsigned long int) ); // Vendor ID
   ptr += sizeof(unsigned long int);
   *ptr++ = (unsigned char)vsaType;                 // Vendor Type
   *ptr++ = sizeof(unsigned char) + 2;              // Vendor Length

   memcpy(ptr, &value, sizeof(unsigned char));
   m_sendBufferLen += length;

   return length;
}

int
RadiusMessage::addVsaAttribute( const unsigned char vsaType,
                                const unsigned char *value,
                                const int sizeOfValue,
                                const unsigned long int vendorId )
{
   unsigned char *ptr = m_sendBuffer.buffer;
   unsigned char length = sizeOfValue + 8;
   
   ptr += m_sendBufferLen;
   
   *ptr++ = (unsigned char)PW_VENDOR_SPECIFIC;      // RAD Type
   *ptr++ = length;                                 // RAD length
   memcpy( ptr, &vendorId, sizeof(unsigned long int) ); // Vendor ID
   ptr += sizeof(unsigned long int);
   *ptr++ = (unsigned char)vsaType;                 // Vendor Type
   *ptr++ = (unsigned char)sizeOfValue + 2;         // Vendor Length

   if (sizeOfValue > 0 && ptr) {
      memcpy(ptr, value, sizeOfValue);
   }
   m_sendBufferLen += length;

   return length;
}

int RadiusMessage::md5Calc( unsigned char *output,
                            unsigned char *input, unsigned inLen ) {
   MD5Context context;
   MD5Init(&context);
   MD5Update(&context, input, inLen);
   MD5Final(output, &context);
   return 0;
}



///************************************************************///
///***********************  RadiusStack  **********************///
///************************************************************///

RadiusStack::RadiusStack(const string& local_ip) :
    m_retries( 5 ),
    m_serverName( "unknown" ),
    m_serverAddr( 0 ),
    m_serverPort( 0 ),
    m_clientName( "localhost" ),
    m_clientAddr( 0 ),
    m_clientPort( 0 ),
    m_secretKey( "testing123" ),
    m_connected( false ),
    m_acctStatusType( ACCT_OFF ),
    m_localIp(local_ip)
{}

RadiusStack::~RadiusStack() {
   if (m_connected) {
      if (!accountingOff()) {
         cpLog(LOG_ALERT,
               "Accounting off failed, closing connection anyways");
      }
   }
}

void RadiusStack::setupConnection() {
   // set session specific values

   struct servent *svp;
   struct hostent *hent;
   unsigned short int svcport;  // network byte order
   char nasname[256];

   // find server port

   svp = getservbyname ("radacct", "udp");
   if (svp == 0) {
      svcport = (unsigned short int)htons(PW_ACCT_UDP_PORT);
   }
   else {
      svcport = svp->s_port;  // already in network byte order
   }

   m_serverPort = ntohs(svcport);

   if (gethostname(nasname, sizeof(nasname)) == 0) {
      m_clientName = nasname;
   }
   else {
      string msg("Can't initialize radius connection, gethostname() failed");
      if (errno == EINVAL) {
         msg += " due to invalid name length";
      }
      throw VRadiusException(msg, __FILE__, __LINE__);
   }

   if ((hent = gethostbyname(nasname)) == 0) {
      string msg("Couldn't find IP for ");
      msg += nasname;
      throw VRadiusException(msg, __FILE__, __LINE__);
   }
   else {
      m_clientAddr = *((unsigned long int*) hent->h_addr_list[0]);  // network byte order
   }

   // Get the IP address of the radius accounting server
   if ((hent = gethostbyname(m_serverName.c_str())) == 0) {
      string msg("Couldn't find IP for ");
      msg += m_serverName;
      throw VRadiusException(msg, __FILE__, __LINE__);
   }
   else {
      m_serverAddr = *((unsigned long int*) hent->h_addr_list[0]);  // network byte order
   }
   cpLog(LOG_DEBUG_STACK, "Client Host:%s",
         m_clientName.c_str());
   cpLog(LOG_DEBUG_STACK, "Server Host:%s Server Port:%d",
         m_serverName.c_str(), m_serverPort);
}


void
RadiusStack::initialize( const string &server,
                         const string &secretKey,
                         const int retries )
    throw(VRadiusException&)
{
    memset(&m_recvBuffer, 0, sizeof(m_recvBuffer));

    m_serverName = server;
    m_secretKey = secretKey;
    m_retries = retries;

    setupConnection();

    try {
        m_networkAddr = new NetworkAddress(m_serverName, m_serverPort);
        string empty;
        m_udpConnection = new UdpStack(false, m_localIp, empty, /* local_dev_to_bind_to */
                                       m_networkAddr.getPtr(), m_serverPort, m_serverPort);

        // this may not be necessary, depends on the radius server
        m_udpConnection->connectPorts();
    }
    catch (UdpStackException &e) {
        throw VRadiusException("UdpStackException caught", __FILE__, __LINE__);
    }

    // Send message to the radius server
    accountingOn();
}

bool RadiusStack::handleResponse(unsigned char* buf, int ln) {
   BufType* msg = (BufType*)(buf);
   BufType::AcctHdr *auth = &(msg->acctHdr);

   uint8 respId = auth->id;
   Sptr<RadiusMessage> m = pendingMessages[respId];
   if (m != 0) {
      bool rv = m->handleResponse(msg, ln);
      pendingMessages[m->getRequestId()] = NULL; //Completed transaction
      return rv;
   }
   else {
      cpLog(LOG_ERR, "ERROR:  Could not find pending message for response id: %d",
            (int)(respId));
      return false;
   }  
}//handleResponse


bool RadiusStack::doSendRadius(Sptr<RadiusMessage> m) {
   int retr = m->getRemainingRetries();
   cpLog(LOG_DEBUG_STACK, "Sending request. Retries left: %d", retr);

   m->setRetries(retr - 1);

   int rv = m_udpConnection->doTransmit((char*)(m->getSendBuffer()), m->getSendBufferLen());
   if (rv != m->getSendBufferLen()) {
      if (rv == 0) {
         // EAGAIN, don't count this against the retries.
         m->setRetries(retr);
         return true;
      }
      else {
         cpLog(LOG_ALERT, "Error occured in sendRadius(), transmit: %d, len: %d (%s)",
               rv, m->getSendBufferLen(), strerror(errno));
         return false;
      }
   }
   else {
      cpLog(LOG_DEBUG_STACK, "Transmitted bytes:%d", m->getSendBufferLen());
      return true;
   }
}//doSendRadius


void RadiusStack::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                       uint64 now) {

   int rv = m_udpConnection->receive((char*) & (m_recvBuffer.buffer),
                                     sizeof(m_recvBuffer.buffer));
   if (rv > 0) {
      if (rv >= (int)(sizeof(m_recvBuffer.acctHdr))) {
         handleResponse(m_recvBuffer.buffer, rv);
      }
      else {
         cpLog(LOG_ERR, "ERROR:  Short message received: %d", rv);
      }
   }
   else if (rv < 0) {
      cpLog(LOG_ERR, "ERROR:  Error on UDP receive: %d", rv);
   }
   else {
      // nothing to read, but no errors.... continue as normal
   }
   
   // Loop through all messages...
   for (int i = 0; i<256; i++) {
      Sptr<RadiusMessage> m = pendingMessages[i];
      if (m != 0) {
         if (m->getNextTx() <= now) {
            if (doSendRadius(m)) {
               // Ok
            }
            else {
               if (m->getRemainingRetries() <= 0) {
                  // Give up on this guy
                  cpLog(LOG_ERR, "ERROR:  Never received response for: %s\n",
                        m->toString().c_str());
                  pendingMessages[i] = NULL;
               }
               else {
                  // TODO:  Verify timer, make it adjustable?  Maybe
                  // exponential backoff?
                  m->setNextTx(now + 500); // Try again in 1/2 second
               }
            }
         }
      }
   }
}

int RadiusStack::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        int& maxdesc, uint64& timeout, uint64 now) {

   m_udpConnection->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);

   for (int i = 0; i<256; i++) {
      Sptr<RadiusMessage> m = pendingMessages[i];
      if (m != 0) {
         if (m->getNextTx() <= now) {
            timeout = 0;
            m_udpConnection->addToFdSet(output_fds);
            break; //Can't get less timeout than this so break out.
         }
         else {
            timeout = min(m->getNextTx() - now, timeout);
         }
      }
   }
   return 0;
}


bool RadiusStack::findNextId(uint8& id) {
   static uint8 nextId = 0;
   nextId++;
   if (pendingMessages[nextId] == 0) {
      id = nextId;
      return true;
   }

   // Do exaustive search for a free slot
   for (int i = 0; i<256; i++) {
      if (pendingMessages[i] == 0) {
         id = i;
         return true;
      }
   }
   return false;
}
      

Sptr<RadiusMessage> RadiusStack::createAcctOnMsg() {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int acctOn = htonl(ACCT_ON);
      m->addHeader(PW_ACCOUNTING_REQUEST);
      m->addAttribute(PW_ACCT_STATUS_TYPE, acctOn);
      m->addAttribute(PW_NAS_IP_ADDRESS, m_clientAddr);
      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

Sptr<RadiusMessage>
RadiusStack::createAcctOffMsg() {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int acctOff = htonl(ACCT_OFF);

      m->addHeader(PW_ACCOUNTING_REQUEST);
      m->addAttribute(PW_ACCT_STATUS_TYPE, acctOff);
      m->addAttribute(PW_NAS_IP_ADDRESS, m_clientAddr);
      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

Sptr<RadiusMessage>
RadiusStack::createAcctStartCallMsg( const string &from,
                                     const string &to,
                                     const string &callId ) {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int acctStart = htonl(ACCT_START);
    
      m->addHeader(PW_ACCOUNTING_REQUEST);

      m->addAttribute(PW_ACCT_STATUS_TYPE, acctStart);
      m->addAttribute(PW_ACCT_SESSION_ID, (unsigned char*)callId.c_str(),
                      callId.size());

      m->addAttribute(PW_CALLING_STATION_ID,
                      (unsigned char*)from.c_str(),
                      from.size());
      m->addAttribute(PW_CALLED_STATION_ID,
                      (unsigned char*)to.c_str(),
                      to.size());
      m->addAttribute(PW_USER_NAME, (unsigned char*)callId.c_str(),
                      callId.size());

      m->addAttribute(PW_NAS_IP_ADDRESS, m_clientAddr);

      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

Sptr<RadiusMessage>
RadiusStack::createAcctStopCallMsg( const string &callId ) {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int acctStop = htonl(ACCT_STOP);
    
      m->addHeader(PW_ACCOUNTING_REQUEST);

      m->addAttribute(PW_ACCT_STATUS_TYPE, acctStop);
      m->addAttribute(PW_ACCT_SESSION_ID, (unsigned char*)callId.c_str(),
                      callId.size());
      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

Sptr<RadiusMessage>
RadiusStack::createAccessRqstMsg( const string &callId,
                                  const string &passwd ) {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);

      m->addHeader(PW_AUTHENTICATION_REQUEST);

      // TODO Password still needs to be hidden using
      // TODO RSA message Digest Algorithm MD5
      //
      m->addAttribute(PW_PASSWORD, (unsigned char*)passwd.c_str(),
                      passwd.size());

      m->addAttribute(PW_USER_NAME, (unsigned char*)callId.c_str(),
                      callId.size());
      m->addAttribute(PW_NAS_IP_ADDRESS, m_clientAddr);

      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

bool
RadiusStack::accountingOn() {
   cpLog(LOG_INFO, "Requesting Accounting On");
   return (createAcctOnMsg() != 0);
}

bool
RadiusStack::accountingOff() {
   cpLog(LOG_INFO, "Requesting Accounting Off");
   return (createAcctOffMsg() != 0);
}

bool
RadiusStack::accountingStartCall( const string &from,
                                  const string &to,
                                  const string &callId ) {
   if (!m_connected) {
      if (!accountingOn()) {
         return false;
      }
   }
   cpLog(LOG_DEBUG, "Requesting Accounting Start Call");
   return (createAcctStartCallMsg(from, to, callId) != 0);
}

bool
RadiusStack::accountingStopCall( const string &callId ) {
   if (!m_connected) {
      if (!accountingOn()) {
         return false;
      }
   }
   cpLog(LOG_DEBUG, "Requesting Accounting Stop Call");
   return (createAcctStopCallMsg(callId) != 0);
}


bool
RadiusStack::sendAccessRequest( const string &callId,
                                const string &passwd ) {
   cpLog(LOG_INFO, "Requesting Authentication and Access");
   return (createAccessRqstMsg(callId, passwd) != 0);
}

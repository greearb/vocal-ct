
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


static const char* const MindClient_cxx_Version =
    "$Id: MindClient.cxx,v 1.3 2004/06/15 00:30:10 greear Exp $";


#include "VocalCommon.hxx"
#include <netinet/in.h>
#include <string.h>


#include "MindClient.hxx"
#include "VCdrException.hxx"
#include "VRadiusException.hxx"
#include "cpLog.h"


MindClient* MindClient::m_instance = 0;

MindClient::MindClient(const string& local_ip) :
    RadiusStack(local_ip), /* take care of base class initialization */
    m_vsaStatus( MIND_VSA_FAIL ),
    m_maxCallingDur( 0 ),
    m_billingModel( 0 ),
    m_balance( "9999.99" ),
    m_currency()
{}

void MindClient::initialize( const string& local_ip,
                             const char *server,
                             const char *secretKey,
                             const int retries ) {
   if (!m_instance) {
      m_instance = new MindClient(local_ip);

      string serverNew(m_instance->m_serverName);
      string secretKeyNew(m_instance->m_secretKey);
      int retriesNew(m_instance->m_retries);
      
      if (server != 0) {
         serverNew = server;
      }
      if (secretKey != 0) {
         secretKeyNew = secretKey;
      }
      if (retries != 0) {
         retriesNew = retries;
      }

      try {
         // Call the RadiusStack base class.
         m_instance->RadiusStack::initialize(serverNew, secretKeyNew, retriesNew);
      }
      catch (VRadiusException &e) {
         delete m_instance;
         m_instance = 0;
         throw;
      }
   }
}//initialize


void
MindClient::destroy() {
   if (MindClient::m_instance) {
      delete MindClient::m_instance;
      MindClient::m_instance = 0;
   }
}

void
MindClient::processVsa( const unsigned char vsaType,
                        const unsigned char *ptr,
                        const int bufLen ) {
   string enumStr;
   MindVsaType atype = (MindVsaType)vsaType;
   
   switch (atype) {
   case MIND_VSA_MAX_CALL_DURATION: {
      m_maxCallingDur = ntohl((unsigned long int) * ptr);
      cpLog(LOG_DEBUG_STACK, "Attribute MIND_VSA_MAX_CALL_DURATION:%lu",
            m_maxCallingDur);
      break;
   }

   case MIND_VSA_BILLING_MODEL: {
      m_billingModel = (unsigned char) * ptr;
      cpLog(LOG_DEBUG_STACK, "Attribute MIND_VSA_BILLING_MODEL:%u",
            m_billingModel);
      break;
   }

   case MIND_VSA_BALANCE: {
      m_balance.assign((char*)ptr, bufLen - 2);
      cpLog(LOG_DEBUG_STACK, "Attribute MIND_VSA_BALANCE:%s",
            m_balance.c_str());
      break;
   }

   case MIND_VSA_CURRENCY: {
      m_currency.assign((char*)ptr, bufLen - 2);
      cpLog(LOG_DEBUG_STACK, "Attribute MIND_VSA_CURRENCY:%s",
            m_currency.c_str());
      break;
   }

   case MIND_VSA_STATUS: {
      m_vsaStatus = (MindVsaStatus) * ptr;
      enumStr = enumToString(m_vsaStatus);
      cpLog(LOG_DEBUG_STACK, "Attribute MIND_VSA_STATUS:%s",
            enumStr.c_str());
      break;
   }

   default: {
      cpLog(LOG_DEBUG_STACK, "Ignored attribute type:%d, length:%d",
            (int)atype, (int)bufLen);
      break;
   }
   }//switch
}//processVsa

int
MindClient::addMindCdr( const CdrRadius &ref, Sptr<RadiusMessage> m ) {
   unsigned long int vendorId = htonl(VENDOR_ID);
   unsigned char mindVsaType = (unsigned char)MIND_VSA_CDR;
   unsigned char *ptr = m->getSendBuffer();
   unsigned char *totalLength;
   unsigned char *vsaLength;

   ptr += m->getSendBufferLen();

   *ptr++ = (unsigned char)PW_VENDOR_SPECIFIC;    // RAD Type
   totalLength = ptr++;                           // RAD length
   memcpy( ptr, &vendorId, sizeof(unsigned long int) ); // Vendor ID
   ptr += sizeof(unsigned long int);
   *totalLength = sizeof(unsigned long int) + 2;

   *ptr++ = mindVsaType;                          // Vendor Type
   vsaLength = ptr++;                             // Vendor Length
   *vsaLength = 2;

   *vsaLength += ref.mindCdrVsaFormat(ptr);

   *totalLength += *vsaLength;
   m->setSendBufferLen(m->getSendBufferLen() + *totalLength);

   return *totalLength;
}

Sptr<RadiusMessage>
MindClient::sendAcctStartCallMsg( const CdrRadius &ref ) {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int vendorId = htonl(VENDOR_ID);
      unsigned long int acctStart = htonl(ACCT_START);
      unsigned char protocolNum = (unsigned char) MIND_VSA_DTMF;
      
      m->addHeader(PW_ACCOUNTING_REQUEST);

      m->addAttribute(PW_ACCT_STATUS_TYPE, acctStart);
      m->addAttribute(PW_ACCT_SESSION_ID, (unsigned char*)ref.m_callId,
                      strlen(ref.m_callId));
   
      if (ref.m_DTMFCalledNum[0] == 0) {
         protocolNum = (unsigned char) MIND_VSA_E164;
      }

      if (protocolNum == (unsigned char) MIND_VSA_DTMF) {
         m->addAttribute(PW_CALLED_STATION_ID,
                         (unsigned char*)ref.m_DTMFCalledNum,
                         strlen(ref.m_DTMFCalledNum));
      }
      else {
         m->addAttribute(PW_CALLED_STATION_ID,
                         (unsigned char*)ref.m_E164CalledNum,
                         strlen(ref.m_E164CalledNum));
      }

      m->addAttribute(PW_CALLING_STATION_ID,
                      (unsigned char*)ref.m_userId,
                      strlen(ref.m_userId));
      
      m->addVsaAttribute(MIND_VSA_PROTOCOL_NUMBER, (unsigned char)protocolNum,
                         vendorId);

      m->addVsaAttribute(MIND_VSA_CALLER_ID_TYPE, (unsigned char)MIND_VSA_ANI_CODE,
                         vendorId);

      m->addAttribute(PW_USER_NAME, (unsigned char*)ref.m_userId,
                      strlen(ref.m_userId));

      m->addVsaAttribute(MIND_VSA_CALL_INFO, (unsigned char)ref.m_callType,
                         vendorId);
    
      m->addVsaAttribute(MIND_VSA_CALL_PARTIES, (unsigned char)ref.m_callParties,
                         vendorId);

      m->addVsaAttribute(MIND_VSA_OUTBOUND_TYPE, (unsigned char)1,
                         vendorId);
    
      m->addVsaAttribute(MIND_VSA_QUERY_REQUEST, (unsigned char)1,
                         vendorId);

      unsigned long int startTime = htonl(ref.m_gwStartTime);
      m->addVsaAttribute(MIND_VSA_START_TIME, startTime, vendorId);
    
      m->addAttribute(PW_NAS_IP_ADDRESS, m_clientAddr);

      m->addVsaAttribute(MIND_VSA_ORIGINATE_ADDRESS, (unsigned long int)m_clientAddr,
                         vendorId);

      unsigned char callDirection = (unsigned char) ref.m_callDirection;
      m->addVsaAttribute(MIND_VSA_CALL_DIRECTION, callDirection, vendorId);

      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

Sptr<RadiusMessage>
MindClient::sendAcctStopCallMsg( const CdrRadius &ref ) {
   uint8 id;
   if (findNextId(id)) {
      Sptr<RadiusMessage> m = new RadiusMessage(this, id, m_retries);
      unsigned long int vendorId = htonl(VENDOR_ID);
      unsigned long int acctStop = htonl(ACCT_STOP);
    
      m->addHeader(PW_ACCOUNTING_REQUEST);
      
      m->addAttribute(PW_ACCT_STATUS_TYPE, acctStop);
    
      m->addAttribute(PW_ACCT_SESSION_ID, (unsigned char*)ref.m_callId,
                      strlen(ref.m_callId));

      m->addAttribute(PW_CALLING_STATION_ID,
                      (unsigned char*)ref.m_userId,
                      strlen(ref.m_userId));

      m->addAttribute(PW_ACCT_SESSION_TIME,
                      (unsigned long int) htonl( ref.m_gwEndTime - ref.m_gwStartTime));

      addMindCdr(ref, m);
    
      m->addVsaAttribute(MIND_VSA_CALL_DIRECTION, (unsigned char)ref.m_callDirection,
                         vendorId);
    
      m->addVsaAttribute(MIND_VSA_CALLER_ID_TYPE, (unsigned char)MIND_VSA_ANI_CODE,
                         vendorId);
    
      unsigned long int startTime = htonl(ref.m_gwStartTime);
      m->addVsaAttribute(MIND_VSA_START_TIME, startTime, vendorId);

      m->addMD5();
      pendingMessages[id] = m;
      doSendRadius(m);
      return m;
   }
   return NULL;
}

// true if queued up to send.
bool
MindClient::accountingStartCall( const CdrRadius &ref ) {
   if (!m_connected) {
      if (!accountingOn())
         return false;
   }
   cpLog(LOG_DEBUG_STACK, "Requesting Accounting Start Call");
   return (sendAcctStartCallMsg(ref) != 0);
}

// true if sent and acknowledged by server, false if any problem
bool
MindClient::accountingStopCall( const CdrRadius &ref ) {
   if (!m_connected) {
      if (!accountingOn()) {
         return false;
      }
   }
   cpLog(LOG_DEBUG_STACK, "Requesting Accounting Stop Call");
   return (sendAcctStopCallMsg(ref) != 0);
}

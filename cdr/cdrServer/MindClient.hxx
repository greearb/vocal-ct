#ifndef  MindClient_hxx
#define MindClient_hxx

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


static const char* const MindClient_hxx_Version =
    "$Id: MindClient.hxx,v 1.2 2004/06/09 07:19:34 greear Exp $";


#include "RadiusStack.hxx"
#include "CdrData.hxx"

// Global constants
//
const unsigned long int VENDOR_ID = 999;

// Forward Declarations
class VCdrException;

/**
 **   Singleton class - allow only one connection to the
 **   Mind server for each instance of the program
 */
class MindClient : public RadiusStack {
public:
   ///
   enum MindVsaType {
      MIND_VSA_CALLER_ID_TYPE = 1,       ///
      MIND_VSA_ORIGINATE_ADDRESS = 2,    ///
      MIND_VSA_BALANCE = 3,              ///
      MIND_VSA_CURRENCY = 4,             ///
      MIND_VSA_BILLING_MODEL = 5,        ///
      MIND_VSA_LANGUAGE = 6,             ///
      MIND_VSA_STATUS = 7,               ///
      MIND_VSA_CALL_DIRECTION = 8,       ///
      MIND_VSA_CALL_INFO = 9,            ///
      MIND_VSA_CALL_PARTIES = 10,        ///
      MIND_VSA_LINE = 11,                ///
      MIND_VSA_OUTBOUND_TYPE = 12,       ///
      MIND_VSA_QUERY_REQUEST = 13,       ///
      MIND_VSA_START_TIME = 14,          ///
      MIND_VSA_PROTOCOL_NUMBER = 15,     ///
      MIND_VSA_MAX_CALL_DURATION = 16,   ///
      MIND_VSA_CDR = 17                  ///
   };

public:

   /**
    * Singleton object reference.  Must call initialize once
    * before making this call.
    */
   static MindClient* instance() { return m_instance; }

   /**
    * Initialize the singlegon.
    * @param string& local_ip The local IP to bind with, or "" for the defaut
    * @param char* server name to connect with
    * @param char* secretKey
    * @param int retries
    * @return MindClient& a reference to the instance
    */
   static void initialize( const string& local_ip,
                           const char *server = 0,
                           const char *secretKey = 0,
                           const int retries = 0 );

   /// 
   static void destroy();

   /// Destructor
   virtual ~MindClient() {}

   /**
    * Sent when GW wants to start a call
    * @param CdrRadius& data record
    * @return bool true is success
    */
   bool accountingStartCall( const CdrRadius &ref );

   /**
    * Sent when the call is terminated
    * @param CdrRadius& data record
    * @return bool true is success
    */
   bool accountingStopCall( const CdrRadius &ref );


private:

   /// Constructor is private, must access through instance()
   MindClient(const string& local_ip);
   
   /// Not used (use the one above)
   MindClient();

   /**
    * Fills the m_sendBuffer with accounting start call message
    * @param CdrRadius& data record
    * @return void
    */
   void createAcctStartCallMsg( const CdrRadius &ref );

   /**
    * Fills the m_sendBuffer with accounting stop call message
    * @param CdrRadius& data record
    * @return void
    */
   void createAcctStopCallMsg( const CdrRadius &ref );

   /**
    * Adds CDR to m_sendBuffer, returns number of bytes added
    * @param CdrRadius& data record
    * @return int length of Cdr buffer
    */
   int addMindCdr( const CdrRadius &ref );

   /**
    * Process the VSA
    * @param unsigned char vsaType
    * @param unsigned char* ptr to the start location of the VSA
    * @param int bufLen length of this VSA
    * @return void
    */
   void processVsa( const unsigned char vsaType,
                    const unsigned char *ptr,
                    const int bufLen );
   
private:

   /// Static pointer to singleton instance
   static MindClient* m_instance;
                                             ///
   MindVsaStatus m_vsaStatus;           ///
   unsigned long int m_maxCallingDur;   ///
   unsigned char m_billingModel;        ///
   string m_balance;                    ///
   string m_currency;                   ///
};
#endif

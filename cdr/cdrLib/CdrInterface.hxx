#ifndef  CdrInterface_h
#define CdrInterface_h

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


static const char* const CdrInterface_hxx_Version =
    "$Id: CdrInterface.hxx,v 1.2 2004/06/07 08:32:19 greear Exp $";


#include <string>

#include "Tcp_ClientSocket.hxx"
#include "CdrData.hxx"
#include "VCdrException.hxx"


/**
 **  Singleton class, implements communications to the CDR server.
 **  Example call:
 **  
 **  // Do this early during initialization
 **  CdrInterface::instance( primaryHost, primaryPort,
 **                          backupHost,  backupPort);
 **
 **  CdrInterface &cdr = CdrInterface::instance();
 **
 **  cdr.callStarted("me","you","123");
 **
 **  // other stuff
 **
 **  cdr.callEnded("me","you","123");
 **
 */

class CdrInterface {
public:

   /// Interface to get the Singleton CdrInterface instance
   static CdrInterface& instance( const char *primaryHost = 0,
                                  const int primaryPort = 0,
                                  const char *backupHost = 0,
                                  const int backupPort = 0 );

   /// Destructor, disconnect with the CDR server
   virtual ~CdrInterface();

   /// Sends a call ring message to CDR server
   void ringStarted( const string &from,
                     const string &to,
                     const string &callId ) throw (VCdrException&);
   
   /// Sends a start call message to CDR server
   void callStarted( const string &from,
                     const string &to,
                     const string &callId ) throw (VCdrException&);

   /// Sends an end call message to CDR server
   void callEnded( const string &from,
                   const string &to,
                   const string &callId ) throw (VCdrException&);


   virtual int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                      int& maxdesc, uint64& timeout, uint64 now);
   
   virtual void tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                     uint64 now);

   
   /// Check to see that the connection to the CDR server is up
   bool isAlive() throw (VCdrException&);
   
   /// Connect to the CDR server
   void initialize() throw (VCdrException&);

   ///
   static void destroy();

private:
   /**
      Constructor intentionally made private so that it
      cannot be instantiated directly.
   **/
   CdrInterface( const string &primaryHost,
                 const int primaryPort,
                 const string &backupHost,
                 const int backupPort,
                 const string &marshalIp );

   /// Get seconds and milliseconds since the epoch
   void getTime( unsigned long &secs, unsigned int &millisecs );

   /// send data to Primary CDR Server
   void sendToPrimary( const CdrClient &dat ) throw (VCdrException&);

   /// send data to Primary CDR Server
   void sendToBackup( const CdrClient &dat ) throw (VCdrException&);

   /// Connect to specified CDR server
   Sptr<TcpClientSocket> connect( const string &host,
                                  const int port );

   /// Pointer to the singleton instance
   static CdrInterface* m_instance;

   /// Primary cdr server
   Sptr<TcpClientSocket> primaryCdr;

   /// Backup cdr server
   Sptr<TcpClientSocket> backupCdr;

   ///
   string m_primaryHost;          ///
   int m_primaryPort;          ///
   string m_backupHost;           ///
   int m_backupPort;           ///
   string m_marshalIp;            ///
   unsigned long m_lastPrimaryAttempt;   ///
   unsigned long m_lastBackupAttempt;    ///
}
;
#endif

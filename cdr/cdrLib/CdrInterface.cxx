
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


static const char* const CdrInterface_cxx_Version =
    "$Id: CdrInterface.cxx,v 1.3 2004/06/07 08:32:19 greear Exp $";


#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

#include "CdrInterface.hxx"
#include "NetworkAddress.h"
#include "VNetworkException.hxx"
#include "VCdrException.hxx"
#include "cpLog.h"

// global constants
const unsigned long CHECK_CONNECT_FREQ = 30;     // seconds
const char UNDEFINED_CDR_HOST[] = "undefined";

// Static variable initialization
CdrInterface* CdrInterface::m_instance = 0;


CdrInterface&
CdrInterface::instance( const char *primaryHost,
                        const int primaryPort,
                        const char *backupHost,
                        const int backupPort )
{
    if (!m_instance) {
       if ( primaryHost == 0 || primaryPort == 0 )
          throw VCdrException("Primary Server hostname and port number not specified",
                              __FILE__, __LINE__);

       string bkupHost(UNDEFINED_CDR_HOST);
       if ( backupHost == 0 || backupPort == 0 ) {
          cpLog(LOG_ALERT, "Backup CdrServer host and port unknown");
       }
       else {
          bkupHost = backupHost;
       }
       char nasname[1024];
       if ( gethostname(nasname, sizeof(nasname)) != 0 )
          strcpy(nasname, "localhost");

       NetworkAddress naddr(nasname);

       m_instance = new CdrInterface(primaryHost,
                                     primaryPort,
                                     bkupHost,
                                     backupPort,
                                     naddr.getIpName().convertString());

       m_instance->initialize();
    }
    return *m_instance;
}

CdrInterface::CdrInterface( const string &primaryHost,
                            const int primaryPort,
                            const string &backupHost,
                            const int backupPort,
                            const string &marshalIp ) :
        m_primaryHost(primaryHost),
        m_primaryPort(primaryPort),
        m_backupHost(backupHost),
        m_backupPort(backupPort),
        m_marshalIp(marshalIp),
        m_lastPrimaryAttempt(0),
        m_lastBackupAttempt(0)
{}


CdrInterface::~CdrInterface() {
   // Nothing to do at this point
}

void
CdrInterface::destroy()
{
    delete CdrInterface::m_instance;
    CdrInterface::m_instance = 0;
}

void
CdrInterface::initialize() throw (VCdrException&)
{
   // reinitializes the connection to primary CDR Server
   if (primaryCdr != 0) {
      primaryCdr = 0;
   }

   // reinitializes the connection to backup CDR Server
   if (backupCdr != 0) {
      backupCdr = 0;
   }

   cpLog(LOG_INFO, "Server Host:Port %s:%d, Backup Host:Port %s:%d",
         m_primaryHost.c_str(), m_primaryPort,
         m_backupHost.c_str(), m_backupPort);

   if ( !m_primaryHost.empty() && m_primaryHost != UNDEFINED_CDR_HOST ) {
      primaryCdr = connect(m_primaryHost, m_primaryPort);
   }

   if ( !m_backupHost.empty() && m_backupHost != UNDEFINED_CDR_HOST ) {
      backupCdr = connect(m_backupHost, m_backupPort);
   }

   if ( primaryCdr == 0 && backupCdr == 0 ) {
      cpLog(LOG_ALERT,
            "WARNING: Cannot connect to either primary or backup CDR Server");
      throw VCdrException("Cannot connect to either primary or backup CDR Server",
                          __FILE__, __LINE__);
   }
}

bool
CdrInterface::isAlive() throw (VCdrException&) {
    /*
        // TODO Need to find a way to check the current state of the connection
        try
        {
    	char tmpBuf[256];
    	int  readLeft;
     
    	if(primaryCdr &&
    	   primaryCdr->getConn().readn(tmpBuf,sizeof(tmpBuf),readLeft) < 0)
    	{
    	    primaryCdr = 0;
    	}
     
    	if(backupCdr &&
    	   backupCdr->getConn().readn(tmpBuf,sizeof(tmpBuf),readLeft) < 0)
    	{
    	    backupCdr = 0;
    	}
        }
        catch (VNetworkException &e)
        {
    	cpLog(LOG_ALERT,e.getDescription().c_str());
    	throw VCdrException(
    	    "WARNING: Error occured while trying to access the CDR Server",
    	    __FILE__,__LINE__);
        }
    */

    unsigned long secs;
    unsigned int msecs;
    getTime(secs, msecs);

    if ( primaryCdr == 0 ) {
       if ( m_primaryHost != UNDEFINED_CDR_HOST && !m_primaryHost.empty() ) {
          // Check connect every so often
          if ( secs - m_lastPrimaryAttempt > CHECK_CONNECT_FREQ ) {
             m_lastPrimaryAttempt = secs;
             primaryCdr = connect(m_primaryHost, m_primaryPort);
          }
       }
    }

    if ( backupCdr == 0 ) {
       if ( m_backupHost != UNDEFINED_CDR_HOST && !m_backupHost.empty() ) {
          // Check connect every so often
          if ( secs - m_lastBackupAttempt > CHECK_CONNECT_FREQ ) {
             m_lastBackupAttempt = secs;
             backupCdr = connect(m_backupHost, m_backupPort);
          }
       }
    }

    if ( primaryCdr == 0 && backupCdr == 0 ) {
       cpLog(LOG_ALERT,
             "WARNING: Both primary and alternate CDR Servers are unreachable");
       return false;
    }
    return true;
}

int CdrInterface::setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                         int& maxdesc, uint64& timeout, uint64 now) {
   if (primaryCdr != 0) {
      primaryCdr->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   if (backupCdr != 0) {
      backupCdr->setFds(input_fds, output_fds, exc_fds, maxdesc, timeout, now);
   }
   return 0;
}


void CdrInterface::tick(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
                        uint64 now) {
   if (primaryCdr != 0) {
      primaryCdr->tick(input_fds, output_fds, exc_fds, now);
      if (! primaryCdr->isLive()) {
         primaryCdr = NULL; // Kill it, will re-open directly
      }
   }
   if (backupCdr != 0) {
      backupCdr->tick(input_fds, output_fds, exc_fds, now);
      if (! backupCdr->isLive()) {
         backupCdr = NULL; // Kill it, will re-open directly
      }
   }
}


Sptr<TcpClientSocket>
CdrInterface::connect( const string &host,
                       const int port )
{
   Sptr<TcpClientSocket> sock;
   try {
      string empty_str;
      sock = new TcpClientSocket( host, port, empty_str, empty_str, true, false );
      sock->connect();
   }
   catch (VNetworkException &e) {
      sock = NULL;
      cpLog(LOG_ALERT,
            "WARNING: Cannot connect to CDR server %s:%d", host.c_str(), port);
      cpLog(LOG_ALERT, e.getDescription().c_str());
   }
   return sock;
}

void
CdrInterface::ringStarted( const string &from,
                           const string &to,
                           const string &callId) throw (VCdrException& )
{
    CdrClient dat;

    strncpy( dat.m_callId, callId.c_str(), sizeof(dat.m_callId) );
    strncpy( dat.m_userId, from.c_str(), sizeof(dat.m_userId) );
    strncpy( dat.m_recvId, to.c_str(), sizeof(dat.m_recvId) );
    dat.m_protocolNum = MIND_VSA_DTMF;

    getTime( dat.m_gwStartRing, dat.m_gwStartRingMsec );
    dat.m_callEvent = CALL_RING;
    dat.m_callParties = PHONE_TO_PHONE;
    dat.m_callType = TYPE_VOICE;
    dat.m_callDirection = DIRECTION_IN;

    strncpy( dat.m_originatorIp, m_marshalIp.c_str(), sizeof(dat.m_originatorIp) );
    strncpy( dat.m_terminatorIp, m_marshalIp.c_str(), sizeof(dat.m_terminatorIp) );

    cpLog( LOG_DEBUG, "Call Ringing, call ID:%s", callId.c_str() );

    try {
        sendToPrimary(dat);
    }
    catch (VCdrException &e) {
        cpLog(LOG_ALERT, e.getDescription().c_str());
        cpLog(LOG_ALERT, "Call not sent to Primary CDR Server");
    }

    try {
        sendToBackup(dat);
    }
    catch (VCdrException &e) {
        cpLog(LOG_ALERT, e.getDescription().c_str());
        cpLog(LOG_ALERT, "Call not sent to Backup CDR Server");
    }

    if (primaryCdr == 0 && backupCdr == 0) {
        cpLog(LOG_ALERT,
              "WARNING:Cannot connect to any CDR Server, No calls will be billed");
        throw VCdrException("Not connected to any CDR Servers, call NOT billed",
                            __FILE__, __LINE__);
    }
}

void
CdrInterface::callStarted( const string &from,
                           const string &to,
                           const string &callId ) throw (VCdrException&)
{
    CdrClient dat;

    strncpy( dat.m_callId, callId.c_str(), sizeof(dat.m_callId) );
    strncpy( dat.m_userId, from.c_str(), sizeof(dat.m_userId) );
    strncpy( dat.m_recvId, to.c_str(), sizeof(dat.m_recvId) );
    dat.m_protocolNum = MIND_VSA_DTMF;

    getTime( dat.m_gwStartTime, dat.m_gwStartTimeMsec );
    dat.m_callEvent = CALL_START;
    dat.m_callParties = PHONE_TO_PHONE;
    dat.m_callType = TYPE_VOICE;
    dat.m_callDirection = DIRECTION_IN;

    strncpy( dat.m_originatorIp, m_marshalIp.c_str(), sizeof(dat.m_originatorIp) );
    strncpy( dat.m_terminatorIp, m_marshalIp.c_str(), sizeof(dat.m_terminatorIp) );

    cpLog( LOG_DEBUG, "Call Started, call ID:%s", callId.c_str() );

    try {
        sendToPrimary(dat);
    }
    catch (VCdrException &e) {
        cpLog(LOG_ALERT, e.getDescription().c_str());
        cpLog(LOG_ALERT, "Call not sent to Primary CDR Server");
    }

    try {
        sendToBackup(dat);
    }
    catch (VCdrException &e) {
        cpLog(LOG_INFO, e.getDescription().c_str());
        cpLog(LOG_INFO, "Call not sent to Backup CDR Server");
    }

    if ( primaryCdr == 0 && backupCdr == 0 ) {
        cpLog(LOG_ALERT,
              "WARNING:Cannot connect to any CDR Server, No calls will be billed");
        throw VCdrException("Not connected to any CDR Servers, call NOT billed",
                            __FILE__, __LINE__);
    }
}

void
CdrInterface::callEnded( const string &from,
                         const string &to,
                         const string &callId ) throw (VCdrException&)
{
    CdrClient dat;

    strncpy( dat.m_callId, callId.c_str(), sizeof(dat.m_callId) );
    strncpy( dat.m_userId, from.c_str(), sizeof(dat.m_userId) );
    strncpy( dat.m_recvId, to.c_str(), sizeof(dat.m_recvId) );

    getTime( dat.m_gwEndTime, dat.m_gwEndTimeMsec );
    dat.m_callEvent = CALL_END;
    dat.m_callDisconnect = END_NORMAL;

    cpLog(LOG_DEBUG, "Call Ended, call ID:%s", callId.c_str());

    try {
        sendToPrimary(dat);
    }
    catch (VCdrException &e) {
        cpLog( LOG_ALERT, e.getDescription().c_str() );
        cpLog( LOG_ALERT, "Call not sent to Primary CDR Server" );
    }

    try {
        sendToBackup(dat);
    }
    catch (VCdrException &e) {
        cpLog( LOG_INFO, e.getDescription().c_str() );
        cpLog( LOG_INFO, "Call not sent to Backup CDR Server" );
    }

    if ( primaryCdr == 0 && backupCdr == 0 ) {
        cpLog( LOG_ALERT,
               "WARNING:There are no connections to any CDR Servers, No calls will be billed");
        throw VCdrException("Not connected to any CDR Servers, call NOT billed",
                            __FILE__, __LINE__);
    }
}

void
CdrInterface::sendToPrimary( const CdrClient &dat ) throw (VCdrException&)
{
   if ( primaryCdr == 0 ) {
      if (m_primaryHost != UNDEFINED_CDR_HOST && !m_primaryHost.empty()) {
         // Check connect every so often
         unsigned long secs;
         unsigned int msecs;
         getTime(secs, msecs);
         if ( secs - m_lastPrimaryAttempt > CHECK_CONNECT_FREQ ) {
            m_lastPrimaryAttempt = secs;
            primaryCdr = connect( m_primaryHost, m_primaryPort );
            if (primaryCdr == 0)
               return ;
         }
      }
      throw VCdrException("No connection exists", __FILE__, __LINE__);
   }

   CdrClient tmp(dat);
   primaryCdr->getConn()->queueSendData((const char*)(&tmp), sizeof(tmp));
}

void
CdrInterface::sendToBackup( const CdrClient &dat ) throw (VCdrException&) {
   if ( backupCdr == 0 ) {
      if ( m_backupHost != UNDEFINED_CDR_HOST && !m_backupHost.empty() ) {
         // Check connect every so often
         unsigned long secs;
         unsigned int msecs;
         getTime(secs, msecs);
         if (secs - m_lastBackupAttempt > CHECK_CONNECT_FREQ) {
            m_lastBackupAttempt = secs;
            backupCdr = connect(m_backupHost, m_backupPort);
            if (backupCdr == 0)
               return ;
         }
      }
      throw VCdrException("No connection exists", __FILE__, __LINE__);
   }

   CdrClient tmp(dat);
   backupCdr->getConn()->queueSendData((const char*)(&tmp), sizeof(tmp));
}

void
CdrInterface::getTime( unsigned long &secs, unsigned int &millisecs ) {
    struct timeval tval;

    // get the seconds and milliseconds for the day
    gettimeofday(&tval, 0);
    secs = (unsigned long) tval.tv_sec;
    millisecs = (unsigned int) tval.tv_usec / 1000;
}

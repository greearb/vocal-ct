
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


static const char* const CdrMarshal_cxx_Version =
    "$Id: CdrMarshal.cxx,v 1.2 2004/06/14 00:33:53 greear Exp $";


#include "CdrMarshal.hxx"
#include "CdrManager.hxx"
#include "VNetworkException.hxx"
#include "cpLog.h"


CdrMarshal::CdrMarshal() :
    EventObj( (int)0, true ),      // ensure use of constructor EventObj(int)
    m_conn( false )          // set non-blocking
{}

void
CdrMarshal::onData() {
   try {
      getData();
   }
   catch ( VNetworkException &e ) {
      // remove marshal from list when it's connection is closed
      m_done = true;
      
      cpLog( LOG_ALERT, "Marshal marked for removal from list. Reason:%s",
             e.getDescription().c_str() );
   }
}

void
CdrMarshal::getData() throw (VNetworkException&) {
   CdrClient msg;
   static int datasize = sizeof(msg);

   
   int n = m_conn.read();

   if (( n < datasize ) || (!m_conn.isLive())) {
      m_conn.close();
      // TODO:  Gotta be a cleaner way than throwing an exception here.
      throw VNetworkException( "Socket read failed, connection terminated",
                               __FILE__, __LINE__ );
   }

   
   while (m_conn.getRcvBytesWaiting() >= datasize) {
      m_conn.peekRcvdBytes((unsigned char*)(&msg), datasize);
      m_conn.consumeRcvdBytes(datasize);
      CdrManager::instance().addCache(msg);
   }
}//getData

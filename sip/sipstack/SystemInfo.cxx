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

#include <iostream>
#include <sys/types.h>
#include <errno.h>

#include "global.h"
#include "cpLog.h"
#include "NetworkAddress.h"
#include "SystemInfo.hxx"
#include <misc.hxx>

#ifndef __WIN32__
#include <netdb.h>
#endif

using namespace Vocal;

// 24/11/03 fpi
// WorkAround Win32
#ifdef WIN32
#include <ws2tcpip.h>
#endif

SystemInfo Vocal::theSystem;

SystemInfo::SystemInfo() {
    int err;

    sessionName = "VOVIDA Session";
    userName = "-";

#ifdef __WIN32__
    // Initialize winsock

    WORD wVersionRequested;
    WSADATA wsaData;
    
    wVersionRequested = MAKEWORD( 2, 0 );
    if ((err = WSAStartup( wVersionRequested, &wsaData )) != 0) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        cerr << "ERROR:  Could not load Winsock 2.0 DLLs, err: " << err << endl;
    }

#endif

    char tmp[NI_MAXHOST];
    err = gethostname(tmp, NI_MAXHOST);
    if (err != 0) {
        cerr << "ERROR:  Failed to gethostname, err: " << err << "  "
             << VSTRERROR << endl;
        tmp[0] = 0;
    }
    else {
        hostName = tmp;
    }
}

bool SystemInfo::toIpv4String(const char* ip, uint32& retval) {
    return vtoIpString(ip, retval);
}


const string SystemInfo::resolveIpToHostname(const string& ipaddr) {
    Data d(ipaddr);
    NetworkAddress na(d);
    string rv = na.getHostName().c_str();
    return rv;
}


SystemInfo::~SystemInfo() { }

void
Vocal::parseSystemInfo(char* tag, char* type, char* value)
{
    if (strcmp(tag, "REGISTER") == 0)
    {
        //check for url and domain.
        if (strcmp(type, "url") == 0)
        {
            Vocal::theSystem.setUrlToRegister(value);
        }
        else if (strcmp(type, "domain") == 0)
        {
            if (strcmp(value, "DEFAULT") != 0)
            {
                theSystem.setRegisterDomain(value);
            }
        }
    }
    else if (strcmp(tag, "FROM") == 0)
    {
        //handle the username, and displayname.
        if (strcmp(type, "user") == 0)
        {
            theSystem.setUserName(value);
        }
        else if (strcmp(type, "display") == 0)
        {
            if (strcmp(value, "DEFAULT") != 0)
            {
                theSystem.setDisplayName(value);
            }
        }
    }
    else if (strcmp(tag, "TO") == 0)
    {
        //currently do nothing.
    }

    else if (strcmp(tag, "SESSION") == 0)
    {
        //handle the session name.
        if (strcmp(type, "name") == 0)
        {
            theSystem.setSessionName(value);
        }
    }
}

bool
SystemInfo::isMe(const NetworkAddress& na)
{
    NetworkAddress me(hostName, mySystemPort);
    return (na.getIpAndPortName() == me.getIpAndPortName());
}

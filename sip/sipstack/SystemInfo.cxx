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

static const char* const SystemInfo_cxx_Version =
"$Id: SystemInfo.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <iostream>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "global.h"
#include "cpLog.h"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "SystemInfo.hxx"

using namespace Vocal;

// 24/11/03 fpi
// WorkAround Win32
#ifdef WIN32
#include <ws2tcpip.h>
#endif

SystemInfo Vocal::theSystem;

SystemInfo::SystemInfo()
: userName(0),
  displayName(0),
  hostName(0),
  hostIP(0),
  sessionName(0),
  registerDomain(0),
  registerUrl(0)
{
    hostName = new char[NI_MAXHOST+1];
    hostIP = new char[NI_MAXHOST+1];
    int err;

#ifdef WIN32
    WORD wVersionRequested;
    WSADATA wsaData;

    wVersionRequested = MAKEWORD( 2, 2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    assert(err == 0);
#endif

    makeCopy(&sessionName , "VOVIDA Session");
    makeCopy(&userName , "-");

    //initialize registerDomain.
    makeCopy(&registerDomain , "");
    makeCopy(&registerUrl , "");
    makeCopy(&displayName , "");

    err = gethostname(hostName, NI_MAXHOST);
    assert( err == 0 );

    // can not call cpLog becuase it may not be constructed yet

    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr *sa;

    memset(&hints, 0, sizeof(hints));
    
    /* set-up hints structure */
    hints.ai_family = NetworkConfig::instance().getAddrFamily();

    err = getaddrinfo(hostName, NULL, &hints, &res);

    if (err) {
       cpLog(LOG_ERR, "Failed to resolve the host name, reason %s",
                  gai_strerror(err));
       assert(0);
    } else {
       sa = (struct sockaddr *) res->ai_addr;
       err = getnameinfo(sa, res->ai_addrlen, hostIP, 256, NULL, 0, NI_NUMERICHOST); 
       if (err) {
	  cpLog(LOG_ERR, "Failed to get the host name info, %s" ,hostIP);
          perror("getnameinfo");
          assert(0);
       }
       if (res->ai_family == PF_INET6) { // Enclose IP in ['s
		string tmp(hostIP);
		tmp.insert(0,"[");
		tmp+="]";
		const char* tmp2 = tmp.c_str();
                memcpy((void *)hostIP, (void *)tmp2, tmp.length());
       }
    }
    freeaddrinfo(res);

/*    char buf[2048];
    int myErrno;

    int retval = NetworkAddress::getHostByName(hostName, &ent, buf, 2048, &myErrno);
    if (retval != NetworkAddress::getHostLookupOK)
    {
        ipName = string("0.0.0.0");
    }
    else
    {
        unsigned char* loc = (unsigned char*)ent.h_addr_list[0];

        char buf[1000];
        sprintf(buf, "%d.%d.%d.%d",
                int(*(loc + 0)),
                int(*(loc + 1)),
                int(*(loc + 2)),
                int(*(loc + 3)) );

        ipName = buf;
    }

    makeCopy(&hostIPAddress, ipName.c_str());
    breakIP();*/

    // can not call cpLog becuase it may not be constructed yet
}


/*
void
SystemInfo::breakIP()
{
    //get hostAddress and get first three octects in this.

    char buf[256];

    // can not call cpLog becuase it may not be constructed yet

    int i1, i2, i3, i4;
    sscanf(hostIPAddress, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);

    sprintf(buf, "%d.%d.%d", i1, i2, i3);
    firstIPpart = new char[strlen(buf) + 1];
    strcpy(firstIPpart, buf);
}*/



bool SystemInfo::toIpv4String(const char* ip, unsigned long& retval) {
   retval = 0;

   if (strcmp(ip, "0.0.0.0") == 0) {
      return true;
   }

   struct hostent *hp;
   hp = gethostbyname(ip);
   if (hp == NULL) {
      return false;
   }//if
   else {
      retval = ntohl(*((unsigned long*)(hp->h_addr_list[0])));
      if (retval != 0) {
         return true;
      }
      else {
         return false;
      }
   }
   return false;
}


const string SystemInfo::resolveIpToHostname(const string& ipaddr) {
    Data d(ipaddr);
    NetworkAddress na(d);
    string rv = na.getHostName().c_str();
    return rv;
}


void
SystemInfo::makeCopy(char** dest, const char* src)
{
    delete [] *dest;
    *dest = new char[strlen(src) + 1];
    strcpy(*dest, src);
}

SystemInfo::~SystemInfo()
{
    delete [] userName;
    delete [] displayName;
    delete [] hostName;
    delete [] hostIP;
    delete [] sessionName;
    delete [] registerDomain;
    delete [] registerUrl;
}

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
/*
const char* const 
SystemInfo::getFirstIPpart() const
{ //return the first three octects of the IP address
    cpLog(LOG_DEBUG_STACK, "FirstIPPart: %s", firstIPpart);
    return firstIPpart;
}*/

bool
SystemInfo::isMe(const NetworkAddress& na)
{
    NetworkAddress me(hostName, mySystemPort);
    return (na == me);
}

///
// NOTE:  You probably should use the local_ip as configured by the
// user, or at least check to see if the user set the local_ip before
// using this method. --Ben
const char* const 
SystemInfo::gethostAddress (char* buf, int len, int family) const
{
    struct addrinfo hints;
    struct addrinfo *res;
    struct sockaddr *sa;

    assert(buf);
    memset(&hints, 0, sizeof(hints));
    
    /* set-up hints structure */
    hints.ai_family = family;

    int err = getaddrinfo(hostName, NULL, &hints, &res);

    if (err) {
       cpLog(LOG_ERR, "Failed to resolve the host name, reason %s",
                  gai_strerror(err));
       buf[0] = 0; 
    } else {
       sa = (struct sockaddr *) res->ai_addr;
       err = getnameinfo(sa, res->ai_addrlen, buf, len, NULL, 0, NI_NUMERICHOST); 
       if (err) {
	  cpLog(LOG_ERR, "Failed to get the host name info for %s, reason %s" ,hostName, gai_strerror(err));
          buf[0] = 0;
       }
    }
    freeaddrinfo(res);
    return buf;
}



/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

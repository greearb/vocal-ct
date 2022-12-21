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


#include <string>
#if defined(__FreeBSD__)
#include <sys/types.h>
#include <netdb_r.h>
#endif
#if defined (__APPLE__)
#include <sys/types.h>
#endif
#include "errno.h"
#include <errno.h>
#include <sstream>
#include <iostream>
#include <string.h>

#include "global.h"
#include "NetworkAddress.h"
#include "cpLog.h"

#include "vsock.hxx"

NetworkAddress::NetworkAddress ( const Data& hostname, int port /* = -1 */)
    : aPort(port),
      ipAddressSet(false)
{
    if (hostname == "") {
        hostName = getLocalHostName();
    }
    else {
        setHostName(hostname);
    }
}

const Data& NetworkAddress::getLocalHostName() {
   static Data localHostName;
   if (localHostName.size() == 0) {
      char buffer[1024];
      if (gethostname(buffer, 1024) == -1) {
         cpLog(LOG_ERR, "ERROR:  Failed to get hostname: %s will use 'localhost'\n", VSTRERROR);
         localHostName = "localhost";
      }
      else {
         localHostName = buffer;
      }
   }
   return localHostName;
}

void NetworkAddress::setHostName ( const Data& hostname ) {
   //cpLog (LOG_DEBUG_STACK, "NetworkAddress::setHostName(%s)", hostname.c_str());

   ipAddressSet = false;
   bool v6Addr = false;
   Data tmpAddr = hostname;
   int pos = hostname.find("]");
   //case 1 [ipv6Address]
   if (pos != Data::npos) {
      int match = hostname.find("[");
      if (match != Data::npos) {
         //cpLog (LOG_DEBUG_STACK, "Detected v6 address");
         hostName = hostname;
         tmpAddr = hostname.substr(match+1,pos-1);
         v6Addr = true;
      }
      pos = hostname.find(":", pos);
      if (pos != Data::npos) {
         setPort(atoi(hostname.substr(pos+1, Data::npos).c_str()));
      }
   }
   else if(is_valid_ip6_addr(hostname)) {
      //case 2 - ipv6address
      //Do nothing
      v6Addr = true;
   }
   else {
      pos = hostname.find(":", 0);
      if (pos != Data::npos) {
         hostName = hostname.substr(0,pos);
         tmpAddr = hostName;
         setPort(atoi(hostname.substr(pos+1, Data::npos).c_str()));
      }
      else {
         hostName = hostname;
      }
   }
  
   //fall through - all other cases 
   if (v6Addr || (is_valid_ip_addr(tmpAddr))) { // Hostname is infact an IP address
      //cpLog (LOG_DEBUG_STACK, "Setting ipAddress to %s", tmpAddr.c_str());
      hostIPstring = tmpAddr;
      ipAddressSet = true;
      if (vtoIpString(hostIPstring.c_str(), hostIP) < 0) {
         cpLog(LOG_ERR, "ERROR:  Could not resolve hostname: %s, error: %s\n",
               hostName.c_str(), VSTRERROR);
         // Set to zeros.
         hostIP = 0;
      }
      hostName = "";  //Set it later when someone asks for it
   }
   else {
      ipAddressSet = false;
   }
}


void NetworkAddress::setPort( int iPort ) {
   aPort = iPort;
}

string NetworkAddress::resolveHostName(uint32 addr) {
   struct hostent *hp;
   if ((hp = gethostbyaddr((char*)(&addr), sizeof(addr), AF_INET))) {
      return hp->h_name;
   }
   return vtoStringIp(addr);
}

const Data& NetworkAddress::getHostName( )  const {
   //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getHostName()");
   if (hostName.length() == 0) {
      hostName = resolveHostName(hostIP);
   }
   return hostName;
}


const Data& NetworkAddress::getIpName () const {
   //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getIpName()");
   if (!ipAddressSet)
      initIpAddress();
   //cpLog(LOG_DEBUG_STACK, "returning: %s", ipAddress.logData());
   //assert(ipAddress != "127.0.0.1");
   return hostIPstring;
}


// Returns something like: "192.168.1.1:666"
const string NetworkAddress::getIpAndPortName() const {
    getIpName(); //Ensure ipAddress cache is up to date
    int sz = hostIPstring.size() + 20;
    char tmp[sz];
    snprintf(tmp, sz, "%s:%d", hostIPstring.c_str(), getPort());
    return tmp;
}



uint32 NetworkAddress::getIp4Address () const {
   if (!ipAddressSet)
      initIpAddress();
   return hostIP;
}


int NetworkAddress::getPort () const {
   //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getPort()");
   return aPort;
}

/** For debugging, can put this in logs, for instance */
string NetworkAddress::toString() const {
    ostringstream oss;
    oss << hostName << "(" << hostIPstring << ")";

    oss << ":" << aPort;

    oss << " " << hostIP << " ip-set: " << ipAddressSet << endl;

    return oss.str();
}


ostream & NetworkAddress::print ( ostream & xStr ) const {
   xStr << hostName << "(" << getIpName() << ")";

   if ( aPort ) {
      xStr << ":" << aPort;
   }

   return xStr;
}

uint32 NetworkAddress::hashIpPort( ) const {
   return hashIpPort(getIp4Address(), getPort());
}


uint32 NetworkAddress::hashIpPort( const uint32 lipAddress, const int port ) {
   return (lipAddress << 16) | (port & 0xFF);
}


void NetworkAddress::initIpAddress() const {
   //cpLog(LOG_DEBUG_STACK, "initIpAddress() for hostName %s", hostName.c_str());
           
   if (ipAddressSet)
      return;

   if (hostName.size() == 0) {
      // Haven't resolved it yet, IP address better be valid!
      if (!is_valid_ip4_addr(hostIPstring)) {
         cpLog(LOG_ERR, "ERROR:  invalid state, this: %s", toString().c_str());
         assert(is_valid_ip4_addr(hostIPstring));
      }
      else {
         // Resolve the IP addr
         if (vtoIpString(hostIPstring.c_str(), hostIP) < 0) {
            cpLog(LOG_ERR, "ERROR:  Could not resolve hostname: %s, error: %s\n",
                  hostIPstring.c_str(), VSTRERROR);
            // Set to zeros.
            hostIP = 0;
         }
         ipAddressSet = true;
      }
      return;
   }

   if (vtoIpString(hostName.c_str(), hostIP) < 0) {
      cpLog(LOG_ERR, "ERROR:  Could not resolve hostname: %s, error: %s\n",
            hostName.c_str(), VSTRERROR);
      // Set to zeros.
      hostIP = 0;
   }

   hostIPstring = vtoStringIp(hostIP);
   ipAddressSet = true;
}

NetworkAddress::NetworkAddress( const NetworkAddress& x) {
   *this = x;
}

NetworkAddress& NetworkAddress::operator=( const NetworkAddress& x ) {
   aPort = x.aPort;
   hostIPstring = x.getIpName();
   ipAddressSet = false;
   hostName = x.hostName;
   initIpAddress();
   return *this;
}

/**
  * Return TRUE if the address is a valid IP address (dot-decimal form)
  * such as 128.128.128.128. Checks for invalid or ill formed addresses 
  * If the address is valid, copy it into hostIPstring
  */
bool NetworkAddress::is_valid_ip_addr(const Data& addr) const {
   return (is_valid_ip4_addr(addr) || is_valid_ip6_addr(addr));
}

bool NetworkAddress::is_valid_ip6_addr(const Data& addr) {
   //cpLog (LOG_DEBUG_STACK, "is_valid_ip6_addr(%s)", addr.c_str());
   bool double_colon = false;
   int pos = addr.find("[");
   if (pos!=Data::npos) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v6 address (found [)");
      return false;
   }

   pos = addr.find(":");
   if (pos!=Data::npos) {
      //"a.b.c.d:5060"
      pos = addr.find(":", pos+1); 
      if(pos != Data::npos) {
         pos = addr.find("::", 0); 
         if(pos != Data::npos) {
            double_colon = true;
            //ONLY "::"
            if (pos == (addr.length()-2)) {
               cpLog (LOG_DEBUG_STACK, "Invalid v6 address (invalid ::)");
               return false;
            }
            //"::abcd::"
            pos = addr.find("::", pos+1);
            if (pos!=Data::npos) {
               cpLog (LOG_DEBUG_STACK, "Invalid v6 address");
               return false;
            }
         }
      }
      else {
         return false;
      }
   }

   int count = 0;
   char* szInitialString = new char[255];
   strncpy(szInitialString, addr.c_str(), 254);
   szInitialString[254] = '\0';

   char* szTokOut = strtok(szInitialString, ":");
   while(szTokOut) {
      if (!is_v6_string(szTokOut)) {
         //cpLog (LOG_DEBUG_STACK, "Invalid v6 address (invalid string %s)", szTokOut);
         //      return false;
      }
      count++;
      
      if (count > 8) {
         break;
      }
      szTokOut=strtok(NULL, ":");
   };

   delete[] szInitialString;

   if (double_colon) {
       //cpLog (LOG_DEBUG_STACK, "Valid v6 address (contains ::)");
       return true;
   }

   if (count == 8) {
       //cpLog (LOG_DEBUG_STACK, "Valid v6 address");
       return true;
   }

   //cpLog (LOG_DEBUG_STACK, "Invalid v6 address");
   return false;
}

bool NetworkAddress::is_v6_string(const Data& addr) {
   if (addr.length() > 4) {
      return false;
   }
   int next = 0;
   Data tmp;
   char buf[5];
   memcpy(buf, addr.c_str(), 4);
   
   while (next < addr.length()) {
      if (!isdigit(buf[next]) && !isxdigit(buf[next])) {
         return false;
      }
      next++;
   }
   return true;
}



bool NetworkAddress::is_valid_ip4_addr(const Data& addr) {
   //cpLog (LOG_DEBUG_STACK, "is_valid_ip4_addr(%s)", addr.c_str());

#if defined(WIN32)
   typedef ULONG ulong;
#endif

   unsigned long maskcheck = ~255; // mask to check if 'tween 0 and 255
   const char *advancer = addr.c_str();
   unsigned long octet;
   char *nextchar;
   
   // always check for spaces and right number
   // first and last fields must be 1-255, middle two 0-255
    
   if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t')) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }
   octet = strtoul(advancer, &nextchar, 10);
   if((*nextchar != '.') || (octet & maskcheck) || (octet == 0)) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }

   advancer = nextchar+1;
   if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t')) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }
   octet = strtoul(advancer, &nextchar, 10);
   if((*nextchar != '.') || (octet & maskcheck)) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }

   advancer = nextchar+1;
   if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t')) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }
   octet = strtoul(advancer, &nextchar, 10);
   if((*nextchar != '.') || (octet & maskcheck)) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }

   advancer = nextchar+1;
   if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t')) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }
   octet = strtoul(advancer, &nextchar, 10);
   if((*nextchar) || (octet & maskcheck) || (octet == 0)) {
      //cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
      return false;
   }
   
   //cpLog (LOG_DEBUG_STACK, "Valid v4 address");
   return true;
}

// End of File


#if defined (WIN32)
char* formatWindowsError(char* buffer, int size)
{
        int err = GetLastError();

        FormatMessage(
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL,
                err,
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                (LPTSTR)buffer,
                size,
                NULL
                );

        return buffer;
}
#endif


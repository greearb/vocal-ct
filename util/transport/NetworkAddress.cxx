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


static const char* const NetworkAddress_cxx_Version =
"$Id: NetworkAddress.cxx,v 1.4 2004/10/29 07:22:35 greear Exp $";

#include <string>
#if defined(__FreeBSD__) || defined (__APPLE__)
#include <sys/types.h>
#include <netdb_r.h>
#endif
#if defined (__APPLE__)
#include <sys/types.h>
#include "netdb_r.h"
#endif
#include "errno.h"
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sstream>
#include <netdb.h>
#include <iostream>
#include <string.h>

#include "global.h"
#include "NetworkAddress.h"
#include "NetworkConfig.hxx"
#include "cpLog.h"

#include "vsock.hxx"
#ifdef WIN32
#include "Lock.hxx"
#include "Mutex.hxx"
using Vocal::Threads::Mutex;
using Vocal::Threads::Lock;
#endif

const int NetworkAddress::getHostLookupFailed = 1;
const int NetworkAddress::getHostLookupOK = 0;
string NetworkAddress::localHostName;

NetworkAddress::UnresolvedException::UnresolvedException(const string& reason, 
                                                         const string& file,
                                                         const int line) :
   VException(reason, file, line)
{
}


NetworkAddress::NetworkAddress ( const Data& hostname, int port /* = -1 */)
    : aPort(port),
      ipAddressSet(false),
      sockAddrSet(false)
{
    if (hostname == "") {
        hostName = getLocalHostName();
    }
    else {
        setHostName(hostname);
    }
}

Data
NetworkAddress::getLocalHostName() 
{
   if(localHostName.size() == 0)
   {
       char buffer[1024];
       if (gethostname(buffer, 1024) == -1)
       {
          throw UnresolvedException("Could not get hostname", __FILE__,__LINE__);
       }
       localHostName = buffer;
   }
   return localHostName;
}

void
NetworkAddress::setHostName ( const Data& hostname )
{
    //cpLog (LOG_DEBUG_STACK, "NetworkAddress::setHostName(%s)", hostname.c_str());

   ipAddressSet = false;
   bool v6Addr = false;
   sockAddrSet = false;
   Data tmpAddr = hostname;
   int pos = hostname.find("]");
   //case 1 [ipv6Address]
   if (pos != Data::npos)
   {
       int match = hostname.find("[");
       if (match!=Data::npos) //Found matching [
       {
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
   else if(is_valid_ip6_addr(hostname))
   {
        //case 2 - ipv6address
        //Do nothing
	v6Addr = true;
   }
   else
   {
       pos = hostname.find(":", 0);
       hostName = hostname.substr(0,pos);
       tmpAddr = hostName;
       if (pos != Data::npos) {
         setPort(atoi(hostname.substr(pos+1, Data::npos).c_str()));
       }
   }
  
   //fall through - all other cases 
   if (v6Addr || (is_valid_ip_addr(tmpAddr))) // Hostname is infact an IP address
   {
       //cpLog (LOG_DEBUG_STACK, "Setting ipAddress to %s", tmpAddr.c_str());
	ipAddress = tmpAddr;
	ipAddressSet = true;
        hostName = "";  //Set it later when someone asks for it
        //Get the hostName from the IP address
        //hostName = getHostByAddress(tmpAddr);
   }
   else 
   {
       ipAddressSet = false;
   }
   //assert(getIpName() != "127.0.0.1");
}


void
NetworkAddress::setPort( int iPort )
{
    if(iPort != aPort)
    {
         //cpLog (LOG_DEBUG_STACK, "NetworkAddress::setPort(%d)", iPort);
         sockAddrSet = false;
         aPort = iPort;
    }
}


const Data&
NetworkAddress::getHostName( )  const
{
    //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getHostName()");
   if(hostName.length() == 0)
   {
       hostName = getHostByAddress(ipAddress);
   }
   return hostName;
}


const Data&
NetworkAddress::getIpName () const {
    //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getIpName()");
    if (!ipAddressSet)
        initIpAddress();
    //cpLog(LOG_DEBUG_STACK, "returning: %s", ipAddress.logData());
    //assert(ipAddress != "127.0.0.1");
    return ipAddress;
}


// Returns something like: "192.168.1.1:666"
const string NetworkAddress::getIpAndPortName() const {
    getIpName(); //Ensure ipAddress cache is up to date
    int sz = ipAddress.size() + 20;
    char tmp[sz];
    snprintf(tmp, sz, "%s:%d", ipAddress.c_str(), getPort());
    return tmp;
}



u_int32_t
NetworkAddress::getIp4Address () const
{
   cpLog(LOG_ERR, "****** UNSUPPORTED NetworkAddress::getIp4Address() called");
   
   // TODO!!
/*
    u_int32_t lTmp;
    initIpAddress();
    memcpy((void *) &lTmp, ipAddress, IPV4_LENGTH);
    return lTmp;
    */
    return 0;
}


void
NetworkAddress::getSockAddr (struct sockaddr_storage * socka,
                             struct addrinfo* uhints /* default 0 */) const {
   //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getSockAddr()");

   if (sockAddrSet) {
       memcpy(socka, &sa_cache, sizeof(sa_cache));
       return;
   }

   char port_stor[56];
   char* port = NULL;
   port_stor[0] = 0;
   
   // Check port isnt -1
   if (aPort == -1) {
       cpLog(LOG_DEBUG, "No port for host name:%s", hostName.c_str());
   }
   else {
       port = port_stor;
       snprintf(port, 55, "%u", aPort);
   }
   
   if ((!ipAddressSet) || (!is_valid_ip_addr(ipAddress))) {
       initIpAddress();
   }
   
   if ((ipAddress.length() == 0) && (port == NULL)) {
       cpLog(LOG_ERR, "Failed to get address info, hostname: %s",
             hostName.c_str());
       return; // Nothing for us to do
   }
   
   //cpLog(LOG_DEBUG_STACK, "ipAddress: %s", ipAddress.logData());
   
   struct addrinfo hints;
   struct addrinfo *res = 0; // xxx workaround for glibc ambiguity
   
   // Setup structures
   memset(&hints, 0, sizeof(hints));
   //cpLog(LOG_DEBUG_STACK, "getaddrinfo()");
   int error;
   if (uhints != 0) {
       //Use the hints caller has provide
       memcpy(&hints, uhints, sizeof(hints));
       if(hints.ai_flags == AI_PASSIVE) {
           error = getaddrinfo(0, port, &hints, &res);
       }
       else {
           error = getaddrinfo(ipAddress.logData(), port, &hints, &res);
       }
   }
   else {
       hints.ai_flags = AI_NUMERICHOST;
       hints.ai_family = NetworkConfig::instance().getAddrFamily();
       hints.ai_socktype = SOCK_DGRAM;
       if (ipAddress.length()) {
           error = getaddrinfo(ipAddress.logData(), port, &hints, &res);
       }
       else {
           error = getaddrinfo(0, port, &hints, &res);
       }
   }
   
   if (error) {
       cpLog(LOG_ERR, gai_strerror(error));
       cpLog(LOG_ERR, "IP Address: %s, port: %s\n", ipAddress.logData(), port_stor);
   } 
   else {
       memcpy(socka, res->ai_addr, res->ai_addrlen);
       // Cache it
       memcpy(&sa_cache, res->ai_addr, res->ai_addrlen);
       sockAddrSet = true;
   }
   
   if (res != 0) {
       freeAddrInfo(res);
   }
}


int
NetworkAddress::getPort () const
{
    //cpLog(LOG_DEBUG_STACK, "NetworkAddress::getPort()");
    return aPort;
}

bool
operator == ( const NetworkAddress & xAddress,
              const NetworkAddress & yAddress )
{
    //cpLog(LOG_DEBUG_STACK, "hostname %s %s", xAddress.getHostName().c_str(), yAddress.getHostName().c_str());
    //cpLog(LOG_DEBUG_STACK, "ipAddress %s %s", xAddress.getIpName().c_str(), yAddress.getIpName().c_str());
    //cpLog(LOG_DEBUG_STACK, "port %d %d", xAddress.getPort(), yAddress.getPort());
	
    if ((xAddress.getHostName() == yAddress.getHostName()) &&
        (xAddress.getIpName() == yAddress.getIpName()) &&
        (xAddress.getPort()==yAddress.getPort())) {
        return true;
    }
    else {
        return false;
    }
}


bool
operator != ( const NetworkAddress & xAddress,
              const NetworkAddress & yAddress )
{
    return !(xAddress == yAddress);
}


NetworkAddress& 
NetworkAddress::operator=( const NetworkAddress& x )
{
    if(this != &x)
    {
	aPort = x.getPort();
	ipAddress = x.getIpName();
	ipAddressSet = false; // Will re-cache next time we are asked.
        sockAddrSet = false;
	hostName = x.hostName;
        memset(&sa_cache, 0, sizeof(sa_cache));
    }
    return ( *this );
}

/** For debugging, can put this in logs, for instance */
string NetworkAddress::toString() const {
    ostringstream oss;
    try {
        oss << hostName << "(" << getIpName() << ")";
    }
    catch (UnresolvedException& e) {
        oss << hostName << "(unresolved)";
    }

    if ( aPort ) {
        oss << ":" << aPort;
    }

    return oss.str();
}


ostream &
NetworkAddress::print ( ostream & xStr ) const
{
   try
   {
//      initIpAddress();
      xStr << hostName << "(" << getIpName() << ")";
   }
   catch (UnresolvedException& e)
   {
      xStr << hostName << "(unresolved)";
   }
   if ( aPort )
   {
      xStr << ":" << aPort;
   }

   return xStr;
}

u_int32_t
NetworkAddress::hashIpPort( ) const
{
    struct sockaddr_storage xSrc;
    getSockAddr(&xSrc);
    u_int32_t ipAddressLocal;
    //Either it is IPv4 or IPv6, just take the first 32 bits
    memcpy(&ipAddressLocal, &xSrc, sizeof(ipAddressLocal));
    int port = getPort ();
    return (hashIpPort( ipAddressLocal, port )) ;
}


u_int32_t
NetworkAddress::hashIpPort( const u_int32_t lipAddress, const int port )
{
    u_int32_t hashKey = 0x0;
    hashKey = (lipAddress << 16) | (port & 0xFF);

    return hashKey;
}


void
NetworkAddress::initIpAddress() const {
   //cpLog(LOG_DEBUG_STACK, "initIpAddress() for hostName %s", hostName.c_str());
           
   if (ipAddressSet)
      return;

   if (hostName.size() == 0) {
      // Haven't resolved it yet, IP address better be valid!
      assert(is_valid_ip4_addr(ipAddress));
      ipAddressSet = true;
      return;
   }
   
   struct addrinfo hints;
   struct addrinfo *res = 0;

   // Setup structures
   memset(&hints, 0, sizeof(hints));
   hints.ai_socktype = SOCK_STREAM;
   
   Data the_host = hostName;

   if (the_host.find("[", 0) != Data::npos) {
      the_host = the_host.substr(1, the_host.length()-2);
   }

   bool nameIsAddress = false;
   hints.ai_family = NetworkConfig::instance().getAddrFamily();
   if (is_valid_ip6_addr(the_host)) {
      hints.ai_family = PF_INET6;
      nameIsAddress = true;
   }
   else if (is_valid_ip4_addr(the_host)) {
      hints.ai_family = PF_INET;
      nameIsAddress = true;
   }
   
   //if nameIsAddress, we are done
   if (nameIsAddress) {
      ipAddress = the_host;
      //cpLog(LOG_DEBUG_STACK, "Set ipAddress to %s", ipAddress.c_str());
      ipAddressSet = true;
      return;
   }
   
   //cpLog(LOG_DEBUG_STACK, "getaddrinfo()");
   //it is a host name to do nslookup to get the IP address
   int error = getaddrinfo(the_host.c_str(), NULL, &hints, &res);
   if (error) {
      cpLog(LOG_ERR, "Failed to resolve %s, reason:%s",
            the_host.c_str(), gai_strerror(error));
      return;
   }
   
   char ip[256];
   //cpLog(LOG_DEBUG_STACK, "getnameinfo()");
   error = getnameinfo(res->ai_addr, res->ai_addrlen, ip, 256, NULL, 0, NI_NUMERICHOST );
   if (error) {
      cpLog(LOG_ERR, "Failed to getnameinfo %s, reason:%s",
            the_host.c_str(), gai_strerror(error));
      freeAddrInfo(res);
      return;
   }
   
   freeAddrInfo(res);
   ipAddress = ip;
   //cpLog(LOG_DEBUG_STACK, "Set ipAddress to %s", ipAddress.c_str());
   ipAddressSet = true;
}

NetworkAddress::NetworkAddress( const NetworkAddress& x)
{
    aPort = x.aPort;
    ipAddress = x.getIpName();
    ipAddressSet = false;
    hostName = x.hostName;
    sockAddrSet = false;
    initIpAddress();
}


int
NetworkAddress::getHostByName(const char* hostName,
                              struct addrinfo* res,
                              struct hostent* hEnt /* default arg */)
{
#if defined(__GLIBC__)
   struct addrinfo hints;
   // Setup structures
   memset(&hints, 0, sizeof(hints));
   hints.ai_flags = AI_CANONNAME;
   hints.ai_family = NetworkConfig::instance().getAddrFamily();
   hints.ai_socktype = SOCK_DGRAM;
   int error = getaddrinfo(hostName, NULL, &hints, &res);
   if (error) {
        cpLog(LOG_ERR, gai_strerror(error));
        return(NetworkAddress::getHostLookupFailed);
   }

   return NetworkAddress::getHostLookupOK;
#else
#if defined(__svr4__) || defined(__FreeBSD__)|| defined (__SUNPRO_CC) || defined(__sun)|| defined(__QNX__) || defined(__APPLE__) || defined(__OpenBSD__)
   struct addrinfo hints;
   // Setup structures
   memset(&hints, 0, sizeof(hints));
   hints.ai_flags = AI_CANONNAME;
   hints.ai_family = PF_UNSPEC;
   hints.ai_socktype = SOCK_DGRAM;
   int error = getaddrinfo(hostName, NULL, &hints, &res);
   if (error) {
        cpLog(LOG_ERR, gai_strerror(error));
        return(NetworkAddress::getHostLookupFailed);
   }
   return NetworkAddress::getHostLookupOK;
#else
#if defined (WIN32)
    struct hostent* x = 0;
    
    x = gethostbyname(hostName);
    if (x == 0) 
    {
	cpLog(LOG_DEBUG, "********* Failed to get host by name:%s", hostName); 
	return getHostLookupFailed;
    }
    *hEnt = *x;
    return getHostLookupOK;
#else
#error no implementation for critical function
#endif
#endif
#endif
}


#if defined (WIN32)
struct hostent FAR *_gethostbyname( const char FAR *hostName )
{
    struct hostent* x;

#undef gethostbyname
    x = gethostbyname(hostName);
    if (x == 0) 
    {
        int address = inet_addr(hostName);
        if( address != 0xFFFFFFFF )
        {
            // Tried gethostbyaddr, but it did not seem to work...
//          x = gethostbyaddr((char *)&address, IPV4_LENGTH, AF_INET);
	    if (x == 0) 
            {
                struct hostentex
                    : hostent 
                {
                  char FAR *       hex_aliases;
                  char             hex_alias;
                  char FAR *       hex_addr_list;
                  char             hex_addr[IPV4_LENGTH];
                  char             hex_name[IPV4_LENGTH * 4];
                };
                static struct hostentex entex;

                entex.h_name        = entex.hex_name;
                entex.h_aliases     = & entex.hex_aliases;
                entex.h_addrtype    = AF_INET;
                entex.h_length      = IPV4_LENGTH;
                entex.h_addr_list   = & entex.hex_addr_list;

                entex.hex_aliases   = & entex.hex_alias;
                entex.hex_alias     = '\0';
                entex.hex_addr_list = & entex.hex_addr[0];

                memcpy((void *)entex.hex_addr, (void*)& address, IPV4_LENGTH);
                strncpy(entex.hex_name, hostName, IPV4_LENGTH - 1);
                entex.hex_name[IPV4_LENGTH - 1] = '\0';
                x = & entex;
            }
        }
    }
    return x;
}
#endif


/**
  * Return TRUE if the address is a valid IP address (dot-decimal form)
  * such as 128.128.128.128. Checks for invalid or ill formed addresses 
  * If the address is valid, copy it into ipAddress
  */
bool
NetworkAddress::is_valid_ip_addr(const Data& addr) const
{
	return (is_valid_ip4_addr(addr) || is_valid_ip6_addr(addr));
}

bool
NetworkAddress::is_valid_ip6_addr(const Data& addr)
{
    //cpLog (LOG_DEBUG_STACK, "is_valid_ip6_addr(%s)", addr.c_str());
   bool double_colon = false;
   int pos = addr.find("[");
   if (pos!=Data::npos)
   {
       //cpLog (LOG_DEBUG_STACK, "Invalid v6 address (found [)");
       return false;
   }

   pos = addr.find(":");
   if (pos!=Data::npos)
   {
        //"a.b.c.d:5060"
        pos = addr.find(":", pos+1); 
        if(pos != Data::npos)
        {
           pos = addr.find("::", 0); 
           if(pos != Data::npos)
           {
               double_colon = true;
               //ONLY "::"
               if (pos == (addr.length()-2))
               {
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
        else
        {
           return false;
        }
   }

   int count = 0;
   char* szInitialString = new char[255];
   strncpy(szInitialString, addr.c_str(), 254);
   szInitialString[254] = '\0';
   char* ptr;

   char* szTokOut = strtok_r(szInitialString, ":", &ptr);
   while(szTokOut)
   {
      if (!is_v6_string(szTokOut))
      {
          //cpLog (LOG_DEBUG_STACK, "Invalid v6 address (invalid string %s)", szTokOut);
    //      return false;
      }
      count++;

      if (count > 8)
      {
          break;
      }
      szTokOut=strtok_r(NULL, ":", &ptr);
   };

   if (double_colon)
   {
       //cpLog (LOG_DEBUG_STACK, "Valid v6 address (contains ::)");
       delete []szInitialString;
       return true;
   }

   if (count == 8)
   {
       //cpLog (LOG_DEBUG_STACK, "Valid v6 address");
       delete []szInitialString;
       return true;
   }

   //cpLog (LOG_DEBUG_STACK, "Invalid v6 address");
   delete []szInitialString;
   return false;
}

bool
NetworkAddress::is_v6_string(const Data& addr)
{
   if (addr.length() > 4)
   {
       return false;
   }
   int next = 0;
   Data tmp;
   char buf[5];
   memcpy(buf, addr.c_str(), 4);
   
   while (next < addr.length())
   {
       if (!isdigit(buf[next]) && !isxdigit(buf[next]))
       {
           return false;
       }
       next++;
   }
   return true;
}



bool
NetworkAddress::is_valid_ip4_addr(const Data& addr)
{
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
    
    if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t'))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }
    octet = strtoul(advancer, &nextchar, 10);
    if((*nextchar != '.') || (octet & maskcheck) || (octet == 0))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }

    advancer = nextchar+1;
    if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t'))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }
    octet = strtoul(advancer, &nextchar, 10);
    if((*nextchar != '.') || (octet & maskcheck))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }

    advancer = nextchar+1;
    if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t'))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }
    octet = strtoul(advancer, &nextchar, 10);
    if((*nextchar != '.') || (octet & maskcheck))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }

    advancer = nextchar+1;
    if ((*(advancer) == 0) || (*(advancer) == ' ') || (*(advancer) == '\t'))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }
    octet = strtoul(advancer, &nextchar, 10);
    if((*nextchar) || (octet & maskcheck) || (octet == 0))
    {
    	//cpLog (LOG_DEBUG_STACK, "Invalid v4 address");
        return false;
    }
    
    //cpLog (LOG_DEBUG_STACK, "Valid v4 address");
    return true;
}

Data
NetworkAddress::getHostByAddress(Data& ipAddr) const
{
        Data retVal;
	struct addrinfo hints;
	struct addrinfo *res = 0;

	// Setup structures
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;

	Data the_host = ipAddr;

	if(the_host.find("[", 0) != Data::npos)
		the_host = ipAddr.substr(1, the_host.length()-2);
        bool hostNameIsAddress = false;
	hints.ai_family = NetworkConfig::instance().getAddrFamily();
	if (is_valid_ip4_addr(the_host)) 
        {
            hints.ai_family = PF_INET;
            hostNameIsAddress = true;
        }
	else if (is_valid_ip6_addr(the_host)) 
        {
            hints.ai_family = PF_INET6;
            hostNameIsAddress = true;
        }

        retVal = the_host;
        if(hostNameIsAddress)
        { 
            //cpLog(LOG_DEBUG_STACK, "getaddrinfo():%s", the_host.c_str());
	    int error = getaddrinfo(the_host.c_str(), NULL, &hints, &res);
	    if (error) {
		cpLog(LOG_ERR, gai_strerror(error));
                if(res != 0)
                {
                    freeAddrInfo(res);
                }
                return(the_host);
	    }
            char hName[256+1];
            memset(hName, 0, 256+1);
            //Get the host name of the address
	    error = getnameinfo(res->ai_addr, res->ai_addrlen, hName, 256, NULL, 0, NI_NAMEREQD );
             if (error)
             {
                  cpLog(LOG_NOTICE, "Failed to resolve address (%s) to a name:%s", the_host.c_str(), hName);
                  retVal = the_host;
             }
             else 
             {
                 retVal = hName;
             }
	     freeAddrInfo(res);
        }
        return retVal;
}

void
NetworkAddress::freeAddrInfo(struct addrinfo* res)
{
      struct addrinfo* tmp = res;
#if defined (__SUNPRO_CC)
      //On Solaris freeaddrinfo leaks, not freeing ai_addr
      while(res)
      {
          free(res->ai_addr);
          res->ai_addr=0;
          res = res->ai_next;
      }
#endif
      if(tmp != 0)
      {
          freeaddrinfo(tmp);
      }
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


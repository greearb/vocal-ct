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


#include "global.h"
#include "MACAddress.hxx"
#include "Socket.hxx"

#ifndef __MINGW32__
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#endif

#include <unistd.h>
#include <string.h>


using Vocal::Transport::MACAddress;
using Vocal::Transport::Socket;


MACAddress::MACAddress()
    :	high_(0),
    	low_(0)
{
   int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
   getMACAddress(fd);
   close(fd);
}


MACAddress::MACAddress(const Socket & sock)
{
    getMACAddress(sock.getFD());
}


MACAddress::~MACAddress()
{
}


u_int32_t   	    	    
MACAddress::high()
{
    return ( high_ );
}   


u_int32_t   	    	    
MACAddress::low()
{
    return ( low_ );
}



void	    	    	    
MACAddress::getMACAddress(int sock) {
#if !defined(SIOCGIFHWADDR) && !defined(SIOCGENADDR)
   return;

#else

   if ( sock < 0 ) {
      return;
   }

   ifconf 	ifc;

   char    	buf[1024];

   memset(buf, 0, sizeof(buf));
   ifc.ifc_len = sizeof(buf);
   ifc.ifc_buf = buf;
   
   if ( ioctl(sock, SIOCGIFCONF, (char *)&ifc) < 0) {
      return;
   }
    
   ifreq 	ifr, *ifrp;
   
   int size = ifc.ifc_len;
   u_int8_t    *   a = 0;
   
   for (int i = 0; i < size; i += sizeof(ifreq) ) {
      ifrp = (ifreq *)((char *)ifc.ifc_buf + i);
      
      strncpy(ifr.ifr_name, ifrp->ifr_name, IFNAMSIZ);
      
#if defined(SIOCGIFHWADDR)
      if ( ioctl(sock, SIOCGIFHWADDR, &ifr) < 0 ) {
         continue;
      }
      a = (unsigned char *)(&ifr.ifr_hwaddr.sa_data);
      
#elif defined(SIOCGENADDR)
      if ( ioctl(sock, SIOCGENADDR, &ifr) < 0 ) {
         continue;
      }
      a = (unsigned char *)ifr.ifr_enaddr;
#endif

      if ( !a[0] && !a[1] && !a[2] && !a[3] && !a[4] && !a[5] ) {   	
         continue;
      }
      
      high_ 	=   ( a[0] << 8 )   + a[1];
      low_  	=   ( a[2] << 24 )  + ( a[3] << 16 )
         +   ( a[4] << 8 )   + a[5];
   }

    #endif // !defined(SIOCGIFHWADDR) || !defined(SIOCGENADDR)
}

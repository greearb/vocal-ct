#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   



#ifndef WIN32
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#endif

#include <cassert>
#include <iostream>


#include "udp.h"
#include "stun.h"

using namespace std;


int
main(int argc, char* argv[])
{
   assert( sizeof(UInt8 ) == 1 );
   assert( sizeof(UInt16) == 2 );
   assert( sizeof(UInt32) == 4 );
    
   initNetwork();
    
   bool verbose = false;
   int testNum = 0;
    
   if ( argc == 3 )
   {
      verbose = true;
      testNum = atoi( argv[1] );
      cerr << "running test number " << testNum  << endl; 
   }
    
   StunAddress4 stunServerAddr;
	
   if ( argc == 1 )
   {
      stunParseServerName( "stun.vovida.org", stunServerAddr);
   }

   if (argc == 2 || argc == 3)
   {
      stunParseServerName( argv[argc-1], stunServerAddr);
   }

   if (verbose) 
   {
      clog << "STUN test with host " << stunServerAddr << endl;
   }

   if (testNum==0)
   {
      NatType stype = stunNatType( stunServerAddr, verbose );

      clog << "Internet connection is type: ";

      switch (stype)
      {
         case StunTypeOpen:
            clog << "Open";
            break;
         case StunTypeConeNat:
            clog << "Type Cone Nat";
            break;
         case StunTypeRestrictedNat:
            clog << "Restricted Nat";
            break;
         case StunTypePortRestrictedNat:
            clog << "Port Restricted Nat";
            break;
         case StunTypeSymNat:
            clog << "Symetric Nat";
            break;
         case StunTypeSymFirewall:
            clog << "Symetric Firewall";
            break;
         case StunTypeBlocked:
            clog << "Blocked";
            break;
         default:
            clog << stype;
            break;
      }
      clog << endl;
   }
   else if (testNum==-1)
   {
#if 0
      StunAddress4 fd1Addr;
        
      int fd1 = stunOpenSocket( stunServerAddr, &fd1Addr );
        
      clog << "Opened a stun socket at " << fd1Addr << endl;
        
      StunAddress4 fd2Addr;
        
      int fd2 = stunOpenSocket( stunServerAddr, &fd2Addr );
        
      clog << "Opened a stun socket at " << fd2Addr << endl;
        
      StunAddress4 fd3Addr;
        
      int fd3,fd4;
        
      bool ok = stunOpenSocketPair(stunServerAddr,
                                   &fd3Addr,
                                   &fd3,
                                   &fd4 );
      if ( ok )
      {
         clog << "Opened a stun socket pair at " << fd3Addr << endl;
         close(fd3);
         close(fd4);
      }
      else
      {
         clog << "Opened a stun socket pair FAILED" << endl;
      }
        
      close(fd1);
      close(fd2);
#endif   
   }
   else
   {
      stunTest( stunServerAddr,testNum,verbose );
   }

   return 0;
}


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

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:

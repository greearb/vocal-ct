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
#include <stdlib.h>
#endif

#include "udp.h"
#include "stun.h"


//#if defined (sun) || defined (WIN32) || defined(__MACH__)
extern int optind;
extern char *optarg;

struct option
{
      const char *name;
      int has_arg;
      int *flag;
      int val;
};
//#endif


/**
   Implementation of getopt_long(3) for Windows and Solaris.
   This version is VERY stupid
   and assumes that the options are passed in a [-x y] format where x is
   a single character and y is the value.  No other fancy smancy stuff is
   supported.
   @param argc Number of command line args.
   @param argv Vector of command line args.
   @param optstring A single character option string (list).
   @param longopts The long-style options (unsupported in Win32 environment)
   @param longindex The index into @{link longopts longopts}.
*/
int local_getopt_long( int argc,	
                       char * const argv[],
                       const char *optstring,
                       const struct option *longopts,
                       int *longindex
   );

#if 0
#if defined(linux)
#  include <unistd.h>
#  if !defined(_GNU_SOURCE)
#    define _GNU_SOURCE
#  endif
#  include <getopt.h>
#endif
#endif

using namespace std;


int
main(int argc, char* argv[])
{
   assert( sizeof(UInt8 ) == 1 );
   assert( sizeof(UInt16) == 2 );
   assert( sizeof(UInt32) == 4 );

   initNetwork();
   
   StunAddress4 myAddr;
   StunAddress4 altAddr;
   bool verbose=false;
   bool background=false;
   
   myAddr.addr = 0;
   altAddr.addr = 0;
   myAddr.port = STUN_PORT;
   altAddr.port = STUN_PORT+1;

   UInt32 interfaces[10];
   int numInterfaces = stunFindLocalInterfaces(interfaces,10);

   if (numInterfaces == 2)
   {
      myAddr.addr = interfaces[0];
      myAddr.port = STUN_PORT;
      altAddr.addr = interfaces[1];
      altAddr.port = STUN_PORT+1;
   }

   while (true)
   {
      static struct option long_options[7] = 
         {
            {"other-port", 1, 0, 'o'},
            {"alternate-server",1, 0, 'a'},
            {"port", 1, 0, 'p'},
            {"server", 1, 0, 'h'},
            {"verbose", 0, 0, 'v'},
            {"background", 0, 0, 'b'},
            {0,0,0,0}
         };

      int c;
      int option_index = 0;
    
      c = local_getopt_long(argc,argv,"a:h:o:p:vb",long_options,&option_index);
      if (c == -1) break;

      switch (c)
      {
         case 'h':
            stunParseServerName(optarg, myAddr);
            break;

         case 'p':
            myAddr.port = UInt16(strtol(optarg, NULL, 0));
            break;

         case 'o':
            altAddr.port = UInt16(strtol(optarg,NULL,0));
            break;

         case 'v':
            verbose = true;
            break;

         case 'b':
            background = true;
            break;

         case 'a':
            stunParseServerName(optarg,altAddr);
            break;

         default:
            cerr << "bad command line" << endl;
            exit(1);
      }
   }

   if (  (myAddr.addr == 0) || (altAddr.addr == 0) )
   {
      clog << "If your machine does not have exactly two ethernet interfaces, "
           << "you must specify the server and alt server" << endl;
      exit(-1);
   }

   if ( verbose )
   {
      clog << "Running with on interface "
           << myAddr << " with alternate " 
           << altAddr << endl;
   }
    
   if (( myAddr.port  == 0) ||
       ( altAddr.port == 0) ||
       ( myAddr.addr == 0 ) ||
       ( altAddr.addr == 0 ) )
   {
      cerr << "Bad command line" << endl;
      exit(1);
   }
   
#if defined(WIN32)

   if (  background )
   {
      cerr << "The -b background option does not work in windows" << endl;
   }

   StunServerInfo info;
   bool ok = stunInitServer(info, myAddr, altAddr);
   
   while (ok)
   {
      ok = stunServerProcess(info, verbose);
   }

#else
   if ( background )
   {
      pid_t pid = fork();

      if (pid < 0)
      {
         cerr << "fork: unable to fork" << endl;
         return -1;
      }

      if (pid == 0) //child
      {
         // Should dissociate from terminal etc here....
         StunServerInfo info;
         bool ok = stunInitServer(info, myAddr, altAddr);
   
         while (ok)
         {
            ok = stunServerProcess(info, verbose);
         }

         // NOTREACHED
      }
   }
   else
   {
      StunServerInfo info;
      stunInitServer(info, myAddr, altAddr);
   
      while (true)
      {
         stunServerProcess(info, verbose);
      }
   }
#endif

   return 0;
}


//#if defined(WIN32) || defined(sun) || defined(__MACH__)
#if 1
char *optarg = 0;
int optind = 0;

/**
 * This is a very hacked version for WIN32 that will support only what
 * we need.  This is NOT a generic getopt_long()
 */
int local_getopt_long(int argc,	
                      char * const argv[],
                      const char *optstring,
                      const struct option *longopts,
                      int *longindex			
   )
{
   static int carg = 0;
   //static int nextchar = 0;
  
   carg++;
   if ( carg >= argc ) return -1;

   char * p = argv[carg];
   //register int al = strlen(argv[carg]);

   if (*p  == '-' && isalnum(*(p+1)))
   {
      char o = *(p+1);

      register int l = strlen(optstring);

      for(register int i = 0 ; i < l; i++)
      {

         if (optstring[i] == ':') continue;

         if ( optstring[i] == o )	// match option char
         {
	
            if ( optstring[i+1] == ':' ) // arg option
            {
               optind = ++carg;
               optarg = argv[optind];

            }
            else
            {
               optind = 0;
               optarg = 0;
            }
            return (int)o;
         }

      }

      return (int)'?';
   }
   return (int)'?';
}
#else
 
int local_getopt_long(int argc,	
                      char * const argv[],
                      const char *optstring,
                      const struct option *longopts,
                      int *longindex			
   )
{
   return getopt_long(argc,argv,optstring,longopts,longindex);
}

#endif

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

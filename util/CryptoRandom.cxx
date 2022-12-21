
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


#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>

#include "CryptoRandom.hxx"
#include "cpLog.h"
#include "global.h"
#include "support.hxx"
#include "misc.hxx"


static bool srandomCalled = false;

CryptoRandom::CryptoRandom()
{
}

CryptoRandom::~CryptoRandom()
{
}

// Note that this call will block if there is not enough entropy gathered by the
// kernel. It will eventually return when enough data has been gathered. Use
// getPseudoRandom if delay may be a problem. 
int CryptoRandom::getCryptoRandom(unsigned char* buf, int num)
{
#ifdef USE_CRYPTO_RANDOM
    ssize_t len = 0;

    //open the io device /dev/random.
    // if no blocking is desired, use /dev/urandom instead. 
    FILE* fp = fopen( "/dev/urandom", "r");
    if ( fp == 0 )
    {
       cpLog(LOG_INFO, "CryptoRandom::unable to open /dev/random. Use pseudo random instead.");
       return getPseudoRandom(buf, num);
    }
    else
    {
       // by reading it this way, we will not do a short read. 
       if ( (len = fread(buf, 1, num, fp)) != num)
       {
          cpLog(LOG_INFO, "CryptoRandom::Error reading from /dev/random (%d/%d) ferror=%d. Use pseudo random instead.", len, num, ferror(fp));
          fclose(fp);
          return getPseudoRandom(buf, num);
       }
    }
    //close the io device /dev/random.
    fclose(fp);

    return len;
#else
    assert(0);
    return 0;
#endif
}


#ifndef WIN32
// this is still broken
int CryptoRandom::getPseudoRandom(unsigned char* buf, int num)
{
   int count=0;
   for (count=0; count < (num-3); count+=4)
   {
      //generate a random number;
      if (!srandomCalled)
      {
         struct timeval tv;
         gettimeofday(&tv, 0);
         
         srandom((unsigned int)(tv.tv_usec ^ tv.tv_sec));
         srandomCalled = true;
      }
      
      int rand = random();
      memcpy(buf+count, &rand, 4);  //copy 4 bytes from rand to buf.
   }
   
   // now copy the remainder
   int remainder = num % 4;
   if (remainder)
   {
      int rand = random();
      memcpy(buf+count, &rand, remainder);  //copy 4 bytes from rand to buf.
   }
   
   return num;
}
#else
int CryptoRandom::getPseudoRandom(unsigned char* buf, int num)
{
    //generate a random number;
    struct timeval tv;
    vgettimeofday(&tv, 0);

    if (!srandomCalled)
    {
        srand((unsigned int)(tv.tv_usec ^ tv.tv_sec));
        srandomCalled = true;
    }
    int r = rand();
    r += tv.tv_usec;

	 // 15/12/03 fpi
	 // BugZilla 766: apply personal patch
	 // TBR
	 assert(!(num%4));
	 r <<= 16;
	 for(int i= num/4; i != 0; i--) {
		 r += rand();
		 memcpy(buf, &r, 4);  //copy 4 bytes from rand to buf.		 
		 buf += 4;
		 r += ::GetTickCount();
		 r <<= 8;
	 }
   
    return num;
}
#endif // WIN32

//char* CryptoRandom::getRandom()
int CryptoRandom::getRandom(unsigned char* buf, int num)
{
    int len;

#  ifdef USE_CRYPTO_RANDOM
    len = getCryptoRandom(buf, num);
#  else
    len = getPseudoRandom(buf, num);
#  endif

    assert (len == num);
    return len;
}

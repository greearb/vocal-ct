#ifndef NO_YARROW
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
#include "CryptoRandom.hxx"
#include "YarrowPseudoRandom.hxx"
#include "cpLog.h"

extern "C" {
   int yarrow_verbose = 0;
};

unsigned char YarrowPseudoRandom::entropyBuf[ENTROPY_BUF_SIZE];
YarrowPseudoRandom* YarrowPseudoRandom::yarrowInst = 0; 

YarrowPseudoRandom::YarrowPseudoRandom()
{
   //initialize yarrow
   //yarrow_verbose =1 ;
    int ret = Yarrow_Init(&yarrow, NULL);
    if (ret != YARROW_OK && ret != YARROW_NOT_SEEDED)
    {
        assert(0);
    }
    else
    {
       unsigned r1, r2;
       Yarrow_New_Source( &yarrow, &r1 );
       Yarrow_New_Source( &yarrow, &r2 );

       for (int i=0; i<7; i++)
       {
          CryptoRandom crypto;
          crypto.getCryptoRandom(entropyBuf, 20);
          Yarrow_Input(&yarrow, r1, entropyBuf, 20, 3*20);
          //cout << Yarrow_Str_Error( ret ) << endl;
       }

       for (int i=0; i<40; i++)
       {
          CryptoRandom crypto;
          crypto.getCryptoRandom(entropyBuf, 4);
          Yarrow_Input(&yarrow, r2, entropyBuf, 20, 2*4);
          //cout << Yarrow_Str_Error( ret ) << endl;
       }
    }
}
    
    
YarrowPseudoRandom::~YarrowPseudoRandom()
{
}
    
    
YarrowPseudoRandom* YarrowPseudoRandom::instance()
{
    if (yarrowInst == 0)
    {
	yarrowInst = new YarrowPseudoRandom();
    }
    return yarrowInst;
}
    

int YarrowPseudoRandom::getRandom(unsigned char* buf, int num)
{
   int ret = Yarrow_Output(&yarrow, buf, num);
   assert(ret == YARROW_OK);
   return ret;
}

#endif

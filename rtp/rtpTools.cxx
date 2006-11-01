
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
#include <iostream>
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "cpLog.h"
//#include "vsock.hxx"

/* ----------------------------------------------------------------- */
/* --- Number Functions -------------------------------------------- */
/* ----------------------------------------------------------------- */

/*  32-bit random number     */
u_int32_t generate32 ()
{
    // should be seeded by main program
    return rand();
}


/*  random SRC number        */
RtpSrc generateSRC()
{
    // doesn't check for collision
    RtpSrc src = 0;
    while (src == 0)
        src = generate32();
    return src;
}


/*  prints raw bits of memory */
void printBits (char* packetData, int len)
{
    unsigned short int b;
    unsigned int c = 0;
    cerr << "\n-----------------------------------\n";
    for (int i = 0; i < len; i++)
    {
        if (i % 4 == 0 && i != 0)
        {
            printf ("  %2.2X%2.2X%2.2X%2.2X\n",
                    static_cast < unsigned char > (packetData[i - 4]),
                    static_cast < unsigned char > (packetData[i - 3]),
                    static_cast < unsigned char > (packetData[i - 2]),
                    static_cast < unsigned char > (packetData[i - 1])
                   );
            c = 0;
        }
        b = static_cast < unsigned short int > (
                static_cast < unsigned char > (packetData[i]));
        char out[9];
        for (int j = 0; j < 8; j++)
        {
            if (b % 2)
                out[7 - j] = '1';
            else
                out[7 - j] = '0';
            c = 2 * c + b % 2;
            b = b >> 1;
        }
        out[8] = '\0';
        cerr << out;
        cerr << " ";
    }
}

void printInt(int x)
{
    char* packetData = reinterpret_cast < char* > (&x);
    for (int i = 0; i < 4; i++)
    {
        char out[9];
        unsigned short int b = static_cast < unsigned short int > (
                                   static_cast < unsigned char > (packetData[i]));
        for (int j = 0; j < 8; j++)
        {
            if (b % 2)
                out[7 - j] = '1';
            else
                out[7 - j] = '0';
            b = b >> 1;
        }
        out[8] = '\0';
        cerr << out;
        cerr << " ";
    }
    printf ("  %2.2X%2.2X%2.2X%2.2X\n",
            static_cast < unsigned char > (packetData[0]),
            static_cast < unsigned char > (packetData[1]),
            static_cast < unsigned char > (packetData[2]),
            static_cast < unsigned char > (packetData[3])
           );
    return ;
}


bool RtpSeqGreater (RtpSeqNumber a, RtpSeqNumber b)
{
    return (RtpSeqDifference(a, b) > 0);
}


// a - b, with wrapping taken into account
int RtpSeqDifference (RtpSeqNumber a, RtpSeqNumber b)
{
    assert(a <= RTP_SEQ_MOD);
    assert(b <= RTP_SEQ_MOD);

    if (a == b) {
       return 0;
    }

    if (a > b) {
       if ((a - b) > (RTP_SEQ_MOD >> 1)) {
          // A is much greater than b, assume wrap
          return -((RTP_SEQ_MOD - a) + b);
       }
       else {
          // A is just a bit bigger than b, there was no wrap
          return a - b;
       }
    }
    else {
       if ((b - a) > (RTP_SEQ_MOD >> 1)) {
          // B is much greater than a, assume a wrapped
          return a + (RTP_SEQ_MOD - b);
       }
       else {
          return -(b - a);
       }
    }
}


bool RtpTimeGreater( RtpTime a, RtpTime b )
{
//    assert(a >= 0);
//    assert(b >= 0);
    assert(a <= RTP_TIME_MOD);
    assert(b <= RTP_TIME_MOD);

    if (a == b) {
       return false;
    }

    if (a > b) {
       if ((a - b) > (RTP_TIME_MOD >> 1)) {
          // A is much greater than b, assume wrap
          return false; // B is greater, it has wrapped
       }
       else {
          // A is just a bit bigger than b, there was no wrap, so true
          return true;
       }
    }
    else {
       if ((b - a) > (RTP_TIME_MOD >> 1)) {
          // B is much greater than a, assume a wrapped
          return true;
       }
       else {
          return false;
       }
    }
}

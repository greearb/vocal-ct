/* ====================================================================
 * The Vovida Software License, Version 1.0
 *
 * Copyright (c) 2001,2003 Vovida Networks, Inc.  All rights reserved.
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

#include "rtpTypes.h"
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "Verify.hxx"

char* a = "\x80";

char packet[] = "\x80\x00\x15\x42\x06\x49\x8B\xC0\xA7\xE6\xC2\x94\x7E\x7E\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\x7F\x7E\x7F\x7F\x7F\xFF\xFF\x7F\xFF\xFF\xFF\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7F\xFE\xFE\xFE\xFE\xFE\xFF\xFF\xFE\xFF\x7F\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\x7E\x7F\x7F\x7F\x7F\xFF\xFE\xFE\xFE\xFF\xFE\xFE\x7F\xFF\xFF\x7F\x7F\x7F\xFF\xFF\xFF\xFE\xFE\xFF\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\x7F\xFE\xFE\xFF\xFF\xFE\xFF\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\xFE\xFE\xFF\xFE\xFE\xFE\xFE\xFE\xFE\xFE\xFF\x7F\x7F\x7F\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7E\x7F\x7F\x7F\xFF\xFF\x7F\xFF\xFE\xFF\xFF\xFF\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F\x7F";


#if 0
struct Evil
{
    /// protocal version
    unsigned char version:    2;
    /// padding flag - for encryption
    unsigned char padding:    1;
    /// header extension flag
    unsigned char extension:  1;
    /// csrc count
    unsigned char count:      4;


    

    /// marker bit - for profile
    unsigned char marker:      1;
    /// payload type
    unsigned char type:        7;



    /// sequence number of this packet -- this needs to be in network
    /// byte order
    unsigned short sequence : 16;
};
#endif

void output(char c)
{
    unsigned int i = c;
    printf("%x\n", i % 0xff);
}


typedef RtpHeader Foo;


int main()
{
    // this is needed to get things into word alignment on sun boxes.
    char* y = new char[32];
    memcpy(y, packet, 32);


    Foo* x = reinterpret_cast<Foo*>(y);

    test_verify(x->version == 0x2);
    test_verify(ntohs(x->sequence) == 0x1542);


#if 0
    printf("%x\n", x->version);
    printf("%x\n", ntohs(x->sequence));
 

    printf("%x %x\n", x->type, x->marker);
    printf("%x %x %x %x\n", x->version, x->padding, x->extension, x->count);
    printf("%x %x\n", x->version, x->count);


    output(packet[0]);
    output(packet[1]);
    output(packet[2]);
    output(packet[3]);
#endif

    return test_return_code(2);
}



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


static const char* const Base64_cxx_Version = 
    "$Id: Base64.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "global.h"
#include <string.h>
#include "Base64.hxx"


using Vocal::Base64;


char Base64::basis_64[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


char Base64::index_64[128] = 
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
};


Base64::Base64()
{
}


Base64::~Base64()
{
}


#define char64(c)  (((c) > 127) ? -1 : index_64[(c)])


int Base64::encode(unsigned char* outputBuf, unsigned int* outputLen,
                   unsigned char* inputBuf, unsigned int inputLen)
{
    unsigned char c1, c2, c3;
    unsigned char *pOutputBuf = outputBuf;

    unsigned int count = 0, charsPerLine = 0;



    // Need to initialize c1,c2,c3 to 0 to ensure correct operation
    // in case of a message one byte long

    c1 = 0x00;
    c2 = 0x00;
    c3 = 0x00;

    while (count < inputLen)
    {
        c1 = inputBuf[count++];
        if (count == inputLen)
        {
            c2 = 0x00;
            *pOutputBuf++ = basis_64[c1 >> 2];
            *pOutputBuf++ = basis_64[(((c1 & 0x3) << 4) | ((c2 & 0xF0) >> 4))];
            *pOutputBuf++ = '=';
            *pOutputBuf++ = '=';
        }
        else	// count < inputLen
        {
            c2 = inputBuf[count++];
            if (count == inputLen)
            {
                c3 = 0x00;
                *pOutputBuf++ = basis_64[c1 >> 2];
                *pOutputBuf++ = basis_64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
                *pOutputBuf++ = basis_64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
                *pOutputBuf++ = '=';
            }
            else // count  < inputLen
            {
                c3 = inputBuf[count++];
                *pOutputBuf++ = basis_64[c1 >> 2];
                *pOutputBuf++ = basis_64[(((c1 & 0x03) << 4) | ((c2 & 0xF0) >> 4))];
                *pOutputBuf++ = basis_64[(((c2 & 0x0F) << 2) | ((c3 & 0xC0) >> 6))];
                *pOutputBuf++ = basis_64[(c3 & 0x3F)];
            }
        }

        charsPerLine += 4;

    } //end while

    *pOutputBuf++ = '\0';

    *outputLen = strlen((char*)outputBuf);

    return *outputLen;
}


int Base64::decode(unsigned char* outputBuf, unsigned int* outputLen,
                   unsigned char* inputBuf, unsigned int inputLen)
{
    unsigned char c1, c2, c3, c4;
    unsigned int count = 0;



    c1 = *inputBuf++;
    while (c1 != '\0')
    {
        c2 = *inputBuf++;
        c3 = *inputBuf++;
        c4 = *inputBuf++;

        c1 = char64(c1);
        c2 = char64(c2);

        outputBuf[count++] = ((c1 << 2) | ((c2 & 0x30) >> 4));

        if (c3 != '=')
        {
            c3 = char64(c3);
            outputBuf[count++] = (((c2 & 0XF) << 4) | ((c3 & 0x3C) >> 2));

            if (c4 != '=')
            {
                c4 = char64(c4);
                outputBuf[count++] = (((c3 & 0x03) << 6) | c4);
            }
        }

        c1 = *inputBuf++;
        if (c1 == '\r')
        {
            c1 = *inputBuf++;
            if (c1 == '\n')
                c1 = *inputBuf++;
        }
    }// while c1 != 0

    *outputLen = count;



    return count;
}



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

static const char* const ParseFile_cxx_Version =
    "$Id: ParseFile.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "SipMsg.hxx"
#include <stdio.h>
#include "Data.hxx"
#include <sys/time.h>
#include <unistd.h>

using namespace Vocal;



int main(int argc, char** argv)
{
    static int timeArray[1000000];
    int timeIndex = 0;

    char linebuf[65000];
    char msgbuf[65000];

    while (!feof(stdin))
    {
        msgbuf[0] = '\0';
        bool msgReady = false;
        while (!msgReady)
        {
            fgets(linebuf, 65000, stdin);

            if ((linebuf[0] == '.') && (linebuf[1] == ' '))
            {
                strcat(msgbuf, linebuf + 2);
            }
            else
            {
                msgReady = true;
            }
        }

        struct timeval start, end;

        Data inData(msgbuf);

        SipMsg* msg;

        gettimeofday(&start, 0);
        msg = SipMsg::decode(inData);
        gettimeofday(&end, 0);

        if (msg != 0)
        {
            Data outData = msg->encode();
            timeArray[timeIndex] = (end.tv_sec - start.tv_sec) * 1000000
                                   + (end.tv_usec - start.tv_usec);
            timeIndex++;
        }
        else
        {
            puts("no\n");
        }
    }

    for (int i = 0 ; i < timeIndex; i++)
    {
        printf("%d\n", timeArray[i]);
    }
    return 0;
}

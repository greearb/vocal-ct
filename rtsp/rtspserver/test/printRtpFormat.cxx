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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const printRtpFormat_cxx_version =
    "$Id: printRtpFormat.cxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include <stdio.h>
#include <netinet/in.h>


const unsigned int MaxRtpPacketSize=2000;
char buffer[MaxRtpPacketSize];


int main(int argc, char* argv[])
{
    unsigned short tempSeqNum;
    unsigned int tempTS;
    unsigned int prevTS = 0;
    unsigned int uRtpBlockSize;
    FILE* xRtpAudioFile;
    int cc;
    
    if(argc != 2)
    {
        printf("usage: %s <rtp file name>\n", argv[0]);
        return 0;
    }

    xRtpAudioFile=fopen(argv[1],"rb");
    if(!xRtpAudioFile)
    {
        printf("Can't open %s \n", argv[1]);
        return 0;
    }

    while(!feof(xRtpAudioFile))
    {
        cc = fread((void*)&uRtpBlockSize, 4, 1, xRtpAudioFile);
        if(cc<=0)
        {
            printf("Done Reading rtp file\n" );
            return 1;
        }
        uRtpBlockSize=ntohl(uRtpBlockSize);
        if(uRtpBlockSize>MaxRtpPacketSize)
        {
            printf("Can't read Rtp file - incorrect size of Rtp packet (%d)\n", uRtpBlockSize);
            return -1;
        }
        cc = fread((void*)&tempSeqNum, 2, 1, xRtpAudioFile);
        if(cc<=0)
        {
            printf("Can't read Rtp file (SeqNum)" );
            return -1;
        }
        cc = fread((void*)&tempTS, 4, 1, xRtpAudioFile);
        if(cc<=0)
        {
            printf("Can't read Rtp file (TS)" );
            return -1;
        }
        cc = fread( buffer, uRtpBlockSize-2-4, 1, xRtpAudioFile);
        if(cc<=0) 
        {
            printf("Can't read Rtp file (Payload)" );
            return -1;
        }

        printf("%6d %8u %10u(%u)\n", uRtpBlockSize-2-4, tempSeqNum, tempTS, tempTS - prevTS);
        prevTS = tempTS;

    }

    fclose(xRtpAudioFile);
    return 0;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


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
static const char* const rtpRecordC_c_Version =
    "$Id: rtpRecordC.c,v 1.1 2004/05/01 04:15:23 greear Exp $";
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rtpTypes.h"
#include "rtpApi.h"


int main (int argc, char *argv[])
{
    int file;
    RtpSessionPtr stack;
    RtpPacketPtr packet;
    int i;
    int ret;

    if (argc != 2)
    {
        fprintf(stderr, "Usage: rtpRecordC file\n");
        return -1;
    }
    file = open("FILErecv", O_CREAT | O_APPEND | O_WRONLY);

    if (file == -1)
    {
        fprintf(stderr, "Could not open file: FILErecv\n");
        return -1;
    }

    // receiving on localhost on port 5000/5001
    RtpSession_create1( &stack, "localhost", 0, 5000, 5002, 5001);
    RtpPacket_create1( &packet, 256, 0, 0);

    // stop loop after 400 packet attempts
    for (i = 0; i < 999; i++)
    {
        RtpSession_receive( stack, &packet, &ret);
        if (ret > 0)
        {
            char* location;
            int length;

            RtpPacket_getPayloadLoc( packet , &location );
            RtpPacket_getPayloadUsage( packet, &length );

            /* loop until packet is written to file */
            while ( length > 0 )
            {
                int written = write( file, location, length );
                if ( written == -1 )
                {
                    fprintf(stderr, "\nProblem writing to file\n");
                }
                location += written;
                length -= written;
            }

            fprintf(stderr, "*");

            RtpSession_intervalRTCP( stack, &ret);
            if (ret) RtpSession_transmitRTCP ( stack, &ret);
            RtpSession_receiveRTCP( stack, &ret);
        }
        else
        {
            // no packet received
            fprintf(stderr, "x");
        }

        // slow program down
        usleep (2 * 1000);
    }

    return 0;
}

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

static const char* const RtcpPacket_cxx_Version =
    "$Id: RtcpPacket.cxx,v 1.2 2006/03/12 07:41:28 greear Exp $";


#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>


// networking
#include <sys/types.h>

#include "global.h"
#include "cpLog.h"
#include "vsock.hxx"

#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "NtpTime.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"



/* ----------------------------------------------------------------- */
/* --- rtcp Packet Constructor ------------------------------------- */
/* ----------------------------------------------------------------- */

RtcpPacket::RtcpPacket ()
{
    memset (packetData, 0, RTCP_PACKETSIZE);
    unusedSize = RTCP_PACKETSIZE;
    packetAlloc = RTCP_PACKETSIZE;
}


RtcpPacket::~RtcpPacket ()
{
   // Nothing at this time
}



/* --- Size and Locations ------------------------------------------ */

char* RtcpPacket::getPacketData ()
{
    return packetData;
}

char* RtcpPacket::freeData ()
{
    return packetData + (packetAlloc - unusedSize);
}

int RtcpPacket::allocData (int s)
{
    if (unusedSize < s)
    {
        //cpLog (LOG_ERR, "RTCP: Out of allocated memory for RTCP packet");
        assert(0);
        return -1;
    }
    unusedSize -= s;
    memset (freeData(), 0, s);
    return s;
}


/*     unused memory    */

int RtcpPacket::getPacketAlloc ()
{
    return packetAlloc;
}

int RtcpPacket::getUnused ()
{
    return unusedSize;
}

void RtcpPacket::setTotalUsage (int size)
{
    assert (size <= packetAlloc);
    unusedSize = packetAlloc - size;
}



/* --- Packet Header functions ------------------------------------- */

int RtcpPacket::getVersion ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->version;
}

int RtcpPacket::getPadbyteFlag ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->padding;
}

int RtcpPacket::getCount ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return header->count;
}

RtcpType RtcpPacket::getPayloadType ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return static_cast < RtcpType > (header->type);
}


int RtcpPacket::getLength ()
{
    RtcpHeader* header = reinterpret_cast < RtcpHeader* > (packetData);
    return ntohs(header->length);
}


void RtcpPacket::printPacket ()
{
    printBits (packetData, getTotalUsage());
    cerr << "\n-----------------------------------\n";
}

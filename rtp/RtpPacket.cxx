
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

static const char* const RtpPacket_cxx_Version =
    "$Id: RtpPacket.cxx,v 1.3 2005/08/23 00:27:55 greear Exp $";


#include "global.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/types.h>
#include "vtypes.h"
#include <unistd.h>
#include <string.h>
//#include <map>
//#include <utility>                     // pair

// error handling
//#include <errno.h>

#include <sstream>

// network socket
#include <netinet/in.h>                // struct socketaddr_in
//#include <sys/socket.h>
//#include <netdb.h>

#include "cpLog.h"
#include "vsock.hxx"

#include "NetworkAddress.h"
#include "NtpTime.hxx"
#include "rtpTypes.h"
#include "rtpTools.hxx"
#include "Rtp.hxx"
#include "Rtcp.hxx"
#include "rtpCodec.h"


#ifdef USE_LANFORGE
RtpStatsCallbacks* rtpStatsCallbacks = NULL;
#endif

unsigned int RtpPacket::_cnt = 0;

/* ----------------------------------------------------------------- */
/* --- RtpPacket Constructor --------------------------------------- */
/* ----------------------------------------------------------------- */

RtpPacket::RtpPacket (int newpayloadSize, int _npadSize, int _csrc_count) {
   header = NULL;
   _cnt++;

   // check given paramters
   assert (_csrc_count >= 0);
   assert (_csrc_count <= 15);
   assert (newpayloadSize >= 0);
   assert (_npadSize >= 0);

   csrc_count = _csrc_count;
   npadSize = _npadSize;
   
   recalcPacketAlloc(newpayloadSize);

   setPadbyteSize (npadSize);

   clear();

   if (unusedSize != newpayloadSize) {
      cpLog(LOG_ERR, "ERROR: unusedSize: %d != newpayloadSize: %d\n",
            unusedSize, newpayloadSize);
      assert(0);
   }

}

void RtpPacket::recalcPacketAlloc(int newpayloadSize) {
   packetAlloc = (sizeof(RtpHeader) - sizeof(RtpSrc)
                  + csrc_count * sizeof(RtpSrc) + newpayloadSize + npadSize);
}

void RtpPacket::clear() {
   assertNotDeleted();
   assert (packetAlloc < RTP_MTU);

   memset (packetData, 0, packetAlloc);

   // set private variables
   header = reinterpret_cast < RtpHeader* > (packetData);
   setPayloadUsage (0);

   // set rtp header values
   header->version = RTP_VERSION;
   header->padding = (npadSize > 0) ? 1 : 0;
   header->extension = 0;
   header->count = csrc_count;
   header->marker = 0;
   header->type = rtpPayloadUndefined;
   header->sequence = 0;
   header->timestamp = 0;

   // set flags
   sequenceSet = false;
   timestampSet = false;

   converted_to_nbo = false;
   converted_to_hbo = false;
}//clear



// clones the rtp header
RtpPacket::RtpPacket (RtpPacket* clone, int newpayloadSize) {
    header = NULL;
    _cnt++;

    // create memory allocation
    packetAlloc = sizeof(RtpHeader) - sizeof(RtpSrc)
                  + clone->getCSRCcount() * sizeof(RtpSrc)
                  + newpayloadSize + clone->getPadbyteSize();
    assert (packetAlloc < RTP_MTU);

    memset (packetData, 0, packetAlloc);

    // set private variables
    header = reinterpret_cast < RtpHeader* > (packetData);
    setPadbyteSize (clone->getPadbyteSize());
    setPayloadUsage (0);
    assert (unusedSize == newpayloadSize);

    // clone header
    setVersion (clone->getVersion());
    setPaddingFlag (clone->getPaddingFlag());
    setExtFlag (clone->getExtFlag());
    setCSRCcount (clone->getCSRCcount());
    setMarkerFlag (clone->getMarkerFlag());
    setPayloadType (clone->getPayloadType());
    setSequence (clone->getSequence());
    setRtpTime (clone->getRtpTime());
    assert (getSequence() == clone->getSequence());

    // set flags
    sequenceSet = false;
    timestampSet = false;

    converted_to_nbo = false;
    converted_to_hbo = false;
}


RtpPacket::~RtpPacket () {
   _cnt--;
}



/* --- Size and Locations ------------------------------------------ */


/*     payload          */
char* RtpPacket::getPayloadLoc () {
   assert (header);
   return (packetData + sizeof(RtpHeader) - sizeof(RtpSrc)
           + (header->count)*sizeof(RtpSrc));
}

int RtpPacket::getPayloadSize () const
{
    assert (header);
    return (packetAlloc - sizeof(RtpHeader) + sizeof(RtpSrc)
            - (header->count)*sizeof(RtpSrc) - getPadbyteSize());
}

void RtpPacket::setPayloadUsage (int size)
{
    if (!(size <= getPayloadSize()))
        cerr << "ERR" << size << " " << getPayloadSize();
    assert (size <= getPayloadSize());
    unusedSize = getPayloadSize() - size;
}

int RtpPacket::getPayloadUsage () const
{
    return getPayloadSize() - unusedSize;
}



/*     padbyte          */
char* RtpPacket::getPadbyteLoc ()
{
    return getPayloadLoc() + getPayloadSize();
}

void RtpPacket::setPadbyteSize (int size)
{
    // future: not implemented
    // ? write size to last octlet of packetData
}


int RtpPacket::getPadbyteSize () const 
{
    // future: not implemented
    // ? read last octlet of packetData
    return 0;
}



/* --- Packet Information ------------------------------------------ */

/*     payload type     */
void RtpPacket::setPayloadType (RtpPayloadType type)
{
    assert (type >= 0);
    assert (type <= 127);
    header->type = type;
}

RtpPayloadType RtpPacket::getPayloadType () const
{
    return static_cast < RtpPayloadType > (header->type);
}



/*     sequence number  */
void RtpPacket::setSequence (RtpSeqNumber nseq)
{
    assert (header);
    sequenceSet = true;
    header->sequence = htons(nseq);
}

RtpSeqNumber RtpPacket::getSequence () const
{
    assert (header);
    return ntohs(header->sequence);
}



/*     timestamp        */
void RtpPacket::setRtpTime (RtpTime time)
{
    assert (header);
    timestampSet = true;
    header->timestamp = htonl(time);
}

RtpTime RtpPacket::getRtpTime () const
{
    return ntohl(header->timestamp);
}



/*     ssrc             */
void RtpPacket::setSSRC (RtpSrc src)
{
    assert (header);
    header->ssrc = htonl(src);
}

RtpSrc RtpPacket::getSSRC () const
{
    assert (header);
    return ntohl(header->ssrc);
}



/*     csrc             */
int RtpPacket::getCSRCcount () const
{
    assert (header);
    return header->count;
}

// use with caution
void RtpPacket::setCSRCcount (int i)
{
    assert (header);
    header->count = i;
}

void RtpPacket::setCSRC (RtpSrc src, unsigned int i)
{
    assert (header);
    assert (i >= 1);
    assert (i <= header->count);

    RtpSrc* srcPtr = &(header->startOfCsrc);
    *(srcPtr + i - 1) = htonl(src);
}

RtpSrc RtpPacket::getCSRC (unsigned int i) const
{
    assert (header);
    assert (i >= 1);
    assert (i <= header->count);

    RtpSrc* srcPtr = &(header->startOfCsrc);
    return ntohl(*(srcPtr + i - 1));
}


bool RtpPacket::isValid()
{
    // check packet size
    if( getTotalUsage() <= 0 )
        return false;

    // check version
    if( getVersion() != RTP_VERSION )
    {
        cpLog( LOG_DEBUG_STACK, "Wrong RTP version" );
        return false;
    }

    // modify known payload types
    switch( getPayloadType() )
    {
        case( 13 ):
        {
            setPayloadType( rtpPayloadPCMU );
            break;
        }
        default:
            break;
    }

    return true;
}


void RtpPacket::printPacket () {
   cerr << toString();
}

string RtpPacket::toString() const {
   ostringstream oss;
   oss << "\n-----------------------------------\n";
   oss << "PacketAlloc: " << getPacketAlloc() << "  ";
   oss << "PayloadSize: " << getPayloadSize() << "  ";
   oss << "PayloadUsage: " << getPayloadUsage() << "  ";
   oss << "isSilence: " << isSilenceFill() << "  ";
   oss << endl;
   oss << "Unused: " << getUnused() << "  ";
   oss << "TotalUsage: " << getTotalUsage();
   oss << "\n-----------------------------------\n";
   oss << getVersion() << " " << getPaddingFlag() << " " << getExtFlag() << " ";
   oss << getCSRCcount() << " " << getMarkerFlag() << " ";
   oss << (unsigned int)getPayloadType() << " ";
   oss << getSequence() << "  " << getRtpTime() << "  " << getSSRC();
   oss << "\n-----------------------------------\n";
   return oss.str();
}

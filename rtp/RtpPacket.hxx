#ifndef RTPPACKET_HXX
#define RTPPACKET_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const RtpPacket_hxx_Version =
    "$Id: RtpPacket.hxx,v 1.2 2004/11/05 07:25:06 greear Exp $";

#include "rtpTypes.h"
#include  <assert.h>
#include <string>
#include <BugCatcher.hxx>

/** 
    Data structure for RTP data packets.   Each RTP payload is encapsulated
    in one of these objects.

    @see RtpSession
*/
class RtpPacket : public BugCatcher {
public:
   /** Constructor that allocates RTP packetData
       @param newpayloadSize Payload size not including RTP header 
       @param npadSize Number of pad bytes (not implemented)
       @param csrc_count Number of contributing sources in packet
   **/
   RtpPacket (int newpayloadSize, int npadSize = 0, int csrc_count = 0);

   /// clones an existing packet to a new size
   RtpPacket (RtpPacket* clonePacket, int newpayloadSize);

   /// Destructor that deallocates RTP packetData memory
   virtual ~RtpPacket ();

   // Reset values to be just like when we created the pkt.  Can reuse the pkt safely
   // after using this call.
   void clear();

   // Sets up packageAlloc based on a new payload size, useful when re-using pkts.
   void recalcPacketAlloc(int newpayloadSize);

   /// Pointer to packet data
   char* getPacketData () { return packetData; }

   /// Pointer to packet header
   RtpHeader* getHeader () { return header; }

   /// Pointer to beginning of payload
   char* getPayloadLoc ();

   /// Maximum payload size
   int getPayloadSize () const;

   /** Sets payload usage
       @param size doesn't include RTP header
   **/
   void setPayloadUsage (int size);

   /// Size of payload stored
   int getPayloadUsage () const;

   /// Pointer to begnning of padbyte (Not implemented)
   char* getPadbyteLoc ();

   /// Sets size of payload (Not implemented)
   void setPadbyteSize (int size);

   /// Size of padbyte (Not fully implemented)
   int getPadbyteSize () const;

   /// Entire size of RTP packet including header, unused, and padbyte
   int getPacketAlloc () const { return packetAlloc; }

   /// Size of unused memory
   int getUnused () const { return unusedSize; }

   /** Sets size of RTP packet including header and padbyte
       Extra memory will be set as unused memory
   **/
   void setTotalUsage (int size) {
      assert (size <= packetAlloc);
      unusedSize = packetAlloc - size;
   }

   /// Size of RTP packet not including unused memory
   int getTotalUsage () const { return packetAlloc - unusedSize; }

   ///
   void setPayloadType (RtpPayloadType payloadType);
   ///
   RtpPayloadType getPayloadType () const;

   ///
   void setSequence (RtpSeqNumber newseq);
   ///
   RtpSeqNumber getSequence () const;

   ///
   RtpTime getRtpTime() const;
   ///
   void setRtpTime (RtpTime time);

   ///
   void setSSRC (RtpSrc src);
   ///
   RtpSrc getSSRC () const;

   /// Gets number of contributing sources
   void setCSRCcount (int i);   // use with cuation
   int getCSRCcount () const;

   ///
   void setCSRC (RtpSrc src, unsigned int index);

   /** index value less range 1 to csrc_count
       @param index value less range 1 to csrc_count
   **/
   RtpSrc getCSRC (unsigned int index) const;


   ///
   void setVersion (int i) { header->version = i; }
   int getVersion () const { return header->version; }
   ///
   void setPaddingFlag (int i) { header->padding = i; }
   int getPaddingFlag () const { return header->padding; }
   ///
   void setExtFlag (int i) { header->extension = i; }
   int getExtFlag () const { return header->extension; }
   ///
   void setMarkerFlag (int i) { header->marker = i; }
   int getMarkerFlag () const { return header->marker; }


   /// valid check
   bool isValid();

   /// Print packet contents
   void printPacket ();

   string toString() const;

   bool isTimestampSet() const { return timestampSet; }
   bool isSequenceSet() const { return sequenceSet; }

   void setTimestampSet(bool b) { timestampSet = b; }
   void setSequenceSet(bool b) { sequenceSet = b; }

   bool convertedToNBO() const { return converted_to_nbo; }
   bool convertedToHBO() const { return converted_to_hbo; }
   bool isMissing() const { return is_logically_missing; }
   bool isSilenceFill() const { return is_silence_fill; }

   void setConvertedToNBO(bool b) { converted_to_nbo = b; }
   void setConvertedToHBO(bool b) { converted_to_hbo = b; }
   void setIsMissing(bool b) { is_logically_missing = b; }
   void setIsSilenceFill(bool b) { is_silence_fill = b; }

   static int getInstanceCount() { return _cnt; }

private:

   static unsigned int _cnt;
   /// flags
   bool sequenceSet;
   bool timestampSet;
   bool converted_to_nbo;
   bool converted_to_hbo;
   bool is_logically_missing; /* Is this a 'filler' packet (un-filled but logically
                               * empty */
   bool is_silence_fill; // Is this filled with silence by the local codec/logic?

   int npadSize;
   int csrc_count;

   /// Pointer to raw memory allocation
   char packetData[RTP_MTU];

   /// Allocated packet size
   int packetAlloc;

   /// Size of unused memory
   int unusedSize;

   /// Easy access to header information
   RtpHeader* header;
};


#endif // RTPPACKET_HXX

#ifndef RTCPTRANSMITTER_HXX
#define RTCPTRANSMITTER_HXX

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000,2001,2003 Vovida Networks, Inc.  All rights reserved.
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

static const char* const RtcpTransmitter_hxx_Version =
    "$Id: RtcpTransmitter.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include "Rtcp.hxx"

#include <sys/types.h>

#include "rtpTypes.h"
#include "UdpStack.hxx"
#include "NetworkAddress.h"

class RtcpReceiver;
class RtcpPacket;

/** Class to transmit RTCP packets.  Called inside RtpSession.
    @see RtpSession
    
 */
class RtcpTransmitter
{
    public:
        /** Constructor creating outgoing RTCP stack using port ranges
            @param remoteHost DNS name receiving the packets
            @param port associated port
            @param local_ip  Local IP to bind to, use "" for system default.
         * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
         */
        RtcpTransmitter (const string& local_ip,
                         const string& local_dev_to_bind_to,
                         const char* remoteHost, int remoteMinPort,
                         int remoteMaxPort, RtcpReceiver* receiver);
        /** Constructor creating outgoing RTCP stack using specified port
            @param remoteHost DNS name receiving the packets
            @param port associated port
            @param local_ip  Local IP to bind to, use "" for system default.
         * @param local_dev_to_bind_to  If not "", we'll bind to this device with SO_BINDTODEV
         */
        RtcpTransmitter (const string& local_ip,
                         const string& local_dev_to_bind_to,
                         const char* remoteHost, int remotePort,
                         RtcpReceiver* receiver);
        /// consturctor init (don't call this function)
        void constructRtcpTransmitter ();

        /// Deconstructor
        ~RtcpTransmitter ();


        /** Transmits RTCP packet to remoteHost/port.
            Doesn't remove packet from memory.
            Returns -1 failure or number of bytes sent on success
         **/
        int transmit (RtcpPacket& packet);

        /// set timer to next interval
        void updateInterval ();

        /// Check if time to send RTCP packet.  Returns 1 then time up
        int checkInterval ();


        /// Adds SR packet into compound packet
        int addSR (RtcpPacket* packet, int npadSize = 0);

        /// Adds specificed SDES item to compound packet
        int addSDES (RtcpPacket* packet, RtcpSDESType item, int npadSize = 0);

        /// Adds all known SDES items to compound packet
        int addSDES (RtcpPacket* packet, int npadSize = 0);

        /// Adds specificed SDES items in SDESlist, which ends with RTCP_SDES_END
        int addSDES (RtcpPacket* packet, RtcpSDESType* SDESlist, int npadSize = 0);

        /** Adds BYE packet using transmitter's SRC numbers
            @param reason optional text, null-term
         **/
        int addBYE (RtcpPacket* packet, char* reason = NULL, int npadSize = 0);

        /** Adds BYE packet using specified list of SRC numbers
            @param reason optional text, null-term
            @param count number of items in list
         **/
        int addBYE (RtcpPacket* packet, RtpSrc* list, int count,
                    char* reason = NULL, int npadSize = 0);

        /// future: not implemented
        int addAPP (RtcpPacket* packet, int newpadbyeSize = 0);


        /// Used for calculating RR information
        u_int32_t calcLostFrac (RtpTranInfo* source);

        /// Used for calculating RR information
        u_int32_t calcLostCount (RtpTranInfo* source);

        ///
        void setSdesCname ();
        ///
        void setSdesName (char* text);
        ///
        void setSdesEmail (char* text);
        ///
        void setSdesPhone (char* text);
        ///
        void setSdesLoc (char* text);
        ///
        void setSdesTool (char* text);
        ///
        void setSdesNote (char* text);
        ///
        char* getSdesCname ();
        ///
        char* getSdesName ();
        ///
        char* getSdesEmail ();
        ///
        char* getSdesPhone ();
        ///
        char* getSdesLoc ();
        ///
        char* getSdesTool ();
        ///
        char* getSdesNote ();

        /// Sets RTP transmitter if one is used
        void setRTPtran (RtpTransmitter* tran);
        /// Sets RTCP transmitter if one is used
        void setRTPrecv (RtpReceiver* recv);
        /// Sets RTCP receiver if one is used
        void setRTCPrecv (RtcpReceiver* rtcpRecv);

        /// Port this tramsitter is sending it singal
        int getPort ();
        /// Socket File Descriptor used for select()
        int getSocketFD ();

        /// get the ptr of my UdpStack
        UdpStack* getUdpStack()
        {
            return myStack;
        }

        ///
        NetworkAddress* getRemoteAddr ()
        {
            return &remoteAddr;
        }

        ///
        void setRemoteAddr (const NetworkAddress& theAddr) ;


    private:
        /// Next time to submit RTCP packet
        NtpTime nextInterval;

        /// Transmission interval in ms
        static const int RTCP_INTERVAL;

        /** Transmitter SDES information
         *  data stored as null-term strings
         **/
        SDESdata* SDESInfo;

        ///
        RtpTransmitter* tran;
        ///
        RtpReceiver* recv;
        ///
        RtcpReceiver* rtcpRecv;

        /// my UDP stack
        UdpStack* myStack;

        ///
        bool freeStack;

        NetworkAddress remoteAddr;
};


#endif // RTCPTRANSMITTER_HXX

#ifndef RtspTransportHdr_hxx
#define RtspTransportHdr_hxx
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


static const char* const RtspTransportHdr_hxx_Version = 
    "$Id: RtspTransportHdr.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "Data.hxx"
#include "RtspBadDataException.hxx"
#include "Sptr.hxx"
#include "CharDataParser.hxx"

/** Grammer for Transport Header
   Transport           =    "Transport" ":"
                            1\#transport-spec
   transport-spec      =    transport-protocol/profile[/lower-transport]
                            *parameter
   transport-protocol  =    "RTP"
   profile             =    "AVP"
   lower-transport     =    "TCP" | "UDP"
   parameter           =    ( "unicast" | "multicast" )
                       |    ";" "destination" [ "=" address ]
                       |    ";" "interleaved" "=" channel [ "-" channel ]
                       |    ";" "append"
                       |    ";" "ttl" "=" ttl
                       |    ";" "layers" "=" 1*DIGIT
                       |    ";" "port" "=" port [ "-" port ]
                       |    ";" "client_port" "=" port [ "-" port ]
                       |    ";" "server_port" "=" port [ "-" port ]
                       |    ";" "ssrc" "=" ssrc
                       |    ";" "mode" = <"> 1\#mode <">
   ttl                 =    1*3(DIGIT)
   port                =    1*5(DIGIT)
   ssrc                =    8*8(HEX)
   channel             =    1*3(DIGIT)
   address             =    host
   mode                =    <"> *Method <"> | Method


   Example:
     Transport: RTP/AVP;multicast;ttl=127;mode="PLAY",
                RTP/AVP;unicast;client_port=3456-3457;mode="PLAY"
 */
class RtspTransportSpec
{
    public:
        /** */
        RtspTransportSpec();
        /** */
        RtspTransportSpec(const RtspTransportSpec& x);

        /** */
        ~RtspTransportSpec() {}

        /** */
        RtspTransportSpec& operator= (const RtspTransportSpec& x);
        /** */
        bool operator== (const RtspTransportSpec& x) const;

        /** */
        void reset();

        /** */
        void parse(CharDataParser& parser);
        /** */
        void parseProtocol(CharDataParser& paramParser);
        /** */
        void parseMulticastOrMode(CharDataParser& paramParser);
        /** */
        void parseUnicast(CharDataParser& paramParser);
        /** */
        void parseDestination(CharDataParser& paramParser);
        /** */
        void parseSource(CharDataParser& paramParser);
        /** */
        void parseAppend(CharDataParser& paramParser);
        /** */
        void parseClientPort(CharDataParser& paramParser);

        /** */
        Data encode();

        /** if it is false then it is RTP/AVP/UDP */
        bool myIsTcp; // TODO
        /** if it is false then it is unicast */
        bool myIsMulticast; // TODO
        /** mode: If it is false, it is PLAY */
        bool myIsRecord; 
        /** for record: If it is false, it is overiwritten mode
         *  if it is true but server is not support it, need to refuse 
         *  the request
         */
        bool myIsAppend;
        /** */
        Data myDestination;
        /** */
        Data mySource;
        //
//        bool myIsInterleaved; // TODO
        //
//        u_int8_t myChannelA; // for interleaved
        //
//        u_int8_t myChannelB; // for interleaved
        //
//        u_int8_t myTtl; // for multicast
        //
//        u_int32_t myLayers; // for multicast
        //
//        u_int16_t myPortA; // for multicast
        //
//        u_int16_t myPortB; // for multicast
        //
        u_int16_t myClientPortA;
        /** */
        u_int16_t myClientPortB;
        /** */
        u_int16_t myServerPortA;
        /** */
        u_int16_t myServerPortB;
        //
//        u_int32_t mySSRC; // TODO

    private:

};

class RtspTransportHdr
{
    public:
        /** */
        RtspTransportHdr()
        { }

        /** to check a TransportSpec, return false if it is using TCP
         * or multicast or clientPort is not provided.
         *
         * TODO need to let server to tell if they want TCP or multicast
         *
         */
        bool isSuitableSpec(Sptr< RtspTransportSpec > spec);

        /** It is a list transport-spec, parser finds the first suitable option
         *  and return it; if it couldnot find one, return 0
         */
        Sptr< RtspTransportSpec > parseTransportHeader(CharDataParser& 
            parser); // throw (RtspBadDataException&);

        /** Given a TransportSpec and append it to the exist myTransport 
         * If it is empty, append "Transport:" first.
         * use ',' deliminator between each spec
         */
        void appendTransportSpec(Sptr< RtspTransportSpec > transSpec);
        /** return the exist myTransport for encoder */
        Data encode() 
        { if (myTransport.length() > 0)
              return (myTransport + "\r\n"); 
          else
              return ""; }
        
    private:
        /** The data for response Transport hdr */
        Data myTransport;
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

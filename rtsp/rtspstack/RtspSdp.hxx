#ifndef RtspSdp_Hxx
#define RtspSdp_Hxx
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


static const char* const RtspSdp_hxx_Version =
    "$Id: RtspSdp.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "SdpSession.hxx"
#include "Sptr.hxx"
#include "Data.hxx"
#include <vector>

/** */
class RtspSdp
{
    public:
        /** The default C'tor. Need to use set functions to fill info
         *  RtspSdp provides encode() to form a SDP Data. 
         *  This can be used to creating SDP on the fly, for DESCRIBE response
         */
        RtspSdp();
        /** get the contentData (msgBody) from a RtspMsg
         *  RtspSdp constructor will decode it and check it if valid 
         *  RtspSdp provides get functons for application to read the sdp info
         *  This can be used to decode the SDP in ANNOUNCE
         */
        RtspSdp( const Data& contentData );

        /** */
        Sptr< SdpSession > getSdpSession()
            { return &mySdpSession; }

        /** */
        void decode( const Data& contentData );

        /** */ 
        Data encode()
            { return mySdpSession.encode(); }

        /** */
        bool isValid() { return mySdpSession.isValidDescriptor(); }

        /** The c= addr for unicast
            TODO for multicast
         */
        void setConnAddress(const Data& connAddr);
        /** The o= addr */
        void setOwnerAddress(const Data& ownerAddr);
        /** */
        void setUserName(const Data& userName);
        /** */
        void setSessionName(const Data& sessionName);
        /** for the session level attribute */
        void addAttribute(const char* attribute, const char* value);

        /** Following set functions are setting data for the *first* SdpMedia
         *  it creates a SdpMedia if there is none
         *  If you want to add more than one SdpMedeia,
         *  you need to create SdpMedia by yourself and use getSdpSession 
         *  to add onto it
         */
        void setMediaPort(int port);
        /** */
        void setMediaConnAddress(const Data& connAddr);
        /**  for the media level attribute */
        void addMediaAttribute(const char* attribute, const char* value);
        /** */
        void addMediaFormat(const int payloadType);
        /** add one rtpmap and add associate Format in the SdpMedia */
        void addRtpmap(const int payloadType, 
                                const Data& encodeName,
                                const int cloackRate);


        /** The c= addr for unicast
            TODO multicast
         */
        Data getConnAddress() ;
        /** The o= addr */
        Data getOwnerAddress() ;
        /** */
        Data getUserName() ;
        /** */
        Data getSessionName() ;
        /** get session level attribute */
        Data getAttributeValue(const char* attribute) ;
        /** Following get functions are getting the info for *first* SdpMedia
         *  If you want to get more than one SdpMedeia,
         *  you need to use getSdpSession to get the list of SdpMedia
         */
        int getMediaPort() ;
        /** */
        Data getMediaConnAddress() ;
        /** get media level attribute */
        Data getMediaAttributeValue(const char* attribute) ;
        /** get format list */
        vector < int > * getMediaFormatList() ;
        /** get rtpmap list  */
        vector < SdpRtpMapAttribute* > * getRtpmapList() ;
        /** get codec and codecString */
        void getCodecNCodecString( int& sdpCodec, string& sdpCodecString );

    private:
        /** */
        RtspSdp(const RtspSdp& src); 
        /** */
        RtspSdp& operator=(const RtspSdp&);
        /** */
        SdpSession mySdpSession;
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

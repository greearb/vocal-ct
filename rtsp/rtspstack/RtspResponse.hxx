#ifndef RtspResponse_hxx
#define RtspResponse_hxx
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


static const char* const RtspResponse_hxx_Version =
    "$Id: RtspResponse.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspUtil.hxx"
#include "Data.hxx"
#include "RtspMsg.hxx"
#include "RtspRequest.hxx"
#include "RtspBadDataException.hxx"
#include "RtspTransportHdr.hxx"
#include "RtspRangeHdr.hxx"
#include "RtspLocationHdr.hxx"
#include "RtspRtpInfoHdr.hxx"
#include "RtspSdp.hxx"

/** */
class RtspResponse : public RtspMsg
{
    public:
        /** */
        RtspResponse();

        /** The constructor will create the initial response including
         *  statusLine, Cseq and sessionId in myData
         */
        RtspResponse(Sptr< RtspRequest > rtspRequest, 
                     RtspStatusCodesType statusCode);

        /** */
        RtspResponse( const RtspResponse& src);
        /** */
        RtspResponse& operator=( const RtspResponse& src );
        
        /** */
        const u_int32_t getStatusCodeNumber() const 
            { return RtspUtil::getStatusCodeInNumber(myStatusCodeType); }

        /** To append new sessionId for SETUP response */
        void appendSessionId(u_int32_t sessionId);

        /** append the spec which rtspRequest has and add the server ports */
        void appendTransportHdr(Sptr< RtspRequest > theRequest, 
                                u_int32_t serverPortA, u_int32_t serverPortB);
        /** append a new spec created by server or application */
        void appendTransportHdr(Sptr< RtspTransportSpec > spec);
        /** for DESCRIBE response, and PLAY/PAUSE/RECORD response */
        void appendNPTRangeHdr(double  startTime, double endTime);
        /** for 201 created */
        void appendLocationHdr(Sptr< RtspLocationHdr > hdr);
        /** */
        void appendRtpInfoHdr(Sptr< RtspRtpInfoHdr > hdr);
        //
//        void appendDateHdr(Data& date);
        //
//        void appendExpireHdr(Data& expire);
        //
//        void appendContentTypeHdr(Data& contentType);
        //
//        void appendContentBaseHdr(Data& contentBase);
        /** for TEARDOWN response */
        void appendConnectionCloseHdr();
        /** append the empty line if there is no content data */
        void appendEndOfHeaders();

        /** for general contentData */
//        void appendContentData(char* contentData, 
//                               char* contentType,
//                               u_int32_t contentLength);
        /** for sdp extracted from a file, it's already encoded
         *  it will add contentType and contentLength hdrs
         */
        void appendSdpData( RtspSdp& sdp );

        /** It triggers the parsing of startline and assign myStatusCode */
        const u_int32_t getStatusCode() throw (RtspBadDataException&);

        /** */
        virtual Data encode();

    private:
        //
//        bool compareRtspRequest(const RtspResponse& src) const;
        //
//        bool compareFileLocation(const RtspResponse& src) const;
        //
        void setStatusLine();
        /** */
        void appendCSeqHdr(Sptr< RtspRequest > theRequest);
        /** For the rtspRequest that already has session id */
        void appendSessionHdr(Sptr< RtspRequest > theRequest);
        /** The associated request for this reponse */
        Sptr< RtspRequest > myRtspRequest;
        /** for encoder */
        RtspStatusCodesType myStatusCodeType;
        /** get from parser */
        u_int32_t myStatusCode;

        /** */
        Sptr< RtspLocationHdr > myFileLocation;        
        /** */
        Sptr< RtspRtpInfoHdr > myRtpInfo;
        
};


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif
   

#ifndef RtspMsgParser_hxx
#define RtspMsgParser_hxx
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



static const char* const RtspMsgParser_hxx_Version =
    "$Id: RtspMsgParser.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";

#include "RtspBadDataException.hxx"
#include "Sptr.hxx"
#include "RtspMsg.hxx"


/** */
class RtspMsgParser
{
    public:
        /** */
        RtspMsgParser()
        { }

        /** If the savedMsg is null, it parses the given RTSP data and 
            returns a RtspMsg object with preparsed info filed in. 
            The RtspMsg object is allocated in the call and it is the 
            responsibility of the caller to free the memory
            The returned RtspMsg can be an incomplete rtsp msg 
            to wait for completion;
            If the savedMsg is not null, it append the savedMsg with more
            data or complete it and return it after it parses the data.
            @param data the TcpBufferData get from TCP stream
            @param bytesNeedtoDecode bytes need to be decoded
            @param bytesDecode bytes has been decoded after this function
            @param savedMsg the msg that incomplete from last preparse
          */
        static Sptr< RtspMsg > preParse(const char* data, 
                                        int bytesNeedtoDecode,
                                        int& bytesDecode,
                                        Sptr< RtspMsg > savedMsg) 
            throw (RtspBadDataException&);
        /** create the corresponding RtspMsg according the data's first word
            @param data the raw data from TCP stream
            @oaram bytesNeedtoDecode the bytes available for decoding
         */
        static Sptr< RtspMsg > createRtspMsg(const char* data, int bytesNeedtoDecode);

};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

#ifndef SipSdp_HXX
#define SipSdp_HXX

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

#include "global.h"

#include "Data.hxx"
#include "Sdp2Session.hxx"
#include "Sdp2Connection.hxx"
#include "SipContentData.hxx"

namespace Vocal
{
    
/// data container for Sdp object in the body of a SIP message.
class SipSdp : public SipContentData
{
    public:
        /// Create one with default values
        SipSdp(const string& local_ip);
        ///
        SipSdp(const Vocal::SDP::SdpSession& descriptor, const string& local_ip);
        ///
        SipSdp(const SipSdp& src);
        ///
        SipSdp& operator =(const SipSdp&);

#if 0
        /// compare equality -- xxx not implemented
        bool const operator==(const SipSdp& rhs) const;
#endif
        ///
        SipSdp( const Data& data, const string& local_ip );

	///
	virtual ~SipSdp();
    
        ///
        Vocal::SDP::SdpSession&
        getSdpDescriptor() ;
        ///
        void
        setSdpDescriptor(const Vocal::SDP::SdpSession& sdp);
        ///
        void
        setConnAddress(const Vocal::SDP::SdpConnection& connAddr, const char* dbg);
        ///
        Data
        getConnAddress();
        ///
        void
        setUserName(const Data& name);
        ///
        Data
        getUserName();
        ///
        void
        setAddressType(SDP::AddressType);
        ///
	SDP::AddressType
        getAddressType();
        ///
        void
        setAddress(const Data& address);
        ///
        Data
        getAddress();
        ///
        void
        setRtpPortData(const Data& port);
        ///
        Data
        getRtpPortData();
        ///
        void
        setRtpPort(int port);
        /** get the RTP port requested in this SDP message. if none
            can be found, return -1 */
        int
        getRtpPort() const;
        ///
        void
        setSessionName(const Data& session);
        ///
        Data
        getSessionName();
        ///
        void
        setSessionId(unsigned int sessionId);
        ///
        unsigned int
        getSessionId();

        ///
        Data
        encodeHeaders() const;
        /** encode the body of the ContentData and return it.  
	    @param len   returns the length in bytes of the encoded body.
	*/
        Data encodeBody(int& len);
	/// virtual function to duplicate the header	
	virtual SipContentData* duplicate() const;

	///
	bool doCodecsMatch() const;

    private:
        Vocal::SDP::SdpSession sdpDescriptor;

        SipSdp(); //NA
};

ostream&
operator<<(ostream& s, SipSdp &msg);
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

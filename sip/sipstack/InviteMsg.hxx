#ifndef INVITEMSG_H
#define INVITEMSG_H

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

#include "SipCommand.hxx"
#include "StatusMsg.hxx"
#include "symbols.hxx"
#include "BaseUrl.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

static const char* const InviteMsg_hxx_Version =
    "$Id: InviteMsg.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";


namespace Vocal
{

/**
    Implements a SIP INVITE request.
    <p>
    <b>Example</b>
    <p>
    <pre>
    Data url;
    url = "sip:destination@192.168.5.12";
    Sptr <BaseUrl> baseurl = BaseUrl::decode(url);
    Sptr <SipUrl> tourl;
    tourl.dynamicCast(baseurl);
    int listenPort = 5060;
    int rtpPort = 3456;
    InviteMsg invite(tourl, listenPort, rtpPort);
    </pre>
*/
class InviteMsg : public SipCommand
{
    public:
        ///
        enum InviteMsgForward { inviteMsgForward };
        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        InviteMsg(const string& local_ip);

        /// Create a new invite based on a diversion of a prev invite
        InviteMsg( const InviteMsg& msg, Sptr <BaseUrl> uri,
                   const string& local_ip);

        /// The listenPort is required to build the INVITE msg, in the Via field.        
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        InviteMsg(Sptr <BaseUrl> url, const string& local_ip,
                  int listenPort = 5060 , int rtpPort = 3456);  
        /// for forwarding
        InviteMsg(const InviteMsg& msg, enum InviteMsgForward);

        /// UAS uses
        InviteMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        InviteMsg(const StatusMsg& statusMsg, const string& local_ip);

        /** construct a new InviteMsg based on the text of an INVITE msg. 
            this should not be called by anyone but the SipMsg class
         *  @param data  message to be parsed
         *  @exception   thrown if message fails basic parsing
         *  @param local_ip cannot be "" here, must be the local IP we are bound to locally
                   or 'hostaddress' if we are not specifically bound.
        */
        InviteMsg( const Data& data, const string& local_ip ) throw (SipParserException&);

        ///
        InviteMsg& operator =(const InviteMsg&);
        ///
        bool operator ==(const InviteMsg& src) const;
        ///
        InviteMsg(const InviteMsg&);
        ///
        virtual ~InviteMsg(){}
        ///
        virtual Method getType() const;

        //--- SessionTimer headers //
        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        const SipSessionExpires& getSessionExpires(const string& local_ip) const;        

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        const SipMinSE& getMinSE(const string& local_ip) const ;

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        void setSessionExpires(const SipSessionExpires& sess, const string& local_ip);

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        void setMinSE(const SipMinSE& minSe, const string& local_ip);

    private:
        void setInviteDetails(Sptr <BaseUrl> url, const string& local_ip,
                              int listenPort, int rtpPort);

        InviteMsg(); // Not implemented
};
 
}// namespace Vocal

/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */

#endif

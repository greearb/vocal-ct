#ifndef ACKMSG_H
#define ACKMSG_H

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

static const char* const AckMsgVersion = 
"$Id: AckMsg.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "SipCommand.hxx"

namespace Vocal
{
    
class StatusMsg;

/**  \ingroup SIP
     Class to implement the ACK method of a SIP message 
     <p>
     <b>Example</b>
     <p>
     <pre>
       Data url;
       url = "sip:destination@192.168.5.12";
       Sptr <BaseUrl> tourl;
       tourl = BaseUrl::decode(url);
       int listenPort = 5060;
       int rtpPort = 3456;

       InviteMsg invite(tourl, listenPort, rtpPort);
       StatusMsg status(invite, 200);

       AckMsg ack(status);
       ack.getMutableRequestLine().setUrl(tourl);
     </pre>
*/
class AckMsg : public SipCommand
{
    public:
        enum AckMsgForward { ackMsgForward };

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        AckMsg(const Data& d, const string& local_ip );
        ///
        AckMsg(const AckMsg&);

        /** this interface is deprecated and will not work with loose routing.
            @deprecated
        */
        AckMsg(const AckMsg&, enum AckMsgForward);

        /// UAS uses
        AckMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        AckMsg(StatusMsg& statusMsg, const string& local_ip);

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        AckMsg(const string& local_ip);

        ///
        AckMsg& operator =(const AckMsg&);

        ///
        bool operator ==(const AckMsg& src) ;

        ///
        virtual ~AckMsg();

        ///
        Method getType() const;

    private:
        /// to be deleted
        void setAckDetails(const StatusMsg& statusMsg);

        // Not Implemented.
        AckMsg();
};
 
}



/* Local Variables: */
/* c-file-style:"stroustrup" */
/* c-basic-offset:4 */
/* c-file-offsets:((inclass . ++)) */
/* indent-tabs-mode:nil */
/* End: */

#endif

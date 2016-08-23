#ifndef BYEMSG_H
#define BYEMSG_H

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

namespace Vocal
{

class StatusMsg;

/**
    Impements SIP BYE message.
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
    SipContact myContact(url);
    status.setContact(myContact);

    AckMsg ack(status);
    ack.getMutableRequestLine().setUrl(tourl);

    //construct BYE on status. (If you were UAC for initial INVITE)
    ByeMsg byeMsg(status);
    //Reverse the record route list and append contact at the end
    //if RecordRoute list contains A,B,C,D
    //reverse it an create a list 
    //D,C,B,A, see UaBase::saveRouteList() for details
    SipRecordRouteList rrList = status.getrecordrouteList();
    vector<SipRoute*> newRoutes; //Assume newRoutes contains the processed route list(rrList)

    ///ProcessRoutes
    if(newRoutes.size())
    {
        byeMsg.setRouteList(newRoutes);
        SipRoute siproute = byeMsg.getRoute(0);
        byeMsg.removeRoute(0);
        SipRequestLine& reqLine = byeMsg.getMutableRequestLine();
        reqLine.setUrl( siproute.getUrl() );
    }
    //Set the via 
    byeMsg.flushViaList();
    SipVia via;
    via.setHost(localHostAddr);
    via.setPort(localSipPort);
    byeMsg.setVia(via);

    //Set the seq number to be oldSeq+1, say oldSeq=101
    unsigned int cseq = 102;
    SipCSeq sipCSeq;
    sipCSeq.setCSeq( cseq );
    byeMsg->setCSeq( sipCSeq );
    ///End Process Routes

    //construct BYE on Ack.(if you were UAS for initial INVITE)
    ByeMsg byeAck(ack);
    //Get the record route list from the original request received
    //if RecordRoute list contains A,B,C,D
    //an create a list as A,B,C,D
    //see UaBase::saveRouteList() for details
    vector<SipRoute*> newRoutes; //Assume newRoutes contains the processed route list(rrList)
    //Follow the processing of the route and setting via, same as above

    </pre>
*/
class ByeMsg : public SipCommand
{
    public:
        ///
        enum ByeMsgForward { byeMsgForward };

        ///
        ByeMsg(const SipCommand& sipMessage);
        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        ByeMsg(const Data& data, const string& local_ip);

        ///
        ByeMsg(const ByeMsg&);

        /** this interface is deprecated and will not work with loose routing.
            @deprecated 
        */
        ByeMsg(const ByeMsg&, enum ByeMsgForward);

        /// UAS uses
        ByeMsg(const SipCommand& src, const SipVia& via, const SipCSeq& cseq);

        /// Proxy and UAC use
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        ByeMsg(const StatusMsg& statusMsg, const string& local_ip);

        ///
        ByeMsg& operator =(const ByeMsg&);
        ///
        bool operator ==(const ByeMsg& src); 

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        ByeMsg(const string& local_ip);

        // Help up-cast safely.
        virtual bool isCancelMsg() const { return false; }
        virtual bool isByeMsg() const { return true; }

        ///
        virtual Method getType() const;
    private:
        /// to be deleted
        void setByeDetails(const SipMsg& sipMessage);

        // Not Implemented.
        ByeMsg();
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

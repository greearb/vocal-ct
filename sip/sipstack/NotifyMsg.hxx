#ifndef NOTIFYMSG_H
#define NOTIFYMSG_H

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

static const char* const NotifyMsg_hxx_Version =
    "$Id: NotifyMsg.hxx,v 1.2 2004/05/05 06:37:33 greear Exp $";

#include "SipCommand.hxx"
#include "SipSubsNotifyEvent.hxx"
#include "SubscribeMsg.hxx"
#include "SipCallLeg.hxx"
#include "SipStatusLine.hxx"
#include "VException.hxx"

namespace Vocal
{

///
class NotifyMsg : public SipCommand
{
    public:
        ///
        enum NotifyMsgForward { notifyMsgForward };

        class NotReferException : public VException
        {
            public:
                ///
                NotReferException(const string& file,
                                  const int line,
                                  const int error = 0 )
                    : VException("", file, line, error) {}

                string getName() const { return "NotifyMsg::NotReferException"; }
        };

        ///default constructor
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        NotifyMsg(const string& local_ip);

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        NotifyMsg(const Data& data, const string& local_ip);

        ///Notify messages can only be constructed if a SUBSCRIBE method was first sent.
        ///  Since a NOTIFY message can only have one Event header, the event that is
        ///  being notified must be passed to the constructor.  A check is made to be
        ///  sure the event exists in the SUBSCRIBE
        ///  message, if not, no event header will be added to the NOTIFY message
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        NotifyMsg(const SubscribeMsg& subscribeRequest, const SipSubsNotifyEvent& event,
                  const string& local_ip);

        ///constructor so that only the call-leg of a SUBSCRIBE message can be stored,
        ///not an entire SUBSCRIBE message for each user's SUBSCRIBE message.
        NotifyMsg(const SipCallLeg& callLeg, const SipSubsNotifyEvent& event,
                  const string& local_ip);

        ///copy constructor
        NotifyMsg(const NotifyMsg&);
        ///construct notify, to forward to the next host(used by the proxy)
        NotifyMsg(const NotifyMsg&, enum NotifyMsgForward);
        ///assignment operator
        const NotifyMsg& operator =(const NotifyMsg&);
        ///comparison operator
        bool operator ==(const NotifyMsg& src);
        ///destructor
        virtual ~NotifyMsg();
        ///called to get the type of message.
        virtual Method getType() const;


        // Help up-cast safely.
        virtual bool isCancelMsg() const { return false; }

        /// the status line of the wrapped message
        SipStatusLine getReferredStatusLine() const;
    private:
        /// message is used as the same from, to, and call-id, however, the
        /// from must be used as the request-uri to send the NOTIFY message back to.
        void setNotifyDetails(Sptr <BaseUrl> fromUrl);

        NotifyMsg(); //Not implemented.
};
 
} // namespace Vocal


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

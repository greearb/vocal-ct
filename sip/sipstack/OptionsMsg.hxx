#ifndef OPTIONSMSG_H
#define OPTIONSMSG_H

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

static const char* const OptionsMsgVersion
= "$Id: OptionsMsg.hxx,v 1.2 2004/05/05 06:37:33 greear Exp $";

namespace Vocal
{

class SipUrl;

///
class OptionsMsg : public SipCommand
{
    public:
        ///
        enum OptionsMsgForward { optionsMsgForward };
        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        OptionsMsg(const string& local_ip);

        ///
        virtual ~OptionsMsg(){};
        //The listenPort is required to build the INVITE msg, in the Via field.
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        OptionsMsg(Sptr <BaseUrl> url, const string& local_ip,
                   int listenPort = 5060, int rtpPort = 3456);  // TODO fix rtpPort default

        /// this should not be called by anyone but the SipMsg class
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        OptionsMsg( const Data& data, const string& local_ip );

        ///
        OptionsMsg& operator =(const OptionsMsg&);
        ///
        bool  operator ==(const OptionsMsg& src); 
        ///
        OptionsMsg(const OptionsMsg&);

        ///
        OptionsMsg(const OptionsMsg&, enum OptionsMsgForward);


        // Help up-cast safely.
        virtual bool isCancelMsg() const { return false; }

        ///
//        virtual Data encode() const;  // get details and create Message, and return.
        ///
        virtual Method getType() const;

    private:
        void setOptionsDetails(Sptr <BaseUrl> url,
                               int listenPort, int rtpPort);

        OptionsMsg(); //Not Implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

#ifndef STATUSMSG_HXX_
#define STATUSMSG_HXX_

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

static const char* const StatusMsg_hxx_Version
    = "$Id: StatusMsg.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include <vector>
#include "cpLog.h"
#include "SipStatusLine.hxx"
#include "SipWwwAuthenticate.hxx"
#include "SipServer.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipSession.hxx"
#include "SipMsg.hxx"
#include "SipInReplyTo.hxx"
#include "symbols.hxx"
#include "SipWarning.hxx"
#include "SipSessionExpires.hxx"
#include "SipMinSE.hxx"

namespace Vocal
{
    
class SipUnsupported;
class SipCommand;

enum SipStatusErrorType
{
    STATUS_UNKNOWN_NETTYPE,
    STATUS_UNKNOWN_ADDRTYPE,
    STATUS_PARAM_NUMERR,
    STATUS_UNDEF_MODIFIER,
    STATUS_UNKNOWN_FIELD,
    STATUS_FAILED,
    STATUS_PARAM_NOTSET
};


///class to handle exceptions of SDPMedia.
class SipStatusException
{
    public:
        SipStatusException(SipStatusErrorType i)
        {
            value = i;
        }
        SipStatusErrorType value;
};


/**
   Class for all SIP STATUS messages.
   <p>
   <b>Example</b>
   <p>
   <pre>
      Data url;
      url = "sip:destination@192.168.5.12";
      Sptr<SipUrl> tourl = new SipUrl (url);
      int listenPort = 5060;
      int rtpPort = 3456;

      InviteMsg invite(tourl, listenPort, rtpPort);

      //Create Status message off the Invite
      StatusMsg status(invite, 183);
      SipSession testSession("Media");
      status.setSession(testSession);
   </pre>
*/
class StatusMsg : public SipMsg
{
    public:
        ///
        StatusMsg(SipCommand& command, int statusCode);
        ///
        StatusMsg(const Data& data, const string& local_ip) throw (SipParserException&);
        ///
        StatusMsg(const string& local_ip);
        ///
        StatusMsg(const StatusMsg&);
        ///
        const StatusMsg& operator = (const StatusMsg& src);
        ///
        bool operator ==(const StatusMsg& statusMsg); 
        ///
        void decode(const Data& data);
        ///
        virtual ~StatusMsg();

        ///mime
        void mimeDecode( const Data& gdata );
     
	// ----------------- StatusLine Header Methods ------------------
        /// Get the current StatusLine header
        const SipStatusLine& getStatusLine() const;
        /// Set the StatusLine header
        void setStatusLine( const SipStatusLine& );
        /// Set the StatusLine header
        void setStatusLine( const Data& textData);

        // ------------ ProxyAuthenticate Header Methods ------------------
        /// Get the current ProxyAuthenticate header
        const SipProxyAuthenticate& getProxyAuthenticate() const;
        /// Set the ProxyAuthenticate header
        void setProxyAuthenticate( const SipProxyAuthenticate& );
        /// Set the ProxyAuthenticate header
        void setProxyAuthenticate( const Data& textData);

        // ----------------- Server Header Methods ------------------
        /// Get the current Server header
        const SipServer& getServer() const;
        /// Set the Server header
        void setServer( const SipServer& );
        /// Set the Server header
        void setServer( const Data& textData); 

        /**@name Unsupported Header Methods
          */
        //@{

        /// Get the i'th Via item. If i is -1, it gets the last one 
        const SipUnsupported& getUnsupported( int i=-1) const;

        /** set or add another Unsupported itme, if the index is -1,
            it is appended to the current list */

        void setUnsupported(const SipUnsupported& item, int index = -1);

         /** set or add another Unsupported itme, if the index is -1,
             it is appended to the current list */
        void setUnsupported(const Data& item, int index = -1);
        /** Set number of Unsupported items.
        If i is less than current number then the extras are deleted. */
        void setNumUnsupported(int i);
        ///
        void flushUnsupportedList();
        ///
        void copyUnsupportedList( const StatusMsg& srcMsg);
        ///
        void addsipUnsupported(SipUnsupported* sipUnsupported);
        ///
        const vector < SipUnsupported* > & getUnsupportedList() const;
        ///return true if both the objects contain identical unsupported lists.
        bool compareUnsupportedList(const StatusMsg& src) const; 
        /// get the number of Via items 
        int getNumUnsupported() const;      

        //@}
     
        // ----------------- Warning Header Methods  -----------------
        /// get the number of Warning item

        /** set or add another Warning itme, if the index is -1, it is appended
        to the current list */
        void setWarning(const SipWarning& item, int index = -1);
    
        /** set or add another Warning itme, if the index is -1, it is appended
        to the current list */
        void setWarning(const Data& item, int index = -1);
        /** Set number of Warning items.
        If i is less than current number then the extras are deleted. */
        void setNumWarning(int i);
        ///
        const vector < SipWarning* > & getWarningList() const;
        ///
        void copyWarningList( const StatusMsg& srcMsg);
        ///
        void flushWarningList();

        /** return true if both the objects contain identical
            unsupported lists. */
        bool compareWarningList(const StatusMsg& src) const;       
        /// get the number of Via items 
        int getNumWarning() const;
        /// Get the i'th Via item. If i is -1, it gets the last one 
        const SipWarning& getWarning( int i=-1) const;

        // ----------------- WwwAuthenticate Header Methods ------------------
        /// Get the current WwwAuthenticate header
        const SipWwwAuthenticate& getWwwAuthenticate() const;
        /// Set the WwwAuthenticate header
        void setWwwAuthenticate( const SipWwwAuthenticate& );
        /// Set the WwwAuthenticate header
        void setWwwAuthenticate( const Data& textData);
        ///
        Data encodeWarningList() const;
        ///
        Data encodeUnsupportedList() const;
        ///
        virtual Data encode() const;
        ///
        void setReasonPhrase(const Data& reason);
        ///
        Data getReasonPhrase() const;
        ///
        Method getType() const;
	/**@name InRePlyTo
          */
        //@{

	///
	void setInReplyTo(const SipInReplyTo& item, int index = -1 );
	///
	void setInReplyTo(const Data& item, int index = -1 );
	///
	void setNumInReplyTo(int index);
	///
	const vector < SipInReplyTo* > & getInReplyToList() const;
        ///
        void addsipInReplyTo(SipInReplyTo* sipwarning);
        ///
        void copyInReplyToList( const StatusMsg& srcMsg);
        ///
        void flushInReplyToList();
        ///
        Data encodeInReplyToList() const;
        ///return true if both the objects contain identical InReplyTo lists.
        bool compareInReplyToList(const StatusMsg& src) const;       
        /// get the number of InReplyTo items 
        int getNumInReplyTo() const;
        /// Get the i'th InReplyTo item. If i is -1, it gets the last one 
        const SipInReplyTo& getInReplyTo( int i=-1) const;
         
        //@}

        /**@name SessionTimer headers
          */
        //@{

        ///
        const SipSessionExpires& getSessionExpires() const;
        ///
        const SipMinSE& getMinSE() const ;
        ///
        void setSessionExpires(const SipSessionExpires& sess);
        ///
        void setMinSE(const SipMinSE& minSe);

        //@}

	//Session methods.
	///
	void setSession(const SipSession& newSession);
	///
	void setSession(const Data& newSession);
	///
	const SipSession& getSession() const;
	
	///Check to see if message is a candidate for retransmission
	virtual bool toBeRetransmitted() const;

        // ------------------------------------------------------------
        // authentication stuff
        // ------------------------------------------------------------

        /**@name authentication methods
          */
        //@{

        /** add a basic-based Proxy-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
        **/
        void challengeBasicProxyAuthenticate(Data realm);

        /** add a digest-based Proxy-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
            @param nonce       nonce 
        **/
        void challengeDigestProxyAuthenticate(Data realm, Data nonce);


        /** add a basic-based Www-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
        **/
        void challengeBasicWwwAuthenticate(Data realm);

        /** add a digest-based Www-Authenticate: challenge header,
            given the realm, and a nonce.

            @param realm       The realm for the challenge
            @param nonce       nonce 
        **/
        void challengeDigestWwwAuthenticate(Data realm, Data nonce);

        //@}

    private:
        SipStatusLine statusLine;

        bool isStatusCodes(const Data& data);
        bool isprotocolSIP(const Data& header);  //only returns true for SIP
        void parseHeaders(const Data& sdata);
        void parseStartLine(const Data& line1);
        void sipdecode(const Data& fgdata);
        void setStatusDetails();
        void addsipWarning(SipWarning* sipwarning);

        StatusMsg(); // Not implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

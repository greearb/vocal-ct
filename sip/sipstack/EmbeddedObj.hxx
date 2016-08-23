#ifndef EMBEDDEDOBJ_HXX
#define EMBEDDEDOBJ_HXX

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

#include <string>
#include <vector>
#include <map>

#include "SipRoute.hxx"
#include "global.h"
#include "SipParserMode.hxx"
#include "Data.hxx"
#include "Sptr.hxx"
#include "SipHeader.hxx"
#include "SipHeaderList.hxx"
#include "SipRawHeaderContainer.hxx"
#include "SipContact.hxx"


namespace Vocal
{
    class EmbeddedObj : public BugCatcher
    {
	    typedef map<string, char*> EscMap;
	public:

	    ///

            // local_ip cannot be "" here, must be the local IP we are bound to locally
            // or 'hostaddress' if we are not specifically bound.
	    EmbeddedObj(const Data& raw, const string& local_ip);

	    ///
            // local_ip cannot be "" here, must be the local IP we are bound to locally
            // or 'hostaddress' if we are not specifically bound.
	    EmbeddedObj(const string& raw, const string& local_ip);

	    ///

	    EmbeddedObj(const EmbeddedObj&);

	    ///
	    const EmbeddedObj& operator =(const EmbeddedObj& EmbeddedObj);

	    ///

	    void decode(const Data& data, const string& local_ip);

	    ///
#if 0
	    const SipHeader& getHeader(SipHeaderType type, int index=0) const;
#endif
	    ///

	    Data getHeaderData() const { return _rawData;};

	    ///

	    Data getforwardEscapeheader() const {return _escFData;}
	    const SipRawHeaderContainer& getHeaderList() const { return _headerList; }

	    ///

	    const SipRoute& getRoute(const string& local_ip, int i = -1) const;

	    ///

	    const vector < Sptr<SipRoute> > getRouteList(const string& local_ip) const ;

            ///

	    const SipContact& getContact() const;  

	    ///

	    Data doForwardEscape(const string& src);

	    ///

	    Data doReverseEscape(const string& src);

	    ///

	    void setHeader(const Data& rawheader);     

	    ///

	    class reverseMap : public map<string, const char*> 
	    {
		public:
		    reverseMap();
	    } ;
  
	    ///
	     class forwardMap : public map<string, const char*> 
	    {
		public:
		    forwardMap();
	    };
	    
	private:
            ///
	    SipRawHeaderContainer _headerList;
	    ///
	    Data _rawData;
	    ///
	    Data _escFData;
	    static const reverseMap _escRMap;
	    static const forwardMap _escFMap;
	    bool fastDecode(Data mydata, const string& local_ip);
	    
    };
}

#endif

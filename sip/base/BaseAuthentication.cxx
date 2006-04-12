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


#include "BaseAuthentication.hxx"
#include "SipProxyAuthenticate.hxx"
#include "SipCommand.hxx"
#include <RandomHex.hxx>

using namespace Vocal;

void
addAuthorization(const StatusMsg& errorMsg,
		 SipCommand& cmdMsg,
                 Data username,
		 Data password,
		 bool useProxyAuthenticate)
{

    /// xxx this should be cleaned up
    Data authScheme;
    Data nonce;
    Data realm;
    Data alg;
    Data opaque;
    Data qop;

    if (!useProxyAuthenticate) {
	SipWwwAuthenticate proxyAuth = errorMsg.getWwwAuthenticate();
	
	authScheme = proxyAuth.getAuthScheme();
	nonce = proxyAuth.getTokenValue("nonce");
	realm = proxyAuth.getTokenValue("realm");
	alg = proxyAuth.getTokenValue("algorithm");
	opaque = proxyAuth.getTokenValue("opaque");
	qop = proxyAuth.getTokenValue("qop");
    }
    else {
	SipProxyAuthenticate proxyAuth = errorMsg.getProxyAuthenticate();
	
	authScheme = proxyAuth.getAuthScheme();
	nonce = proxyAuth.getTokenValue("nonce");
	realm = proxyAuth.getTokenValue("realm");
	alg = proxyAuth.getTokenValue("algorithm");
	opaque = proxyAuth.getTokenValue("opaque");
	qop = proxyAuth.getTokenValue("qop");
    }

    cpLog(LOG_ERR, " NOTE:  Auth Scheme is: %s  alg: %s  proxyAuth: %d",
          authScheme.logData(), alg.c_str(), useProxyAuthenticate);

    if ( authScheme == AUTH_DIGEST ) {
	Data method = cmdMsg.getRequestLine().getMethod();
    
	Sptr< BaseUrl > baseUrl = cmdMsg.getRequestLine().getUrl();
	assert( baseUrl != 0 );
    
	Data reqUri = baseUrl->encode();
    
	Data noncecount;
	Data cnonce;
    
	cpLog( LOG_DEBUG, "qop: %s", qop.logData() );
	if (qop.length()) {
	    //cnonce, and noncecount are SHOULD.
            // Seems they are MUST if qop exists. --Ben
	    noncecount = "00000001";
	    cnonce = RandomHex::get(16);
            LocalScopeAllocator lo;
	    unsigned int pos = string(qop.getData(lo)).find("auth");
	
	    if  (pos  != string::npos) {
		qop = "auth";
		//the client can also find if this has a auth-int, and
		//if it wants to support it, can set qop to auth-int.
	    }
	    else {
		qop = "";
	    }
	}
    
	cpLog( LOG_DEBUG, "nonce: %s", nonce.logData() );
	cpLog( LOG_DEBUG, "realm: %s", realm.logData() );
	cpLog( LOG_DEBUG, "username: %s", username.logData() );
	cpLog( LOG_DEBUG, "password: %s", password.logData() );
	cpLog( LOG_DEBUG, "method: %s", method.logData() );
	cpLog( LOG_DEBUG, "reqUri: %s", reqUri.logData() );
	cpLog( LOG_DEBUG, "qop: %s", qop.logData() );
	cpLog( LOG_DEBUG, "cnonce: %s", cnonce.logData() );
	cpLog( LOG_DEBUG, "alg: %s", alg.logData() );
	cpLog( LOG_DEBUG, "noncecount: %s", noncecount.logData() );
	cpLog( LOG_DEBUG, "opaque: %s", opaque.logData() );
    
	if(!useProxyAuthenticate) {
	    cmdMsg.setAuthDigest(nonce, username, password, method, 
                                 realm, reqUri, qop, cnonce, alg,
                                 noncecount, opaque);
	}
	else {
	    cmdMsg.setProxyAuthDigest(nonce, username, password, method, 
				      realm, reqUri, qop, cnonce, alg,
				      noncecount, opaque);
	}
    }
    else if (authScheme == AUTH_BASIC) {
	cmdMsg.setAuthBasic(username, password);
    }
    else {
        cpLog(LOG_ERR, "ERROR:  Un-handled authScheme: %s\n", authScheme.c_str());
    }
}


void
addProxyAuthorization(const StatusMsg& errorMsg,
		      SipCommand& cmdMsg,
                      Data username,
		      Data password)
{
    addAuthorization(errorMsg, cmdMsg, username, password, true);
}


void
addWwwAuthorization(const StatusMsg& errorMsg,
		    SipCommand& cmdMsg,
                    Data username,
		    Data password)
{
    addAuthorization(errorMsg, cmdMsg, username, password, false);
}

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
#include "BaseUrl.hxx"
#include "symbols.hxx"
#include "cpLog.h"
#include "Data.hxx"
#include "SipUrl.hxx"
#include "TelUrl.hxx"
#include "ImUrl.hxx"
#include "PresUrl.hxx"
#include "NullUrl.hxx"
#include "UnknownUrl.hxx"

using namespace Vocal;

BaseUrl::BaseUrl()
{
}
    
Sptr<BaseUrl>
BaseUrl::decode(const Data& data, const string& local_ip)
{
    Sptr <BaseUrl> newUrl;
    //parse the first line here.if this is sip, invoke SipUrl::decode,
    //else if tel, invoke TelUrl::decode.
    //return the created object.
    Data urlType;
    Data myData = data;
    
    int ret = myData.match(":", &urlType, false);
    if ( (ret == NOT_FOUND) || (ret == FIRST) )
    {
	cpLog(LOG_DEBUG, "parse failed: could not find ':' separating URL type from body");
        cpLog(LOG_DEBUG, "bad data in BaseUrl::decode: <%s>", data.logData());
        newUrl = new UnknownUrl(data);
    }
    else if ( isEqualNoCase(urlType, "sip") )
    {

	newUrl = new SipUrl(data, local_ip);
    }
    else if ( isEqualNoCase(urlType, "tel") )
    {
	newUrl = new TelUrl(data);
    }
    else if ( isEqualNoCase(urlType, "im") )
    {
	newUrl = new ImUrl(data);
    }
    else if ( isEqualNoCase(urlType, "pres") )
    {
	newUrl = new PresUrl(data);
    }
    else      
    {
	cpLog(LOG_DEBUG, "parse failed: scheme not SIP or TEL: %s", data.logData());
//	newUrl = new NullUrl;
        newUrl = new UnknownUrl(data);
    }
    return newUrl;
}
    

#if 0
bool
operator==(const BaseUrl& left, const BaseUrl& right)
{
    if(left.getType() == right.getType())
    {
	
    }
}
#endif

ostream& 
BaseUrl::dump(ostream& s) const
{
    s << encode();
    return s;
}


ostream&
Vocal::operator<<(ostream& s, const Sptr<BaseUrl>& url)
{
    if (url == 0)
    {
        s << "NULL";
        return s;
    }
    else
    {
        return url->dump(s);
    }
}


Sptr<BaseUrl> 
Vocal::duplicateUrl(const Sptr<BaseUrl> original, bool returnNullUrl)
{
    static Sptr<BaseUrl> null = new NullUrl;

    if(original != 0 && original->getType() != NULL_URL)
    {
        return original->duplicate();
    }
    else
    {
	if(returnNullUrl)
	{
	    return null;
	}
	else
	{
	    return 0;
	}
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

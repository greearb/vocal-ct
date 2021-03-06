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
#include "cpLog.h"
#include "UnknownUrl.hxx"
#include "NullUser.hxx"

    
using namespace Vocal;

UnknownUrl::UnknownUrl()
{
}


UnknownUrl::UnknownUrl(const Data& data)
    :myUrl(data)
{
}


UnknownUrl::UnknownUrl(const UnknownUrl& url)
    :myUrl(url.myUrl)
{
}
    
const UnknownUrl&
UnknownUrl::operator=(const UnknownUrl& url)
{
    if(&url != this)
    {
	myUrl = url.myUrl;
    }
    return *this;
}


bool
UnknownUrl::operator==(const UnknownUrl& url) const
{
    return myUrl == url.myUrl;
}


Data 
UnknownUrl::encode() const
{
    return myUrl;
}


Sptr<BaseUser> 
UnknownUrl::getUser() const
{
    return new NullUser;
}


Data 
UnknownUrl::getUserValue() const
{
    return "";
}

Data 
UnknownUrl::getUniqueKey() const
{
    return myUrl;
}

void 
UnknownUrl::setUser(Sptr <BaseUser> )
{
    // does nothing
}

bool
UnknownUrl::setUserValue(const Data&, const Data& dummy)
{
    // does nothing
    return true;
}

UrlType
UnknownUrl::getType() const
{
    return UNKNOWN_URL;
}

Sptr<BaseUrl> 
UnknownUrl::duplicate() const
{
    return new UnknownUrl;
}

bool 
UnknownUrl::areEqual(Sptr<BaseUrl> other) const
{

    return 
	(other != 0) &&  
	(other->getType() == UNKNOWN_URL) && 
	(myUrl == other->getUniqueKey()) ;
}

bool
UnknownUrl::isLessThan(Sptr<BaseUrl> other) const
{
    if(other == 0)
    {
	return false;
    }

    // the Null URL is less than anything other than itself
    if (other->getType() == NULL_URL)
    {
	return false;
    }
    else
    {
	return myUrl < other->getUniqueKey();
    }
}

bool 
UnknownUrl::areEqual(const BaseUrl& other) const
{
    return ((other.getType() == UNKNOWN_URL) &&
	    (myUrl == other.getUniqueKey()));
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

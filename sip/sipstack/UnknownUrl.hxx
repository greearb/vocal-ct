#ifndef UNKNOWNURL_HXX_
#define UNKNOWNURL_HXX_

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

static const char* const UnknownUrl_hxx_Version =
    "$Id: UnknownUrl.hxx,v 1.2 2004/05/05 06:37:33 greear Exp $";

#include "BaseUrl.hxx"
#include "BaseUser.hxx"

namespace Vocal
{
    
/// data container for URLs of unknown type 
class UnknownUrl : public BaseUrl
{
    public:
	/// Create one with default values
	UnknownUrl();

	/// Create one with default values
	explicit UnknownUrl(const Data& data);


	/// Create one with default values
	UnknownUrl(const UnknownUrl& url);
    
	///
	~UnknownUrl();

        // Help up-cast safely.
        virtual bool isSipUrl() const { return false; }

        ///
        const UnknownUrl& operator=(const UnknownUrl& url);

        ///
        bool operator==(const UnknownUrl& url) const;

	///
	virtual Data encode() const;
	///
	virtual Sptr<BaseUser> getUser() const;
	///
	virtual Data getUserValue() const;
	///
	virtual Data getUniqueKey() const;
	///
	virtual void setUser(Sptr <BaseUser> );
	///
	virtual bool setUserValue(const Data&, const Data& dummy = "");
	///
	virtual UrlType getType() const;
	///
	virtual Sptr<BaseUrl> duplicate() const;
	///
	virtual bool areEqual(Sptr<BaseUrl>) const;
	///
	virtual bool isLessThan(Sptr<BaseUrl>) const;
	/// comparison operator is used by others
	virtual bool areEqual(const BaseUrl& other) const;
    private:
        Data myUrl;
};

 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

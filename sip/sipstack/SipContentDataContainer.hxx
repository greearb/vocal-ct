#ifndef SipContentDataContainer_HXX
#define SipContentDataContainer_HXX

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


#include "Data.hxx"
#include "SipContentType.hxx"
#include "SipContentLength.hxx"
#include "SipContentDisposition.hxx"
#include "SipContentData.hxx"
#include <vector>

namespace Vocal
{

/// data container for ContentLength header
class SipContentDataContainer
{
    public:
        /// Create one with default values
        SipContentDataContainer(const string& local_ip);

        ///
        void encode(Data*) const;
        ///
        void decode(const Data&, Sptr<SipContentLength>, 
		    Sptr<SipContentType>, 
		    Sptr<SipContentDisposition> );
        ///
        virtual ~SipContentDataContainer();
        ///
        SipContentDataContainer( const SipContentDataContainer& rhs );
        ///
        SipContentDataContainer& operator=( const SipContentDataContainer& rhs );
        ///
        void setContentLength(Sptr <SipContentLength>);
        ///
        Sptr <SipContentLength>  getContentLength();
        /// 
        void setContentType(Sptr <SipContentType>);
        ///
        Sptr <SipContentType> getContentType();
        ///
        void setContentDisposition(Sptr <SipContentDisposition>);
        ///
        Sptr <SipContentDisposition> getContentDisposition();
        ///
        void setContentData(Sptr<SipContentData> contData, int index=-1);
        ///
        Sptr <SipContentData> getContentData(int index=-1);
        ///
        void removeContentData(int index=-1);
        ///
        int getNumContentData();
        ///
        void setNumContentData(int index);
        ///
        bool operator==(const SipContentDataContainer& );
        /// force all headers to be parsed
        void forceParse();

        const string& getLocalIp() const { return local_ip; }
    
    private:

        void copyRhs2This( const SipContentDataContainer& rhs );

        Sptr <SipContentType>  contentType;
        Sptr <SipContentLength> contentLength;
        Sptr <SipContentDisposition> contentDisposition;
    
    
        ///
        vector< Sptr <SipContentData> > mimeDataList;
   
        bool parse() ;
    
        bool parseMultiMime(Data boundary);
    
        bool parseSingleMime(Data singleMimeData, Sptr<SipContentLength> len, 
			     Sptr<SipContentType> type, 
			     Sptr<SipContentDisposition> disp);

        Data mimeData;
        bool isParsed;
        Data actualBoundary;
        string local_ip;

        SipContentDataContainer(); //Not implemented
};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

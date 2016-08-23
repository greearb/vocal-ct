#if !defined(VOCAL_PROTOCOL_FACTORY_HXX)
#define VOCAL_PROTOCOL_FACTORY_HXX

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


#include "Singleton.hxx"
#include "Sptr.hxx"
#include "ProtocolCreator.hxx"
#include <string>
#include <map>


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to making network transport layer 
 *  connections.<br><br>
 */
namespace Transport 
{


class IPAddress;


using Vocal::Singleton;
using std::map;
using std::string;


/** 
 *
 */
class ProtocolFactory : public Singleton<ProtocolFactory>
{
    public:


    	/** Constructor.
	 */
    	ProtocolFactory();

        /**
         */
        void    addCreator( Sptr<ProtocolCreator> );
        
        
        /**
         */
        Sptr<Protocol>      createProtocol(
            const string        &   ianaName,
            const string        &   proto
        ) const;
        
        /**
         */
        Sptr<Protocol>      createProtocol(
            const string        &   ianaName,
            const string        &   proto,
            const IPAddress     &   ipAddress
        ) const;
        

    private:
        
        struct ProtocolKey
        {
            ProtocolKey(ProtocolCreator & creator)
                :   ianaName(creator.ianaName()),
                    proto(creator.proto())
            {}

            ProtocolKey(const string & i, const string & p)
                :   ianaName(i),
                    proto(p)
            {}
                
            const string  &     ianaName;
            const string  &     proto;
            
            bool operator==(const ProtocolKey & src) const
            {
                return ( ianaName == src.ianaName && proto == src.proto );
            }

            bool operator<(const ProtocolKey & src) const
            {
                return ( ianaName < src.ianaName ? true : proto < src.proto );
            }
        };        

        typedef  map<ProtocolKey, Sptr<ProtocolCreator> >    CreatorMap;
        
        CreatorMap      myCreators;
};


} // namespace Transport
} // namespace Vocal


#endif // !defined(VOCAL_PROTOCOL_FACTORY_HXX)

#ifndef SDP2CONNECTION_HXX_
#define SDP2CONNECTION_HXX_

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

#include <vector>
#include <strstream>
#include <cassert>

#include "Sdp2Externs.hxx"
#include "cpLog.h"
#include "Data.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


///
enum TransportType
{
    TransportTypeUnknown,
    TransportTypeRTP,
    TransportTypeUDP
};

///
enum AddressType
{
    AddressTypeUnknown,
    AddressTypeIPV4,
    AddressTypeIPV6
};

///
enum NetworkType
{
    NetworkTypeUnknown,
    NetworkTypeInternet
};


const int MulticastTtlValMin = 0;
const int MulticastTtlValMax = 255;
const int MaxAddressLen = 128;

/** 
    A Multicast address and the parameter that goes with it in the
    SdpConnection.

  */
class SdpMulticast
{
    public:
        ///
        SdpMulticast();
        ///
        SdpMulticast(const char* addr, int ttlval = 0, int numAddr = 1)
        {

            setAddress(addr);
            setTtl(ttlval);
            setnumAddr(numAddr);
        }
        ///
        void setTtl(int ttlval)
        {
            if ( (ttlval < MulticastTtlValMin) ||
                    (ttlval > MulticastTtlValMax) )
                cpLog(LOG_ERR, "ttl out of range");
            else
                ttl = ttlval;
        }
        ///
        int getTtl()
        {
            return ttl;
        }
        ///
        void setnumAddr(int numAddr)
        {
            numAddress = numAddr;
        }
        ///
        int getnumAddr()
        {
            return numAddress;
        }
        ///
        Data getAddress()
        {
            return address;
        }
        ///
        void setAddress(Data addr)
        {
            address = addr;
        }

        ///
        void encode (ostrstream& s);

    private:
        Data address;
        int ttl;
        int numAddress;
};


/** 
    SDP Connection details.
    For a unicast address:<p>
    <pre>
    c=&lt;network type> &lt;address type> &lt;connection address>
    </pre>
    <p>
    For a multicast address:<p>
    <pre>
    c=IN IP4 &lt;base multicast address>/&lt;ttl>/&lt;number of addresses>
    </pre>
    <p>
    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession
*/
class SdpConnection
{
    public:
        ///
        SdpConnection();
        ///
        SdpConnection(Data str);
        ///
        SdpConnection( const SdpConnection& connection );
        ///
        void setUnicast(Data addr)
        {
            address = addr;
            //assert(address != "127.0.0.1");
        }
        ///
        void setMulticast(const char* addr, int ttl, int numAddr = 1)
        {
            if (!multicast)
            {
                multicast = new SdpMulticast(addr, ttl, numAddr);
            }
            else
            {
                multicast->setAddress(addr);
                multicast->setTtl(ttl);
                multicast->setnumAddr(numAddr);
            }
        }
        ///
        SdpMulticast* getMulticast() const
        {
            return multicast;
        }
        ///
        Data getUnicast() const
        {
            return address;
        }
        ///
        void setNetworkType(NetworkType networkType)
        {
            networktype = networkType;
        }
        ///
        NetworkType getNetworkType() const
        {
            return networktype;
        }
        ///
        void setAddressType(AddressType addressType)
        {
            addresstype = addressType;
        }
        ///
        AddressType getAddressType() const
        {
            return addresstype;
        }
        ///
        SdpConnection& operator=(const SdpConnection& connection);
        ///
        ~SdpConnection()
        {
            if (multicast)
                delete multicast;
        }

        ///
        void encode (ostrstream& s);

        Data encode() const;

        /**Implements INVITE(HOLD)
         * set  <connection address> to "0.0.0.0"
         */
        void setHold();      
        bool isHold();

    private:
        ///
        Data networkTypeString() const;
        ///
        Data addressTypeString() const;

        NetworkType networktype;

        AddressType addresstype;

        /// Unicast address
        Data address;

        SdpMulticast* multicast;
};


}


}

#endif

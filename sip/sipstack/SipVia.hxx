#ifndef SipVia_HXX
#define SipVia_HXX

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


static const char* const SipVia_hxx_Version =
    "$Id: SipVia.hxx,v 1.2 2004/06/16 06:51:26 greear Exp $";

#include "global.h"
#include "Data.hxx" 
#include "Data.hxx"
#include "BaseUrl.hxx"
#include "SipHeader.hxx"

namespace Vocal
{
    
enum SipViaErrorType
{
   DECODE_VIA_FAILED,
   SCAN_VIA_FAILED,
   MANDATORY_VIA_ITEM_MISSING,
   COMMENT_ERR,
   PARSE_VIAPRAM_ERR
  
};

/// Exception handling class SipViaParserException
class SipViaParserException :public VException
{
   public:
      ///
      SipViaParserException( const string& msg,
			     const string& file,
			     const int line,
			     const int error = 0 ); 
      ///
      SipViaParserException( const string& msg, 
			     const string& file,
			     const int line,
			     SipViaErrorType i)
	 : VException( msg, file, line, static_cast<int>(i))
      {
	 value = i;
      }
      ///
      const SipViaErrorType& getError() const{return value;}
      ///
      string getName() const ;
   private:
      SipViaErrorType value;

}; 


/// data container for Via header 
class SipVia : public SipHeader
{
    public:
        ///
        SipVia( const Data& data, const string& local_ip );// throw(SipViaParserException &);
        ///
        SipVia( const SipVia& src );
        /// 
        const SipVia& operator =( const SipVia& src );
        ///
        bool operator<( const SipVia& src ) const;
        ///
        bool operator>( const SipVia& src ) const;
        ///
        bool operator ==( const SipVia& src ) const;
        ///
        Data encode() const;
        ///
        void setprotoName(const Data& protoName);
        ///
        Data getprotoName() const;
        ///
        void setprotoVersion(const Data& protoVer);
        ///
        Data getprotoVersion() const;
        ///
        void setTransport(const Data& trans);
        ///
        Data getTransport() const;
        ///
        void setHost(const Data& data);
        ///
        const Data& getHost() const;
        ///
        void setPort(int data);
        ///
        void setPortData(const Data& data);
        ///
        Data getPortData() const;
        ///
        int getPort() const;
        ///
        Data getExtensionValue() const ;
        ///
        Data getExtensionParm() const;
        ///
        void setExtensionParm(const Data & newextparm);
        ///
        void setExtensionValue(const Data& newextvalue);

        ///
        void sethidden();
        ///
        void setTtl(const Data& newttl);
        ///
        Data getTtl() const ;
        ///
        void setMaddr(const Data& newmaddr);
        ///
        Data getMaddr() const;
        ///
        Data getReceivedhost() const;
        ///
        void setReceivedhost(const Data& recvdhost);
        ///
        Data getReceivedport() const;
        ///
        void setReceivedport(const Data& recvdport);
        ///
        void setBranch(const Data& newtoken);
        ///
        Data getBranch() const;
        ///
        void setHidden(const Data& hidden);
        ///
        bool getHidden() const;
        ///
        void setComment(const Data& newcomment);
        ///
        Data getComment() const;
        /// set the rport
        void setRPort(const Data& port);
        /// set the rport
        void setRPort(const bool flag);
        /// get the rport
        Data getRPort() const;
        /// true if the via is received
        bool isViaReceived() const;      
	/// method for copying sip headers of any type without knowing which type
	Sptr<SipHeader> duplicate() const;
	/// compare two headers of (possibly) the same class
	virtual bool compareSipHeader(SipHeader* msg) const;
    private:
        //Data sipVia; //this is returned in encode.  
    
        Data host;
        Data port;
        Data protocolName;
        Data protocolVersion;
        Data transport;

        //Via specific details.
        Data receivedHost;
        Data receivedPort;

        Data branchToken;
        Data comment;
        Data ttl;
        Data extparm;
        Data extvalue;
        Data maddr;
        bool viaBranch;
        bool viaComment;
        bool viaReceived;
        bool viaHidden;
        bool viaTtl;
        bool viaMaddr;
        bool rport;
        Data rportValue;
        ///
        void parseRestVia(const Data& restdata);
        ///
        void parseViaParms( const Data& data);
        ///
        void parseViaParm( const Data& data, const Data& value);
        ///
        void parseViaParmHidden( const Data& data, const Data& value);
        ///
        void parseBranch(const Data& branchdata);
        ///
        void parseRecevied(const Data& recevieddata);
        ///
        void parseExtension(const Data& extensiondata, const Data& extensionvalue);
        ///
        void parseHidden(const Data& hiddendata);
        ///
        void scanSipVia(const Data &tmpdata);
        ///
        void parseComment(const Data& cdata);
        ///
        void parseTransport(const Data& tdata);
        ///
        void parseHost(const Data& data);
        void decode(const Data& data);
        Data formOtherparms() const; 
        SipVia(); // Not Implemented
};

 
} // namespace Vocal

#endif

#ifndef SIPCOMMAND_HXX_
#define SIPCOMMAND_HXX_

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

static const char* const SipCommand_hxx_Version
= "$Id: SipCommand.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include <vector>
#include "SipMsg.hxx"
#include "SipRequestLine.hxx"

namespace Vocal
{
class SipAlso;
class SipAuthorization;
class SipContentDisposition;
class SipHide;
class SipMaxForwards;
class SipOrganization;
class SipOspAuthorization;
class SipPriority;
class SipProxyAuthorization;
class SipProxyRequire;
class SipRequire;
class SipResponseKey;
class SipRoute;
class SipSubject;
class StatusMsg;

//All command classes throw exceptions of this type.
enum SipCommandErrorType
{
    COMMAND_CSEQ_OUT_OF_RANGE,
    COMMAND_VIA_NOT_PRESENT,
    COMMAND_PARSE_FAILED,
    COMMAND_INDEX_OUT_OF_BOUND
};

/// exception class
class SipCommandException
{
    public:
        SipCommandException(SipCommandErrorType i) {
            value = i;
        }
        SipCommandErrorType value;
};

///Base class for all SIP requests
class SipCommand : public SipMsg
{
    public:
        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        SipCommand(const string& _local_ip, const char* class_name);

        ///
        SipCommand& operator =(const SipCommand&);

        // Help with up-casting.
        virtual bool isSipCommand() const { return true; }
        virtual bool isStatusMsg() const { return false; }

        ///
        bool operator ==(const SipCommand& src) const;
        ///
        SipCommand(const SipCommand&); 

        ///
        SipCommand(const SipCommand&, const SipVia&, const SipCSeq& ); 

        ///
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
        SipCommand(const StatusMsg&, const string& _local_ip,
                   const char* class_name);

        /** decode a SipCommand.
            @return            true if there is an error while parsing, false
            if there is no error

            @param inputData   the message to be parsed
        */
        bool decode(const Data& inputData);
        
        /// Set's the branch number on top (most recent) via
        void setViaBranch( int i );

        /// Get's the branch number on top (most recent) via
        int getViaBranch();

        /// Returns a COPY of the request line
        const SipRequestLine& getRequestLine() const;

        /// Returns a reference to the request line.
        SipRequestLine& getMutableRequestLine() ;

        /// Set the RequestLine header
        void setRequestLine( const SipRequestLine& );
        
        ///
        const SipAuthorization& getAuthorization() const;

        /// Set the Authorization header
        void setAuthorization( const SipAuthorization& auth);

        /// Set the Authorization header
        void setAuthorization( const Data& textData);
       
        ///
        const SipHide& getHide() const;

        /// Set the Hide header
        void setHide( const SipHide& );
        ///
        void setHide( const Data& textData);

        ///
        const SipAlso& getAlso() const;
        ///
        void setAlso(const SipAlso& );

        ///
        const SipContentDisposition& getContentDisposition() const;
        ///
        void setContentDisposition(const SipContentDisposition&);
        ///
        void setContentDisposition(const Data&);

        ///
        const SipMaxForwards& getMaxForwards() const;
        
        /// Set the MaxForwards header
        void setMaxForwards( const SipMaxForwards& );

        /// Set the MaxForwards header
        void setMaxForwards( const Data& textData);
        
        ///returns false , if maxforwards becomes < 0.
        bool checkMaxForwards();

        /**returns false , if maxforwards becomes < 0. also adds MaxForwards
          *header if did not exist and sets value = 70. see rfc 
          */
        bool decrementMaxForwards();
        
        ///
        const SipOrganization& getOrganization() const;

        /// Set the Organization header
        void setOrganization( const SipOrganization& );

        /// Set the Organization header
        void setOrganization( const Data& textData);

        ///
        const SipPriority& getPriority() const;

        /// Set the Priority header
        void setPriority( const SipPriority& );

        /// Set the Priority header
        void setPriority( const Data& textData);

        // ----------------- ProxyAuthorization Header Methods --------

        /// get the proxyauthorization header
        const SipProxyAuthorization& getProxyAuthorization() const;
        
        /// Set the ProxyAuthorization header
        void setProxyAuthorization( const SipProxyAuthorization& );

        /// Set the ProxyAuthorization header
        void setProxyAuthorization( const Data& textData);

        /// get the number of Require items
        int getNumProxyRequire() const;

        /// Get the i'th Require item. If i is -1, it gets the last one
        const SipProxyRequire& getProxyRequire( int i = -1) const;
       
        /** set or add another Require itme, if the index is -1, it is
          * appended to the current list  
          */
        void setProxyRequire(SipProxyRequire item, int index = -1);
      
        /** set or add another Require itme, if the index is -1, it is
          * appended to the current list  
          */
        void setProxyRequire(const Data&item, int index = -1);

        /** Set number of Require items.
            If i is less than current number then the extras are deleted. */
        void setNumProxyRequire(int i);

        ///
        void copyProxyRequireList(const SipCommand& src);

        ///
        void flushProxyrequireList();

        ///
        const vector < SipProxyRequire* >& getProxyrequireList() const;

        ///
        void addsipProxyrequire(SipProxyRequire* sipProxyrequire);

        // ----------------- ResponseKey Header Methods ------------------

        /// get the responsekey
        const SipResponseKey& getResponseKey() const;

        /// Set the ResponseKey header
        void setResponseKey( const SipResponseKey& );

        // ----------------- Require Header Methods  ----------------
        /// get the number of requires
        int getNumRequire() const;

        /// Get the i'th Require item. If i is -1, it gets the last one
        const SipRequire& getRequire( int i = -1) const;

        /** set or add another Require itme, if the index is -1, it is appended
            to the current list */
        void setRequire(const SipRequire& item, int index = -1);

        /** set or add another Require itme, if the index is -1, it is appended
            to the current list */
        void setRequire(const Data&item, int index = -1);

        /** Set number of Require items.
            If i is less than current number then the extras are deleted. */
        void setNumRequire(int i);
        
        ///
        void copyRequireList(const SipCommand& src);
        
        ///
        void flushrequireList();
        
        ///
        const vector < SipRequire* >& getrequireList() const;
        
        ///
        void addsiprequire(SipRequire* siprequire);
        
        /// ----------------- Route Header Methods  -----------------
        int getNumRoute() const;

        /// Get the i'th Route item. If i is -1, it gets the last one 
        const SipRoute& getRoute( int i = -1) const;

        /** set or add another Route itme, if the index is -1, it is
          * appended to the current list  
          */
        void setRoute(const SipRoute& item, int index = -1);

        /** set or add another Route itme, if the index is -1, it is
          * appended to the current list  
          */
        void setRoute(const Data&item, int index = -1);

        /** Set number of Route items.If i is less than current number then
          * the extras are deleted.  
          */
        void setNumRoute(int i);

        /// highest in the command, next element to be sent to
        void routePushFront(const SipRoute& item);

        /// lowest in the command, last element to be sent to
        void routePushBack(const SipRoute& item);
        void routePopFront();
        void routePopBack();
        const SipRoute& routeFront() const;
        const SipRoute& routeBack() const;
        bool routeEmpty() const;
        
        ///
        void copyRouteList(const SipCommand& src);

        ///
        void flushrouteList();
    
        ///
        const vector < Sptr<SipRoute> > getRouteList() const;

        ///
        void addsiproute(SipRoute* siproute);

        ///
        void setRouteList(const vector < SipRoute* > sipRouteList);
        
        /// replacement for previous interface vector <SipRoute*> 
        void setRouteList(const vector < Sptr<SipRoute > > list);
        
        ///
        void removeRoute(int index = -1);

        /// ----------------- Subject Header Methods ------------------
        /// get the subject header
        const SipSubject& getSubject() const;
        
        /// Set the Subject header
        void setSubject( const SipSubject& );

        /// Set the Subject header
        void setSubject( const Data& textData);

        /// Basic auth details
        void setAuthBasic(const Data& data, const Data& pwd = "");
        
        ///
        bool checkAuthBasic(const Data& data, const Data& pwd = "");

        ///
        void setProxyAuth(const Data& data, const Data& pwd = "");
        
        ///
        bool checkProxyAuth(const Data& data, const Data& pwd = "");

        /// Digest Proxy-Authentication Details
        void setProxyAuthDigest(const Data& nonce, const Data& user,
                                const Data& pwd, const Data& method,
                                const Data& realm, const Data& requestURI,
                                const Data& qop, const Data& cnonce,
                                const Data& alg, const Data& noncecount,
                                const Data& opaque);

        /// Digest Authentication Details
        void setAuthDigest(const Data& nonce, const Data& user,
                           const Data& pwd, const Data& method,
                           const Data& realm, const Data& requestURI,
                           const Data& qop, const Data& cnonce,
                           const Data& alg, const Data& noncecount,
                           const Data& opaque);   

        /** verify the digest returned by the user is correct.  Here,
         *  pass the following items to be verified.  The other
         *  headers should not be needed (e.g. requestURI, algorithm)
         *  and can be gotten from the message
         *
         * */
        bool checkAuthDigest(const Data& nonce, 
                             const Data& user, 
                             const Data& pwd,
                             const Data& requestURI,
                             const Data& realm, 
                             const bool authReg = false);
        
        ///
        void setSipOspAuthorization(const SipOspAuthorization &);
    
        ///
        void setSipOspAuthorization( const Data& data);
         
        ///
        const SipOspAuthorization& getSipOspAuthorization() const;

        /// 
        virtual ~SipCommand();
    
        /** Form cryptographically unique branch, should be combined with
          * results of computeBranch to satisfy draft bis09. 
          */
        static Data computeUniqueBranch();

        /** return a branch to be used by a proxy for via (for loop detection)
           As per draft bis09, this computes a branch using computeUniqueBranch
           combined with a cryptographically random suffix
           "z9hG4bK" + computeUniqueBranch() + "." + cryptokey
        */
        Data computeProxyBranch() const;

        /** This version does not add a cryptographically random suffix but is
          * otherwise the same as computeProxyBranch
          */
        Data computeStatelessProxyBranch() const;
        
        /** detect loops in this command - only if branch was computed using
            computeProxyBranch
        */
        bool detectLoop() const;

        ///form MD5 hash for the branch here.
        Data computeBranch(Data hashBranch = "") const;
 
        
        /// alternate form which uses local data as well
        Data computeBranch2(int system_port) const;
            
        /// Moved from public to private
        Data encodeProxyRequireList() const;
        
        ///
        Data encodeRequireList() const;
        
        ///
        Data encodeRouteList() const;
          
        bool compareProxyRequireList(const SipCommand& src) const;

        bool compareRouteList(const SipCommand& src) const;

        bool compareRequireList(const SipCommand& src) const;

        ///
        virtual Data encode() const;

        ///Check to see if message is a candidate for retransmission
        virtual bool toBeRetransmitted() const;

        /** return the next hop - based on the message's request-uri 
          * and/or route-set  see 16.6 section 6 and section 7. 
          * If a client wants to send a SipCommand with a req-uri of tel:xyz,
          * they must load a route into the ROUTE header to tell the stack where
          * to send the message. 
         */
        Sptr<SipUrl> postProcessRouteAndGetNextHop();
        
    protected:
        SipRequestLine myRequestLine;

        bool isMethod(const Data& data);
        bool parseStartLine(const Data& line1);
        void sipdecode(const Data& fgdata);

    private:
        // Not implemented.
        SipCommand();
};
    
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

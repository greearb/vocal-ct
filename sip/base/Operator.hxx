#ifndef OPERATOR_HXX
#define OPERATOR_HXX


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


static const char* const Operator_hxx_Version = 
    "$Id: Operator.hxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "Sptr.hxx"
#include "SipProxyEvent.hxx"
#include "State.hxx"
#include "SipUrl.hxx"

namespace Vocal
{

/* We have circular dependancies, so forward declare.
 */
class State;
class SipUrl;
class SipRecordRoute;
class SipVia;


/**Object Operator
<pre>
<br> Usage of this Class </br>

    Operator is a base class.  The Operator::process() is pure virtual.
    Event handler is implemented by the derived class of the operator

</pre>
*/
class Operator: public BugCatcher
{
    public:


        /** Default constructor.
         */
        Operator();


        /** Virtual destructor
         */
        virtual ~Operator();


        /** The extender of this class must provide the name of this
         *  class via this abstract method.
         */
        virtual const char* const name() const = 0;


        /** Process the incoming event. 
         */
        virtual const Sptr < State > process(const Sptr < SipProxyEvent > event)
	{ 
	    return ( 0 );
	}
        
        
        /** Logging helper method
         */
        virtual void doCpLog() const
        {
            cpLog( LOG_DEBUG, "Operator: %s", name() );
        }


        /** Add the local host address, port and version 2.0 to the 
         *  given via list. 
         *
         * my_local_ip should be either what the user configured, or
         * default local hostname.
         *
         *  Don't know that this is the correct place for this method.
         */
        void OpAddVia(SipVia& sipvia, const string& my_local_ip, int port = 5060) const;


        /** Takes an empty recordRoute, an existing sipUrl, and optionally
         *  a port. It sets the host and port of the sipUrl to that of the
         *  caller and sets the recordRoute's url to the new sipUrl.
         *
         * my_local_ip should be either what the user configured, or
         * default local hostname.
         *
         *  Don't know that this is the correct place for this method.
         */
        void OpAddRecordRoute( SipRecordRoute       &   recordRoute,
                               const Vocal::SipUrl  &   recordRouteUrl,
                               const string& my_local_ip,
                               const int            port = 5060) const;


    private:


        /** Suppress copying
         */
        Operator(const Operator& rhs);


        /** Suppress copying
         */
        Operator& operator=(const Operator& rhs);
};
 
}

#endif // OPERATOR_HXX

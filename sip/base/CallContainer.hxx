#ifndef CALL_CONTAINER_HXX
#define CALL_CONTAINER_HXX

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

static const char* const CallContainer_hxx_Version =
    "$Id: CallContainer.hxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "CallInfo.hxx"

namespace Vocal
{

/** Object CallContainer

<pre>
<br> Usage of this Class </br>

    CallContainer is a base class mainly used by the UserAgent and
    feature servers for CallInfo objects.

    Sptr < CallContainer >  myCallContainer = new CallContainer ;
    Sptr < Builder >  myBuilder;

    myBuilder->setCallContainer(myCallContainer);

    Check HeartLessProxy.cxx for more details on the usage.
</pre>
*/
class CallContainer: public BugCatcher
{
    public:


        /** Default constructor
         */
        CallContainer();


        /** Destructor method
         */
        virtual ~CallContainer();


        /** checks for the passed call leg in the CallContainer.
	 *  It creates a new CallInfo object if the call leg is not found.
	 *  @param newOrExistingCall pointer to the call leg to search for.
	 *  @return Sptr< CallInfo > pointer to the CallInfo object
	 */ 
        virtual Sptr < CallInfo > 
        getCall(const Sptr < SipCallLeg > newOrExistingCall);

       virtual Sptr < CallInfo >
        getCall(const Sptr < SipCallLeg > neworExistingCall, 
		Sptr < InviteMsg > invite,
                Sptr < SipTransceiver > sipStack);


        /** findCall checks if the call leg is present in the CallContainer.
	 *  @param newOrExistingCall pointer to the call leg to search for.
	 *  @return Sptr< CallInfo > pointer to the CallInfo object if found, otherwise 0
	 */
        virtual Sptr < CallInfo > 
        findCall(const Sptr < SipCallLeg > newOrExistingCall);


        /** addCall adds a CallInfo object to the CallContainer
	 *  @param newOrExistingCall pointer to the call leg Key
	 *  @param theCallInfo pointer to the CallInfo object
	 *  @return void
	 */
        virtual Sptr < CallInfo > 
        addCall(const Sptr < SipCallLeg > newOrExistingCall,
                const Sptr < CallInfo > theCallInfo);


        /** remove call removes a CallInfo object from the CallContainer
	 *  @param existingCall pointer to the call leg Key
	 *  @return void
	 */
        virtual void removeCall(const Sptr < SipCallLeg > existingCall);

        /** checkCalls traverses the hash map to retrieve timestamps of CallInfo objects
         *
         */
        virtual void checkCalls();
        
        /** cancelCall removes a CallInfo object from the CallContainer
         *  and tears down the call by sending "Cancel" to the destination
         *  party and "480 Temporarily Moved" to the originator.
         */

        virtual void cancelCall(const Sptr < CallInfo > callInfo);

    protected:


        /** typedef map of CallInfo pointers, using a SipCallLeg key
         */
        typedef map < Sptr<SipCallLeg>, Sptr < CallInfo > > Table;


        /** Table iterator 
         */
        typedef Table::iterator TableIter;


    private:
    
        /** CallContainer map object
         */
        Table myCallInfo;
        


        /** Suppress copying
         */
        CallContainer(const CallContainer &);


        /** Suppress copying
         */
        const CallContainer & operator=(const CallContainer &);
};
 
}

#endif // CALL_CONTAINER_HXX

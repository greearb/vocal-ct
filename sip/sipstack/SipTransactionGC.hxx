#ifndef _Sip_Transaction_GC__hxx
#define _Sip_Transaction_GC__hxx

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

static const char* const SipTransactionGC_hxx_version =
    "$Id: SipTransactionGC.hxx,v 1.3 2004/05/27 04:32:18 greear Exp $";


#include "SipTransactionLevels.hxx"
#include "SipTransactionList.hxx"
#include "SipTransHashTable.hxx"

namespace Vocal
{
    
///////////////////////////////////
#define MESSAGE_CLEANUP_DELAY    (SipTransactionGC::msgCleanupDelay)
#define INVITE_CLEANUP_DELAY     (SipTransactionGC::invCleanupDelay)
#define ORPHAN_CLEANUP_DELAY     (SipTransactionGC::orphanCleanupDelay)
///////////////////////////////////

class SipTransDebugger;

class SipTransactionGC
{
    public:
	static SipTransactionGC* instance();

	static int msgCleanupDelay;
	static int invCleanupDelay;
	static int orphanCleanupDelay;

    private:
	friend class SipTransDebugger;
	SipTransactionGC();
	virtual ~SipTransactionGC();
	SipTransactionGC(const SipTransactionGC&);
	SipTransactionGC& operator= (const SipTransactionGC&);
	bool operator==(const SipTransactionGC&);

    protected:
	void trashSMC(Sptr<SipMsgContainer> item);
	bool doneFlag;

    public:
	void collect(Sptr<SipMsgContainer> item, int delay=0);

    private:
	struct GCType;
	static SipTransactionGC *myInstance;

	SipTransactionList<GCType*> bins;
	typedef SipTransactionList<GCType*>::SipTransListNode BinsNodeType;

	struct GCType {
		const int myDelay;
		int currDelay;
		int currBin;
		SipTransactionList<SipMsgContainer*>  *bin;
		GCType(int delay): myDelay(delay), currDelay(0), currBin(0),
				   bin(0) {
		    assert(delay>=0);
		    bin = new SipTransactionList<SipMsgContainer*>[delay+2];
		}
	    private:
		GCType();
	};
};    

 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

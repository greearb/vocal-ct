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

static const char* const SdpBandwidth_cxx_Version =
    "$Id: Sdp2Bandwidth.cxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "global.h"
#include "Data.hxx"
#include "support.hxx"
#include <cassert>
#include <list>

#include "Sdp2Exceptions.hxx"
#include "Sdp2Bandwidth.hxx"


using Vocal::SDP::SdpBandwidth;


SdpBandwidth::SdpBandwidth (Data& s)
{
    //This is of the form:
    //<modifier>:<bandwidth-value>
    list<Data> bandwidthList;

	    bool finished = false;

	    while (!finished)
	    {
		Data x = s.parse(":", &finished);
		if(finished)
		{
		    x = s;
		}
		bandwidthList.push_back(x);
	    }


    if (bandwidthList.size() < 2)
    {
        cpLog(LOG_ERR, "SdpBandwidth: Not enough parameters");
        throw SdpExceptions(PARAM_NUMERR);
    }
    else
    {
	Data x = *bandwidthList.begin();
	Data y = *(++bandwidthList.begin());

        if (x == SdpBandwidthModifierAS)
        {
            modifier = BandwidthModifierTypeAS;
        }
        else if (x == SdpBandwidthModifierCT)
        {
            modifier = BandwidthModifierTypeCT;
        }
        else if (x == SdpBandwidthModifierRR)
        {
            modifier = BandwidthModifierTypeRR;
        }
        else
        {
            cpLog(LOG_ERR, "SdpBandwidth: Undefined Modifier");
            throw SdpExceptions(UNDEF_MODIFIER);
        }

	LocalScopeAllocator lval;
        value = atoi( y.getData(lval));
    }
}


///
Data
SdpBandwidth::modifierString ()
{
    Data s;

    switch (modifier)
    {
        case BandwidthModifierTypeAS:
        {
            s = SdpBandwidthModifierAS;
            break;
        }
        case BandwidthModifierTypeCT:
        {
            s = SdpBandwidthModifierCT;
            break;
        }
        case BandwidthModifierTypeRR:
        {
            s = SdpBandwidthModifierRR;
            break;
        }
        default:
        {
            // modifier value is corrupted
	    assert(0);
        }
    }
    return s;
}    // SdpBandwidth::modifierString


///
void
SdpBandwidth::encode (ostrstream& s)
{
    s << "b=" << modifierString() << ':' << value << "\r\n";
}    // SdpBandwidth::encode


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

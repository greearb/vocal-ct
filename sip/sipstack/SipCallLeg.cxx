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

static const char* const SipCallLeg_cxx_Version =
    "$Id: SipCallLeg.cxx,v 1.4 2004/10/25 23:21:14 greear Exp $";

#include "global.h"
#include "symbols.hxx"
#include "SipUrl.hxx"
#include "SipCallLeg.hxx"

using namespace Vocal;

SipCallLeg::SipCallLeg(const string& local_ip)
    : SipHeader(local_ip),
    from(),
    to(),
    callId("", local_ip),
    cseq("", local_ip)
{
}


SipCallLeg::SipCallLeg( const SipFrom& newFrom, const SipTo& newTo,
                        const SipCallId& newCallId, const string& local_ip)
    : SipHeader(local_ip),
      callId(newCallId),
      cseq("", local_ip)
{
    setTo(newTo);
    setFrom(newFrom);
}


SipCallLeg::SipCallLeg( const SipCallLeg& src)
    : SipHeader(src),
        from(src.from),
        to(src.to),
	callId(src.callId),
        cseq(src.cseq)
{
}


SipCallLeg&
SipCallLeg::operator=( const SipCallLeg& rhs )
{
    if (&rhs != this)
    {
        from = rhs.from;
        to = rhs.to;
        callId = rhs.callId;
        cseq = rhs.cseq;
    }
    return (*this);
}


SipFrom 
SipCallLeg::getFrom() const
{
    return SipFrom(from, getLocalIp());
}


void 
SipCallLeg::setFrom( const SipFrom& newfrom )
{
    Sptr <SipUrl> sipUrl((SipUrl*)(newfrom.getUrl().getPtr()));
    if (sipUrl != 0)
    {
        from = sipUrl->getNameAddr();
    }
}


bool
SipCallLeg::operator == (const SipCallLeg& src) const {
   cpLog(LOG_DEBUG, "in SipCallLeg::operator == ");
   if ( getCallId() == src.getCallId()) {
      if (cseq == src.cseq) {
         // If the to or from ends in :5060, ignore it, because that is
         // the default, and other stacks may not put it on...
         char cooked_to[to.size() + 1];
         char cooked_from[from.size() + 1];
         char cooked_src_to[src.to.size() + 1];
         char cooked_src_from[src.from.size() + 1];
         
         if ((to.size() > 5) &&
             (strcmp((to.c_str() + (to.size() - 5)), ":5060") == 0)) {
            strncpy(cooked_to, to.c_str(), to.size() - 5);
            cooked_to[to.size()-5] = 0; //Null terminate
         }
         else {
            strcpy(cooked_to, to.c_str());
         }
         
         if ((src.to.size() > 5) &&
             (strcmp((src.to.c_str() + (src.to.size() - 5)), ":5060") == 0)) {
            strncpy(cooked_src_to, src.to.c_str(), src.to.size() - 5);
            cooked_src_to[src.to.size()-5] = 0; //Null terminate
         }
         else {
            strcpy(cooked_src_to, src.to.c_str());
         }
         
         if ((from.size() > 5) &&
             (strcmp((from.c_str() + (from.size() - 5)), ":5060") == 0)) {
            strncpy(cooked_from, from.c_str(), from.size() - 5);
            cooked_from[from.size()-5] = 0; //Null terminate
         }
         else {
            strcpy(cooked_from, from.c_str());
         }
         
         if ((src.from.size() > 5) &&
             (strcmp((src.from.c_str() + (src.from.size() - 5)), ":5060") == 0)) {
            strncpy(cooked_src_from, src.from.c_str(), src.from.size() - 5);
            cooked_src_from[src.from.size()-5] = 0; //Null terminate
         }
         else {
            strcpy(cooked_src_from, src.from.c_str());
         }
         
         if ((strcasecmp(cooked_to, cooked_src_to) == 0) &&
             (strcasecmp(cooked_from, cooked_src_from) == 0)) {
            cpLog( LOG_DEBUG_STACK, "Exact Match" );
            return true;
         }
         else {
            if ((strcasecmp(cooked_to, cooked_src_from) == 0) &&
                (strcasecmp(cooked_from, cooked_src_to) == 0)) {
               cpLog( LOG_DEBUG_STACK, "To-From cross match" );
               return true;
            }
            else{
               // Hack to make us match when the peer sets to/from to be the
               // same.  This is really ugly..I think VOCAL should match on Call-ID
               // and quit worrying so much about the call-leg, at least for
               // matching responses, etc.
               if ((strcasecmp(cooked_to, cooked_src_to) == 0) &&
                   (strcasecmp(cooked_src_to, cooked_src_from) == 0)) {
                  cpLog(LOG_DEBUG_STACK, "To-To-From match, gateway is funny??");
                  return true;
               }
               else {
                  cpLog(LOG_DEBUG_STACK, "NOT matched, to: %s  src.to: %s  from: %s  src.from: %s  cto: %s cfrom: %s  scto: %s scfrom: %s",
                        to.c_str(), src.to.c_str(), from.c_str(), src.from.c_str(),
                        cooked_to, cooked_from, cooked_src_to, cooked_src_from);
               }
            }
         }
      }
      else {
         cpLog(LOG_DEBUG_STACK, "cseq don't match, cseq: %s  src.cseq: %s",
               cseq.encode().c_str(), src.cseq.encode().c_str());
      }
   }
   else {
      cpLog(LOG_DEBUG_STACK, "call ids do not match, callId: %s  src: %s\n",
            getCallId().encode().c_str(), src.getCallId().encode().c_str());
   }
   return false;
}

bool
SipCallLeg::operator != (const SipCallLeg& src) const
{
    cpLog(LOG_DEBUG, "in SipCallLeg::operator != ");
    return (!operator==(src));
}


bool
SipCallLeg::operator < (const SipCallLeg& rhs) const
{
    Data lhsTo = to;
    Data lhsFrom = from;
    if ( callId < rhs.callId )
    {
        cpLog( LOG_DEBUG, "CallId < rhs.CallId" );
        return true;
    }
    else if ( callId == rhs.callId )
    {
        if(cseq < rhs.cseq)
        {
            cpLog( LOG_DEBUG, "cseq < rhs.cseq" );
            return true;
        }
        else
        {
            cpLog( LOG_DEBUG, "Call ID  && cseq match" );

            Data rhsTo = rhs.to;
            Data rhsFrom = rhs.from;

        /*
               cpLog( LOG_DEBUG, "    To  : %s", to.logData() );
               cpLog( LOG_DEBUG, "    From: %s", from.logData() );
               cpLog( LOG_DEBUG, "rhs To  : %s", rhsTo.logData() );
               cpLog( LOG_DEBUG, "rhs From: %s", rhsFrom.logData() );
        */

            if ( lhsFrom < lhsTo )
            {
                // Sort this
                Data temp( lhsTo );
                lhsTo = lhsFrom;
                lhsFrom = temp;
            }
            if ( rhsFrom < rhsTo )
            {
                // Sort rhs
                Data temp( rhsTo );
                rhsTo = rhsFrom;
                rhsFrom = temp;
            }

            if ( lhsTo < rhsTo )
            {
                return true;
            }
            else if ( lhsTo == rhsTo )
            {
                if ( lhsFrom < rhsFrom )
                {
                    return true;
                }
                else if ( lhsFrom == rhsFrom )
                {
                    return false;
                }
            }
        }
    }
    return false;
}


SipTo 
SipCallLeg::getTo() const
{
    return SipTo(to, getLocalIp());
}


void 
SipCallLeg::setTo( const SipTo& newto )
{
    Sptr <SipUrl> sipUrl((SipUrl*)(newto.getUrl().getPtr()));
    if (sipUrl != 0)
    {
        to = sipUrl->getNameAddr();
    }
}


const SipCallId& 
SipCallLeg::getCallId() const
{
    return callId;
}


void 
SipCallLeg::setCallId( const SipCallId& newcallId )
{
    callId = newcallId;
}

const SipCSeq& 
SipCallLeg::getCSeq() const
{
    return cseq;
}


void 
SipCallLeg::setCSeq( const SipCSeq& newcseq )
{
    cseq = newcseq;
}

Data 
SipCallLeg::encode() const
{
////// check if no one "important" is using this then just return from and to
////// instead of encoding the urls... and hence remove them altogether from
////// class structure!
    Data sipCallLeg;
    sipCallLeg = from;
    sipCallLeg += to;
    sipCallLeg += callId.encode();
    return sipCallLeg;
}


Sptr<SipHeader>
SipCallLeg::duplicate() const
{
    return new SipCallLeg(*this);
}


bool
SipCallLeg::compareSipHeader(SipHeader* msg) const
{
    SipCallLeg* otherMsg = dynamic_cast<SipCallLeg*>(msg);
    if(otherMsg != 0)
    {
	return (*this == *otherMsg);
    }
    else
    {
	return false;
    }
}

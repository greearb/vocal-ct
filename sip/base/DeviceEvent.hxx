#ifndef DEVICE_EVENT_HXX
#define DEVICE_EVENT_HXX

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

static const char* const DeviceEvent_hxx_Version = 
    "$Id: DeviceEvent.hxx,v 1.3 2004/06/03 07:28:15 greear Exp $";

#include "global.h"
#include <string>
#include "Sptr.hxx"
#include "SipProxyEvent.hxx"
#include "Data.hxx"

namespace Vocal
{

typedef enum
{
    DeviceEventUndefined = 0,
    DeviceEventShutdown,
    DeviceEventHookUp,
    DeviceEventHookDown,
    DeviceEventFlash,
    DeviceEventDtmf0 = 100,
    DeviceEventDtmf1,
    DeviceEventDtmf2,
    DeviceEventDtmf3,
    DeviceEventDtmf4,
    DeviceEventDtmf5,
    DeviceEventDtmf6,
    DeviceEventDtmf7,
    DeviceEventDtmf8,
    DeviceEventDtmf9,
    DeviceEventDtmfStar,
    DeviceEventDtmfHash,
    DeviceEventReferUrl,
    DeviceEventCallUrl = 200
} DeviceEventType;

/** Object DeviceEvent
<pre>
<br> Usage of this Class </br>

   A place holder for events from a device

</pre>
*/
class DeviceEvent : public SipProxyEvent {
public:

   DeviceEvent();


   /** Virtual destructor
    */
   virtual ~DeviceEvent();


   /** Name of this class.
    */
   virtual const char* const name() const {
      return "DeviceEvent";
   }

   DeviceEventType type;
   int             id;
   Data            item;    
};
 
}

#endif // DEVICE_EVENT_HXX

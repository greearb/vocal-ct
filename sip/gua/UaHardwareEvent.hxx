#ifndef UaHardwareEvent_H
#define UaHardwareEvent_H

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

static const char* const UaHardwareEventVersion =
    "$Id: UaHardwareEvent.hxx,v 1.2 2004/06/19 00:51:08 greear Exp $";

#include <string>
#include <vector>
#include "SipProxyEvent.hxx"


namespace Vocal
{

namespace UA
{

///
enum HardwareMsgType
{
    HardwareSignalType,
    HardwareAudioType
};

///
enum HardwareAudioRequestType
{
    AudioStart,
    AudioStop,
    AudioSuspend,
    AudioResume
};

#define RGW_HOST_STRING_LEN (128)
#define RGW_MODE_LEN (128)

///
struct HardwareAudioRequest
{
    HardwareAudioRequestType type;
    int localPort;
    int remotePort;
    char localHost[RGW_HOST_STRING_LEN];
    char remoteHost[RGW_HOST_STRING_LEN];
    char mode[RGW_MODE_LEN];  // should this be an enum ???
    bool echoCancellation;
    int rtpPacketSize;
    bool sendRingback;
};

/// Message for hardware
class UaHardwareEvent : public SipProxyEvent {
public:
   ///
   UaHardwareEvent(int id);
   ///
   virtual ~UaHardwareEvent();
   ///
   virtual const char* const name() const;
   
   ///
   HardwareMsgType type;
   ///
   struct HardwareAudioRequest request;
   ///
   vector < string > dataList;
   ///
   int myId;
};
 
}

}

#endif

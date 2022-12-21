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

#include "global.h"
#include "SipMethod.hxx"
#include <cassert>

using namespace Vocal;


Data 
Vocal::methodToData(Method method)
{
    Data data;

    switch (method)
    {
    case SIP_INVITE:
        data = INVITE_METHOD;
        break;
    case SIP_CANCEL:
        data = CANCEL_METHOD;
        break;
    case SIP_ACK:
        data = ACK_METHOD;
        break;
    case SIP_BYE:
        data = BYE_METHOD;
        break;
    case SIP_REGISTER:
        data = REGISTER_METHOD;
        break;
    case SIP_OPTIONS:
        data = OPTIONS_METHOD;
        break;
    case SIP_INFO:
        data = INFO_METHOD;
        break;
    // TODO: remove case for TRANSFER (deprecated)
    case SIP_TRANSFER:
        data = TRANSFER_METHOD;
        break;
    case SIP_REFER:
        data = REFER_METHOD;
        break;
    case SIP_SUBSCRIBE:
        data = SUBSCRIBE_METHOD;
        break;
    case SIP_NOTIFY:
        data = NOTIFY_METHOD;
        break;
    case SIP_MESSAGE:
        data = MESSAGE_METHOD;
        break;
    default:
        assert(0);
        break;
    }
    return data;
}


Method 
Vocal::dataToMethod(Data data)
{
    Method type = SIP_UNKNOWN;

    if (data == INVITE_METHOD)
    {
	type = SIP_INVITE;
    }
    else if (data == CANCEL_METHOD)
    {
	type = SIP_CANCEL;
    }
     else if (data == REFER_METHOD)
    {
	type = SIP_REFER;
    }
    else if (data == ACK_METHOD)
    {
	type = SIP_ACK;
    }
    else if (data == BYE_METHOD)
    {
	type = SIP_BYE;
    }
    else if (data == INFO_METHOD)
    {
	type = SIP_INFO;
    }
    else if (data == OPTIONS_METHOD)
    {
	type = SIP_OPTIONS;
    }
    else if (data == REGISTER_METHOD)
    {
	type = SIP_REGISTER;
    }
#if 0
    // TODO: remove case for TRANSFER (deprecated)
    else if (data == TRANSFER_METHOD)
    {
        type = SIP_TRANSFER;
    }
#endif
    else if (data == SUBSCRIBE_METHOD)
    {
	type = SIP_SUBSCRIBE;
    }
    else if (data == NOTIFY_METHOD)
    {
	type = SIP_NOTIFY;
    }
    else if (data == "SIP/2.0")
    {
        type = SIP_STATUS;
    }
    else if (data == MESSAGE_METHOD)
    {
        type = SIP_MESSAGE;
    }
    
    return type;
}
/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

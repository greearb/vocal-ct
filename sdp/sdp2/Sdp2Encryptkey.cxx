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

static const char* const SdpEncryptkey_cxx_Version =
    "$Id: Sdp2Encryptkey.cxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "global.h"
#include <cassert>
#include <strstream>
#include <list>
#include <deque>

#include "Data.hxx"
#include "Sdp2Encryptkey.hxx"
#include "support.hxx"


using Vocal::SDP::SdpEncryptkey;


SdpEncryptkey::SdpEncryptkey(Data& s)
{
    // k=<method>:<encryption key> or
    // k=<method> for "prompt" method

    // For "uri" method, the <encryption key> field may contains ":",
    // we can't use split() with ":"
//    Data tmp_Data = s.substr(0, 3);

    Data tmp_Data = s.parse(":");

    if (tmp_Data == "uri")
    {
        encrypt_method = EncryptMethodURI;
        encrypt_key = s;
    }
    else
    {
        deque<Data> encryptkeyList;

	bool finished = false;
	
	while (!finished)
	{
	    Data x = s.parse(":", &finished);
	    if(finished)
	    {
		x = s;
	    }
	    encryptkeyList.push_back(x);
	}

        if (encryptkeyList.size() == 1)	// prompt method
        {
            if (encryptkeyList[0] == SdpEncryptkeyMethodPrompt)
            {
                encrypt_method = EncryptMethodPrompt;
                encrypt_key = "";
            }
            else
            {
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].logData());
                cpLog(LOG_ERR, "SdpEncryptkey: or valid method but no required value");
                throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
            }
        }
        else if (encryptkeyList.size() == 2) // clear, base64, or URI
        {
            if (encryptkeyList[0] == SdpEncryptkeyMethodClear)
            {
                encrypt_method = EncryptMethodClear;
                encrypt_key = encryptkeyList[1];
            }
            else if (encryptkeyList[0] == SdpEncryptkeyMethodBase64)
            {
                encrypt_method = EncryptMethodBase64;
                encrypt_key = encryptkeyList[1];
            }
            else if (encryptkeyList[0] == SdpEncryptkeyMethodPrompt)
            {
                // Prompt method should not have value
                // Log a warning and set empty value
                encrypt_method = EncryptMethodPrompt;
                encrypt_key = "";
                cpLog(LOG_WARNING, "SdpEncryptkey: Prompt method shouldn't have value");
            }
            else
            {
                cpLog(LOG_ERR, "SdpEncryptkey: Undefined Method: %s", encryptkeyList[0].logData());
                throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
            }

        }
        else  // bad "k=" line
        {
            cpLog(LOG_ERR, "SdpEncryptkey: bad k line: %s", s.logData());
            throw SdpExceptions(UNKNOWN_ENCRYPT_METHOD);
        }
    }
}


void
SdpEncryptkey::encode (ostrstream& s)
{
    switch (encrypt_method)
    {
        case EncryptMethodClear:
	    s << "k=" << SdpEncryptkeyMethodClear << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodBase64:
	    s << "k=" << SdpEncryptkeyMethodBase64 << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodURI:
	    s << "k=" << SdpEncryptkeyMethodURI << ':' << encrypt_key << "\r\n";
	    break;
        case EncryptMethodPrompt:
	    s << "k=" << SdpEncryptkeyMethodPrompt << "\r\n";
	    break;
        default:     // Log an error for unknown method but don't terminate
	    cpLog(LOG_ERR, "SdpEncryptkey: unknown method: %d", encrypt_method);
    }
}    // SdpEncryptkey::encode


bool
SdpEncryptkey::dump ()
{
    cpLog(LOG_DEBUG, "ENCRYPTION KEY ------------");

    switch (encrypt_method)
    {
        case EncryptMethodClear:
        cpLog(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodClear);
        break;
        case EncryptMethodBase64:
        cpLog(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodBase64);
        break;
        case EncryptMethodURI:
        cpLog(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodURI);
        break;
        case EncryptMethodPrompt:
        cpLog(LOG_DEBUG, "    Method\t\t%s", SdpEncryptkeyMethodPrompt);
        break;
        default:
        cpLog(LOG_DEBUG, "    Method\t\t%s", "unknown");
    }
    if (encrypt_key.length() > 0)
        cpLog(LOG_DEBUG, "    Key\t\t\t%s", encrypt_key.logData());

    return true;
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

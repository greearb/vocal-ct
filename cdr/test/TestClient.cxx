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

#include <sys/time.h>
#include <netinet/in.h>

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <pwd.h>
#include <stdlib.h>
#include <errno.h>
#include <strings.h>

#include "TestClient.hxx"
#include "VRadiusException.hxx"
#include "cpLog.h"

TestClient* TestClient::_instance = 0;

TestClient::TestClient()
{}


TestClient&
TestClient::instance(const char *server,
                     const char *secretKey,
                     const int retries)
{
    if (!_instance)
    {
        _instance = new TestClient();

        try
        {
            _instance->initialize(server, secretKey, retries);
        }
        catch (VRadiusException &e)
        {
            delete _instance;
            _instance = 0;
            throw;
        }
    }
    return *_instance;
}

void
TestClient::destroy()
{
    if (TestClient::_instance)
    {
        delete TestClient::_instance;
        TestClient::_instance = 0;
    }
}

void
TestClient::processVsa(const unsigned char vsaType,
                       unsigned char *ptr,
                       int bufLen)
{
    char logMsg[128];
    string enumStr;
    unsigned short var;
    VsaType atype = (VsaType)vsaType;

    switch (atype)
    {
        case VSA_STATUS:
        var = *ptr;
        sprintf(logMsg, "Attribute VSA_STATUS:%d", var);
        cpLog(LOG_INFO, logMsg);
        break;

        default:
        sprintf(logMsg, "Ignored attribute type:%d, length:%d",
                (int)atype, (int)bufLen);
        cpLog(LOG_DEBUG, logMsg);
        break;
    }
}

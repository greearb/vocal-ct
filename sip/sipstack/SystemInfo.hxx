#ifndef __VOCAL__SYSINFO_H
#define __VOCAL__SYSINFO_H

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

static const char* const SystemInfo_hxx_Version =
    "$Id: SystemInfo.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "Sptr.hxx"
#include <string>

class NetworkAddress;    

namespace Vocal
{
    
class SystemInfo
{
    public:
        ///
        SystemInfo();
        ~SystemInfo();

        //Accessors methods
        const char* const getUrlToRegister()const
        {
            return registerUrl;
        }
        ///
        const char* const getRegisterDomain() const
        {
            return registerDomain;
        }
        ///
        const char* const getUserName() const
        {
            return userName;
        }
        ///
        const char* const getDisplayName() const
        {
            return displayName;
        }


        ///
        // NOTE:  You probably should use the local_ip as configured by the
        // user, or at least check to see if the user set the local_ip before
        // using this method. --Ben
        const char* const gethostAddress () const
        {
            return hostIP;
        }

        ///
        // NOTE:  You probably should use the local_ip as configured by the
        // user, or at least check to see if the user set the local_ip before
        // using this method. --Ben
        const char* const gethostAddress (char* buf, int len, int family) const;

        ///
        const char* const gethostName() const
        {
            return hostName;
        }

        const string resolveIpToHostname(const string& ipaddr);

        // Returns true if succeeded in resolving the IP.
        static bool toIpv4String(const char* ipaddr, unsigned long& rv);

        ///
        const char* const getSessionName() const
        {
            return sessionName;
        };
        ///
        const char* const getFirstIPpart() const;

        ///Mutators
        void setUrlToRegister(const char* regUrl)
        {
            makeCopy(&registerUrl , regUrl);
        }
        ///
        void setRegisterDomain(const char* regDomain)
        {
            makeCopy(&registerDomain, regDomain);
        }
        ///
        void setUserName(const char* name)
        {
            makeCopy(&userName , name);
        }
        ///
        void setDisplayName(const char* name)
        {
            makeCopy(&displayName , name);
        }
        ///
        void setSessionName(const char* session)
        {
            makeCopy(&sessionName , session);
        }
        ///
        void setSystemPort(int port)
        {
            mySystemPort = port;
        }
        ///
        int getSystemPort()
        {
            return mySystemPort;
        }

        /// return true if the host and port refers to this element
        bool isMe(const NetworkAddress& na);

        friend ostream& operator<<(ostream& os, const SystemInfo& sysInfo);

    private:
        ///
        void breakIP();

        ///
        void makeCopy(char** dest, const char* src);

        char* userName;
        char* displayName;
        char* hostName;
        char* hostIP;
        char* sessionName;
        char* registerDomain;
        char* registerUrl;
        int mySystemPort;
};

///
extern SystemInfo theSystem;
///
extern void parseSystemInfo(char* tag, char* type, char* value);
inline ostream& operator<<(ostream& os, const SystemInfo& sysInfo)
{
    os.width(20);
    os << "User:" << sysInfo.getUserName() << endl;
    os.width(20);
    os << "Display name:" << sysInfo.getDisplayName() << endl;
    os.width(20);
    os << "Host name:" << sysInfo.gethostName() << endl;
    os.width(20);
    os << "Host IP:" << sysInfo.gethostAddress() << endl;
    os.width(20);
    os << "Session :" << sysInfo.getSessionName() << endl;
    os.width(20);
    os << "Register domain:" << sysInfo.getRegisterDomain() << endl;
    os.width(20);
    os << "Register URL:" << sysInfo.getUrlToRegister() << endl;
    return os;
}

extern void parseSystemInfo(char* tag, char* type, char* value);
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

#ifndef VEnvVar_hxx
#define VEnvVar_hxx

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





static const char* const VEnvVarHeaderVersion =
    "$Id: VEnvVar.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";



#include "global.h"
#include "mstring.hxx"


class VEnvVar
{
    public:
        /**
           Constructor to create EnvVariable object.If variable is 
           not set , defvalue is taken.
        */
        VEnvVar(const mstring& variable, const mstring& defvalue);

        /**
           Constructor to create EnvVariable object.If variable is 
           not set , marked as not set;
        */
        VEnvVar(const mstring& variable);

        bool isSet()
        {
            return _set;
        };

        const mstring& value()
        {
            return _value;
        }

        operator mstring() const
        {
            return _value;
        }
        operator const char*() const
        {
            return _value.c_str();
        }
        operator int() const
        {
            return _value;
        }

        VEnvVar& operator =(const VEnvVar& other)
        {
            if (this != &other)
            {
                _variable = other._variable;
                _value = other._value;
                _set = other._set;
            }
            return *this;
        }

        //Add the new Env Variables as we go along
        static VEnvVar VPS_PORT;
        static VEnvVar VCONFIG_DIR;
        static VEnvVar VLISTENER_PORT;
        static VEnvVar VLOG_FILE_NAME;
        static VEnvVar VPS_FILESYSTEM;
        static VEnvVar VPS_SERVER;
        static VEnvVar VLOG_LEVEL;
        static VEnvVar VLOCAL_IP;

    private:
        std::mstring _variable;
        std::mstring _value;
        bool _set;
};

#endif

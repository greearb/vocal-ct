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

static const char* const VEnvVar_cxx_Version =
    "$Id: VEnvVar.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "global.h"
#include "VEnvVar.hxx"
#include "vocalconfig.h"

using std::mstring;


VEnvVar VEnvVar::VPS_PORT("VPS_PORT", "6005");
VEnvVar VEnvVar::VPS_SERVER("VPS_SERVER", "216.102.92.37");
VEnvVar VEnvVar::VCONFIG_DIR("VCONFIG_DIR", VOCAL_INSTALL_PATH "/provisioning_data/Config");
VEnvVar VEnvVar::VLISTENER_PORT("VLISTENER_PORT", "6006");
VEnvVar VEnvVar::VLOG_FILE_NAME("VLOG_FILE_NAME", VOCAL_INSTALL_PATH "/provisioning_data/Logs/pserver_log");
VEnvVar VEnvVar::VPS_FILESYSTEM("VPS_FILESYSTEM", VOCAL_INSTALL_PATH "/provisioning_data");
VEnvVar VEnvVar::VLOG_LEVEL("VLOG_LEVEL", "6");  //INFO from cplog.h
VEnvVar VEnvVar::VLOCAL_IP("VLOCAL_IP", "");


VEnvVar::VEnvVar(const mstring& variable, const mstring& defvalue)
        : _variable(variable), _value(defvalue), _set(true)
{
    char *c;
    c = getenv(_variable.c_str());
    if (c)
    {
        _value = c;
    }
}

VEnvVar::VEnvVar(const mstring& variable)
        : _variable(variable), _set(false)
{
    char *c;
    c = getenv(variable.c_str());
    if (c)
    {
        _value = c;
        _set = true;
    }
}

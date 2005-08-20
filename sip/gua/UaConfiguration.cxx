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

static const char* const UaConfiguration_cxx_Version =
    "$Id: UaConfiguration.cxx,v 1.5 2005/08/20 06:57:42 greear Exp $";

#include "global.h"
#include <cassert>
#include <cstdio>

#include "cpLog.h"
#include "parse3tuple.h"
#include "SystemInfo.hxx"
#include "Data.hxx"
#include "UaConfiguration.hxx"


using namespace Vocal;
using namespace Vocal::UA;

UaConfiguration* UaConfiguration::uaConfiguration = 0;


UaConfiguration& UaConfiguration::instance() {
    assert( uaConfiguration != 0 );
    return *uaConfiguration;
}

void UaConfiguration::destroy() {
    delete uaConfiguration;
    uaConfiguration = NULL;
}

// this needs to match up with the enum in UaConfiguration.hxx 
static const char* UaConfigurationTagString [] =
    {
        "User_Name",
        "Display_Name",
        "Pass_Word",
        "Local_SIP_Port",
        "Min_RTP_Port",
        "Max_RTP_Port",
        "SIP_Transport",
        "Proxy_Server",
        "Register_On",
        "Register_From",
        "LocalSipNetworkDev",
        "LocalRtpNetworkDev",
        "Register_To",
        "Register_Expires",
        "Ua_Xfer_Mode",
        "Device_Name",
        "Call_Waiting_On",
        "Subscribe_On",
        "Subscribe_Expires",
        "Subscribe_To",
	"LogFilename",
	"NATAddress_IP",
	"Network_RTP_Rate",
	"LogLevel",
        "Video",
        "VmServer",
        "Sphinx_Data_Dir",
        "Sphinx_LM_Name",
        "Sphinx_HMM_Dir",
        "Sphinx_Wavefile_Dir",
        "Sphinx_Utterance_File",
        "Wave_File",
        "Save_Wave_File",
        "LANforge",
        "ReadStdin",
        "G711uPrio",
        "SpeexPrio",
        "G726_16Prio",
        "G726_24Prio",
        "G726_32Prio",
        "G726_40Prio",
        "G729aPrio",
        "Force_IPv6",
        "IP_TOS",
        "PKT_PRIORITY",
	"VAD_ON",
	"VADMsBeforeSuppression",
        "VADForceSendAfterMs",
        "Unknown"
    };
// the above needs to match up with the enum in UaConfiguration.hxx 


void
UaConfiguration::parseCfgFileCallBack( char* tag, char* type, char* value )
{
    UaConfigurationTags ltag = static_cast<UaConfigurationTags>(strConst2i( tag, UaConfigurationTagString, UaConfigurationTagMax )); 
    UaConfiguration::instance().setValue(ltag, value);
}    // UaConfiguration::parseCfgFileCallBack


void
UaConfiguration::instance( const string cfgFile )
{
    assert( uaConfiguration == 0 );

    uaConfiguration = new UaConfiguration(cfgFile);
    assert( uaConfiguration != 0 );
    uaConfiguration->parseConfig();

}    // UaConfiguration::instance

void
UaConfiguration::parseConfig()
{
    setValue(UserNameTag,"");
    setValue(DisplayNameTag,"");
    setValue(PasswordTag,"\0");
    setValue(ProxyServerTag,"");
    setValue(ProxyServerTag,"");
    setValue(RegisterToTag,"");
    setValue(RegisterFromTag,"");
    setValue(LocalSipNetworkDevTag,"");
    setValue(LocalRtpNetworkDevTag,"");
    setValue(RegisterExpiresTag,"300");
    setValue(MinRtpPortTag, "10000");
    setValue(MaxRtpPortTag, "10050");
    setValue(SipTransportTag,"UDP");
    setValue(RegisterOnTag,  "true");
    setValue(UaXferModeTag, "false");
    setValue(CallWaitingOnTag, "false");
    setValue(SubscribeOnTag, "false");
    setValue(SubscribeExpireTag,"1000");
    setValue(LogLevelTag,"LOG_ERR");
    setValue(NetworkRtpRateTag,"20");
    setValue(LocalSipPortTag, "0");
    setValue(NATAddressIPTag, "");
    setValue(VideoTag, "0");
    setValue(WaveFileNameTag, "default_in.wav");
    setValue(SaveWaveFileNameTag, "default_out.wav");
    setValue(UseLANforgeDeviceTag, "0");
    setValue(ReadStdinTag, "1");
    setValue(G711uPrioTag, "1");
    setValue(SpeexPrioTag, "0");
    setValue(G726_16PrioTag, "0");
    setValue(G726_24PrioTag, "0");
    setValue(G726_32PrioTag, "0");
    setValue(G726_40PrioTag, "0");
    setValue(G729aPrioTag, "0");
    setValue(ForceIPv6Tag, "false");
    setValue(IP_TOS_TAG, "0");
    setValue(PKT_PRIORITY_TAG, "0");
    setValue(VAD_ON_TAG, "0");
    setValue(VADMsBeforeSuppressionTag, "250");
    setValue(VADForceSendAfterMsTag, "5000");

    if( !parse3tuple( myConfigFile.c_str(), parseCfgFileCallBack ) )
    {
        // Stop UA
        exit( -1 );
    }

    show();
}

const string UaConfiguration::getConfiguredLocalIp() const {
    return getValue(RegisterFromTag).c_str();
}

const string UaConfiguration::getConfiguredLocalRtpDev() const {
    return getValue(LocalRtpNetworkDevTag).c_str();
}

const string UaConfiguration::getConfiguredLocalSipDev() const {
    return getValue(LocalSipNetworkDevTag).c_str();
}


const string UaConfiguration::getMyLocalIp() const {
    string cip = getValue(RegisterFromTag).c_str();
    if (cip.size()) {
        return cip;
    }
    else {
        return theSystem.gethostAddress(); //default system ip
    }
}

void
UaConfiguration::show()
{
    cpLog( LOG_INFO, "SIP User Agent Configuration" );
    cpLog( LOG_INFO, "--- User ---" );
    cpLog( LOG_INFO, "              Name : \"%s\"", getValue(UserNameTag).c_str() );
    cpLog( LOG_INFO, "      Display Name : \"%s\"", getValue(DisplayNameTag).c_str() );
    cpLog( LOG_INFO, "         Pass Word : \"%s\"", getValue(PasswordTag).c_str() );

    cpLog( LOG_INFO, "--- SIP ---" );
    cpLog( LOG_INFO, "              Port : \"%s\"", getValue(LocalSipPortTag).c_str() );
    cpLog( LOG_INFO, "         Transport : \"%s\"", getValue(SipTransportTag).c_str() );
    cpLog( LOG_INFO, "      Proxy Server : \"%s\"", getValue(ProxyServerTag).c_str() );

    cpLog( LOG_INFO, "--- Registration ---" );
    cpLog( LOG_INFO, "          Register : \"%s\"", getValue(RegisterOnTag).c_str());
    cpLog( LOG_INFO, "              From : \"%s\"", getValue(RegisterFromTag).c_str() );
    cpLog( LOG_INFO, "   SIP Network-Dev : \"%s\"", getValue(LocalSipNetworkDevTag).c_str() );
    cpLog( LOG_INFO, "   RTP Network-Dev : \"%s\"", getValue(LocalRtpNetworkDevTag).c_str() );
    cpLog( LOG_INFO, "                To : \"%s\"", getValue(RegisterToTag).c_str() );
    cpLog( LOG_INFO, "           Expires : %s s", getValue(RegisterExpiresTag).c_str());

    cpLog( LOG_INFO, "--- Subscribe ---" );
    cpLog( LOG_INFO, "         Subscribe : \"%s\"", getValue(SubscribeOnTag).c_str());
    cpLog( LOG_INFO, "                To : \"%s\"", getValue(SubscribeToTag).c_str() );
    cpLog( LOG_INFO, "           Expires : %s s", getValue(SubscribeExpireTag).c_str() );

    cpLog( LOG_INFO, "         Xfer Mode : \"%s\"", getValue(UaXferModeTag).c_str());
    cpLog( LOG_INFO, "       Call Waiting: \"%s\"", getValue(CallWaitingOnTag).c_str());
    cpLog( LOG_INFO, "          Min RTP Port : %s", getValue(MinRtpPortTag).c_str() );
    cpLog( LOG_INFO, "          Max RTP Port : %s", getValue(MaxRtpPortTag).c_str());

    cpLog( LOG_INFO, "       G711u Priority: %s", getValue(G711uPrioTag).c_str());
    cpLog( LOG_INFO, "       Speex Priority: %s", getValue(SpeexPrioTag).c_str());
    cpLog( LOG_INFO, "       G726_16 Priority: %s", getValue(G726_16PrioTag).c_str());
    cpLog( LOG_INFO, "       G726_24 Priority: %s", getValue(G726_24PrioTag).c_str());
    cpLog( LOG_INFO, "       G726_32 Priority: %s", getValue(G726_32PrioTag).c_str());
    cpLog( LOG_INFO, "       G726_40 Priority: %s", getValue(G726_40PrioTag).c_str());
    cpLog( LOG_INFO, "       G729a Priority: %s", getValue(G729aPrioTag).c_str());
    cpLog( LOG_INFO, "       Force IPv6:   %s", getValue(ForceIPv6Tag).c_str());
    cpLog( LOG_INFO, "       IP_TOS:       %s", getValue(IP_TOS_TAG).c_str());
    cpLog( LOG_INFO, "       PKT_PRIORITY: %s", getValue(PKT_PRIORITY_TAG).c_str());

    cpLog( LOG_INFO, "--- Behaviour Control Options ---" );
    cpLog( LOG_INFO, "       VAD_ON      : %s", getValue(VAD_ON_TAG).c_str());
    cpLog( LOG_INFO, "       VADMsBeforeSuppression: %s", getValue(VADMsBeforeSuppressionTag).c_str());


    cpLog( LOG_INFO, "\n\n");


}    // UaConfiguration::show


string
UaConfiguration::getValue(UaConfigurationTags tag) const
{
    assert(tag < UaConfigurationTagMax);
    
    return myConfigData[tag]; 
}  

void
UaConfiguration::setValue(UaConfigurationTags tag, string value)
{
    cpLog(LOG_INFO, "Setting value :%s for tag:%d", value.c_str(), tag);
    if(tag < UaConfigurationTagMax)
    {
         myConfigData[tag] = value;
         if(tag == VmServerTag) myVmServers.push_back(value);
    }
    else
    {
        cpLog(LOG_DEBUG, "UaConfiguration::setValue, unknown tag for value <%s> ignoring", value.c_str());
    }
}

UaConfiguration::UaConfiguration(const string cfgFile)
    : myConfigFile(cfgFile)
{
}  


UaConfiguration::~UaConfiguration()
{
}    // UaConfiguration::~UaConfiguration

const list<string>&
UaConfiguration::getVmServers() const
{
   return myVmServers;
}

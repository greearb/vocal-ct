#ifndef UA_CONFIGURATION_HXX
#define UA_CONFIGURATION_HXX

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

static const char* const UaConfigurationVersion =
    "$Id: UaConfiguration.hxx,v 1.6 2005/08/20 06:57:42 greear Exp $";

#include <string>
#include <list>

#include <MRtpSession.hxx>

using namespace std;

namespace Vocal
{

namespace UA
{


typedef enum {
    UserNameTag = 0,
    DisplayNameTag,
    PasswordTag,
    LocalSipPortTag,
    MinRtpPortTag,
    MaxRtpPortTag,
    SipTransportTag,
    ProxyServerTag,
    RegisterOnTag,
    RegisterFromTag,
    LocalSipNetworkDevTag,
    LocalRtpNetworkDevTag,
    RegisterToTag,
    RegisterExpiresTag,
    UaXferModeTag,
    DeviceNameTag,
    CallWaitingOnTag,
    SubscribeOnTag,
    SubscribeExpireTag,
    SubscribeToTag,
    LogFileNameTag,
    NATAddressIPTag,
    NetworkRtpRateTag,
    LogLevelTag,
    VideoTag,
    VmServerTag,
    SphinxDataDirTag,
    SphinxModelNameTag,
    SphinxHMMDirTag,
    SphinxWavefileDirTag,
    SphinxUtteranceFileTag,
    WaveFileNameTag, // Used by LANforgeCustomDevice
    SaveWaveFileNameTag, // Used by LANforge to save the PCM bits to a wav file.
    UseLANforgeDeviceTag, // When true, we'll use this device for our media.
    ReadStdinTag, // Defaults to 1 (true), but if set to "0", we won't read stdin.
    G711uPrioTag, // SDP priority for this (default) protocol
    SpeexPrioTag, // SDP priority for Speex codec.
    G726_16PrioTag, // SDP priority
    G726_24PrioTag, // SDP priority
    G726_32PrioTag, // SDP priority
    G726_40PrioTag, // SDP priority
    G729aPrioTag,   // SDP priority
    ForceIPv6Tag,
    IP_TOS_TAG,
    PKT_PRIORITY_TAG,
    VAD_ON_TAG,         // Is Voice Activity Detection enabled ( 0 == disabled)
    VADMsBeforeSuppressionTag,  // How many milliseconds of silence before we stop
                                // sending RTP packets to the other end 
    VADForceSendAfterMsTag, /* Force a pkt to be sent after this many ms, even
                          * if we are in VAD active mode. */
    UaConfigurationTagMax
} UaConfigurationTags;

/**
 *  SIP User Agent configuration
 */
class UaConfiguration {
public:
   ///
   static UaConfiguration& instance();
   ///
   static void instance( const string cfgFile );
   static void destroy();
   
   ///
   static void parseCfgFileCallBack( char* tag, char* type, char* value );
   ///
   void show();
   
   ///
   string getValue(UaConfigurationTags tag) const;
   ///
   void setValue(UaConfigurationTags tag, string value);
   
   ///
   ~UaConfiguration();
   
   ///
   void parseConfig();
   
   const string getMyLocalIp() const;
   
   const string getConfiguredLocalIp() const;
   const string getConfiguredLocalSipDev() const;
   const string getConfiguredLocalRtpDev() const;
   
   bool shouldUseGui() const;
   
   ///
   const list<string>& getVmServers() const;
   
private:
   ///
   UaConfiguration(const string cfgFile);
   ///
   string myConfigFile;
   ///
   list<string>  myVmServers;
   ///
   string myConfigData[UaConfigurationTagMax];
   ///
   static UaConfiguration* uaConfiguration;
};
 
}

}

#endif

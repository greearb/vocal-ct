
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
static const char* const VideoDevice_cxx_Version = 
    "$Id: VideoDevice.cxx,v 1.1 2004/05/01 04:15:25 greear Exp $";



#include "global.h"
#include "vtypes.h"
#include <cassert>
#include <fstream>
#include "VideoDevice.hxx"
#include "cpLog.h"

#define DECLARE_CONFIG_VARIABLES 1
#define HAVE_STDINT_H 1

//MPEG4IP
#include "stdint.h"
//#include "mp4live.h"
//#include "media_flow.h"
//#include "video_v4l_source.h"
//#include "audio_oss_source.h"
#include "mp4live_config.h"
#include "codec_plugin_private.h"

                             
using namespace Vocal::UA;
using namespace Vocal::SDP;
using Vocal::Threads::Lock;

VideoDevice::VideoDevice(int id)
    : MediaDevice(RTP, VIDEO), 
      myId(id), myHandle(0)
{
    char userFileName[256];
    char sdpFileName[256];
    char* home = getenv("HOME");
    if (home) {
        sprintf(userFileName, "%s/%s", home, "/.mp4live_rc");
        sprintf(sdpFileName, "%s/%s", home, "/.localSdp.sdp");
    }

    myCConfig = 0;
    try 
    {
        myCConfig = new CLiveConfig(MyConfigVariables,
                    sizeof(MyConfigVariables) / sizeof(SConfigVariable),
                    home ? userFileName : NULL);

        // read user config file if present
        myCConfig->ReadDefaultFile();

        myCConfig->m_appAutomatic = false;

        extern bool PrintDebugMessages;
        PrintDebugMessages =
                myCConfig->GetIntegerValue(CONFIG_APP_DEBUG);
        myCConfig->SetStringValue(CONFIG_SDP_FILE_NAME, sdpFileName);
        myCConfig->SetStringValue(CONFIG_VIDEO_SOURCE_TYPE, "V4L");
    }
    catch (CConfigException* e) 
    {
         cpLog(LOG_ERR, "Failed to create the MP4Live config");
         delete e;
    }
    free(home);
}  // end VideoDevice::VideoDevice()

int 
VideoDevice::setupSession(SdpSession& remoteSdp, SdpSession& localSdp)
{
   cpLog(LOG_ERR, "Calling Video with SDP :%s", remoteSdp.encode().logData());

   int retVal = generateSdpData(localSdp);
   return retVal;
}

int
VideoDevice::generateSdpData(SdpSession& localSdp)
{
   Data sdpData;
   SdpConnection* conn = localSdp.getConnection();
   assert(conn != 0);
   myCConfig->SetStringValue( CONFIG_RTP_DEST_ADDRESS, conn->getUnicast().logData());

   myCConfig->Update();

   ///Create the SDP
   bool ret = GenerateSdpFile(myCConfig);
   if(!ret)
   {
       cerr << "Failed to generate the SDP data" << endl;
       return -1;
   }

   ifstream inF(myCConfig->GetStringValue(CONFIG_SDP_FILE_NAME));
   char buf[1024];
   while( inF.getline(buf, 1024))
   {
       sdpData += buf;
       sdpData += "\r\n";
   }
   cerr << "SDP data:" << sdpData.logData() << endl;
   localSdp.decode(sdpData.logData());
   return 0;
}


VideoDevice::~VideoDevice(void)
{
    close_plugins();
    delete myCConfig;
    myCConfig = 0;
} // end VideoDevice::~VideoDevice()


int
VideoDevice::startMedia (SdpSession& remoteSdp)
{
   //Get the Remote IP and Port details
   SdpConnection* conn = remoteSdp.getConnection();
   int defaultAudioDestPort=0, defaultVideoDestPort=0;
   assert(conn != 0);
   myCConfig->SetStringValue( CONFIG_RTP_DEST_ADDRESS, conn->getUnicast().logData());

   defaultAudioDestPort = myCConfig->GetIntegerValue(CONFIG_RTP_AUDIO_DEST_PORT);
   defaultVideoDestPort = myCConfig->GetIntegerValue(CONFIG_RTP_VIDEO_DEST_PORT);

   list < SdpMedia* > mediaList = remoteSdp.getMediaList();
   list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();
   for(; mediaIterator != mediaList.end(); mediaIterator++)
   {
       SdpMedia* cMedia = (*mediaIterator);
       if(cMedia->getMediaType() == MediaTypeAudio)
       {
          myCConfig->SetIntegerValue(CONFIG_RTP_AUDIO_DEST_PORT, cMedia->getPort()); 
       }
       else if(cMedia->getMediaType() == MediaTypeVideo)
       {
          myCConfig->SetIntegerValue(CONFIG_RTP_VIDEO_DEST_PORT, cMedia->getPort()); 
       }
   }

   char userFileName[256];
   char* home = getenv("HOME");
   if (home) {
       sprintf(userFileName, "%s/%s", home, "/.mp4live_rc.r");
   }
   
   myCConfig->WriteToFile(userFileName);

   //Save back the default
   myCConfig->SetIntegerValue(CONFIG_RTP_AUDIO_DEST_PORT, defaultAudioDestPort);
   myCConfig->SetIntegerValue(CONFIG_RTP_VIDEO_DEST_PORT, defaultVideoDestPort);
   free(home);
   return 0;
} 

int
VideoDevice::stop(void)
{
    cpLog(LOG_ERR, "Calling Video Stop with Handle(%ld)", myHandle);
    return 0;
} 

void
VideoDevice::shutdown()
{
    myCConfig = 0;
}

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

#include "MediaController.hxx"
#include "Sdp2Session.hxx"
#include "Verify.hxx"
#include "SoundCard.hxx"
#include "VThread.hxx"
#include "Condition.hxx"
#include <fstream>
#include <iostream>
#include "cpLog.h"
#include "CodecG711U.hxx"
#include "MRtpSession.hxx"

using namespace Vocal;
using namespace Vocal::SDP;
using namespace Vocal::UA;

NetworkAddress myAddress;
Condition start;
bool nowStart = false;

typedef struct cTag
{
   string fileToPlay;
   int serverPort;
   int localPort;
} ATestStruct;

ATestStruct cStruct1;

unsigned int sessionId = 0;

class AudioDevice : public MediaDevice 
{
   public:
     AudioDevice(const char* deviceName)
             : MediaDevice(SOUNDCARD, AUDIO),
               mySoundCard(deviceName)
     {
         // open audio hardware device
         if( mySoundCard.open() == -1 )
         {
             cpLog( LOG_ERR, "Failed to open %s", deviceName );
             exit( -1 );
         }
         myCodec = new CodecG711U();
     }
     ///Read back from the device
     void processAudio ()
     {
         //Assuming card is set to PCMU and network rate is 20ms
         unsigned char buf[160];
         mySoundCard.read(buf, 160);
         processRaw((char*) buf, 160, myCodec->getType());
     }

     ///Write to device
     void sinkData(char* data, int length, VCodecType type=G711U)
     {
         cpLog(LOG_DEBUG, "Sink Data: length %d", length);
         mySoundCard.write( (unsigned char*) data, length);
     }

     int start()  { return 0; }
     int stop() { return 0; }
     int suspend()  { return 0; }
     int resume() { return 0; }
     ///
     SoundCard mySoundCard;
};


void test()
{
    MediaController::instance(9000, 9010); 
    Sptr<MediaDevice> mDevice = new AudioDevice("/dev/dsp");
    MediaController::instance().registerDevice(mDevice);

    Data s("v=0\r\no=- 1025036129 1025036129 IN IP4 192.168.123.10\r\ns=SIP Call\r\nc=IN IP4 192.168.123.10\r\nt=0 0\r\nm=audio 9006 RTP/AVP 0 100\r\na=rtpmap:0 pcmu/8000\r\na=rtpmap:100 telephone-event/8000\r\na=fmtp:100 0-15\r\n");
    SdpSession session;
    session.decode(s);
    SdpConnection conn;
    NetworkAddress na;
    conn.setUnicast(na.getIpName());
    session.setConnection(conn);
    cerr << "Remote SDP" << session.encode().logData() << endl;

    SdpSession lSession = MediaController::instance().createSession(session);
    sessionId = lSession.getSessionId();
    cerr << "Local SDP" << lSession.encode().logData() << endl;

    //Add audio device to the session
    MediaController::instance().addDeviceToSession(sessionId, mDevice);

    MediaController::instance().addToSession(lSession, session);
    MediaController::instance().startSession(sessionId);

}

void* client(void* args)
{

cerr << "Client:" << endl;
    ATestStruct* cStruct = static_cast<ATestStruct*>(args);
    ifstream file(cStruct->fileToPlay.c_str());
    NetworkRes local(cStruct->localPort);
    NetworkRes remote(myAddress.getIpName().c_str(), cStruct->serverPort);
    Sptr<CodecAdaptor> rtpCodec = new CodecG711U();
    MRtpSession rtpSession(sessionId, local, remote, rtpCodec, 0);
    RtpPacket* packet = NULL;
    VMutex myMutex;
    myMutex.lock();
    while(1)
    {
        start.wait(&myMutex, 100);
        if(nowStart) break;
    }
    myMutex.unlock();

    // send out data until end of file
    int result;
    RtpTime prevRtpTime = rand();
    while (!file.eof())
    {
        char buf[160]; 
        // load data from file into packet
        file.read (buf, 160);
        rtpSession.sinkData(buf, 160, rtpCodec->getType());
        // simulating blocking read call
        usleep (10*1000);
        cerr << ".";
    }
    file.close();
    return 0;
}

int main()
{
    //cpLogSetPriority(LOG_DEBUG);
    cpLogSetPriority(LOG_CRIT);
    test();
    VThread thread1;
    cStruct1.fileToPlay = "a.raw";
    cStruct1.serverPort = 9000;
    cStruct1.localPort = 9006;
   
    thread1.spawn(client, &cStruct1); 

    start.signal();
    nowStart = true;

    thread1.join();
    
    sleep(1);
    return 0;
}

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000-2003 Vovida Networks, Inc.  All rights reserved.
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
static const char* const VmcpDevice_cxx_Version = 
    "$Id: VmcpDevice.cxx,v 1.2 2004/06/20 07:09:38 greear Exp $";



#include "global.h"
#include <cassert>


#include <VTime.hxx>

/* error handling */
#include <cerrno>

#include "Vmcp.h"
#include "VmcpDevice.hxx"
#include "UaConfiguration.hxx"
#include "UaFacade.hxx"
#include "DeviceEvent.hxx"
#include "cpLog.h"
#include "CodecG711U.hxx"
#include "MediaController.hxx"
#include "UaCommandLine.hxx"


                             
using namespace Vocal;
using namespace Vocal::UA;



VmcpDevice::VmcpDevice(int id)
    : MediaDevice(WAVE, AUDIO),
      hasPlayed(false),
      myId(id),
      audioActive(false),
      hookStateOffhook(false),
      serverAvailable(false),
      fwdFlag(false),
      ss(0)
{
    myVmStack = new Vmcp();
    prevRtpPacketTime.tv_usec = 0;
    myCodec = new CodecG711U();
    //myCodec = new CodecG729C();

}  // end VmcpDevice::VmcpDevice()


//***************************************************************************
// VmcpDevice::~VmcpDevice
// description:  Destructor
//***************************************************************************


VmcpDevice::~VmcpDevice(void)
{
    cpLog(LOG_DEBUG, "VmcpDevice::~VmcpDevice(void)");
    if (ss) {
        myVmStack->sendClose();
        close(ss);   
    }
    delete myVmStack;
} // end VmcpDevice::~VmcpDevice()


#warning "Port to non-blocking, non-threaded model."
void
VmcpDevice::vmThread ()
{
    // process forever on behalf of VmcpDevice hardware
    fd_set readfds;
    struct timeval tv;
    int retval;
    int maxFd = 128;  

    if(ss <= 0) 
    {
       //vusleep(50000);
        return;
    }
    // reset file descriptor
    FD_ZERO(&readfds);
    addToFdSet(&readfds);

    // block on select for asyncronous events, but poll to process
    // audio and signal requests from endpoints in message queue
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    if ((retval = select(maxFd, &readfds, 0, 0, &tv)) < 0)
    {
        cpLog(LOG_ERR, "select() returned with an error" );
    }
    else
    {
        if (process(&readfds) < 0)
        {
            cpLog(LOG_ERR, "hardware encountered an error");
        }
    }

}

void
VmcpDevice::processAudio ()
{
    if ( (!audioActive) || (!hookStateOffhook) || hasPlayed )
    {
       //vusleep(50000);
       return ;
    }

    int wait = networkPktSize - (vm_gettimeofday() - nextTime);
 
    if ( wait > 0 )
    {
       //vusleep(wait*1000);
    }
 
    nextTime += networkPktSize;
 
    //Since we wakeup, see if any of the device state has changed
    if ( (!audioActive) || (!hookStateOffhook) || hasPlayed )
    {
        return ;
    }

    char buffer[1024];
    memset(buffer, 0xFE, networkPktSize*8);
    if ( !player.getData(buffer, networkPktSize*8) )
    {
       if(player.isListEmpty())
       {
         cpLog(LOG_DEBUG, "Done playing");
         myVmStack->playStopped();
         hasPlayed = true;
       }
    }

    if(!hasPlayed)
    {
        processRaw((char*)buffer, networkPktSize*8, G711U, NULL, false);
    }
}




//***************************************************************************
// VmcpDevice::process
//
// description:  process any events from vmserver of type VMCP. 
//               Like play the file, start recording, stop recording etc.             
//***************************************************************************

int
VmcpDevice::process (fd_set* fd)
{ 
    if ( (hookStateOffhook == true) && (ss > 0 ))
    {
        if (FD_ISSET(ss, fd))
        {
            int Msg;
	    Msg=myVmStack->getMsg();

            if ( Msg < 1 )  
            {
                reportEvent(DeviceEventHookDown);
                hookStateOffhook = false;
		close(ss);
                ss = -1;
                return 0;
            }
            switch (Msg)
            {
		case Vmcp::Close:
		{
                    cpLog(LOG_DEBUG,"VMCP:Close");
                    reportEvent(DeviceEventHookDown);
                    hookStateOffhook = false;
		    ///myVmStack->sendClose();
		    close(ss);
                    ss = -1;
		}
		break;
		case Vmcp::PlayFile:
                    cpLog(LOG_DEBUG,"VMCP:Playing file %s",myVmStack->getPlayFileName().c_str());
                    player.add(myVmStack->getPlayFileName());
                    break;

                case Vmcp::StartPlay:
                    cpLog(LOG_DEBUG,"VMCP:Start player");
                    if( !player.start() )
		    {
			myVmStack->playStopped();
		    }
		    else
		    {
		        hasPlayed = false;
		        nextTime = vm_gettimeofday();
		    }
                    break;

                case Vmcp::RecordFile:
                    cpLog(LOG_DEBUG,"VMCP:RecordFile");
                    recorder.open(myVmStack->getRecordFileName());
                    break;

                case Vmcp::StartRecord:
                    cpLog(LOG_DEBUG,"VMCP:StartRecord");
                    recorder.start();
		    nextRecTime = vm_gettimeofday();
                    break;

                case Vmcp::StopRecord:
                    cpLog(LOG_DEBUG,"VMCP:StopRecord");
                    recorder.close();
                    break;

                case Vmcp::StopPlay:
                    cpLog(LOG_DEBUG,"VMCP:StopPlayer");
                    player.stop();
		    if(player.isListEmpty())
		    {
		        hasPlayed = true;
		    }
                    break;     
                case Vmcp::Ack:
                    cpLog(LOG_DEBUG,"VMCP:Ack");
	            break;	
		default:
                    cpLog(LOG_DEBUG_STACK,"VMCP:Unknown command");
		    break;               
            }

        }
    }
    return 0;
} // end VmcpDevice::process()

int
VmcpDevice::addToFdSet (fd_set* fd)
{
    if( hookStateOffhook && (ss > 0 ))
    {
	// set the VM controller to active
        FD_SET(ss,fd);     
    }

    return 0;
} // end VmcpDevice::addToFdSet()


int
VmcpDevice::start(VCodecType codec_type)
{
    if ( audioActive )
    {
        cpLog(LOG_ERR, "Audio channel is already active. Ignoring");
        return 0;
    }

    MediaDevice::start(codec_type);

    //Connect to the VM server
    if( connectToVmServer() < 0)
    {
        cpLog(LOG_ERR, "failed to connect to the VmServers");
        return 0;
    };

    // allocate RTP packet spaces
    networkPktSize = atoi(UaConfiguration::instance().getValue(NetworkRtpRateTag).c_str());

    // mark audio as active
    cpLog(LOG_DEBUG, "setting audio active");
    nextTime = vm_gettimeofday();
    nextRecTime = vm_gettimeofday();
    audioActive = true;
    hasPlayed = false;

    return 0;
} 

int
VmcpDevice::stop (void)
{
    if (!audioActive) 
    {
         return 1;
    }

    MediaDevice::stop();

    // mark audio as deactivated.
    cpLog(LOG_DEBUG, "Audio Stop received.");
    audioActive = false;
    hookStateOffhook = false;
    
    ///reportEvent(DeviceEventHookDown);

    player.stop();
    recorder.close();

    myVmStack->sendClose(); 
	
    close(ss);
    ss = -1;
    cpLog(LOG_DEBUG,"End of session");  
    hasPlayed = false;

    return 0;
} 

void
VmcpDevice::sinkData(char* data, int length, VCodecType type,
                     Sptr<CodecAdaptor> codec, bool silence_pkt)
{
    cpLog(LOG_DEBUG_STACK, "Sink Data: length %d", length);
    if(type == DTMF_TONE)
    {
        char digit;
        memcpy(&digit, data, 1);

	switch ( digit )
	  {
	  case 0:
	    digit = '0';
	    break;
	  case 1:
	    digit = '1';
	    break;
	  case 2:
	    digit = '2';
	    break;
	  case 3:
	    digit = '3';
	    break;
	  case 4:
	    digit = '4';
	    break;
	  case 5:
	    digit = '5';
	    break;
	  case 6:
	    digit = '6';
	    break;
	  case 7:
	    digit = '7';
	    break;
	  case 8:
	    digit = '8';
	    break;
	  case 9:
	    digit = '9';
	    break;
	  case 10:
	    digit = '*';
	    break;
	  case 11:
	    digit = '#';
	    break;
	  default:
	    cpLog(LOG_ERR,"Unrecognized DTMF Received");
	    return;
	  }

        cpLog( LOG_DEBUG, "***  DTMF %c  ***", digit );
        myVmStack->sendDtmf(digit);
        return;
    }

    //Synchronize writing
    if ( (!audioActive) || (!hookStateOffhook) )
    {
       //vusleep(30000);
        return ;
    }
    int wait = networkPktSize - (vm_gettimeofday() - nextRecTime);
    if ( wait > 0 )
    {
       //vusleep(wait*1000);
    }
    nextRecTime += networkPktSize;

    if (type != myCodec->getType())
    {
        //First convert the data from type to myType and then feed
        //it to the soundcard
        //cpLog(LOG_ERR, "Need codec conversion!!!!");
        if (codec.getPtr() == 0) {
           codec = MediaController::instance().getMediaCapability().getCodec(type);
        }
        ///Convert from codec type to PCM
        int decLen = 1024;
        char decBuf[1024];
        int decodedSamples = 0;
        int decodedPerSampleSize = 0;
        codec->decode(data, length, decBuf, decLen, decodedSamples, decodedPerSampleSize);
        cpLog(LOG_DEBUG, "declength: [%d]", decLen);
        cpLog(LOG_DEBUG, "decbuf: [%d]", decBuf);
          
        int encLen = 1024;
        char encBuf[1024];
        myCodec->encode(decBuf, decodedSamples, decodedPerSampleSize, encBuf, encLen);
        cpLog(LOG_DEBUG, "enclength: [%d]", encLen);
        cpLog(LOG_DEBUG, "encbuf: [%d]", encBuf);
        recorder.write( (unsigned char*) encBuf, encLen);
    }
    else
    {
        /*  
        Sptr <CodecAdaptor> cAdp = new CodecG729C();
    
        int decLen = 1024;
        char decBuf[1024];    
        int encLen = 1024;
        char encBuf[1024];
        
        myCodec->decode(data, length, decBuf, decLen);
        
        cpLog(LOG_DEBUG, "G711Decoded Length: [%d]", decLen);
        cpLog(LOG_DEBUG, "G711Decoded Data:   [%s]", decBuf);
        
        cAdp->encode(decBuf, decLen, encBuf, encLen);
        
        cpLog(LOG_DEBUG, "G729Encoded Length: [%d]", encLen); 
        cpLog(LOG_DEBUG, "G729Encoded Data: [%s]", encBuf);
        
                
        cAdp->decode(data, length, decBuf, decLen);
        
        cpLog(LOG_DEBUG, "G729Decoded Length: [%d]", decLen);
        cpLog(LOG_DEBUG, "G729Decoded Data: [%d]", decBuf); 

        recorder.write( (unsigned char*) decBuf, decLen);
        */
        recorder.write( (unsigned char*) data, length);
        cpLog(LOG_DEBUG, "enclength: [%d]", length);
        cpLog(LOG_DEBUG, "Data: [%d]", data);
    } 
}


void
VmcpDevice::provideCallInfo(Data sCallerId, Data sCalleeId, Data sForwardReason)
{
    cpLog(LOG_DEBUG, "Call from %s to %s, reason %s", sCallerId.c_str(), sCalleeId.c_str(), sForwardReason.c_str());

    CallerId = sCallerId;
    CalleeId = sCalleeId;
    if ( CalleeId == "" ) CalleeId = "5000";
    if ( CallerId == "" ) CallerId = "unknown";

    cpLog(LOG_DEBUG,"Sending call info");   
    if ( sCalleeId == "5000" )
    {
        cpLog(LOG_DEBUG," Direct call to 5000");
        NumberOfForwards = 0;
        ForwardReason = "UNK";
    }
    else if ( sForwardReason == "" || sForwardReason == "Unknown" && !fwdFlag )
    {
	cpLog(LOG_DEBUG," Forward reason unknown");
        NumberOfForwards = 0;
        ForwardReason = "UNK";
    }
    else
    {
	cpLog(LOG_DEBUG," Forward No Answer");
        NumberOfForwards = 1;
        ForwardReason = "FNA";
    }

}

void
VmcpDevice::reportEvent(DeviceEventType eventType )
{
    cpLog( LOG_INFO, "Got device event: %d", eventType );
    Sptr < DeviceEvent > event = new DeviceEvent();
    event->type = eventType;
    event->id = myId;
    UaFacade::instance().queueEvent(event.getPtr());
}

int
VmcpDevice::connectToVmServer()
{
    int retVal = -1;
    if( ss > 0 )
    {
        cpLog(LOG_ERR, "Already active!!! (%d)", ss);
        return 0 ;
    }

    /* Redundancy : Try to connect to the one of the Vmservers
     * configured in the config file.
     */
    const list<string>& vmServers=UaConfiguration::instance().getVmServers();

    if(vmServers.size() == 0)
    {
        // there are NO VM servers configured.
        cpLog(LOG_ALERT, "No VmServers are configured in the config file.");
        return (-1);
    }

    for( list<string>::const_iterator itr = vmServers.begin();
                itr != vmServers.end(); itr++)
    {   
		if(UaCommandLine::instance()->getIntOpt("vmadmin")) {
        cpLog(LOG_DEBUG,"Trying to connect to vmadin %s:%d",(*itr).c_str(),9024);
        if( myVmStack->connect((*itr).c_str(),9024) )
        {
           cpLog(LOG_DEBUG,"Server connected");
           ss = myVmStack->getFd();
           myVmStack->setSessionInfo(CallerId.c_str(),CalleeId.c_str(),"5000",ForwardReason.c_str(),NumberOfForwards); 
           hookStateOffhook = true;
           reportEvent(DeviceEventHookUp);
           retVal = 0;
           break;
        }
        else
        {
            cpLog(LOG_DEBUG,"Server not responding, trying the next one");
        }
    } else {
		cpLog(LOG_DEBUG,"Trying to connect to vmserver %s:%d",(*itr).c_str(),8024);
        if( myVmStack->connect((*itr).c_str(),8024) )
        {
           cpLog(LOG_DEBUG,"Server connected");
           ss = myVmStack->getFd();
           myVmStack->setSessionInfo(CallerId.c_str(),CalleeId.c_str(),"5000",ForwardReason.c_str(),NumberOfForwards); 
           hookStateOffhook = true;
           reportEvent(DeviceEventHookUp);
           retVal = 0;
           break;
        }
        else
        {
            cpLog(LOG_DEBUG,"Server not responding, trying the next one");
        }
    }
}
		
    return retVal;
}

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


#include "global.h"
#include "PlayQueue.h"
#include <unistd.h>
#include <fcntl.h>
#include "cpLog.h"
#include <errno.h>
#include "../../rtp/codec/g711.h"  // for linear2ulaw conversion.

PlayQueue::PlayQueue()
{
    m_iFd = 0;
    m_bActive = false;
    loop_wavefile_count = 1;
}


PlayQueue::~PlayQueue()
{
    m_bActive = false;
    if(m_iFd) ::sf_close(m_iFd);
}


void PlayQueue::add(const string & fileName)
{
    cpLog(LOG_DEBUG, "added %s to play queue", fileName.c_str());
    m_xFileQueue.push(fileName);
}


int PlayQueue::start()
{

    if( m_bActive )
    {
	return true;
    }

    m_bActive = true;

    loop_wavefile_sofar = 0;
    
    return reStart();
}

int PlayQueue::reStart() {
    while ( !m_xFileQueue.empty() ) {
        string fileName = m_xFileQueue.front();
        cpLog(LOG_DEBUG, "Opening: %s" , fileName.c_str());
        m_xFileQueue.pop();
	cpLog(LOG_DEBUG,"Next file in queue %s", fileName.c_str());

        loop_wavefile_sofar++;

        if ((loop_wavefile_count == 0) ||
            (loop_wavefile_sofar < loop_wavefile_count)) {
            // Push on end of queue, we'll play it again...
            cpLog(LOG_DEBUG, "Re-queueing wave file -:%s:-, loop_wavefile_count: %d  sofar: %d\n",
                  fileName.c_str(), loop_wavefile_count, loop_wavefile_sofar);
            m_xFileQueue.push(fileName);
        }
        else {
            cpLog(LOG_DEBUG, "NOT Re-queueing wave file -:%s:-, loop_wavefile_count: %d  sofar: %d\n",
                  fileName.c_str(), loop_wavefile_count, loop_wavefile_sofar);
        }

        // sf_open_read figures out the file type...
        memset(&read_info, 0, sizeof(read_info));

#ifndef NEW_SNDFILE
        m_iFd = ::sf_open_read(fileName.c_str(), &read_info);
#else
        m_iFd = ::sf_open(fileName.c_str(), SFM_READ, &read_info);
#endif
	if ( m_iFd != NULL )
	{
            cpLog(LOG_DEBUG, "Opened: %s" , fileName.c_str());
            m_bActive = true;
	    return m_bActive;
	}
	else
	{
            char errBuf[128];
            sf_error_str(m_iFd, errBuf, 128);
	    cpLog(LOG_ERR, "cannot open file %s: %s", fileName.c_str(), errBuf);
            // Continue, maybe next is queue can be played...
	}
    }
    cpLog(LOG_DEBUG, "finished playing queue");
    m_bActive = false;
    return false;
}


bool PlayQueue::getData(unsigned char *buffer, int bytes_to_read) {
    if ( !m_bActive ) {
	//cpLog(LOG_DEBUG, "tried to get data when not playing");
	return true;
    }

    int samples = bytes_to_read;
    int rc;
    short tmpbuf[samples];
    rc = ::sf_read_short(m_iFd, tmpbuf, samples);

    /* Copy PCM audio data into sendBuf as ulaw audio data*/
    for (int i = 0; i < rc; i++) {
       buffer[i] = (unsigned char) linear2ulaw(tmpbuf[i]);
    }

    if ( rc != samples ) {
	cpLog(LOG_DEBUG, "finished reading sound file");
        char* cbuf = (char*)(buffer);
        memset(cbuf + rc, 0x7F, samples - rc);
        ::sf_close(m_iFd);
        m_iFd = 0;
        return reStart();
    }

    return true;
}


void PlayQueue::stop() {
   m_bActive = false;
   while ( !m_xFileQueue.empty() ) {
      m_xFileQueue.pop();
   }
}

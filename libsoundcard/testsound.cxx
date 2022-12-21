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

#include "SoundCard.hxx"
#include "cpLog.h"
#include "rtpTypes.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

using Vocal::UA::SoundCard;
using Vocal::UA::SoundCardFormat;



void record(const char* filename)
{
    SoundCard mySoundCard("/dev/dsp");

    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", "" );
        exit( -1 );
    }

    // set audio API type
    cout << "sound card format: " << mySoundCard.getFormat() << endl;

    cpLog(LOG_DEBUG, "Opened audio device");

    cout << "Press return to stop recording" << endl;

    int wav = creat(filename, 0666);

    int cc;

    while (1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        
        struct timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval)
            break;

	unsigned char dataBuffer[1024];
	cc = mySoundCard.read( dataBuffer, 10 );
	if(cc > 0)
	{
	    write(wav, dataBuffer, cc);
	}
    }

    cout << "\ndone" << endl;

    close(wav);
    mySoundCard.close();
}


void play(const char* filename, bool noAutoStop )
{
    SoundCard mySoundCard("/dev/dsp");

    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", "" );
        exit( -1 );
    }

    // set audio API type
    cout << "sound card format: " << mySoundCard.getFormat() << endl;

    cpLog(LOG_DEBUG, "Opened audio device");

    cout << "Press return to stop playing" << endl;

    int wav = open(filename, O_RDONLY);

    if (wav < 0)
    {
        perror("failed to open file");
    }

    int bytes = 0;
    unsigned char dataBuffer[1024];

    bool done = false;

    while (1)
    {
        bytes = read(wav, dataBuffer, 160);
        if(bytes > 0)
        {
            mySoundCard.write ( dataBuffer, bytes );
        }
        else if (bytes == 0) 
        {
            done = true;
        }

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        
        struct timeval tv;

        if(done)
        {
            if(mySoundCard.writerBuffered())
            {
                tv.tv_sec = 0;
                tv.tv_usec = 200000;
            }
            else
            {
                break;
            }
        }
        else
        {
            tv.tv_sec = 0;
            tv.tv_usec = 0;
        }

        int retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval)
        {
            cout << "pressed return" << endl;
            break;
        }

    }

    if(bytes < 0)
    {
        perror("error reading");
    }

    if(noAutoStop)
    {
        cout << "press return to stop audio" << endl;
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        
        select(1, &rfds, NULL, NULL, NULL);
    }
    close(wav);
    mySoundCard.close();
    cout << "\ndone" << endl;
}


void silence()
{
    SoundCard mySoundCard("/dev/dsp");

    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", "" );
        exit( -1 );
    }

    // set audio API type
    cout << "sound card format: " << mySoundCard.getFormat() << endl;

    cpLog(LOG_DEBUG, "Opened audio device");

    cout << "Press return to stop silence" << endl;

    unsigned char dataBuffer[100];

    for(int i = 0; i < 100; i+=1)
    {
        dataBuffer[i] = 0x7f;
    }

    while (1)
    {
        mySoundCard.write ( dataBuffer, 100 );

        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        
        struct timeval tv;

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval)
        {
            cout << "pressed return" << endl;
            break;
        }
    }

    mySoundCard.close();
}


void duplex_record(const char* write_filename, const char* read_filename)
{
    SoundCard mySoundCard("/dev/dsp");

    // open audio hardware device
    if( mySoundCard.open() == -1 )
    {
        cpLog( LOG_ERR, "Failed to open %s", "" );
        exit( -1 );
    }

    // set audio API type
    cout << "sound card format: " << mySoundCard.getFormat() << endl;

    cpLog(LOG_DEBUG, "Opened audio device");

    cout << "Press return to stop recording (full duplex)" << endl;

    unsigned char dataBuffer[100];

    for(int i = 0; i < 100; i+=1)
    {
        dataBuffer[i] = 0x7f;
    }

    int wav = creat(write_filename, 0666);
    int cc;
    int readfd = -1;

    if(read_filename)
    {
        readfd = open(read_filename, O_RDONLY);

        if (readfd < 0)
        {
            perror("failed to open file");
        }
    }

    while (1)
    {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        
        struct timeval tv;

        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int retval = select(1, &rfds, NULL, NULL, &tv);

        if (retval)
        {
            cout << "pressed return" << endl;
            break;
        }

	unsigned char recBuffer[10];
//        cout << "a" << endl;
	cc = mySoundCard.read( recBuffer, 10 );
//        cout << "z" << endl;
	if(cc > 0)
	{
	    write(wav, recBuffer, cc);
	}
        else if (cc == 0)
        {
            cout << "no data!" << endl;
        }
        else
        {
            perror("error reading from sound card");
        }
        int readcount;
        unsigned char readBuffer[10];
        if(readfd >= 0)
        {
            if ((readcount = read(readfd, readBuffer, 10)) > 0)
            {
//                cout << "1" << endl;
                mySoundCard.write ( readBuffer, 10 );
//                cout << "2" << endl;
            }
            else
            {
                perror("error reading from file");
            }
        }
        else
        {
//            cout << "3" << endl;
            mySoundCard.write ( dataBuffer, 10 );
//            cout << "4" << endl;
        }

    }

    close(wav);
    close(readfd);
    mySoundCard.close();
}


void print_usage()
{
    cout << "Usage: testsound option\n"
         << "\n"
         << "Where option is one of:\n\n"
         << "-r filename    record to a file\n"
         << "-p filename    play from a file and quit when it has been played\n"
         << "-P filename    play from a file but continue to play until RETURN is pressed\n"
         << "-s             play silence\n"
         << "-d filename    full duplex recording using silence, write to filename\n"
         << "-D filename    full duplex recording using filename as read file, write to\n"
         << "               testfile.raw\n";
    exit(0);
}

int main(int argc, char *const * argv)
{
    char c;
    while((c = getopt (argc, argv, "P:r:p:sd:D:u")))
    {
        switch(c)
        {
	case 'u':
		cpLogSetPriority(LOG_DEBUG);
		break;
        case 'r':
            record(optarg);
            exit(0);
            break;
        case 'p':
            play(optarg, false);
            exit(0);
        case 'P':
            play(optarg, true);
            exit(0);
        case 's':
            silence();
            exit(0);
        case 'd':
            duplex_record(optarg, 0);
            exit(0);
        case 'D':
            duplex_record("testfile.raw", optarg);
            exit(0);
        default:
            print_usage();
            break;
        }
    }
    print_usage();

    return 0;
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

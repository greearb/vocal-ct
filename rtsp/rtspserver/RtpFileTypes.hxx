#ifndef RtpFileTypes_HXX
#define RtpFileTypes_HXX
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
 * IN EXCESS OF 281421,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const RtpFileTypes_hxx_version =
    "$Id: RtpFileTypes.hxx,v 1.1 2004/05/01 04:15:23 greear Exp $";


#include <sys/stat.h>
#include <string>

/** number of codes in codec table */
extern int myNumberOfCodecs;

/** Basic structure for one codec entry. */
struct RtpFileTypeInfo
{
    /** codec name - used in rtpmap */
    std::string name;

    /** rtp payload type */
    int payloadType;

    /** frame interval in milliseconds */
    int intervalMs;

    /** encoding tag in .au-file,  0x0000 if format tag doesn't exists */
    unsigned int AuFileFormat;

    /** encoding tag in .wav-file, 0x0000 if format tag doesn't exists */
    unsigned int WavFileFormat;

    /** bytewidth - used in libSnd */
    int bytewidth;

    /** packetSize in bytes */
    int packetSize;

    /** rtp time interval */
    int samples;
};


//TODO use only hex numbers in AuFileFormat and WavFileFormat
/** List of supported codecs and their parameters */
static RtpFileTypeInfo rtpFileTypeInfo[] =
{
    // G726R16, G726R24, G726R40 and GSMEFR can be dynamic payload type
    // G726R40 need to be added
    { "UNKNOWN",     -1,     0,  0x0000, 0x0000,    0,    0,    0 },
    { "PCMU",         0,    30,  0x0001, 0x0007,    1,  240,  240 },
    { "PCMU",         0,    20,  0x0001, 0x0007,    1,  160,  160 },
    { "PCMA",         8,    20,  0x001B, 0x0006,    1,  160,  160 },
    { "PCMA",         8,    30,  0x001B, 0x0006,    1,  240,  240 },
    { "L16M",        10,    20,  0x0003, 0x0001,    2,  320,  160 },
    { "G723R63",      4,    30,      48,   5348,   24,   24,  240 },
    { "G723R53",      4,    30,      47,   5347,   20,   20,  240 },
    { "G723AR63",     4,    30,      50,   5350,   24,   24,  240 },
    { "G723AR53",     4,    30,      49,   5349,   20,   20,  240 },
    { "G726R16",     23,    20,      51,   5351,   40,   40,  160 },
    { "G726R24",     22,    20,      52,   5352,   60,   60,  160 },
    { "G726R32",      2,    20,      53,   5353,   80,   80,  160 },
    { "G726R32",      2,    20,      23, 0x0002,   80,   80,  160 },
    { "G728",        15,    20,      55,   5355,   20,   40,  160 },
    { "G729",        18,    20,      39,   5339,   20,   20,  160 },
    //{ "G729A",       18,    20,      40,   5340,   20,   20,  160 },
    { "G729B",       18,    20,      42,   5342,   20,   20,  160 }, 
    //{ "G729AB",      18,    20,      41,   5341,   20,   20,  160 }, 
    //{ "G729IETF",    18,    20,      43,   5343,   20,   20,  160 },
    { "GSM",          3,    20,      44,   5344,   33,   33,  160 },
    { "GSMFR",        3,    20,      44,   5344,   34,   34,  160 },
    { "GSMEFR",      20,    20,      45,   5345,   32,   32,  160 }
};

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */
#endif

====================================================================
README_RTP_1.5.0
====================================================================
Real Time Transport Protocol Library
Release 1.5.0
April 1, 2003

====================================================================
LICENSE AND COPYRIGHT 
====================================================================
The Vovida Software License, Version 1.0
Copyright (c) 2003 Vovida Networks, Inc.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:

1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.

3. The names "VOCAL", "Vovida Open Communication Application Library",
   and "Vovida Open Communication Application Library (VOCAL)" must
   not be used to endorse or promote products derived from this
   software without prior written permission. For written
   permission, please contact vocal@vovida.org.

4. Products derived from this software may not be called "VOCAL", nor
   may "VOCAL" appear in their name, without prior written
   permission of Vovida Networks, Inc.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
--------------------------------------------------------------------
All third party licenses and copyright notices and other required
legends also need to be complied with as well.

====================================================================
INTRODUCTION
====================================================================
RTP protocal provides end-to-end network transport functions suitable
for applications transmitting real-time data, such as audio and video,
over multicast or unicast network services.  RTP is augmented by a
control protocal (RTCP) to monitor data delivery and network statistics.
Together they try to resolve many of the problems an UDP network 
environment may experience, such as lost packets, jitter, and out of 
sequence packets.

This Vovida RTP/RTCP library is intended to implement Real-time Transport 
Protocol version 2 which is specified by RFC1889 of Januanry 1996.

The Vovida Implementation includes the following:
* End to end network transport of any audio and video format
* Unicast service only
* Sending RTCP status and quality reports
* Sending RTCP SDES information
* Resolves lost packets, jitter, and out of sequence packets
* DTMF in RTP (RFC 2833)


====================================================================
GETTING STARTED
====================================================================

COMPILE INSTRUCTIONS
--------------------------------------------------------------------
To make the RTP stack:
From rtp_x_x_x
    make rtp

To make RTP demo/test programs:
From rtp_x_x_x
    make rtp_demo

The executable will be in rtp_x_x_x/test/bin.debug.i686
To run the test programs, please see the README in the rtp/test directory.

For advance topics on build refer to doc/README_BUILD.txt.

====================================================================
Hardware/Software requirements
====================================================================
Refer to http://www.vovida.org/applications/downloads/vocal/platform/1_5_0.html
for details.

====================================================================
SOURCE CODE INFORMATION
====================================================================
Refer to programmer's documentation for RTP stack classes and interface details.


====================================================================
DIRECTORY STRUCTURE 
====================================================================
Relative to the top directory rtp_x_x_x:
   ./rtp             - Implementation of RTP/RTCP stack
   ./rtp/codec       - code for G711, G723 codecs
   ./rtp/test        - Unit test cases and sample applications using stack.
   ./rtp/unit_test   - Automated test cases run by make verify.

======================================================================

Copyright 2000-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.

$Id: README_RTP.txt,v 1.1 2004/05/01 04:15:30 greear Exp $

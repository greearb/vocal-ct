$Id: README_SIP.txt,v 1.1 2004/05/01 04:15:30 greear Exp $

====================================================================
README_SIP_1.5.0
====================================================================
SIP stack and applications
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
The package contains the SIP stack implementation and a set of SIP
user agent(SIPset) and a Back-to_back(B2b) user agent sample 
applications.

SIP stack:
----------
Session Initiation Protocol (SIP), defined in RFC 2543, is an
application-layer control protocol that can establish, modify and
terminate multimedia sessions or calls. These multimedia sessions
include invitations to both unicast and multicast conferences and
Internet telephony applications.

This SIP stack,written in c++,is fully compliant with RFC 2543 (Session
Initiation Protocol, SIP). The stack implements support for both TCP and UDP
transport protocols. The stack also contains implementations of the 
following draft extensions:

- 183 Session Description message
- INFO message
- OPTIONS message
- Multiple MIME support for ISUP messages
- SUBSCRIBE message
- NOTIFY message
- REFER message
- MESSAGE message

The SIP stack can be used to building applications such as:
- SIP user agents
- SIP proxies
- SIP redirect servers

SIPSet ( SIP user Agent:
------------------------
sipset is a graphical user agent that uses Vovida SIP stack.
See the manpage in manpages/generated_html/sipset.html 
for screenshots. 
sipset can also be run in command-line mode with the command 
#> gua -f <configuration file>

A sample configuration file called sample-gua.cfg is in the source
directory. Changes made in the configuration screens of sipset
are saved in this file. This file can also be edited by hand.
See the manpage in manpages/generated_html/sipset.html 
for more details.


b2bua (Back-to-back user agent):
-------------------------------
B2bUa is basically two UAs working back-to-back and thus controlling
the calls going through it.Unlike a sip-proxy, a B2bUa will take an
incoming call, modify it and present it in other form to final
destination.B2bUa is in the call-signalling-path all the times.
There are numerous applications in which b2bUa can be used either
as a media negotiator or simply supervising the calls between two
parties.The software provides a basic framework for B2Bua and we have
added a prepaid billing application on top of that framework.

   -----------------------
  |   PrePaid Application |
  |_______________________|
  |    B2b Base           |
  |_______________________|

The B2bUa can be confgured either by using the file b2bConfig.xml. Refer to
manpage in manpages/generated_html/b2bua.html for details on 
running b2bua.


====================================================================
GETTING STARTED
====================================================================

COMPILE INSTRUCTIONS
--------------------------------------------------------------------
To make the SIP stack and applications:
    make 


For advance topics on build refer to doc/README_BUILD.txt.

====================================================================
Hardware/Software requirements
====================================================================
Refer to http://www.vovida.org/applications/downloads/vocal/platform/1_5_0.html
for details.

====================================================================
SOURCE CODE INFORMATION
====================================================================
Refer to programmer's documentation for SIP stack classes and interface details.

====================================================================
HOW TO RUN
====================================================================
Refer to manpages/generated_man for man pages on applications
b2bua and SIPset.


====================================================================
DIRECTORY STRUCTURE 
====================================================================
Relative to the top directory sip_x_x_x:
   ./sip/sipstack           - Implementation of SIP stack.
   ./sip/examples           - Example code for sending/receiving SIP messages using
   ./sip/test               - Unit test cases and sample applications using stack.
   ./sip/test/performance   - Set of test cases to measure the performance of
                              sip stack.
   ./sip/test/unit_test     - Automated test cases run by make verify.
                              stack.
                              to run the test and see the report
                              cd ./sip/test/unit_test
                              make verify 

   ./sip/base               - The SIP Base Proxy provides a template for designing 
                              and building applications such as Proxy Servers to process 
                              messages received from the VOCAL SIP stack.
                              It contains a number of base classes that the application can 
                              derive from. These include proxy, thread and event classes as 
                              well as a hierarchical builder, feature, state, operator class 
                              structure for the event processing.  Components of the VOCAL 
                              system such as the Marshal Server are based on this code.


   ./sip/libsipua           - Implements the user agent state machine
   ./sip/gua/gui            - Implements the gtk based Graphical user interface
                              for user agent. The sipset GUI communicates with gua 
                              using two named sockets, located in /tmp. 
   ./sip/gua                - Implements the SIP user agent used by the GUI.
   ./sip/ua                 - Old implementation of SIP user agent. As of 1.5.0 the

   ./sip/b2b                - Implements the back-to-back user agent.
   ./sip/b2b/controlState   - Implements the b2bua application state machine
                              code is obsolete and no more supported.

======================================================================

Copyright 2000-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.


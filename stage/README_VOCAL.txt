==================================================================== 
                         VOCAL Readme
==================================================================== 
 
 Software release number: VOCAL 1.5.0
 Software release date: April 1, 2003

 Readme release version: 1.5.0
 Readme release date: April 1, 2003

 VOCAL is licensed under the Vovida Software License, Version 1.0
 See the LICENSE AND COPYRIGHT section  at the end of this document.

====================================================================

                      INTRODUCTION to VOCAL

====================================================================

The Vovida Open Communication Application Library (VOCAL) is an open
source project targeted at speeding the adoption of Voice over
Internet Protocol (VoIP) implementations by helping developers within
the community build VoIP features, applications and services. The
VOCAL software includes a Session Initiation Protocol (SIP) based
Redirect Server (RS), Feature Server (FS), Provisioning Server (PS)
,Marshal Server (MS) and Voice Mail Server (vmserver).

Applications not included included in this release:
1. SIP to MGCP translator
2. Policy server
3. Inet/Conference proxy server.
4. SNMP/NetMgnt
5. SIP to H323 Translator

FEATURE SERVER (fs):

The Feature Server is a SIP proxy in the VOCAL system that executes
one of the features enabled from Vocal Provisioning.   The Feature
Server supports the following CPL scripts generated from Vocal
Provisioning:

      Calling Features                   Called Features
      ================                   ===============
    - 900/976 Blocking                 - Call Return
    - Caller ID Blocking               - Call Screening
                                       - Call Forward All Calls
                                       - Call Forward No Answer/Busy


To manually insert your own script:

1)  From Vocal Provisioning, enable a feature for a user.
    A Called feature should be enabled if your script is to be
    done by the called party.  A Calling feature should be
    enabled if your script is to be executed for the calling
    party.   Feature interaction is shown under "INTRODUCTION."
2)  On your Provisioning host (including redundancy if using it),
    search the provisioning data for the CPL script that was
    generated for the user.
    E.g:
    /usr/local/vocal/provisioning_data/AAA/Features_<IP_PORT>/5000.cpl.xml
3)  Replace the contents of this script with your new script.
4)  Run or restart the Feature Server executable.


MARSHAL SERVER (ms):

Marshal Servers are SIP proxies which surround the VOCAL system.
All external SIP entities must go through a Marshal to exchange SIP
messages with the rest of the VOCAL system.
Any SIP message entering or exiting the VOCAL system passes through
a Marshal. A Marshal acts as a central point to perform any checks,
translations, logging, etc. of SIP messages between the VOCAL
system and all external SIP entities.
In the simplest case, the Marshal may do nothing but forward the
message to the Redirect Server (incoming) or external SIP proxy
(outgoing). In a more complex and realistic case, a Marshal may
need to deal with one or more of the following:

o authentication
o security
o billing
o SIP message translation/rewriting
o heartbeat with redirect server
o load balance incoming requests among redirect servers
o maintenance check of external equipment/connections

There are four types of Marshals, each performing a specialized
task. They are as follows:

o User Agent Marshal
o Gateway Marshal
o Confernece Marshal
o Internetwork Marshal


REDIRECT SERVER (RS):

The Redirect Server (RS) is designed to store and maintain
SIP Registration information for configured users as well as to
provide next hop contact information in response to a SIP Invite
message.

   Processing an Invite requires the RS to perform several steps.
First it generates a contact list by taking the Invite's From and
Request Uri fields, searches for them in the RS's databases and
collects the resulting feature and terminating contacts. The
Invite's Via field is then used to determine the call's current
location in the generated contact list. Once found, the next
contact in the list can be validated and returned in the Status
Response message.

   In order to validate a contact the RS must be able to determine
if the contact's host and port belong to an active server. To do this
the RS must store and maintain data about each server in the system.
This information is obtained from the Provisioning Server (PS) at
startup time. Thereafter, each server in the system sends periodic
heartbeats on a configured multicast address. The RS monitors these
heartbeats and keeps track of which servers are currently active.

   Multiple RS's can be run on the same system, but they must
remain in sync with one another. Since user registration
information is received from Sip messages and stored only within
the RS, each RS must synchronize any user Registration information
it receives with all other RS's. For this purpose each RS has a unique
Sync Port which is configured in provisioning.

VOICE MAIL:

the Voice Mail Server (VmServer). The VmServer Consits of a SIP talking
front end called User Agent Voice Mail (UAVM), Voice Mail Feature Server
(FSVM ) and Vmserver. Uavm and VmServer talk VMCP : Voice Mail Control
Protocol, which is a proprietary protocol.  Vmserver is used to play a
recorded greeting, and to send a recorded voice message as a email
attachment to a user.

Once the request arrives at vmserver, it forks a process to serve the
request. Vm Server then plays a greeting file to the caller. The
location for the greeting file is specified in the configuration file
for voicemail server. Once the greeting file has been played, vmserver
starts recording the incoming message. Once the recording is done,
vmserver sends the message (.wav file) through email as a attachment
to user (callee) whose number has been called.

        If the callee is not configured in the system then a Invalid.wav
gets played by vmserver and then the call gets disconnected by vmserver.


for more details on Vocie mail please refer to :

/vocal/vm/README

FOR OTHER COMPONENTS OF VOCAL PLEASE REFER TO:

/vocal/README_SIP.txt
/vocal/README_RTP.txt

 For futher detailed documentations on VOCAL please check out
 documentation from cvs.vovida.org

  cvs co documentation

How do I check out source code using anonymous CVS?

for more info on above:
   http://www.vovida.org/fom-serve/cache/469.html

See the Documentation section of http://www.vovida.org for more
information about VOCAL.

==================================================================== 

TESTED PLATFORMS

==================================================================== 

Please refer to :

http://www.vovida.org/applications/downloads/vocal/platform/1_5_0.html

 TESTED GATEWAYS AND ENDPOINTS
-------------------------------------------------------------------- 

please refer to:

/documentation/vocal/test/test_plan.txt

to access the above document please check out documentation from cvs.vovida.org

cvs co documentation

How do I check out source code using anonymous CVS?

for more info on above:
   http://www.vovida.org/fom-serve/cache/469.html


Java
 ---------------------------------
 We have removed the requirement to install Java before compiling 
 VOCAL, however if you prefer to use the Java-based provisioning 
 system, as included in previous VOCAL releases, see the section
 in this document called INSTALLING THE TOOLS RPM.

====================================================================

 BEFORE INSTALLING

====================================================================

 If you have a previous installation of VOCAL that uses vocalstart
 to run, you must stop all servers with
      vocalstart stop

 Note that vocalstart is no longer used as of 1.4.0.

 You must be logged in as root to install VOCAL.

 There are two options for downloading and installing VOCAL:
 
 + INSTALLING FROM RPM/PKG

 + INSTALLING FROM SOURCE 

 See the sections below that pertain to your preferred installation
 method.

====================================================================

 INSTALLING FROM RPM/PKG (basic all-in-one system)

====================================================================

 Linux:
 
   Download vocalbin-<version>-x.i386.rpm from www.vovida.org

   To install the RPM, as root, type

     rpm -U vocalbin-<version>-x.i386.rpm


Configuring VOCAL
 ---------------------------------

 To set up configuration,
  On Linux: 
    /usr/local/vocal/bin/allinoneconfigure/allinoneconfigure

   Linux:

  The allinoneconfigure script starts and asks a number 
  of questions.  Answer all questions with the default 
  answers.  

  If everything goes well, the message, "Congratulations: 
  you have successfully installed VOCAL!" appears. 

  If you see an error message instead, your VOCAL system
  was not installed properly.  Please see 
  TROUBLESHOOTING.txt for help.

  The Apache web server has now been reconfigured to 
  provide basic web-based provisioning and must be 
  restarted for this to take effect. 
 
  To restart Apache run
    /etc/rc.d/init.d/httpd restart

  To use the web-based provisioning, point a web browser at
    http://<your server name>/vocal/

  You will be prompted for a password. The username is "vocal".
  During configuration, you were asked to enter a password or
  choose to have one generated for you. If you have forgotten
  the password from this step, you can regenerate one by running
  allinoneconfigure -r.

 
 Verifying the Installation
 ---------------------------------


 After you have run allinoneconfigure, make sure that your VOCAL
 system is running.  You can do this via

 On Linux:
   /usr/local/vocal/bin/vocalctl status

 make sure that you see all of the processes, like so:

    fs 5080               14957                    
    fs 5085               14955                    
    fs 5090               14983                    
    fs 5095               14982                    
    fs 5100               14958                    
    fs 5105               14956                    
    fsvm 5110             15853                    
    ms 5060               15002                    
    ms 5064               15020                    
    ms 5065               15019                    
    ps                    14927                    
    rs 5070               15285                    
    uavm 5170             14976                    
    uavm 5171             14977                    
    uavm 5172             14978                    
    uavm 5173             14979                    
    uavm 5174             14981                    
    vmserver              14966                    

 if you see

    vocald is not running.

 then your VOCAL system isn't running.

 If your VOCAL system is running, you can verify your installation by
 running verifysip.  Passing the -a option causes verifysip to create
 two test users, test1000 and test1001, and make a call from test1000
 to test1001.  After testing, verifysip will remove the two users.
 
 On Linux:
   /usr/local/vocal/bin/verifysip -a

   verifysip will ask you to enter the administrator password for your
   VOCAL system.  please enter it.  if you have forgotten your
   administrator password, you can change it using allinoneconfigure
   -r .

   If your installation is OK, you should see the following 
   text:

   VOCAL basic call test passed

   If you see an error message instead, your VOCAL system 
   is not working properly.  Please see TROUBLESHOOTING.txt 
   for help.



 Making a First Call
 ---------------------------------

 To make a first call, you can run the SIP UA, which is
 included in the VOCAL binary tree.  

 To run the SIP UA:

 please refer to:

  http://vovida.org/document/man/sipset/

====================================================================

 INSTALLING FROM SOURCE

====================================================================

 To compile and install the VOCAL source code on your machine:

 On Linux:  

   1 ./configure
   2 make
   3 make install

   If everything goes well, the message, "Congratulations: 
   you have successfully installed VOCAL!" appears. 

   If you see an error message instead, your VOCAL system
   was not installed properly.  Please see 
   TROUBLESHOOTING.txt for help.

   See the BUILD.txt file for more information about compiling 
   and installing VOCAL.

   After installing your VOCAL system, follow the instructions above
   to configure, verify the installation, and make a first call.

====================================================================

 INSTALLING SIPSET GRAPHICAL USER AGENT FROM SOURCE

====================================================================

 Please Refer to:

  /vocal/README_SIP.txt

==================================================================== 

 DEFAULT PROVISIONING

==================================================================== 

 A newly configured allinone system contains the following 
 provisioning:

     Call Blocking Feature Server on port 5080
     Call Return Feature Server on port 5095
     Call Screening Feature Server on port 5100
     Caller ID Blocking Feature Server on port 5090
     Conference Bridge Marshal Server on port 5064
     Forward All Calls Feature Server on port 5085
     Forward No Answer or Busy Feature Server on port 5105
     Gateway Marshal Server on port 5065
     Redirect Server on port 5070
     UAVM Servers on ports 5170, 5171, 5172, 5173, 5174
     User Agent Marshal Server on port 5060
     Voicemail Feature Server on port 5110

     a dial plan that sends to the PSTN gateway
     - all numbers starting with 9
     - all numbers with 6 or more digits

 To add, delete, or change users, you may use the web-based
 provisioning GUI. To enter the default PSTN gateway, you may use the
 web-based provisioning GUI.

 NOTE: If you use the web-based provisioning GUI under mozilla-based
 browsers (including Netscape 7, Netscape 8, and Mozilla), you should
 not use password management to remember any of the web forms in the
 HTML gui.  Otherwise, you may inadvertently change the user password
 when you make other GUI changes.

 In order to avoid this, make sure that you go to the Tools menu in
 Mozilla/Netscape, and choose Password Manager -> Manage Stored
 Passwords.  Make sure that the Passwords Saved tab does not contain
 any entries for the host you are using for VOCAL.

 When Netscape/Mozilla asks you if you want password manager to
 remember your password for this web site, always answer "Never for
 this site".

 For all other changes you must use the Java provisioning GUI (see
 INSTALLING THE TOOLS RPM)

==================================================================== 

 INSTALLING THE TOOLS RPM

==================================================================== 

 The Tools RPM adds the Java GUI to the system to allow you to
 provision more features of the VOCAL system. The Java GUI must be run
 from the command line.

 Previous versions ran as a Java applet from the web GUI, but the
 current version will not run in this form.

 Download the Java Plug-in from http://java.sun.com/

 For instructions on installing the Java Plugin, see Chapter 2 of the
 Installation Guide. Also look at the FAQ-O-MATIC under Common
 Problems/Java GUI for Provisioning.

 Use Internet Explorer version 5 or higher, or use Netscape version
 4.7 or higher. Other web browsers, such as KDE Konquerer and Mozilla
 should also work.

 Download vocalbin-tools-<version>-x.i386.rpm from www.vovida.org

 Install the Tools RPM with

   rpm -U vocalbin-tools-<version>-x.i386.rpm

 You must be root to do this.

 Now when you point your web browser at the location you went to for
 basic web-based provisioning, you will see additional links to
   Java Provisioning
   User Configuration
 
  *** NOTE for 1.5.0 ***

 Java provisioning does NOT work.  please see 

      http://bugzilla.vovida.org/bugzilla/show_bug.cgi?id=579

 for more information.

 See the documentation at www.vovida.org for more information about 
 these.

==================================================================== 

 INSTALLING THE ATA186 Auto-Provisioning RPM

==================================================================== 
Auto-Provisioning RPM adds capability to auto-provision an ATA186
( Cisco Analog Phone Adaptor) with Vocal system. The RPM requires the
vocal base RPM installed and vocal be running.

Download AtaAuto-<version>-x.i386.rpm from www.vovida.org

 Install the RPM with

   rpm -U AtaAuto-<version>-x.i386.rpm

 You must be root to do this.


==================================================================== 

 UNINSTALLING

==================================================================== 

 If you installed from an RPM:

  You may uninstall by typing

    rpm -e vocalbin

  This will remove some, but not all of the files under
  /usr/local/vocal. The provisioning_data directory is left intact so
  that you will not lose the provisioning from this installation.

  If you wish to remove everything, including the
  provisioning data, then, as root, type
    rm -Rf /usr/local/vocal

 If you installed from source:

 As root, type

   rm -Rf /usr/local/vocal


==================================================================== 

USING THE SOFTWARE 

==================================================================== 

This software needs to be run in retransmission mode for all
applications. Refer to each application's readme file for usage of
each process.

Command line option -r will turn off SIP retransmission. SIP
retransmission is turned on by default. It is recommended to have
retransmission turned on.


==================================================================== 

TESTING THE SOFTWARE 

==================================================================== 

Please refer to test results on VOCAL here:

http://www.vovida.org/applications/downloads/vocal/test/1_5_0.html

====================================================================

TEXT FILE DOCUMENTATION

====================================================================

 The following is a list of some important documentation included in
 the VOCAL source code archive:

   CONTRIBUTORS.txt	Lists the names of those who contributed
                        material to VOCAL and its protocol stacks

   LICENSE.txt		The text of The Vovida Software License, 
                        Version 1.0

   README_BUILD.txt     Explains technical details of building 
                        VOCAL. For a simple cookbook approach to 
                        building VOCAL, please look at README.txt

   README_SIP.txt       Explains about SIPSTACK and example Applications
                        in detail.


   README_RTP.txt        Explains about RTP Package in Detail


The following documentation files are in the doc directory.

   BRANCHES.txt		Information about the branches found
                        within the CVS tree

   CHANGELOG.txt	Information about changes since prior
			releases of VOCAL

   RELEASENOTES.txt     Information about BUG fixes/ Workarounds based
                        of Test plan since prior releases of VOCAL

   SETUP.txt		Describes the configuration procedure for
                        VOCAL

   SOURCES.txt		Describes of the source code layout for 
                        this archive, how to obtain and use doc++, 
                        and where to find more documentation about 
                        the VOCAL source code
   TESTING.txt		Information on the automated test tools
                        found within VOCAL

====================================================================

CONTACT INFORMATION AND WEBSITE 

==================================================================== 
 We welcome your feedback, suggestions and contributions. Contact 
 us via email if you have questions, feedback, code submissions, and 
 bug reports. 

 For general inquiries - info@vovida.org 

 We use a web based interface to our mailing lists.  It is 
 available at

	http://www.vovida.org/mailman/listinfo/

 From there, you may subscribe, unsubscribe, and browse the 
 archives for mailing lists concering VOCAL.

 Here is a partial list of mailing lists which may be of interest 
 to the VOCAL community:



 vocal@vovida.org 		Q&A about installing, testing and 
                                using VOCAL

 announce@vovida.org		Announcements
 bugs@vovida.org		A forum for bug reporting
 documentation@vovida.org	Q&A about the documentation 
 cops@vovida.org 		Q&A about the COPS stack
 mgcp@vovida.org 		Q&A about the MGCP stack
 radius@vovida.org		Q&A about the RADIUS stack
 rtp@vovida.org 		Q&A about the RTP stack
 rtsp@vovida.org 		Q&A about the RTSP stack
 sip@vovida.org 		Q&A about the SIP stack
 trip@vovida.org		Q&A about the TRIP stack
 vocal-dev@vovida.org		Q&A about developing VOCAL
 

You can submit bug, patches, software contributions, and feature 
requests using Bugzilla. Access Bugzilla from http://www.vovida.org

==================================================================== 

ERRATA

==================================================================== 

 For the latest breaking news regarding this prerelease, please
 see the VOCAL Errata section in Faq-o-matic, at

	http://www.vovida.org/cgi-bin/fom?file=855

 Issues and more information will be available from this location.


==================================================================== 

LICENSE AND COPYRIGHT 

==================================================================== 
The Vovida Software License, Version 1.0 
Copyright (c) 2000-2001 Vovida Networks, Inc.  All rights reserved. 

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions 
are met: 

1. Redistributions of source code must retain the above copyright 
   notice, this list of conditions and the following disclaimer. 

2. Redistributions in binary form must reproduce the above copyright 
   notice, this list of conditions and the following disclaimer in 
   the documentation and/or other materials provided with the 
   distribution. 

3. The names "VOCAL", "Vovida Open Communication Application 
   Library", and "Vovida Open Communication Application Library 
   (VOCAL)" must not be used to endorse or promote products derived 
   from this software without prior written permission. For written 
   permission, please contact VOCAL@vovida.org. 

4. Products derived from this software may not be called "VOCAL", 
   nor may "VOCAL" appear in their name, without prior written 
   permission of Vovida Networks, Inc.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND 
NON-INFRINGEMENT ARE DISCLAIMED. IN NO EVENT SHALL VOVIDA NETWORKS, 
INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES IN EXCESS 
OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

-------------------------------------------------------------------- 
All third party licenses and copyright notices and other required 
legends also need to be complied with as well. 
====================================================================

Copyright 2000-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.

$Id: README_VOCAL.txt,v 1.1 2004/05/01 04:15:30 greear Exp $

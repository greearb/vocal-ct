#!/usr/bin/wish -f
## *********************************************************************
## 
## $Id: enrollDevice.tcl,v 1.1 2004/05/01 04:15:31 greear Exp $
## 
## *********************************************************************
## 
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
# 
# 3. The names "VOCAL", "Vovida Open Communication Application Library",
#    and "Vovida Open Communication Application Library (VOCAL)" must
#    not be used to endorse or promote products derived from this
#    software without prior written permission. For written
#    permission, please contact vocal\@vovida.org.
# 
# 4. Products derived from this software may not be called "VOCAL", nor
#    may "VOCAL" appear in their name, without prior written
#    permission of Vovida Networks, Inc.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
# NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
# IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
# DAMAGE.
# 
# ====================================================================
# 
# This software consists of voluntary contributions made by Vovida
# Networks, Inc. and many individuals on behalf of Vovida Networks,
# Inc.  For more information on Vovida Networks, Inc., please see
# <http://www.vovida.org/>.
## 
## *********************************************************************
## 
## $Log: enrollDevice.tcl,v $
## Revision 1.1  2004/05/01 04:15:31  greear
## Initial revision
##
## Revision 1.4  2001/03/08 16:36:11  wbreu
## use Vocal license instead of GNU
##
## Revision 1.3.4.1  2001/03/08 23:26:58  wbreu
## Replace GNU license text with Vocal license.
##
## Revision 1.3  2000/12/05 22:28:25  wbreu
## rearranged the GUI layouts so that they all have the same
## look and feel, and automatically set the default paths
## if $MY_TCLTK_PATH and $OPENSSL_PATH environment variables
## are not set.
##
## Revision 1.2  2000/07/05 22:59:32  hyu
## Merging BRANCH_CURD into HEAD.
##
## Revision 1.1.2.1  2000/06/27 23:13:02  wbreu
## Tk/expect scripts to do device enrollment
##
##
## *********************************************************************
## tcl/tk gui combines the enroll request and retrieve functions as
## both functions take identical parameters
## The request function sends a PKCS#10 certificate request to the 
##     enrollment server for approval
## The retrieve function checks on a pending enrollment request. It
##     queries the enrollment server for the status of the request
##     and report the result as output

# set window title  "." is the main window
wm title . "Device Enrollment GUI" 

frame .overframe -borderwidth 0
set f [frame  .overframe.actionbutton -borderwidth 5]
set f2 [frame .overframe.controlbutton -borderwidth 5]

pack  $f -side left  -ipadx 50
pack  $f2 -side left -ipadx 50

# enter Key/CertReq button to generate a key and certificate request
# via openssl:
set keybut [button $f.keycertreq -text "Key/CertReq"  -command KeyCertReq ]

# enter getCaCert button to obtain Certificate Authority's cert info:
set getcacertbut [button $f.getcacert -text "GetCaCert" -command GetCaCert]

# enter reqbut to send the certificate request for approval.  If
# Enrollment Server approves, it returns the certificate to the
# device (localcert):
set reqbut [button $f.request -text "RequestCert"  -command Request]

# enter retbut to retrieve the local certificate:
set retbut [button $f.retrieve -text "RetrieveCert" -command  Retrieve]
$keybut config -highlightcolor red
$getcacertbut config -highlightcolor red
$reqbut config -highlightcolor red
$retbut config -highlightcolor red
pack $f.keycertreq $f.getcacert $f.request $f.retrieve -side left

button $f2.help -text "Guide" -command Guide 
button $f2.quit -text "Quit" -command Quit 
#set but [button $f2.run -text "Run" -command Run]
#pack $f2.quit $f2.run $f2.help -side right
pack $f2.quit $f2.help -side right

pack .overframe -side top

# create a text widget to log the output 
frame .t
set log [text .t.log -width 80 -height 10 \
    -borderwidth 2 -relief raised -setgrid true \
    -yscrollcommand {.t.scroll set}]
scrollbar .t.scroll -command {.t.log yview}
pack .t.scroll -side right -fill y
pack .t.log -side left -fill both -expand true
pack .t -side top -fill both -expand true

# get path to the tcl/tk script
global myscript_path
if [info exists env(MY_TCLTK_PATH)] {
   set myscript_path "$env(MY_TCLTK_PATH)"
} else {
    $log insert end "default env(MY_TCLTK_PATH) to ./\n"
    set myscript_path "./"
}

proc KeyCertReq {} {
    global myscript_path
    exec $myscript_path/getKeyReq.tk &
}

proc GetCaCert {} {
    global myscript_path
    exec $myscript_path/getcacert.tk &
}

proc Request {} {
    global myscript_path
    exec $myscript_path/reqOrRet.tcl 1 &
}

proc Retrieve {} {
    global myscript_path
    exec $myscript_path/reqOrRet.tcl 2 &
}

proc Guide {} {
    global log
    set helpTxt \
"Select one of the buttons:\nKey/CertReq - to use openssl to generate a (public/private) key and to generate a certificate request message\nGetCaCert - to get the Certificate Authority's information\nRequestCert - to present the certificate request to the Enrollment Server for approval\nRetrieveCert - if an enrollment request is pending, query the enrollment server for the status of request\n"
    $log insert end $helpTxt\n
}

proc Quit {} {
    exit 0
}

# end of enrollDevice.tcl

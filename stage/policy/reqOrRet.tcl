#!/usr/bin/wish -f
## *********************************************************************
## 
## $Id: reqOrRet.tcl,v 1.1 2004/05/01 04:15:31 greear Exp $
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
## $Log: reqOrRet.tcl,v $
## Revision 1.1  2004/05/01 04:15:31  greear
## Initial revision
##
## Revision 1.3  2001/03/08 16:37:00  wbreu
## use Vocal license instead of GNU
##
## Revision 1.2.36.1  2001/03/08 23:28:11  wbreu
## Replace GNU license text with Vocal license.
##
## Revision 1.2  2000/07/05 22:59:34  hyu
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

# 1st argument determines whether a request/ retrieve option:
set choice "none"
set option 0
if {$argc < 1} {
   # default to request
   set option 1
} else {
   set option [lindex $argv 0]
   puts "option = $option\n"
}

# set window title  "." is the main window
wm title . "Enroll request/ retrieve" 

frame .overframe -borderwidth 0
set f [frame  .overframe.radiobutton -borderwidth 5]
set f2 [frame .overframe.controlbutton -borderwidth 5]

pack  $f -side left  -ipadx 50
pack  $f2 -side left -ipadx 50

set reqbut [radiobutton $f.request -text \
            "Request"  -variable $choice -value "request"]
set retbut [radiobutton $f.retrieve -text \
            "Retrieve" -variable $choice -value "retrieve"]
$reqbut config -highlightcolor red
$retbut config -highlightcolor red
pack $f.request $f.retrieve -side left

# highlight the button of choice:
if {$option == 1} { 
   $f.request invoke
   set choice "request"
} else {
   $f.retrieve invoke
   set choice "retrieve"
}

button $f2.help -text "Help" -command Help 
button $f2.quit -text "Quit" -command Quit 
set but [button $f2.run -text "Run" -command Run]
pack $f2.quit $f2.run $f2.help -side right

# proc CommandEntry { name label width command args } {
proc CommandEntry { name label width1 width2 args } {
    frame $name -borderwidth 10
    label $name.label -text $label -width $width1 -anchor w
    eval { entry $name.entry -width $width2 -relief sunken } $args
    pack $name.label -side left
    pack $name.entry -side left -fill x -expand true
    bind $name.entry <Control-c> Stop
#	bind $name.entry <Return> 
    return $name.entry
}

# Let CommandEntry to create a frame for each label/entry
CommandEntry .cacert cacert_file   20 45 -textvariable cacert
CommandEntry .certreq certreq_file  20 45 -textvariable certreq
CommandEntry .customer customer 20 45 -textvariable customer
CommandEntry .device  device   20  45 -textvariable device
# let Enrollment client auto-generate nonce
# CommandEntry .nonce  nonce   20 45 -textvariable nonce
CommandEntry .username  username   20 45 -textvariable username
CommandEntry .password  password 20 45 -textvariable password
CommandEntry .sslurl  sslurl   20 45 -textvariable sslurl

# set default parameters:
set cacert "./CACertB64.dat"
set certreq "./CertReqB64.dat"
set customer "1000"
set device   "1000"
set username "wendy"
set password "breu"
set sslurl   "http://sherbet/cgi-bin/osp_server_iis.dll"

#pack .overframe  .cacert .certreq .customer .device .nonce  
pack .overframe  .cacert .certreq .customer .device        \
     .username .password .sslurl  -side top

# set up key binding equivalents to the radiobuttons
# bind .caurl      <Return> Run
# focus .caurl  

# create a text widget to log the output 
frame .t
set log [text .t.log -width 80 -height 10 \
    -borderwidth 2 -relief raised -setgrid true \
    -yscrollcommand {.t.scroll set}]
scrollbar .t.scroll -command {.t.log yview}
pack .t.scroll -side right -fill y
pack .t.log -side left -fill both -expand true
pack .t -side top -fill both -expand true


global outFileId
set outFileId [open /tmp/reqOrRet.out w 0600]

global firstLine
set firstLine 0

global cacertbuf certreqbuf
set cacertbuf ""
set certreqbuf ""

proc Help {} {
    global log
    set helpTxt \
"Select one of the radio buttons:\nrequest -  to present a certificate request for approval\nretrieve -  to get the certificate if a certificate request is pending\n\nEnter the following fields:\ncacert_file - file name of the Certificate Authority certificate (base 64 encoding)\ncertreq_file - file name of the certificate request (also in base 64 encoding)\ncustomer - customer ID\ndevice - device ID\nusername - name of the user\npassword - password of user\nsslurl - url of the Enrollment Server\n"
    $log insert end $helpTxt\n
    $log see end
}

# check if file, fn exists, if so, read its content as base64
# encoded message (strip each newline char)
proc GetContent {fn content} {
    global log
    upvar $content buf
    set buf ""
    if {[file exists $fn] == 0} {
       $log insert end [format "file: %s does not exist\n" $fn]
       return 0
    }
    # read the content of the file and place content in $buf
    set fId [open $fn r]
    while {[gets $fId line] >= 0} {
       # concat line into one long string buffer:
       # but skip "-----BEGIN" and "-----END" header/ footer:
       if [string match -----BEGIN* $line] {
          $log insert end "GetContent{} skips header line\n"
       } else {
         if [string match -----END* $line] {
            $log insert end "GetContent{} skips footer\n"
         } else {
           set buf [concat $buf $line]       
         }
       }
    }
    close $fId

    # strip out white space
    set buf [join $buf ""]
    # debug only
    # $log insert end [format "file: %s contains:\n%s\n" $fn $buf] 
    return 1
}

# run the program and arrange to read its input
proc Run {} {
    global input log but choice cacert certreq 
    global customer device password username sslurl
    global cacertbuf certreqbuf
    
    # check if ca cert and cert request data files exist:
    # if so, read in its content.
    if {[GetContent $cacert cacertbuf] == 0} {
       return
    }
    if {[GetContent  $certreq certreqbuf] == 0} {
       return
    }
    set parms [format \
      "-username %s -password %s -customer %s -device %s -sslurl %s -cacert %s -certreq %s" \
      $username $password $customer $device $sslurl $cacertbuf $certreqbuf ]
    if {$choice == "request"} {
       set cmdStr [format "./osptnep enroll -function request %s" \
                $parms]
    } else {
       set cmdStr [format "./osptnep enroll -function retrieve %s" \
                $parms]
    }
    puts [format "debug: %s" $cmdStr]
    if [catch {open "|$cmdStr"} input] {
        # catch bogus command
        $log insert end $input\n
    } else {
        fileevent $input readable Log
        $log insert end $cmdStr\n
        $but config -text Stop -command Stop
    }
}

# read and log output from the program
proc Log {} {
    global input log outFileId firstLine
    if [eof $input] {
        Stop
    } else {
        # skip logging first line
        if {$firstLine == 0} {
            incr firstLine 1
            # return
        } else {
            gets $input line
            puts $line
            $log insert end $line\n
            # display the end of the log
            $log see end
            # write output to /tmp/getcert.out; create file if it
            # does not exist
            puts $outFileId $line
        }
    }
}

# stop the program and fix up the radiobutton
proc Stop {} {
    global input but
    catch {close $input}
    $but config -text "Run" -command Run
}

proc Quit {} {
    global outFileId
    # close /tmp/tkTest.out
    close $outFileId 
    exit
}

# end of reqOrRet.tcl

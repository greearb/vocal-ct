#!/usr/bin/python

# ====================================================================
# The Vovida Software License, Version 1.0 
# 
# Copyright (c) 2002 Vovida Networks, Inc.  All rights reserved.
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

# $Id: vm2.cgi,v 1.1 2004/05/01 04:15:31 greear Exp $

import os
import sys
import cgi
import re
import string


def reload_main(number, text):
    print "Content-Type: text/html\n";

    print '<META HTTP-EQUIV="REFRESH" CONTENT="2; URL=/cgi-bin/vm2.cgi?number=' + number + '&action=view">'
    print '<body bgcolor=white>'
    print '<h1>' + text + '</h1>'

def send_wav(filename):
    sound = open(filename)
    data = sound.read()
    sound.close()
    print "Content-Type: audio/wav"
    print "Content-Length:", len(data)
    print 
    print data

def send_greeting(number):
    config = read_config(number)
    filename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + "/" + config["Greeting"]
    if os.access(filename, os.R_OK):
	send_wav(filename)
    else:
	reload_main(number, "Could not access greeting")

def send_alternate(number):
    config = read_config(number)
    filename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + "/newprompt.wav"
    if os.access(filename, os.R_OK):
	send_wav(filename)
    else:
	reload_main(number, "Could not access alternate greeting")

def swap_greetings(number):
    config = read_config(number)
    filename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + "/" + config["Greeting"]
    altfilename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + "/newprompt.wav"
    tmpfilename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + "/tmp.wav"
    if (not os.access(tmpfilename, os.F_OK)) and os.access(filename, os.W_OK) and os.access(altfilename, os.W_OK):
	os.system("mv " + filename + " " + tmpfilename)
	os.system("mv " + altfilename + " " + filename)
	os.system("mv " + tmpfilename + " " + altfilename)
	reload_main(number, "Swap successful")
    else:
	reload_main(number, "Swap failed")
    

def read_config(number):
    filename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + '/config'
    config = open(filename)
    x = 1
    template = {}
    while x:
	x = config.readline()
	# convert to proper form
	if x != None:
	    list = string.split(x)
	    if len(list) > 2:
#		print list[0], ": ", list[2], "<P>"
		template[list[0]] = list[2]
#	    print key , ": ", value, "<p>"
    config.close()
    return template

def make_box(title, contents):
    print '<table bgcolor="#000000"  cellpadding=2 cellspacing=5 border=0 >'
    print '<tr><td>'
    print '<table bgcolor=white cellpadding=10 cellspacing=0 border=0 width=400>'
    print '<tr><td bgcolor="#c0c0ff" fgcolor=white align=center>'
    print '<b>' + title + '</b>'
    print '</tr></td>'
    print '<tr><td>'
    print contents
    print '</tr></td></table></tr></td></table>'



def user_display(number):
    print "Content-Type: text/html\n";

    print "<html><body bgcolor=white>"
    print "<h1>Voicemail Configuration</h1>"


    # get the information about the phone number
    config = read_config(number)

#    print config

    make_box("Configuration Information", "Phone number: " + number + "<br>" + "User name: " + config["Name"] + "<br>" +  "Mail Address: " + config["SendTo"] + "<br>")

    make_box("Actions", '<a href="vm2.cgi/file.wav?number=' + number + '&action=listen">Listen to current greeting</a><p>' + '<a href="vm2.cgi/file.wav?number=' + number + '&action=listenalt">Listen to backup greeting</a><p>' + '<a href="vm2.cgi?number=' + number + '&action=swap">Switch backup and current greeting</a>' + '<p>' + '<a href="vm2.cgi">Return to main menu')
    
    if os.access("/usr/local/vocal/bin/voicemail/vmhome/" + number, os.W_OK):
	# this is writable, so add a form to update stuff

	make_box("Change Backup Voice Mail Greeting", 'To change your backup greeting:<p><ol><li>Call <b>x85000</b> from the <b>extension to change</b><li>At the beep, record your prompt. Hang up when you are finished recording.<li>Your new greeting will be saved as your <b>backup</b> greeting.  To listen to it, '+  '<a href="vm2.cgi/file.wav?number=' + number + '&action=listenalt">Listen to backup greeting</a>' +  '<li>To make your backup greeting your "active" greeting, ' + '<a href="vm2.cgi?number=' + number + '&action=swap">Switch backup and current greeting</a>' + '<li>Verify that you have the correct greeting selected by ' +'<a href="vm2.cgi/file.wav?number=' + number + '&action=listen">Listen to current greeting</a>' +  '</ol>' + '<p>If you make a mistake, begin again from step 1.')

	make_box("Upload Backup Voice Mail Greeting", 'To upload a backup greeting from your personal computer, <p><form action="vm2.cgi" method=post ENCTYPE="multipart/form-data" ENCODING>' + '<input type=hidden name="number" value="' + number + '">' + '<input type=hidden name="action" value="upload">'+ ' Select a  file from your hard drive:<input type=file name="testFile"><p>'+ '<input type=submit value="Upload Backup Greeting">'+ '</form>')


def upload_file(number, data):
    config = read_config(number)
    filename = "/usr/local/vocal/bin/voicemail/vmhome/" + number + '/newprompt.wav'
    newfile = os.popen("sox -t wav - -c -1 -b -U -r 8000 " + filename, "w")
    newfile.write(data)
    newfile.close()


form = cgi.FieldStorage()
done = 0
if form.has_key("number") and form.has_key("action"):
    number = form["number"].value
    action = form["action"].value

    if action == "upload" and form.has_key("testFile"):
	if(form["testFile"].file):
	    audio = form["testFile"].file.read()
	    upload_file(number, audio)
#	user_display(form["number"].value)
	reload_main(number, "upload succesful")
#	print "Successful upload.<p>"
	done = 1
    elif action == "listen":
	send_greeting(number)
	done = 1
    elif action == "listenalt":
	send_alternate(number)
	done = 1
    elif action == "swap":
	swap_greetings(number)
#	user_display(form["number"].value)
#	print "Successful switch.<p>"
	
	done = 1
    elif action == "view":
	user_display(form["number"].value)
	done = 1

if done == 0:
    print "Content-Type: text/html\n"
    print "<html><body bgcolor=white>"
    print "<h1>Voice Mail Configurator</h1>"

    #sys.path.insert(0, "/bin")
    #os.system("/bin/ls /usr/local/vocal/bin/voicemail/vmhome");
    ls = os.popen("/bin/ls /usr/local/vocal/bin/voicemail/vmhome")
    x = 1

    print '<table bgcolor=white cellpadding=0 cellspacing=0 border=0 width="85%">'
    print '<tr><th>Name</th><th>Email Address</th><th>Extension</th></tr>'
    number_regex = re.compile("^[0-9]{5}$")
    toggle = 0
    while x:
        x = ls.readline()
        x = x[:-1]
        if number_regex.match(x):
	    # get the right data
	    config = read_config(x)
	    if toggle:
		print "<tr bgcolor=\"#ffffff\">"
	    else:
		print "<tr bgcolor=\"#CCCCff\">"
	    print "<td>" + config["Name"] + "</td>"
	    print "<td>" + config["SendTo"] + "</td>"
	    print '<td><a href="vm2.cgi?number=' + x + '&action=view">' + x + "</a></td>"
	    print "</tr>"
	    toggle = not toggle
    print "</table>"
    print "\n\n"

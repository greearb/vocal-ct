$Id: README.txt,v 1.1 2004/05/01 04:15:31 greear Exp $

Simplified Web Provisioning for VOCAL


Abstract
--------

This is a simplified web html and CGI based provisioning system for a
VOCAL allinone system.

Requirements
------------

VOCAL
Apache
Perl 5.x
CGI module for perl


Installing
----------

Make sure you have all of the above installed before you go any further.

You need to copy the files

    webconf.cgi
    psinslib.pm
    provactions.pm
    nanoxml.pm
    vprov/
	cplxml.pm
	provxml.pm
	serverxml.pm
	userxml.pm
	ui.pm

to some CGI-enabled directory on your web server.  The web server
needs to be on the same host as the provisioning server.  Make sure
that ExecCGI (or the equivalent) is enabled in this directory in your
http server (e.g. apache) configuration.


To enable voicemail configuration, you must have voicemail configured
and running correctly on your VOCAL system.  Secondly, the cgi scripts
must have write permission in the
/usr/local/vocal/bin/voicemail/vmhome directory, as well as all
directories below this point.  If your web server runs CGI as some
unpriveliged user (e.g. nobody), so then you can do the following:

	chown -R nobody /usr/local/vocal/bin/voicemail/vmhome
	chmod -R u+w /usr/local/vocal/bin/voicemail/vmhome
	chmod -R go-w /usr/local/vocal/bin/voicemail/vmhome

which will set the permissions appropriately.

Note: The allinoneconfigure script will attempt to do all of the above
automatically if you give it a chance.  In this case, everything
should be set up for you already.


Running
-------

From a web browser, view the webconf.cgi file.  By default,
allinoneconfigure will set up your web server to make the webconf
script appear to be in vocalconf, so you can access it via a URL of
the form

     http://hostname/vocalconf/webconf.cgi

From there, you should be able to add, delete, and change user
features, as well as change the gateway.


Features
--------

Add users (user names must be 4 digits long exactly, and composed only
    of digits)

Delete users

Change user information (e.g. enable/disable voicemail, set
    authentication password, etc.)

Configure the gateway


Dialing plan
------------

By default, the dial plan has the following properties:

    - user phones can call through the gateway by first dialing 9.  In
      this case, the call will be sent to the gateway (controlled by
      the change gateway option), with the 9 intact. 

    - Calls to numbers which are more than 4 digits are sent to the
      gateway. A 9 is not prepended.

    - calls to numbers which are 4 digits long are interpreted to be 
      user phone numbers, and are sent to the other user's phone.


Interaction with the Java GUI
-----------------------------

We expect that anyone who wants more than a basic system will
eventually need to use the more complete Java provisioning.  The HTML
system is intended as an easy-to-use introduction, sufficient for
casual users.

I recommend starting with your allinone system and exploring its
configuration with the Java GUI.  You can point the java GUI at an
allinone system and view/edit the configuration, so you can (for
example) add users who have text names.

However, changes made with the Java GUI may be overwritten by the HTML
GUI.  We recommend that once you begin using the Java GUI, you do not
return to using the HTML GUI.

How to use psTranslate.cgi
--------------------------
The cgi scripts provides the capability to translate the existing 
provisioned data from one IP to other. So for example when Vocal gets
installed on a machine with IP ip1, the basic configuration data is 
provisioned using the ip1. However the data will be useless if later
IP of the machine is changed from ip1 to ip2 and would require a clean
install of vocal. The psTranslate script now allowd migrating provisioned
data from one IP to other. The script is run on the host running the pserver
and exepects to have permissions to read the provision data directory.
To migrate provision data from ip1 to ip2, one would run the script
from command line as
#> ./psTranslate.cgi <pserverHost:port> <provision_data_dir> <ip1> <ip2>


Future Plans
------------

Please see DESIGN.txt in the provisioning/web directory for more
information about this software.

======================================================================

Copyright 2002-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.

$Id: README.txt,v 1.1 2004/05/01 04:15:31 greear Exp $

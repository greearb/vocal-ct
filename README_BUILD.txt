Advanced Building Topics in VOCAL

For information about licensing, please see the LICENSE file in this
directory, or http://www.vovida.org/license.html

Building VOCAL (1.5.0)
----------------------------

This document explains technical details of building VOCAL.  For a
simple cookbook approach to building VOCAL, please see README.txt.

For more information, please see the web site, http://www.vovida.org/ .

--------------------------------------------------------------
++++++++++++++++++
READ THIS  FIRST :
++++++++++++++++++
**************************************************
NOTE: VOCAL 1.5.0 is not SUPPORTED ON SOLARIS :(
**************************************************

we are looking forward for Contribution form Community to make it
compile on Solaris again, This README is still maintaing the steps
required for the SOLARIS compile

---------------------------------------------------------------
All platforms: 
	
        GNU make
	Apache (other webservers may work, but we do not provide 
	       instructions here)

    EXCEPTION:

       Only if Using [g++/gcc] compilers on Solaris:
                                                     GNU ar
                                                     GNU ld
        these tools can be obtained by downloading package
        "binutils-2.11.2-sol8-sparc-local" from 
         http://www.sunfreeware.com

      ++++
      Note:  these tools are not required if Using Forte 6u1 on solaris.
      ++++

    Linux

	GNU gcc/g++ 2.91.66 or later

    Solaris:

*************************************************
NOTE: VOCAL 1.5.0 is not SUPPORTED ON SOLARIS :(
*************************************************


         Sun Workshop 6u1 (see below for information for Solaris/g++)
         For more Informantion on Sun Workshop 6u1

             http://www.sun.com/forte

    For SIPSet:

         In order to build SIPSet, the graphical User Agent, you must
         also have GTK+ and Glade installed.  We have tested with the
         following versions under Redhat 7.3:

	     GTK+ 1.2.10
	     Glade 0.6.4

*********
LINUX:
*********
                
configure options
-----------------

A complete list of options to configure can be obtained by running the
command

  ./configure --help

Please read that list for a more complete explanation.

If you would like to use the new Postgres based provisioning, you must
have postgres installed (see newprov/README) and then do

     ./configure --with-newprov




How to Build an RPM
-------------------

In order to build an RPM of VOCAL, you will need to run the following
make command:

     make <OPTIONS> 
     make <OPTIONS> <RPM-OPTIONS> rpm

OPTIONS can include any of the standard options for building VOCAL
(notably, CODE_OPTIMIZE=1).

RPM-OPTIONS must include one option -- BUILDROOT=/path/to/buildroot

the /path/to/buildroot must be enough disk space to install all of the
VOCAL binaries.  By default, /var/tmp is used, but this does not have
enough space on many systems.

Your stage directory must have enough space to keep a tarball of the
vocal files.


So, if you have enough space in /usr/tmp , and you wish to build an
optimized RPM, you can try

    make CODE_OPTIMIZE=1 
    make CODE_OPTIMIZE=1 BUILDROOT=/usr/tmp/vocal-buildroot rpm

or, if you're confident that all will compile

    make CODE_OPTIMIZE=1 BUILDROOT=/usr/tmp/vocal-buildroot all rpm

which will do everything all at once.

The BUILDROOT directory will be erased [if it exists] before building
and created when starting to build the rpm package.  Using a name such
as that given above will ensure that no external files will be mixed
with those used to build the vocal rpms.


After you're finished building, your RPMs should be in

      stage/rpm/RPMS/arch/vocalbin-<version>-x.arch.rpm

e.g.

      stage/rpm/RPMS/i386/vocalbin-1.4.0-1.i386.rpm

The RPMs build by the rpm target will install in /usr/local/vocal .

After you have built an RPM, please see and README_VOCAL.txt for more
information.

*-*-*-*-*
SOLARIS:
*-*-*-*-*
*************************************************
NOTE: VOCAL 1.5.0 is not SUPPORTED ON SOLARIS :(
*************************************************


Compiling with Solaris:

    REMINDER:

      You MUST have GNU Make installed on your system in order to
      compile VOCAL.  GNU Make is available from

     	http://www.sunfreeware.com/

    Forte 6U1

    Make sure that you have set your PATH to include /opt/SUNWspro/bin/ ,
    and that your cc and CC are located there, and NOT in /usr/ccs/bin .
    
     
     ./configure --prefix=PREFIX 
      
     *****
      NOTE: PREFIX is not the string "PREFIX". Instead, it's the 
     *****  filesystem path name under which vocal should 
            be installed. 
            For instance use /opt/vocal for PREFIX above.
            
             **** default path will be /opt/vocal
  
     make <OPTIONS>
     
     in case  you wish to build optimized , you can try
     
     make CODE_OPTIMIZE=1

    in case  you wish to build everything, use the option
    
     make
     
     make install ( this will install, default=/opt/vocal or --prefix path incase 
                     configure was used)
     
     make pkg     (this will create Solaris Package, should be written 
                  in stage/VOCALb-arch.pkg.tar.Z)

    ++++
    Note :  'make usage' will show you other targets for make"
    ++++    
     
   
   GCC

*************************************************
NOTE: VOCAL 1.5.0 is not SUPPORTED ON SOLARIS :(
*************************************************


      WARNING: 

        You MUST have GNU ar to correctly build using gcc / g++ .  The
        ar which comes with Solaris (in /usr/ccs/bin/ar) will not work
        -- it will crash with a segmentation fault if you fail to pass
        a valid location for GNU ar.

        GNU ar is a part of GNU binutils, which is available from

	  http://www.sunfreeware.com/

        Note that GNU ar does NOT work with Forte -- you must use
        /usr/ccs/bin/ar for Forte , and GNU ar for g++.

    In order to compile with g++ under Solaris 8, you must run the
    configure script from within the vocal directory.  Do the
    following before trying to compile anything:
   
    ./configure --prefix=PREFIX \
         --with-toolchain=gnu --with-ar=/usr/local/bin/ar

      NOTE: PREFIX is not the string "PREFIX". Instead, it's the 
            filesystem path name under which vocal should 
            be installed. 
            For instance use /opt/vocal for PREFIX above.


    after this, you may use,

    make 
    
    make install ( this will install in path provided in --prefix of
		   "configure" default is /opt/vocal)
    
    make pkg     (this will create Solaris Package, should be written 
                  in stage/VOCALb-arch.pkg.tar.Z)
   
    
    ++++
    Note :  'make usage' will show you other targets for make"
    ++++    
    
WARNING: 

      DO NOT mix objects compiled with Forte with objects compiled
      with g++.  Doing so may result in compiler errors, as Forte is
      not binary compatible with g++.

*************************************************************************

NOTE: Please do READ README_VOCAL.txt, README_SIP.txt & README_RTP.txt

************************************************************************


***********************************************************************

Next Section of this README is  more For Adavance Users:

--------------------------------------------------------------------
NOTE:

     This section describes technical things about the VOCAL build
     process.  For build instructions please goto the TOP of this
     README.

---------------------------------------------------------------------


************************************************
TECHNICAL THINGS ABOUT THE VOCAL BUILD PROCESS
************************************************

describes technical things about the VOCAL build process.


----------------------------------------------------------------------
Build Process Philosophy
----------------------------------------------------------------------

The build system was written with several goals in mind:

1. To allow for the building of multiple variations of object code in
a single directory tree.  Variations include different hardware
platforms (e.g. x86 and power pc), different operating systems (Linux
vs. Solaris) different compilers (sunpro vs. gnu) and different build
options (debugging vs. optimized).

Comments:

When you build the source files in a directory (such as util), the
objects and binaries are placed in subdirectories named with the
variations, e.g.

	obj.debug.Linux.i686

which is the directory for objects build with the "debug" build
options, for Linux running on an i686 cpu.


Suggested enhancement: On machines with multiple toolchains (such as
solaris) the toolchain choice should probably be there, somehow, as
well, e.g.

	obj.sunpro.debug.Solaris.sun4u

Currently the toolchain is selected orthogonally to build options, but
this is not entirely general -- for example, CODE_OPTIMIZE=2 only
works on compilers (such as gcc) which allow an argument to -O.

A general option would by to hybridize this, forcing a build option to
also mean a specific toolchain, such as

	obj.gcc-debug.Linux.i686

or some such.  The positive side effect is that then things like -O2
aren't specialized cases, as that would be

	obj.gcc-opt2.Linux.i686

versus plain -O

	obj.gcc-opt.Linux.i686

--
Yuck :)
--

but, then you can't leverage the compiler knowledge you have for
multiple compilers which take identical options, such as
cross-compilers:

	obj.gcc-cross-arm-opt.Linux.i686
--

Ian's comment:

When you are cross compiling are you really interested in the
source machine's architecture (i.e. the Linux.i386) or the destination
architecture?  (arm...)

I would argue that the OSARCH you are interested in from the separation
of build point of view is the destination architecture.

--

True.

--

which should be the same as obj.gcc-opt.Linux.i686 except for the
compiler names.

Maybe the right approach is to allow the above, but also to offer some
sort of inheritance approach where you can specify that a given
tool-build combo "inherits" from a previous one.

Using configure/autoconf in the "traditional" way precludes this --
autoconf produces a config.h which is system specific.

--
The way I have always approached this is to define the variables
that can vary from build environment to build environment....

What you have so far are:

  FILE TYPE = obj | bin
  TOOLCHAIN = gcc | sunpro | ...
  DESTINATION OS / ARCH = Linux | SunOS | ... / i386 | sparc | ...
  etc...

This of course assume orthogonality in the variables. I don't mind
this, since in general this works.

--

but also, there is

OBJECT TYPE = optimized | debug | profiling | nodebug

which isn't quite so orthogonal.  Note:

optimized versus optimized-with-O2 vs.
optimized-with-O2-f-no-inline vs. optimzied with-Os-f-no-inline
vs. optimized-with-O2-f-no-unroll-loops-f-no-strength-reduce ...

all of which is orthogonal to debug and profiling


--

2. To allow for the use of third party software in the construction of
VOCAL. Since developers may already have some version of the software
on their system, VOCAL keeps its own versions of the software to
minimize API change issues (and to minimize the number of third-party
packages that must be at a particular version for VOCAL to work). This
may change in the future, but the current approach has simplified the
life of VOCAL's developers.

Comments:

This, while simplifying matters for prior versions of VOCAL, is not
clearly the best choice.  Here are some pros and cons of the above
approach and the alternative, namely requiring libraries to be
installed system-wide before trying to build vocal.  Pros are marked
with +, while cons are marked with -.


Contrib directory
-----------------

+ allows multiple versions (versions of different ages) to be built on
  a single machine

+ allows builds without root access

+ allows greater coherency in any particular version as it doesn't
  depend so much on external libraries

+ allows private fixes

--
I think the weight of the advantage is severly outweighed by the cost.
--

- makes it easier to depend on old versions, and not keep up to date

- makes hard to use shared libraries

- encourages private fixes (instead of contributing back to the developer)

- subtle bugs caused accidentally using current/different system packages
  will happen


Install dependencies first
--------------------------

- depends on system version of libraries being correct

- prevents building versions which depend on old libraries

+ encourages keeping in sync w/ current versions

+ easier to use shared libraries and otherwise reuse installed packages

+ encourages returning fixes

+ encourages properly dependent packaging


Related to this is the open question about modularizing VOCAL
libraries.  In this case, VOCAL's API changes often enough that being
able to do both is probably warranted.

Advantages of allowing VOCAL libraries to be used standalone:

  users who only want to build applications can just install the
  library and not all of VOCAL (which doesn't install libraries
  anyway)

  3rd party software becomes more realistic (they can just use the
  library instead of having to extract the library from VOCAL each
  time they want to upgrade to the latest VOCAL).

  reduced build times :)


3. To allow subsets of VOCAL's directories to be used profitably,
while still allowing efficient building of the total tree. It is
possible to remove portions of the VOCAL tree and still build useful
things (such as the SIP and MGCP libraries). Since both SIP and MGCP
depend on the utilities, however, a separate build of each library
would force users to keep three or even four copies of this code in
sync. Instead, the VOCAL tree is set up in such a way that a single
copy of the util directory is used by all of the software.


4. To allow multiple VOCAL trees, with multiple versions, to coexist
on a single machine. One side effect of this requirement is that the
contrib software is not installed -- instead, VOCAL is designed to be
built with the code living directly in contrib. The advantage of this
is that developers can compile multiple versions of VOCAL, no matter
which system libraries are installed.

Comments:

  see point 2 for why this might go away

5. To allow a transition to non-recursive make. While VOCAL currently
uses a recursive make, we recognize that this isn't optimal. the
current makefile structure is such that the makefiles can be changed
to use non-recursive make at some future time.

Comments:

  doesn't seem like anytime soon.


6. To allow functionality variation in different parts of the
software, while making it relatively easy to change directory
structure. The VOCAL make system allows developers to select
"packages" of functionality (e.g. the UTIL package includes
portability code, the SIP package is the SIP library), so that each
piece does not unduly link or include search too much code.  At the
same time, the paths to link or search are kept in a single place to
simplify maintenance.

--
I agree that this is the philosophy, yet I am torn about this one.
If a new user came in and looked at the package system, assuming they
knew the standard tools, they may not understand it. They definately
wouldn't know the variable availables for manipulation. So we are
rolling our own interface. And centralized control is both a blessing
and a curse, but I could say that about everything here.
--

Comments:

  could be better.


----------------------------------------------------------------------
Adding Packages / Linking
----------------------------------------------------------------------

VOCAL uses a "package" concept to add software libraries to the build
system.  If you want to add a package, follow these directions.

1. For e.g if you want to make a library called libmylib.a and the sources
are under vocal/mylib then add the following in vocal/build/Makefile.pkg:

	MYLIB_INCLUDEDIRS := $(ROOT)/mylib
	MYLIB_LIBDIRS := $(ROOT)/mylib/obj.$(TARGET_NAME)
	MYLIB_LIBNAME := mylib
	MYLIB_LDFLAGS :=
	MYLIB_DEPENDS := $(ROOT)/mylib/obj.$(TARGET_NAME)/libmylib.a

	$(MYLIB_DEPENDS):
		cd $(ROOT)/mylib;$(MAKE)


2. Then in the new directory vocal/mylib, create a Makefile as:

	BUILD = ../build

	include $(BUILD)/Makefile.pre

	PACKAGES =

-
The package concept only applies to the above variable. There are
other mysterious/arbitrary variables here as well. That are more
tied into the build system than the package system...

i.e. SRC, INCDIRS, TARGET, TARGET_LIBRARY, etc...
--

	INCDIRS += .

	TARGET_LIBRARY := libmylib.a

	SRC = \
		Source1.cxx \
		Source2.cxx


	TESTPROGRAMS +=  MyLibTestMain.cxx

	include $(BUILD)/Makefile.post

	docs:   doc++/config.txt
		doc++ --config doc++/config.txt

--

docs is a complete hack. I added that since I didn't want
to add a rule to Makefile.post.

I'd prefer a rule in makefile.post, unless there's a compelling reason not to.

--


3. Include the MYLIB as a package in the vocal/sip/ua/Makefile

	PACKAGES += MYLIB

4. Add entries into the vocal/Makefile

  a) add the following to the usage: entry

	usage:
  [stuff...]
		@echo "mylib        my newly created library"

  b) add the following as an entry for mylib
	mylib: whatever it depends on
		cd mylib; $(MAKE)

  c) include it into .PHONY: and SUBDIRS =


----------------------------------------------------------------------
Options for Make
----------------------------------------------------------------------

Here is a list of options for make.  Please add any you find missing.


CODE_OPTIMIZE=1 (and for GCC only, CODE_OPTIMIZE=n)

    pass -O to the C++ compiler.

CODE_PROFILE=1

    pass -pg (enable profiling) to the C++ compilier

CODE_NODEBUG=1

    do not pass -g to the C++ compiler

USE_YARROW=1

    try to use the Yarrow pseudo-random number generator.  this
    doesn't work right now.

VOCAL_USE_DEPRECATED=1

    allow access to depreciated interfaces in util, including not
    using namespaces, etc.  currently VOCAL does not compile with this
    turned off.


VOCAL_WARN_DEPRECATED=1

    warn people if they use the depreciated interfaces in util (or is
    it warn if the depreciated interfaces are turned on?), but do not
    error.


--
I would really love to see some documentation that lists all of
the makefile variables available used, and which ones are available
to the user.
--


----------------------------------------------------------------------
Files in the Build process
----------------------------------------------------------------------

Makefile.conf   configurable options via configure
global.h        global .h options.  depreciated
Makefile.opt	source build options .  these toggle certain source code
		options
Makefile.osarch	Operating system and machine architecture specific items
Makefile.pre	used to include build functionality in user Makefiles
		(see below)
Makefile.post	used to include build functionality in user Makefiles
		(see below)
Makefile.all	things needed for all other makefile pieces to work
Makefile.pkg	makefile information for packages (e.g. locations, includes,
		library names, etc.)
Makefile.tools	toolchain-related information (e.g. what commands do what)



Generally, the Make process can be broken into 3 parts:

	Prerequisites
	User-configurable section
	Build engine


The Prerequisites set up generic information about the tool chain
(compiler, linker, etc), build options (debug or optimized, which
optional software i'm using) and the like.  This is included in the
user makefile via Makefile.pre (which in turn includes Makefile.conf,
Makefile.opt, Makefile.osarch, Makefile.all, Makefile.pkg, and
Makefile.tools).

The User configuration section is where the user specifies which
packages he is using and which files are being built.

Finally, the build engine (in Makefile.post) issues the appropriate
commands to the tools (specified in the prerequisites) to build the
software (specified in the user configuration section).

----------------------------------------------------------------------
Using the make system in your own makefiles
----------------------------------------------------------------------

This is best done via an example.  Here is one based on the SDP makefile:

    BUILD = ../build

    include $(BUILD)/Makefile.pre

    TARGET_LIBRARY = libsdp.a

    PACKAGES += UTIL PTHREAD

    # name of all files to be compiled (don't include header files)
    SRC = \
    	SdpSession.cxx \
    	SdpMedia.cxx \
    	SdpAttributes.cxx \
    	SdpTime.cxx \
    	SdpBandwidth.cxx \
    	SdpConnection.cxx \
    	SdpEncryptkey.cxx \

    # names of programs (e.g. test programs) to be compiled
    TESTPROGRAMS = \
    	SdpTest.cxx

    include $(BUILD)/Makefile.post

Here are the key elements:

BUILD is a variable set to the relative path from the directory this
Makefile resides in and the build directory where Makefile.pre and
friends live.  It MUST be the first item, and it should be relative,
since the location where a user stores his code is unknown to the make
system.

"include $(BUILD)/Makefile.pre" includes the "pre" portion of the
Makefiles (see above).

TARGET_LIBRARY should be set to the name of the library being built.

PACKAGES lists the packages which these source files depend on.  For a
library, the dependency usually takes the form of the headers being
used, but for programs, the libraries that are part of the package are
used as well.  The PACKAGES variable is order-sensitive -- since some
packages may in turn depend on other packages to provide them some
functionality, a package which uses on a second package must be
specified BEFORE the package it depends on.  So if package A depends
on B, you need to say

	PACKAGES += A B

and not

	PACKAGES += B A

this is the same way that the -l option works to most C compilers in
Unix.

SRC lists the source files for this library.  files which end in .c
(for C code) and .cxx (for C++ code) are accepted.  Other file names
are not handled.

TESTPROGRAMS is the list of source files for test programs to be
built.  Each source file in TESTPROGRAMS represents a separate
program.  Each program is named the base name (the part before the .)
of the source file which specifies it.  Each of these programs is
linked with ALL of the objects built from the SRC target.  This is a
little confusing, so here's an example:

    SRC = x.c y.c z.c
    TESTPROGRAMS = A.c B.c

In this case, two programs named A and B will be built.  The source
files for A will be x.c, y.c, z.c, and A.c, while the source files for
B will be x.c, y.c, z.c, and B.c.

An additional variable not shown here is VERIFY_TESTCASES.
VERIFY_TESTCASES works like TESTPROGRAMS, but the programs it
generates are expected to conform to the Verify test process, which is
documented in util/Verify.hxx .  We specify these separately so that
we can run automatic test scripts which can automatically show which
parts of the system fail regression testing.

"include $(BUILD)/Makefile.post" includes the "post" portion of the
Makefiles (see above).


----------------------------------------------------------------------
How to add a new file to a library or program
----------------------------------------------------------------------

Add the new source file to the SRC target.


----------------------------------------------------------------------
How to add a new test case
----------------------------------------------------------------------

Add the new test case to TESTPROGRAMS or VERIFY_TESTCASES, as
appropriate.

----------------------------------------------------------------------
How to use an additional package
----------------------------------------------------------------------

Add the package to the PACKAGES variable.  Make sure that you add any
PACKAGES it depends on (look at the PACKAGES line in the makefile
which builds it).

----------------------------------------------------------------------
Adding a new directory
----------------------------------------------------------------------

- create directory

- copy makefile from somewhere else

- make sure BUILD is set to point to the build directory (otherwise
	you'll get a no such file or directory for Makefile.pre)

- set SRC

- set TESTPROGRAMS or TARGET_LIBRARY (or both) depending on whether
it's a library or program.


----------------------------------------------------------------------
Build Options and What they do
----------------------------------------------------------------------

Please see documentation where they are used.  I will try to document
these in the Makefiles.  Some are "user accessible" via the make
command line, such as CODE_OPTIMIZE=1.  Those should be documented in
README_BUILD.txt .

----------------------------------------------------------------------
TODO for build process
----------------------------------------------------------------------

- automatic package dependency selection

- a way to specify packages more sanely, and in the directory which
creates the package or something

======================================================================

Copyright 2000-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.

$Id: README_BUILD.txt,v 1.1 2004/05/01 04:14:55 greear Exp $

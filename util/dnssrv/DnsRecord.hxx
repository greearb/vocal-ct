#ifndef DnsRecord_HXX
#define DnsRecord_HXX

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
 * IN EXCESS OF ,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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



static const char* const DnsRecord_hxx_Version =
    "$Id: DnsRecord.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";
/*
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <string>
#include "cpLog.h"


/** simple 5-tuple DNS SRV/A  record:
 *  priority and weight do no apply to A record
 */
class DnsRecord {

  public:
	/// constructor:
	DnsRecord(): myQtype(0), myPriority(0), myWeight(0),
                 myPort(0), myTarget(""), myAddress("")
	{
#if 0
		// the following was used for debugging
		myTimesSelected[0] = myTimesSelected[1] =      
		myTimesSelected[2] = myTimesSelected[3] = 0;
#endif
     }

    // copy constructor
    DnsRecord( const DnsRecord& rhs );

    // assignment operator
    DnsRecord& operator=( const DnsRecord& rhs );

    /// destructor:
	virtual ~DnsRecord() 
	{
        cpLog( LOG_DEBUG, "~DnsRecord() invoked" ); 
	}

    /// accessors:
    int getQtype()    const { return myQtype; }
    ///
	int getPriority() const { return myPriority; }
    ///
    int getWeight()   const { return myWeight; }
    ///
	int getPort()     const { return myPort; }
    ///
    const char*  getTarget() const { return myTarget.c_str(); }	
    ///
	const char* getAddress() const { return myAddress.c_str(); }
	 
	/// mutators:
    void setQtype( int qt )      { myQtype = qt; }
    ///
	void setPriority( int prio ) { myPriority = prio; }
    ///
	void setWeight( int wt ) { myWeight = wt; }
    ///
	void setPort( int prt ) { myPort = prt; }
    ///
	void setTarget( const string& name ) { myTarget.assign( name ); }
    ///
	void setAddress( const string& addr ){ myAddress.assign( addr ); }

    /// to display content of this record:
	void printRecord( ) {
	    cpLog( LOG_DEBUG,
		"t=%3d   p=%3d    w=%4d  port=%5d  target=%s addr=%s\n",
	    myQtype, myPriority, myWeight, myPort, myTarget.c_str(),
		myAddress.c_str());
	}

	/// increment myTimesSelected[ place ]
	void incrTimesSelected( int idx ) 
	{
#if 0
			myTimesSelected[idx]++;
#endif
    }

	/// clear myTimesSelected[]
	void clearTimesSelected( ) 
	{ 
#if 0
	    myTimesSelected[0] = myTimesSelected[1] =
		myTimesSelected[2] = myTimesSelected[3] = 0;
#endif
	}
    
    /// to verifiy the load balancing algorithm works for SRV records
	void printTally( ) {
#if 0
	    int sum = myTimesSelected[1]+myTimesSelected[2]+myTimesSelected[3];
	    cpLog( LOG_DEBUG,
		"p=%3d    w=%4d   1st=%4d   2nd=%4d   3rd=%4d   sum=%4d   t=%s\n",
	    myPriority, myWeight, myTimesSelected[1], myTimesSelected[2],
		myTimesSelected[3], sum, myTarget.c_str());
#endif
	}

  private:
    /// equality operator - suppressed
    bool operator==( const DnsRecord& rhs );

	int myQtype;                // SRV or A record type
    int myPriority;             // appl. to SRV record only
    int myWeight;               // appl. to SRV record only
	int myPort;                 // appl. to SRV record only
    string myTarget;            // server or host name: "xxx.yy.com"
	string myAddress;           // "a.b.c.d" notation

#if 0
	/** for debug only:
	 *  used by printTally()
	 */
	int myTimesSelected[4];     // # myTimes this record gets picked first,
	                          // second or third (0 not used)
#endif
};

#endif

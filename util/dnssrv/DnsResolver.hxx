#ifndef DnsResolver_H
#define DnsResolver_H

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
 * IN EXCESS OF 1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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



static const char* const DnsResolver_hxx_Version =
    "$Id: DnsResolver.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";
//
//  Wrapper for DNS SRV (Server Selection) or A Record

#if defined(__sparc)|| defined(__SUNPRO_CC)
#if !defined(MAXHOSTNAMELEN)
#include <netdb.h>
#if !defined(MAXHOSTNAMELEN)
#define MAXHOSTNAMELEN 64
#endif // !defined(MAXHOSTNAMELEN)
#endif // !defined(MAXHOSTNAMELEN)
#endif // defined(__sparc)

#include <cstdlib>
#include <cstdio>
#include <stdio.h>
#ifdef __linux__
#include <getopt.h>
#endif
#include "sys/types.h"
#include "netinet/in.h"
#include "netdb.h"
#include "errno.h"
#include "arpa/nameser.h"

// need the following, else got:
// src/include/resolv.h:119: invalid exception specifications error
#undef __P
#define __P(args) args
#include <resolv.h>
#include <arpa/inet.h>

#include <vector>
#include <algorithm>

#include "Sptr.hxx"
#include "DnsRecord.hxx"
#include "cpLog.h"



#define PARM1SIZE 128
#define PARM2SIZE 1024


class DnsResolver
{
    typedef vector < Sptr < DnsRecord > >  VECTOR_DNS;

    private:
    ///
    bool     myReady;       // is this object ready to be used?
    int      myNumDns;      // # of Dns records matching the QName
	int      myCurIdx;      // current index to myVDns records
	int      myQType;       // qualified record type: srv or A record
    string   myQName;       // qualified service.protocol.domain name
    string   myTarget;      // domain name

    VECTOR_DNS     myVDns;

    public:
    /// constructor - for testing purposes
    DnsResolver( const char * target);

	/**
	 *  target - e.g. vovida.org
	 *  qType - ns_t_a (A record lookup) or ns_t_srv (SRV record)
	 *  serviceType - e.g. _sip, OK to be null
	 *  protocol - e.g. _udp, OK to be null
     *  (For SRV record type, a qualified name will be built using
     *  "serviceType"."protocol"."target" format)
	 */
	DnsResolver(const char * target,int qType, 
                const char * serviceType, const char * protocol);

    /// destructor
    virtual ~DnsResolver();

	/// 
	bool isReady() const  { return myReady; }

	///
	int getNumDnsRecords() const { return myNumDns; }

	/// do a ns lookup and store result, if any, in myVDns
	void dnsLookUp();

	/** do server selection base on the list of servers already
	    cached locally
	 */
	void reOrder();  
    
    /** return a reference to a DnsRecord given an index, does not
     *  affect/ update myCurIdx
     *  returns myVDns.end() if idx is out of bound
     */
    VECTOR_DNS::iterator getRecord( int idx );

	/** like an iterator, locate the next record in myVDns using
	 *  myCurIdx is an index to the vector of Dns records
	 */
	bool getNextRecord( DnsRecord& rec );

    /** select a DnsRecord based on the callID
     */
    bool selectRecord( const string& callId, DnsRecord& rec );

    /** given a DnsRecord, get the hostname for the domain in the record:
     *  (uses NetworkAddress)
     */
	int getHostByRecord( const DnsRecord& rec, struct hostent* hEnt,
					     char* buf, int buflen );

	/// remove DNS records pointed by myVDns
	void clear();

	/// display dns record error
	void nsError( int error, const char* domain );

	// for debugging only
	void addSrvRecord( int prio, int wt, int port, const string& name ) {
        Sptr < DnsRecord > rec = new DnsRecord;
	    assert (rec != 0);
		rec->setQtype( ns_t_srv );
	    rec->setPriority( prio );
	    rec->setWeight( wt );
	    rec->setPort( port );
	    rec->setTarget( name );
	    myVDns.push_back( rec );
	    myNumDns++;
	    myReady = true;   
	}
	
    /// sort list by priority (lowest number first)
    void sortByPreference( );

	/// clone a copy of the srv vector:
	bool cloneVSrv( VECTOR_DNS& localDns ) { return false; }

    /// 
    void print(int logLevel );

	///
	void printTally( );      // used for debugging only
	

    protected:
    /**  request NameServer to return records matching the given domain, 
	 *   called by dnsLookUp()
	 */
    bool findRecords( void );

    /// invoked by dnsLookUp()
    void loadBalance(void );

	///
	void buildRecords( ns_msg handle, ns_sect section, int responseLen);

	///
	void buildSRVRecords( ns_msg handle, ns_sect section, int responseLen,
                          int numRecords);

	///
	void buildARecords( ns_msg handle, ns_sect section, int responseLen,
                        int numRecords);

	/// sort range of same priority by a random weight factor
    void sortByWeight( int startIdx, int endIdx );

    const u_char * expandName(const u_char *cp, const u_char *msg,
                               char *name, int namelen);

	/// locate a set of records that shares the same priority value
    int findPriorityRange( int startIdx );

	/// calculate the sum of the weights within the given range
    int addWeights( int startIdx, int endIdx );

};

#endif //DnsResolver_H

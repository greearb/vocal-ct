
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


static const char* const DnsResolver_cxx_Version =
    "$Id: DnsResolver.cxx,v 1.1 2004/05/01 04:15:33 greear Exp $";


#include "vtypes.h"
#include "NetworkAddress.h"
#include "DnsResolver.hxx"
  

const int MY_PACKETSZ = 64*1024;     // space to hold tcp answers
const int DEFAULT_PRIO= 10;          
const int DEFAULT_WT  = 10;

// constructor for testing: default myQType to SRV lookup
DnsResolver::DnsResolver(const char * target) :
                         myReady(0), myNumDns(0), myCurIdx(0),
                         myQType(ns_t_srv)
{ 
    assert( target != 0 );
    myTarget.assign(target);
    myQName.assign(target);
	
    (void) res_init();   // init resolve package

}

DnsResolver::DnsResolver(const char * target, int qType, 
                         const char * serviceType, const char* protocol):
                         myReady(false), myNumDns(0), myCurIdx(0),
                         myQType(qType)
{
    assert( target != 0);

    myTarget.assign(target);

    if (qType == ns_t_a)      // A record type
    {
        myQName.assign(target);
    }
    else                     // assume SRV record type
    {
        string s = "";
        string p = "";
        if (serviceType)
        {
            cpLog(LOG_DEBUG, "serviceType = %s", serviceType);
            s = serviceType;
            s+= ".";
        }
        if (protocol)
        {
            cpLog(LOG_DEBUG, "protocol = %s", protocol);
            p = protocol;
            p+= ".";
        }
        s += p;
        cpLog(LOG_DEBUG, "s+p = %s", s.c_str());

        s += target;
        cpLog(LOG_DEBUG, "s+p+target = %s", s.c_str());
        myQName.assign( s );
    }
    cpLog(LOG_DEBUG, "myQName = %s", myQName.c_str());

    (void) res_init();   // init resolve package

	dnsLookUp(); 
}  // DnsResolver

///
DnsResolver::~DnsResolver()
{
    //  deallocate memory
    cpLog( LOG_DEBUG, "entered DnsResolver destructor");
    clear();
}

// This method is called on object instantiation and whenever the
// user wants to refresh the local dns cache.
void DnsResolver::dnsLookUp()
{
    if (myReady)
    {
        clear();    // clean up local cache
    }
	findRecords();         // do the dns lookup
    int num = myVDns.size();
    if ((num == 0) && (myQType == ns_t_srv))
    {
        cpLog( LOG_DEBUG, "no srv records, try finding A records:");
        // reset type to A record, and use domain name instead
        myQType = ns_t_a;
        myQName.assign(myTarget);
        findRecords();
        
    }
    int numRec = myVDns.size();
	if ((isReady()) && (numRec > 1))
    {
        loadBalance();   
    }
    else if (numRec == 1)
    {
        cpLog( LOG_DEBUG, 
               "dnsLookUp() - no loadbalancing done");
    }
    else
    {
        cpLog(LOG_ERR,
              "dnsLookUp() not complete: DnsResolver obj malformed!");
    }
  
	
}  // dnsLookUp

// shuffle the ordering of the list of Dns records based on priorities
// and weights.  Note that if this object contains a list of A records,
// then all entries have the same default priority (10) and weight (10).
void DnsResolver::reOrder()
{
    myCurIdx = 0;   // reset the index pointer
	if (myReady)
    {
        loadBalance();
    }
	else
    {
        cpLog( LOG_DEBUG, 
               "DnsResolver object is not well formed, reOrder not done!");
    }
}  // reOrder

// 
Sptr<DnsRecord> *  /* VECTOR_DNS::iterator */ DnsResolver::getRecord( int idx )
{
    if ((idx < 0) || (idx >= myNumDns))
    {
       cpLog(LOG_DEBUG, "getRecord() fails: bad index %d", idx);
       return myVDns.end();
    }
    return(&(myVDns[idx]));
}

// get the record (SRV or A) indexed by myCurIdx:
bool DnsResolver::getNextRecord( DnsRecord& rec )
{
    if (myReady == false)
    {
        cpLog( LOG_DEBUG, 
               "cannot get next Dns record: DnsResolver obj not well formed!");
        return false;
    }
    if (myCurIdx >= myNumDns)
	{
        cpLog( LOG_DEBUG,
               "list of Dns records exhausts");
		// user may want to call reOrder() to generate a different
		// ordered list and call getNextRecord() again
        return false;	
	}
	// return a reference of myVDns[myCurIdx], and advance myCurIdx
	rec = *(myVDns[myCurIdx++]);
	return true;

}  // getNextRecord()

// 
bool DnsResolver::selectRecord( const string& callId, DnsRecord& rec )
{
    return false;
}

int DnsResolver::getHostByRecord( const DnsRecord& rec, 
                 struct hostent* hEnt, char* buf, int buflen )
{
    int rtn = 0;
	int nsErr = 0;
	const char * host = rec.getTarget();

    // pre-conditions checks:
	assert( hEnt ); 
	assert( buf );
	assert( buflen );

    rtn = NetworkAddress::getHostByName( host,
					hEnt, buf, buflen, &nsErr );
	if (rtn != 0)
    {
        nsError( nsErr, host );
        return rtn;
    }
	return rtn;
}  // getHostByRecord

///
void
DnsResolver::clear()
{
    if (myReady == true)
	{
        // leave myQName and myQType as is
        myVDns.clear();
        myNumDns = 0;
		myCurIdx = 0;
        myReady  = false;
	}
    
}   // clear()


/**
 * 
 */
void
DnsResolver::nsError( int error, const char* domain )
{
    switch(error){
        case HOST_NOT_FOUND:
            cpLog(LOG_DEBUG, "Unknown domain: %s\n", domain);
        break;   
        case NO_DATA:    // same as NO_ADDRESS
            cpLog(LOG_DEBUG, "No DNS records for %s\n", domain);
        break;
		case NO_RECOVERY:
		    cpLog(LOG_DEBUG, 
			"non-recoverable error occurs during nameserver lookup for %s\n", 
			domain);
		break;
        case TRY_AGAIN:
            cpLog(LOG_DEBUG, "No response for DNS query\n");
        break;
        default:
            cpLog(LOG_DEBUG, "Unexpected error=%d, domain=%s\n",
                           error, domain);
        break;
    }

}   // end of nsError()

/**
 * locate the DNS records
 */
bool
DnsResolver::findRecords()
{
    union {
       HEADER hdr;              /* defined in resolv.h */
                                /* next line changed NS_PACKETSZ to PACKETSZ */
       u_char buf[PACKETSZ];    /* defined in arpa/nameser.h */
    } response;                 /* response buffers */

    int responseLen;            /* buffer length */
    ns_msg handle;              /* handle for response packet */
    char* domain = (char*)myQName.c_str();

    // seed the random number generator
    // **************** TBD: don't use srand
    struct timeval t;
    gettimeofday(&t, 0);
    srand( (unsigned int)(t.tv_usec ^ t.tv_sec) );

    // Ref: "DNS and BIND" book
    // Look up the SRV records for the given domain name.    
    cpLog(LOG_DEBUG, "Domain is %s\n",domain);
    if((responseLen =
        res_search(domain,  /* the domain we care about */
        ns_c_in,                   /* Internet class records   */
        myQType,                   /* ns_t_srv or ns_t_a       */
        (u_char *)&response,       /* response buffer          */
        sizeof(response)))         /* buffer size              */
                           < 0)    /* If negative              */
    {
        nsError(h_errno, domain);      /* report the error         */
        return false;                  /* and quit                 */
    }
 
    /* Initialize a handle to this response.  The handle will
     * be used to extract information from the response.  */
    if (ns_initparse(response.buf, responseLen, &handle) < 0)
    {
        cpLog(LOG_INFO, "ns_initparse: %s\n",strerror(errno));
        return false;
    };

    // place records found in a vector, myVSrv: 
	// search result from the answer section
    buildRecords(handle, ns_s_an, responseLen);

    // sort records based on the priority of the srv records
    // records having the same priority:
	if (myVDns.size() > 1)
    {
       sortByPreference();
    }
 
    return myReady;
    
}   // findRecords()

/**
 * Load balancing algorithm based on the weight.
  assume a list of SRV records in myVDns having the same
  priority, find that range:
  1) add up all weights within the range = s
  2) pick a random number between 0..1 = r
  3) calculate the random weight, rw = r*s
  4) the weight of records partitions sumOfWeights: what part
     does the random weight fall into? (find that record, rrd)
     Let current sum of weight so far, cs = 0
     Loop all records w/i the priority range:
        given weight of current record, cw: cs +=cw;
        if rw <= cw
           swap out current record with rrd;
           break from loop
        else
           continue to loop
 */
void 
DnsResolver::loadBalance( )
{
 
    cpLog( LOG_DEBUG, "\nloadBalance(): \n" );

	if (myVDns.size() <= 1)   // don't bother for single record
	{
       return;
	}
    VECTOR_DNS::iterator i = myVDns.begin();
    int prio;
    int startIdx, endIdx;

    endIdx = -1;    // end of previous range

    while (i != myVDns.end())
    {
        prio = (*i)->getPriority();
        startIdx = endIdx+1;     // start of current range

        endIdx = findPriorityRange( startIdx); 

        // let's find all srv records of same priority
        if (startIdx == endIdx) 
        {
            cpLog(LOG_DEBUG, "range of 1 found for priority=",  prio);
            i++;
        }
        else
        {
            // sort same priority range by weight:
            sortByWeight( startIdx, endIdx );

            // advance iterator to first element after endIdx:
            int n  = endIdx - startIdx + 1;
            advance(i,n);
        }
    }

    print( LOG_DEBUG ); 

#if 0
    // **************  debug only   **************
    // applicable for testLoadBalance testcase #4 only:
    myVDns[0]->incrTimesSelected(1);
    myVDns[1]->incrTimesSelected(2);
    myVDns[2]->incrTimesSelected(3);
#endif
}   // loadBalance()

/**
 *  print elements in the myVDns vector
 */
void
DnsResolver::print( int logLevel )
{
   VECTOR_DNS::iterator i;

    if ((logLevel < 0) || (logLevel >= LAST_PRIORITY))
    {
        logLevel = LOG_DEBUG;
    }
    cpLog(logLevel,      "\nprio   weight   target:port address\n");
    for (i=myVDns.begin(); i != myVDns.end(); i++)
    {
         cpLog(logLevel, "\n%d   %5d  %s:%d  %s\n",
                (*i)->getPriority(), (*i)->getWeight(),
                (*i)->getTarget(), (*i)->getPort(),
				(*i)->getAddress());
    }
 
}   // print()

/**
 *  print myVDns vector including times selected to be 1st, 2nd & 3rd:
 *  ************** use for DEBUG ONLY ***************
 */
void
DnsResolver::printTally(  )
{
   VECTOR_DNS::iterator i;

    cpLog(LOG_DEBUG, "\n****** Final tally ******\n");
    for (i=myVDns.begin(); i != myVDns.end(); i++)
    {
         (*i)->printTally();
    }
 
        findRecords();
}   // printTally()


/// Build a list and print the DNS response records found 
//
void DnsResolver::buildRecords(ns_msg handle, ns_sect section, int responseLen)
{
    int numRecords = ns_msg_count(handle, section);
    cpLog(LOG_DEBUG, "Response Length is: %d\n"
                     "Number of records found: %d\n",
                     responseLen, numRecords);
    // for efficiency, reserve capacity of myVDns vector to
    // avoid dynamic reallocation of vector, actual size
    // will be <= capacity (because not all records found
    // will match the Record Class we are looking for: e.g.
    // code will ignore CNAME record
    myVDns.reserve( numRecords );  

    if (myQType == ns_t_srv)
	{
        cpLog( LOG_DEBUG, "building SRV records" );
        buildSRVRecords( handle, section, responseLen, numRecords );
    }
	else if (myQType == ns_t_a)
    {
        cpLog( LOG_DEBUG, "building A records");
        buildARecords( handle, section, responseLen, numRecords );
    }
	else
	{
        cpLog( LOG_DEBUG, "don't know how to build record type=%d", myQType );
	}
}  // buildRecords


void DnsResolver::buildSRVRecords(ns_msg handle, ns_sect section,
                                  int responseLen, int numRecords)
{
    int rrnum;              /* resource record number */
	int rrt;                /* resource record type */
    ns_rr rr;               /* expanded resource record */
    u_char *cp;             /* character point to results (rdata in ns_rr) */
    char name[NS_MAXDNAME]; /* expanded response hostname */

    /* Loop through each record found  */
    for(rrnum = 0; rrnum < numRecords; rrnum++)
    {
        /* Expand the resource record number rrnum into rr.  */
        if (ns_parserr(&handle, section, rrnum, &rr))
        {
            cpLog(LOG_DEBUG, "ns_parserr: %s, skip rr #%d\n", 
							strerror(errno), rrnum);
            continue;
        }
        rrt = ns_rr_type(rr);
		if (rrt != ns_t_srv)
        {
            cpLog(LOG_DEBUG, "skipping record number: %d, type: %d", 
							rrnum, rrt);
            continue;
        }

        DnsRecord *rec = new DnsRecord;

        /* Check to see if answer is authoritive */
        if(!ns_msg_getflag(handle,ns_f_aa))
        {
            cpLog(LOG_DEBUG,"Warning: Nameserver is not authoritative.\n");
        }

        /* Set cp to point the SRV record.  */
        cp = (u_char *)ns_rr_rdata(rr);
        rec->setPriority( ns_get16(cp) );

        cp += sizeof(u_short);
        rec->setWeight( ns_get16(cp) );

        cp += sizeof(u_short);
        rec->setPort( ns_get16(cp) );

        cp += sizeof(u_short);
        cp = (u_char *)expandName(cp, ns_msg_base(handle), name, sizeof(name));
        rec->setTarget( name );

        struct hostent hEnt;
        char buf[2048];
        if (getHostByRecord( *rec, &hEnt, buf, sizeof(buf))==0)
        {
            unsigned char* addr = (unsigned char*)(hEnt.h_addr_list[0]);
            char addrBuf[1000];
            sprintf(addrBuf, "%d.%d.%d.%d",
                    int(*(addr + 0)),
                    int(*(addr + 1)),
                    int(*(addr + 2)),
                    int(*(addr + 3)) );
            rec->setAddress(addrBuf);
        }
        else
        {
            rec->setAddress("0.0.0.0");
        }
        cpLog(LOG_DEBUG, "Target/address = %s/%s", name,
              rec->getAddress());

        // put record in a multimap
        // mmSrv.insert( make_pair( rec->getPriority, rec ) );  

        // insert record at end of vector
        myVDns.push_back( rec );
        // curious: what is the vector's capacity?
        // cpLog(LOG_DEBUG, "Capacity of myVDns = %d", myVDns.capacity());

        cpLog(LOG_DEBUG, "\nSRV Priority field: %d\n"
                         "SRV Weight field: %d\n"
                         "SRV Port field: %d\n"
                         "SRV hostname field: %s\n"
                         "Resource Record Type: %d\n"
                         "Record Class: %d\n"
                         "Resource Record Time To Live (ttl): %d\n",
		             rec->getPriority(), rec->getWeight(), rec->getPort(),
					 name, rrt, ns_rr_class(rr), ns_rr_ttl(rr));
    }
    myNumDns = myVDns.size();
    myReady = true;
    cpLog(LOG_DEBUG, "Number of DNS SRV records = %d\n", myNumDns);
}  // buildSRVRecords()

// parse the result of A Record lookup and build a DnsRecord
void DnsResolver::buildARecords(ns_msg handle, ns_sect section,
                               int responseLen, int numRecords)
{
    int rrnum;              /* resource record number */
	int rrt;                /* resource record type */
    ns_rr rr;               /* expanded resource record */
    u_char *cp;             /* character point to results (rdata in ns_rr) */
    // char name[NS_MAXDNAME]; /* expanded response hostname */

    /* Loop through each record found  */
    for(rrnum = 0; rrnum < numRecords; rrnum++)
    {
        /* Expand the resource record number rrnum into rr.  */
        if (ns_parserr(&handle, section, rrnum, &rr))
        {
            cpLog(LOG_DEBUG, "ns_parserr: %s, skip rr #%d\n", 
							strerror(errno), rrnum);
            continue;
        }
        rrt = ns_rr_type(rr);
		if (rrt != ns_t_a)
        {
            cpLog(LOG_DEBUG, "skipping record num: %d, type: %d", 
							rrnum, rrt);
            continue;
        }
        DnsRecord *rec = new DnsRecord;

        /* Expand the resource record number rrnum into rr.  */
        if (ns_parserr(&handle, section, rrnum, &rr))
        {
            cpLog(LOG_INFO, "ns_parserr: %s\n", strerror(errno));
        }
        /* Check to see if answer is authoritive */
        if(!ns_msg_getflag(handle,ns_f_aa))
            cpLog(LOG_INFO,"Warning: Nameserver is not authoritative.\n");

        /* Set cp to point the A record.  */
        cp = (u_char *)ns_rr_rdata(rr);
		u_int32_t ip4Addr = ns_get32(cp);
        struct in_addr  addr;
		// convert ip4 @ to network byte ordering
		addr.s_addr = htonl(ip4Addr);
		string strAddr = inet_ntoa( addr );
		rec->setAddress( strAddr ); 

		rec->setPriority( DEFAULT_PRIO );
        rec->setWeight( DEFAULT_WT );
		rec->setTarget( myQName );
        cpLog(LOG_DEBUG, "\nA Record host: %s, IP4 @: %d(%s), prio: %d, wt: %d\n" 
						"Resource Record Type: %d\n"
						"Record Class: %d\n"
						"Resource Record Time To Live (ttl): %d\n",
              myQName.c_str(), ip4Addr, strAddr.c_str(), rec->getPriority(),
			  rec->getWeight(), ns_rr_type(rr), ns_rr_class(rr), 
			  ns_rr_ttl(rr));

        // insert record at end of vector
        myVDns.push_back( rec );
        // curious: what is the vector's capacity?
        // cpLog(LOG_DEBUG, "Capacity of myVDns = %d", myVDns.capacity());

    }
    myNumDns = myVDns.size();
	myReady = true;
    cpLog(LOG_DEBUG, "Number of A records found = %d\n", myNumDns);
}  // buildARecords()

/// compare function used to sort the DNS SRV records:
//
struct comp_srv : public binary_function< Sptr <DnsRecord>,
                                          Sptr <DnsRecord>, bool >
{
    bool operator() (Sptr <DnsRecord> iRec1, Sptr <DnsRecord> iRec2)
    {      
         return iRec1->getPriority() < iRec2->getPriority();
    }
};   // comp_srv()

/**
 */
void 
DnsResolver::sortByPreference( )
{
    cpLog( LOG_DEBUG, "\nsortByPreference():\n(orig order)");
	if (myVDns.size() <= 1)
    {
       // no need to sort
       return;
	}
    print(LOG_DEBUG_STACK);   
    sort(myVDns.begin(), myVDns.end(), comp_srv());
	cpLog( LOG_DEBUG_STACK, "\n(new order):" );
    print(LOG_DEBUG_STACK);

}

/**
/// given the start and end indices of the svr vector, sort
// the elements with that range by their weights in a 
// random process
 */
void 
DnsResolver::sortByWeight( int startIdx, int endIdx )
{
    Sptr <DnsRecord> tRec;   // temporary

    cpLog( LOG_DEBUG, "sortByWeight(): startIdx=%d, endIdx=%d", 
           startIdx,endIdx );

	if (myVDns.size() <= 1)
    {
       // no need to sort
       return;
	}
   int sumOfWeights = addWeights( startIdx,  endIdx );
   if (sumOfWeights == 0)
   {
       cpLog( LOG_DEBUG,  "sum of weights is 0" );         
       return;    // nothing to sort
   }

   int j;

   // calculate the sum of weights
   for (int i = startIdx; i < endIdx; i++)
   {
       sumOfWeights = addWeights( i, endIdx );

       // generate a random weight:
       double randWeight = (double (rand() )/RAND_MAX) * sumOfWeights;
       int curWeight  = 0;    // sum of weights so far

       // traverse each element till the one before the last (endIdx),
       // (if j == endIdx, no need to swap)
       for (j = i; j <= endIdx; j++)
       {
           curWeight += myVDns[j]->getWeight();
           if ( randWeight < curWeight )
           {
               // found element matching the random weight         
               // if randWeight falls on the current start range, skip
	           cpLog(LOG_DEBUG_STACK, "randWeight=%d, curWeight=%d, i=%d, j=%d",
                     randWeight, curWeight, i, j );
               break;
           }
       }

       if (j > endIdx)
       {
           j = endIdx;   // make sure j is w/i range
       }

       // swap the selected element w/ the i-th element, this
       // sorts the range
       if (i != j)
       {
           tRec = myVDns[i];
           myVDns[i] = myVDns[j];
           myVDns[j] = tRec;
       }

       // compute sumOfWeights for i+1 through endIdx:
       sumOfWeights -= myVDns[i]->getWeight();  // i th element will not be con-
                                              // sidered in the next iteration

   }
}   // sortByWeight

/**
 *
 */
const u_char*
DnsResolver::expandName(const u_char *cp, const u_char *msg,
            char *name, int namelen)
{
    cpLog( LOG_DEBUG, "expandName()" );
    int n=dn_expand(msg, msg + MY_PACKETSZ, cp, name, namelen - 2);
    if (n < 0)
    {
        return (0);
    }
    if (name[0] == '\0')
    {
        name[0] = '.';
        name[1] = '\0';
    }
    return (cp + n);
}   // expandName()


/**
 *  find the range of srv records that shares the same priority:
 *  for A records, we fake the priority, default to 10
 */
int
DnsResolver::findPriorityRange( int startIdx)
{
    cpLog( LOG_INFO, "findPriorityRange()" );
    int length = myVDns.size();

    assert( startIdx < length );
    int endIdx = startIdx+1;

    int prio = myVDns[startIdx]->getPriority();
    cpLog( LOG_DEBUG, "findPriorityRange(): prio=%d, startIdx=%d",
           prio, startIdx );

    while ((endIdx < length) &&(prio == myVDns[endIdx]->getPriority()))
    {
        endIdx++;
    }

    return endIdx-1;

}   // findPriorityRange()

/** calculate the total sum of the weights
 */
int 
DnsResolver::addWeights( int startIdx, int endIdx )
{
    
    int sumOfWeights = 0;

    for (int x = startIdx; x <= endIdx; x++)
    {
        sumOfWeights += myVDns[x]->getWeight();
    }
    return sumOfWeights;
}   // addWeights()

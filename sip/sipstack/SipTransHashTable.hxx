#ifndef _Sip_Trans_Hash_Table__hxx
#define _Sip_Trans_Hash_Table__hxx

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
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
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

static const char* const SipTransHashTable_hxx_version =
    "$Id: SipTransHashTable.hxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "VRWLock.hxx"
#include "SipTransactionId.hxx"

#include "cpLog.h"

namespace Vocal
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK


/**
 * define this if we want to do rehash in the background as a seperate thread
 */
//#define REHASH_THREAD

#ifdef REHASH_TREAD
#define REHASH_THREASHOLD 10
#endif

/**
 * define this if we want to lock nodes in lookups. it may not be
 * required 'coz we'll process a msg completely before working with another
 * msg. (and only one side of the TransactionDB will be "creating" new nodes)
 *
 * note that this is different from Top Level node locking, which will still
 * be there (but within Level1Node structure).
 *
 *
 * ^^^ and this is required, to control race between findxxxx and the cleanup
 * thread. specifically, some findxxxx caller might be waiting on readlock of
 * the existing top level node, while cleanup has the write lock and deletes
 * the node (which is illegal, coz the lock allows to new/delete on the lower
 * levels, but not the "*this" level)
 *
 * ^^^ is necessary *just* to flush out any slow executing find/inserts
 */
#define USE_NODE_LOCK

/// forward declarations...
struct SipTransLevel1Node;


/**
 * this is a *special purpose* implementation of hash table to be used by
 * TransactionDB.
 *
 * reason for not using stl hash table instead is that we don't want to lock
 * the *whole* hash table, when doing a lookup. in this implementation, it only
 * locks the hashed bucket, which is quickly unlocked once the collision list
 * has been processed.
 *
 * this structure will be concurrently operated by three threads, viz., Worker
 * (thru transceiver, for outgoing messages), Sip (thru filter, for 
 * incoming messages) and the TransactionDB's cleanup (will call erase)
 *
 * "how to use it" instructions for SentResp and SentReq ...
 *
 * ... lookups for requests will be using the findOrInsert method, while for
 * responses will be using find (it does'nt makes sense to creat a new call leg
 * for a response!)
 */


class SipTransHashTable
{
    public:
	typedef SipTransactionId::KeyTypeI KeyType;
	typedef unsigned long SizeType;
	class Bucket;
	class Node;
#ifdef USE_NODE_LOCK
	class SipTransRWLockHelper;
#endif
	SipTransHashTable(SizeType sizeHint);

        /// this might be a problem, may be not, by conflicting w/ erase being called
        /// from cleanup thread. hence, should be called by user only when cleanup
        /// thread has been "joined"
	virtual ~SipTransHashTable();

	Node* find(const KeyType& key
#ifdef USE_NODE_LOCK
		   , SipTransRWLockHelper* rwHelper=0
#endif
	    );
	Node* findOrInsert(const KeyType& key
#ifdef USE_NODE_LOCK
			   , SipTransRWLockHelper* rwHelper=0
#endif
	    );

#ifdef USE_NODE_LOCK
	/// once processing is done, user should call this
	void release(Node* node);
	/// OR use RW helper!!! (**** safer ***)
#endif

	Node& operator[](const KeyType& key);

	/// method specifically for cleanup to acquire lock for top level node
	void lock(Node * node, SipTransRWLockHelper * helper = 0);

	/// this method will be used to erase a node from hash
	void erase(Node* node);

	/// and to release the locks
	void unLock(Node * node);

	///// debugging method
	Data giveDetails();

    private:
	SipTransHashTable& operator= (const SipTransHashTable&);
	bool operator==(const SipTransHashTable&);
	SipTransHashTable(const SipTransHashTable&);
	SipTransHashTable(); //Force user to specify size

    private:
	friend class SipTransDebugger;
	Bucket * buckets;
	SizeType size;
	SizeType count;
#ifdef REHASH_THREAD
	bool rehashFlag;
#endif
	VRWLock rwLock;

	static SizeType nextSize(SizeType sizeHint);
	static SizeType hash(const SipTransactionId::KeyTypeI& key, SizeType size);

	/// when this is called, the table is already locked for write
	/// TODO
	void rehash(SizeType sizeHint);
#ifdef REHASH_THREAD
	///// declare the thread wrapper
#endif
	void rehashFunc(SizeType sizeHint);

    public:
	class Bucket
	{
            public:
		Bucket()
                    :first(0), rwLock() {
                    atomic_inc(&_cnt);
                }
                ~Bucket() {
                    atomic_dec(&_cnt);
                }

                static int getInstanceCount() { return atomic_read(&_cnt); }

	    private:
		Bucket& operator= (const Bucket&);
		Bucket(const Bucket&);

	    private:
		friend class SipTransDebugger;
		friend class SipTransHashTable;
		Node* first;
		/**
		 * this lock is required 'coz operations of TransactionDB will span beyond
		 * the scope of hashtable's method, i.e., lookup. hence, we need some
		 * way of guaranteeing thread safety during erase, rehash and insert
		 *
		 * actually ^^^^^ is NOT correct, although we need the lock, but just
		 * during the scope of the method. outside the scope of hashtable, users
		 * only use the myNode member, and that is not changed/touched by the
		 * hashtable methods
		 */
		VRWLock rwLock;
                static atomic_t _cnt;
	};

	class Node
	{
            public:
		SipTransLevel1Node* myNode;
		SipTransHashTable * myTable;
		Node(const KeyType& key)
		    : myNode(0),
		      myTable(0),
		      myBucket(0),
		      next(0),
		      keyCopy(key)
#ifdef USE_NODE_LOCK
		    ,rwLock()
#endif
		{
                    atomic_inc(&_cnt);
                }

                ~Node() {
                    atomic_dec(&_cnt);
                }

                static int getInstanceCount() { return atomic_read(&_cnt); }
	    private:
		Node& operator= (const Node&);
		Node(const Node&);

	    private:
		friend class SipTransDebugger;
		friend class SipTransHashTable;
		Bucket* myBucket;

		Node *next;
		const KeyType keyCopy;

                static atomic_t _cnt;

/**
 * don't need a RW lock here, 'coz only hash table will modify these nodes, and
 * if bucket is locked then don't need to worry about erase or rehash
 *
 * actually ^^^^ is wrong, we need a rw lock here so that reference to bucket
 * can be safely accessed (race conditions with rehash)
 */

/**
 * even ^^^^ is wrong, actually lock the table first to acheive lock on bucket
 * and hence don't need the lock here
 */

#ifdef USE_NODE_LOCK
/**
 * all of ^^^^ holds, but for the hashtable. for the transactiobDB we still
 * need to lock the node - to take care of find Vs insert
 */
		VRWLock rwLock;
#endif
	};

#ifdef USE_NODE_LOCK
/**
 * this is a RW lock helper specific to the SipTransactionDB and the hash table
 * above. it will be used to take care of read/write lock/unlock on the nodes
 * being passed in between the two classes.
 *
 * user can also directly lock/unlock the nodes, this utility class is just
 * to make the exchange of locked objects safer.
 */
	class SipTransRWLockHelper
	{
	    public:
		SipTransRWLockHelper()
		    : rwLockPtr(0)
		{}

		~SipTransRWLockHelper()
		{
		    if(rwLockPtr)
			rwLockPtr->Unlock();
		}

		void readLock(VRWLock* lockPtr)
		{
		    this->rwLockPtr = lockPtr;
		    this->rwLockPtr->ReadLock();
		}

		void writeLock(VRWLock* lockPtr)
		{
		    this->rwLockPtr = lockPtr;
		    this->rwLockPtr->WriteLock();
		}

		VRWLock * rwLockPtr;
	};
#endif //USE_NODE_LOCK

};
 
} // namespace Vocal

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

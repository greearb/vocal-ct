#ifndef _Sip_Transaction_Levels__hxx
#define _Sip_Transaction_Levels__hxx

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

static const char* const SipTransactionLevels_hxx_version =
    "$Id: SipTransactionLevels.hxx,v 1.3 2004/05/27 04:32:18 greear Exp $";

#include "SipTransactionId.hxx"
#include "SipTransactionList.hxx"
#include "SipTransHashTable.hxx"
#include "SipMsg.hxx"
#include "NetworkAddress.h"
#include "cpLog.h"


namespace Vocal
{
    
#define DEBUG_NEW_STACK LOG_DEBUG_STACK

struct SipTransLevel1Node;
struct SipTransLevel2Node;
struct SipTransLevel3Node;
struct SipMsgPair;
class SipMsgContainer;

static const char * const NOVAL = "";

/**
 * this is top level node that'll be accessible from the hash table. hence,
 * we'll not keep the key value here (it's uniquely accessible)
 */
struct SipTransLevel1Node
{
    /// state specific data
    Data toTag;
    Data fromTag;
    int  msgCount;

    /// next level list
    SipTransactionList<SipTransLevel2Node*> level2;

    /// back reference into hashtable (needed by cleanup thread)
    SipTransHashTable::Node* bucketNode;

    SipTransLevel1Node()
	: toTag(NOVAL), fromTag(NOVAL), msgCount(0),level2(),
	  bucketNode(0)
	{}
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
    findOrInsert(const SipTransactionId& id);

    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *
    find(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findLevel3AckInvite(const SipTransactionId& id);
};

struct SipTransLevel2Node
{
    /// the 2nd level key
    SipTransactionId::KeyTypeII myKey;
    bool seqSet;
    int seqNumber;

    /// next level list
    SipTransactionList<SipTransLevel3Node*> level3;

      /************ TO DO **************\
       * either put a ref to self's container in list of prev level*
       * or some thing to get the ref to self's container *
       * (in level2 and level3 nodes) *
       * modify transaction GC and sent res/req DBs *
      \*********************************/
    /// back reference to the top level node
    SipTransLevel1Node * topNode;
    SipTransactionList<SipTransLevel2Node*>::SipTransListNode *myPtr;

    SipTransLevel2Node() : myKey(), seqSet(false), seqNumber(-1), level3(), topNode(0), myPtr(0)
	{}

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findOrInsert(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    find(const SipTransactionId& id);

    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *
    findInvite();
};


struct SipMsgPair
{
    /// the request
    SipMsgContainer *request;
    /// and the response
    SipMsgContainer *response;

    SipMsgPair() : request(0), response(0)
	{}

    ~SipMsgPair()
	{
	    /// these are deleted explicitly by the cleanup
	}
};

struct SipTransLevel3Node
{
    /// the 3rd level key
    SipTransactionId::KeyTypeIII myKey;

    /// message sequence. we don't want to keep a msg queue here 'coz
    /// on every recieve a copy of msg queue is constructed anyway.
    /// atleast keep the operations fast by direct lookup.
    ///
    /// NOTE: the msg queue for ACK's will be constructed by combining
    /// it w/ level3 node of INVITE
    SipMsgPair msgs;

      /************ TO DO **************\
       * either put a ref to self's container in list of prev level *
       * or some thing to get the ref to self's container *
       * (in level2 and level3 nodes) *
       * modify transaction GC and sent res/req DBs *
      \*********************************/
    /// back reference to level2 node
    SipTransLevel2Node *level2Ptr;
    SipTransactionList<SipTransLevel3Node*>::SipTransListNode *myPtr;

    SipTransLevel3Node() : myKey(), msgs(), level2Ptr(0), myPtr(0)
	{}
};


class SipMsgContainer : public BugCatcher {
public:
   struct m {
      // refence to the message (for incoming messages)
      Sptr<SipMsg> in;
      // reference to raw msg txt (for outgoing messages)
      Data         out;
      //Host
      Sptr<NetworkAddress> netAddr;
      //
      int  type;
      //
      Data  transport;
   } msg;

   /// constructed with default value for stateless mode
   SipMsgContainer()
         : msg(),
           retransCount(1),
           collected(false),
           level3Ptr(0)
      {}

   string toString() const;

   void setRetransCount(int i) { retransCount = i; }
   int getRetransCount() { return retransCount; }

   SipTransLevel3Node * getLevel3Ptr() { return level3Ptr; }
   void setLevel3Ptr(SipTransLevel3Node* n) { level3Ptr = n; }

protected:
   // transport specific data
   int retransCount;
   bool collected;
   int prepareCount; /** How many times have we tried to send this
                      * message.  Used by SipTcpStack at least.
                      */

   /************ TO DO **************\
    * either put a ref to self's container in list of prev level *
    * or some thing to get the ref to self's container *
    * (in level2 and level3 nodes) *
    * modify transaction GC and sent res/req DBs *
    *********************************/
   // back reference to level3 node
   SipTransLevel3Node *level3Ptr;


}; //class SipMsgContainer

 
} // namespace Vocal

#endif

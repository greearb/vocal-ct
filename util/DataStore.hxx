#ifndef DATASTORE_HXX
#define DATASTORE_HXX

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



#include "VFileSystem.hxx"
#include "BugCatcher.hxx"


static const char* const DataStoreVersion =
    "$Id: DataStore.hxx,v 1.3 2004/06/07 08:32:20 greear Exp $";



/// Class that specifies access signature for abstract data storage
class DataStore: public BugCatcher {
protected:

   ///
   DataStore( int aproxNumUsersP );


public:

   ///
   virtual ~DataStore();
    
    
   /// get the specified item
   virtual string
   getItem( const string& group, const string& name )
      throw(VException&) = 0;
    
   /// get the last modification time for the specfied item
   virtual TimeStamp
   getItemTimeStamp( const string& group, const string& name )
      throw(VException&) = 0;
    
   /// get the size in bytes of the data in the specified item
   virtual int
   getItemSize( const string& group, const string& name )
      throw(VException&) = 0;
    
   /**
    * store the data into the specified item, 
    * it time is not specifeid, current time is used
    * Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
   virtual int
   putItem( const string& group,
            const string& name,
            const string& data,
            TimeStamp timeStamp = 0 )
      throw(VException&) = 0;
    
   /** Remove the specified item
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
    virtual int
    removeItem( const string& group, const string& name)
        throw(VException&) = 0;
    
    /// Return true if the specified item exists
    virtual bool
    isItem( const string& group, const string& name)
        throw(VException&) = 0;
    
    /// get list of all items in this group
    virtual StringList
    listItems(const string& group)
        throw(VException&) = 0;
    
   /** add a new group
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
    virtual int
    addGroup( const string& group )
        throw(VException&) = 0;
    
   /** remove a group
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
    virtual int
    removeGroup( const string& group)
        throw(VException&) = 0;
    
    /// return true if the group exists
    virtual bool
    isGroup( const string& group) = 0;
    
    /// get a list of all groups
    virtual StringList
    listGroups() = 0;
    
protected:
    /// best gueses on total number of users
    const int aproxNumUsers;

};

#endif

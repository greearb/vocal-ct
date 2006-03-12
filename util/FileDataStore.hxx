#ifndef FILEDATASTORE_HXX
#define FILEDATASTORE_HXX

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



#if 0

static const char* const FileDataStoreVersion =
    "$Id: FileDataStore.hxx,v 1.4 2006/03/12 07:41:28 greear Exp $";

#include "DataStore.hxx"


/// Class that store data in file system
class FileDataStore : public DataStore {
public:
   ///
   FileDataStore( int numBinsP, const string &fileRootP );

   ///
   virtual ~FileDataStore();

   /// get the specified item
   virtual string
   getItem( const string& group, const string& name )
      throw(VException&);

   /// get the last modification time for the specfied item
   virtual TimeStamp
   getItemTimeStamp( const string& group, const string& name )
      throw(VException&);

   /// get the size in bytes of the data in the specified item
   virtual int
   getItemSize( const string& group, const string& name )
      throw(VException&);

   /**
    * store the data into the specified item, 
    * it time is not specifeid, current time is used
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
   virtual int putItem( const string& group,
                        const string& name,
                        const string& data,
                        TimeStamp timeStamp = 0 )
      throw(VException&);

   /** Remove the specified item
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
   virtual int removeItem( const string& group, const string& name)
      throw(VException&);

   /// Return true if the specified item exists
   virtual bool
   isItem( const string& group, const string& name)
      throw(VException&);

   /// get list of all items in this group
   virtual StringList
   listItems(const string& group)
      throw(VException&);

   /** add a new group
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
   virtual int addGroup( const string& group )
      throw(VException&);

   /** remove a group
    *  Return < 0 on failure, 0 if no changes were made (ie the data-store
    *  was already as requested), > 0 if changes were actually made.
    */
   virtual int removeGroup( const string& group)
      throw(VException&);

   /// return true if the group exists
   virtual bool
   isGroup( const string& group)
      throw(VException&);

   /// get a list of all groups
   virtual StringList
   listGroups();

protected:

private:

   /// get the bin name for a particular bin
   string getBin( int i ) const;

   /// get the bin name for a given item
   string getBin( const string& group, const string& name ) const;

   /// get the full path for the root of the data store
   string rootName( int bin ) const;

   /// get the full path directoy name for a given group
   string dirName(const string& group, const string& name ) const;

   /// get the full path directoy name for a given group
   string dirName(const string& group, int bin ) const;

   /// get the full path file name for a given item
   string fileName(const string& group, const string& name ) const;

   /// take stuff and return number from 0 to numBins-1
   int hash(const string& group, const string& name ) const;

   /// path to root of file system in use
   string fileRoot;

   /// number of directory bins currently in use
   int numBins;

};
#endif

#endif

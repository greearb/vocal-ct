
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

static const char* const FileDataStore_cxx_Version =
    "$Id: FileDataStore.cxx,v 1.4 2006/03/12 07:41:28 greear Exp $";


#include "global.h"
#include <cassert>
#include <cerrno>
#include <cstdio>

#include "FileDataStore.hxx"
#include "VIoException.hxx"


FileDataStore::FileDataStore( int numBinsP, const string &fileRootP )
        : DataStore(numBinsP * 200),
        fileRoot(fileRootP),
        numBins(numBinsP)
{
    assert( numBins < 1000 );
    assert( numBins >= 0 );
    assert( fileRoot.length() > 0 );

    // check the root file system exists
    if ( !VFileSystem::dirExists( fileRoot ) ) {
       throw VIoException( strerror(errno),
                           __FILE__,
                           __LINE__,
                           errno );
    }

    // check all the groups dirs are created
    for ( int bin = 0; bin < numBins; bin++ ) {
       string dir = rootName( bin );
       if ( !VFileSystem::dirExists( dir ) ) {
          VFileSystem::createDir( dir );
       }
    }
}


FileDataStore::~FileDataStore()
{}



string
FileDataStore::getBin( int i ) const
{
    // This turns i into a 3 digit base 16 number and
    // is prepresented with a A for 0, B for 1 ...
    assert( i < 16*16*16 );
    assert( i >= 0 );

    int i3 = i % 16;
    i = i / 16;
    int i2 = i % 16;
    i = i / 16;
    int i1 = i % 16;
    i = i / 16;

    char c[4];
    c[0] = 'A' + i1;
    c[1] = 'A' + i2;
    c[2] = 'A' + i3;
    c[3] = 0;

    string res(c);
    return res;
}


string
FileDataStore::getBin( const string& group, const string& name ) const
{
    int h = hash( group, name );
    return getBin( h );
}


string
FileDataStore::rootName( int bin ) const
{
    string ret = fileRoot;

    if (numBins != 0) {
        ret += "/";
        ret += getBin(bin);
    }
    return ret;
}


string
FileDataStore::dirName(const string& group, const string& name ) const
{
    return dirName( group, hash(group, name) );
}


string
FileDataStore::dirName(const string& group, int bin ) const
{
   string ret = rootName( bin );
   ret += "/";
   
   string g = group;
   
   if ( numBins != 0 ) {
      // flatten the group name - replace / with #
      for (unsigned int i = 0; i < g.length(); i++) {
         if ( g[i] == '/' ) {
            g[i] = '#';
         }
      }
   }

   ret += g;
   return ret;
}


string
FileDataStore::fileName(const string& group, const string& name ) const
{
   string ret = dirName(group, name);
   ret += "/";
   ret += name;
   ret += ".xml";
   return ret;
}


int
FileDataStore::hash(const string& group, const string& name ) const
{
   // There are no doubt better hashes but hey this looks like it will work
   int h1 = 0;
   int h2 = 0;

   for ( unsigned int i = 0; i < name.length(); i++ ) {
      h1 += name[i];
      h2 += (h2 << 4) + name[i];
   }

   int h = (abs(h1 + h2)) % numBins;

   assert( h >= 0 );
   assert( h < numBins );

   return h;
}


string
FileDataStore::getItem( const string& group, const string& name )
throw(VException&)
{
   string ret;
   ret = VFileSystem::readFile( fileName(group, name) );
   return ret;
}


TimeStamp
FileDataStore::getItemTimeStamp( const string& group, const string& name )
throw(VException&)
{
   TimeStamp ret;
   ret = VFileSystem::getFileTime( fileName(group, name) );
   return ret;
}

int
FileDataStore::getItemSize( const string& group, const string& name )
throw(VException&)
{
   int ret;
   ret = VFileSystem::getFileSize( fileName(group, name) );
   return ret;
}


int
FileDataStore::putItem( const string& group,
                        const string& name,
                        const string& data,
                        TimeStamp timeStamp)
throw(VException&)
    // timestamp defaults to 0
{
   int rv = 1;
   try {
      string s = getItem(group, name);
      if (s == data) {
         rv = 0;
      }
   }
   catch (...) {
      // Do nothing
   }

   if (!isGroup(group)) {
      addGroup(group);
   }
   string fName( fileName(group, name) );

   VFileSystem::writeFile( fName, data );
      
   if (timeStamp != 0 ) {
      VFileSystem::setFileTime( fName, timeStamp );
   }
   return rv;
}


int
FileDataStore::removeItem( const string& group, const string& name)
throw(VException&)
{
   if (!isItem(group, name)) {
      return 0;
   }
   VFileSystem::removeFile( fileName(group, name) );
   return 1;
}


bool
FileDataStore::isItem( const string& group, const string& name)
throw(VException&)
{
    bool ret;
    ret = VFileSystem::fileExists( fileName(group, name) );
    return ret;
}


StringList
FileDataStore::listItems(const string& group)
throw(VException&)
{
    StringList list;

    for ( int bin = 0; bin < numBins; bin++ ) {
       StringList dirList( VFileSystem::readDir( dirName(group, bin) ));
       
       StringList::iterator i = dirList.begin();
       while ( i != dirList.end() ) {
          string s = *i;
          i++;

          // filter out "hidden" items in the directory
          static string cvs("CVS");
          if ( s == cvs ) continue;

          // strip .xml off end
          size_t location = s.rfind(".xml");
          if (location != string::npos) {
             string t = s.substr(0, location);
             list.push_back(t);
          }
          else {
             list.push_back(s);
          }
       }
    }
    return list;
}


int
FileDataStore::addGroup( const string& group )
throw(VException&) {
   if (isGroup(group)) {
      return 0;
   }
   for ( int bin = 0; bin < numBins; bin++ ) {
      VFileSystem::createDir( dirName(group, bin) );
   }
   return 1;
}


int
FileDataStore::removeGroup( const string& group)
throw(VException&)
{
   if (!isGroup(group)) {
      return 0;
   }
   for ( int bin = 0; bin < numBins; bin++ ) {
      VFileSystem::removeDir( dirName(group, bin) );
   }
   return 1;
}


bool
FileDataStore::isGroup( const string& group)
throw(VException&)
{
    bool ret;

    ret = VFileSystem::dirExists( dirName(group, 0) );

#ifndef NDEBUG

    char err_buff[256];

    // check the groups exists in all bins
    for ( int bin = 1; bin < numBins; bin++ ) {
       bool v = VFileSystem::dirExists( dirName(group, bin) );
       if ( v != ret ) {
          sprintf (err_buff, "Error: Number of hash bins has changed: %s",
                   strerror(errno) );
          throw VIoException( err_buff,
                              __FILE__,
                              __LINE__,
                              errno );
          // We have an invalid group directory structure
       }
    }
#endif
    return ret;
}


StringList
FileDataStore::listGroups()
{
   StringList ret;
   
   if (numBins == 0 ) {
      // this is not implemented - left as an exersise to the reader ...
      // it would only be needed to upgrade old system to new
      assert(0);
   }

   // loop over all the groups found in bin 0
   StringList dirList( VFileSystem::readDir( rootName(0) ));
   
   StringList::iterator i = dirList.begin();
   while ( i != dirList.end() ) {
      string s = *i;
      i++;
      
      // unflatten group names - replace # with /
      for (unsigned int i = 0; i < s.length(); i++) {
         if ( s[i] == '#' ) {
            s[i] = '/';
         }
      }

      // filter out "hidden" items in the directory
      static string cvs("CVS");
      if ( s == cvs ) continue;
      
      ret.push_back(s);
   }
   return ret;
}

#endif

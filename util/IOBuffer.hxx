// $Id: IOBuffer.hxx,v 1.1 2004/05/29 01:10:33 greear Exp $
// $Revision: 1.1 $  $Author: greear $ $Date: 2004/05/29 01:10:33 $

//
//Copyright (C) 2001-2004  Ben Greear
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU Library General Public License
//as published by the Free Software Foundation; either version 2
//of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU Library General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// To contact the Author, Ben Greear:  greearb@candelatech.com
//
//

#ifndef __IOBUFFER_INCLUDE__BEN_
#define __IOBUFFER_INCLUDE__BEN_


#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include "BugCatcher.hxx"

using namespace std;

class IOBuffer : public BugCatcher {
protected:

   /** NOTE: when head == tail, the buffer is empty.
    *
    * head points to index of next insertion
    * tail points to last byte in the queue
    * max_len is the number of bytes in the 'string' buffer.
    */

   unsigned char *iobuf;
   int max_len; //length of iobuf in bytes
   int head; /* index of next insertion */
   int tail; /* index of last byte in queue */
   int eof;

   /** Just moves 'head' around. */
   int fakeAppend(int len);

public:
   static int string_cnt;
   static int total_bytes;

   IOBuffer  (); //default constructor
   IOBuffer  (const IOBuffer& S);
   IOBuffer  (const int m_len);
   virtual ~IOBuffer ();
   IOBuffer& operator=(const IOBuffer& src);

   int wasEOF() const { return eof; }

   int getCurLen() const ;
   int getMaxLen() const { return max_len - 1; }

   /** This gets a copy of the bytes out of the buffer.  Repeated calls
    * to this method will return the same thing.  Use dropFromTail() to
    * remove the bytes from the buffer, ie to iterate through.
    */
   int peekBytes(unsigned char* buf, int len_to_get) const;

   /** Peeks a single byte, deals with any buffer wrapping. */
   unsigned char charAt(int idx);

   /** Actually removes oldest 'len' bytes from the buffer. */
   int dropFromTail(int len); //returns -1 if it failed


   ///*********************  OPERATORS  ******************************///
   //operator const char* () const;

   /** Doesn't release memory, but virtually clears out the data. */
   void clear();

   /** Releases all the memory it can. */
   void purge();

   int append(const unsigned char* bytes, int len);
   void ensureCapacity(int max_length);

   int  write(const int desc, int max_to_write = -1);

   /** Will write read bytes into os if it's not NULL. */
   int  read(const int desc, const int max_to_read, ostream* os = NULL);

   /** Only does one call to recvfrom
    */
   int recvFrom(const int desc, const int max_to_read, struct sockaddr *from,
                socklen_t *fromlen);

   int getMaxContigFree() const;
   int getMaxContigUsed() const;

   /** Dangerous...make sure you know what you're doing!!! */
   const unsigned char* getTailPointer() { return iobuf + tail; }

   string toString();
   string toStringBrief();
};
#endif

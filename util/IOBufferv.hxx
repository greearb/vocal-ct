// $Id: IOBufferv.hxx,v 1.2 2006/02/24 22:27:52 greear Exp $
// $Revision: 1.2 $  $Author: greear $ $Date: 2006/02/24 22:27:52 $

//
//Copyright (C) 2001  Ben Greear
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

#ifndef __IOBUFFERV_INCLUDE__BEN_
#define __IOBUFFERV_INCLUDE__BEN_


#define TRUE 	1
#define FALSE 	0

#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef __WIN32__
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#endif
#include <unistd.h>
#include <stdarg.h>
//#include "string2.hxx"
#include <BugCatcher.hxx>

using namespace std;

class IOBufferv : public BugCatcher {
protected:

   /** NOTE: when head == tail, the buffer is empty OR full, use
    *   cur_len to tie-break.
    *
    * head points to index of next insertion
    * tail points to last (oldest) byte in the queue
    * max_len is the number of bytes in the 'string' buffer.
    */

   unsigned char *iobuf;
   int max_len; //length of string in bytes
   int cur_len;
   int head; /* index of next insertion */
   int tail; /* index of last byte in queue */
   int eof;

   /** Just moves 'head' around. */
   int fakeAppend(int len);

public:
   static int string_cnt;
   static int total_bytes;

   //static LogStream* logfile;

   IOBufferv  (); //default constructor
   IOBufferv  (const IOBufferv& S);
   IOBufferv  (const int m_len);
   virtual ~IOBufferv();
   IOBufferv& operator=(const IOBufferv& src);

   //static void setLogFile(LogStream* dafile);// { logfile = dafile; }
   //static LogStream* getLogFile();// { return logfile; }


   int wasEOF() const { return eof; }

   int getCurLen() const;
   int getMaxLen() const { return max_len; }
   int isEmpty() const { return (cur_len == 0); }

   /** This gets a copy of the bytes out of the buffer.  Repeated calls
    * to this method will return the same thing.  Use dropFromTail() to
    * remove the bytes from the buffer, ie to iterate through.
    */
   int peekBytes(unsigned char* buf, int len_to_get) const;

   /** Actually removes oldest 'len' bytes from the buffer. */
   int dropFromTail(int len); //returns -1 if it failed

   /** Peeks a single byte, deals with any buffer wrapping. */
   unsigned char charAt(int idx);


   ///*********************  OPERATORS  ******************************///
   //operator const char* () const;

   /** Doesn't release memory, but virtually clears out the data. */
   void clear();

   /** Releases all the memory it can. */
   void purge();

   int append(const unsigned char* bytes, int len);
   void ensureCapacity(int max_length);

   ssize_t write(const int desc, int max_to_write = -1);

   /** Will write read bytes into os if it's not NULL. */
   ssize_t read(const int desc, const int max_to_read, const char* debug,
                 ostream* os = NULL);

   /** Only does one call to recvfrom
    */
   ssize_t recvFrom(const int desc, const int max_to_read, struct sockaddr *from, socklen_t *fromlen);

   int getMaxContigFree() const;
   int getMaxContigUsed() const;

   /** Dangerous...make sure you know what you're doing!!! */
   const unsigned char* getTailPointer() { return iobuf + tail; }

   string toString();
   string toStringBrief();
   void assertSanity(); //Do some internal checks to make sure we are in a sane state.
};
#endif

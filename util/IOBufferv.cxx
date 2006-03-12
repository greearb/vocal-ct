// $Id: IOBufferv.cxx,v 1.3 2006/03/12 07:41:28 greear Exp $
// $Revision: 1.3 $  $Author: greear $ $Date: 2006/03/12 07:41:28 $

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

#include "IOBufferv.hxx"
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sstream>


#define IO_LOGFILE cerr


//LogStream* IOBufferv::logfile = NULL;
int IOBufferv::string_cnt = 0;
int IOBufferv::total_bytes = 0;

//void hexDump(const char* msg, int len, String& retval, int show_decode = TRUE, int add_newlines = TRUE);

/*
//Uncomment this if you don't have your own hexDump method..
 {
   retval = "";
   char buf[50];
   unsigned short tmp = 0;
   int i;
   for (i = 0; i<len; i++) {
      tmp = (unsigned short)(msg[i]);
      tmp &= 0xff; //mask out high bits (left over from signed-ness??)
      sprintf(buf, "%02hx ", tmp);
      retval.append(buf);
      if ((i + 1) % 16 == 0) {
         if (show_decode) {
            retval.append("   ");
            for (int j = i-15; j<=i; j++) {
               if (isprint(msg[j])) {
                  retval.append(msg[j]);
               }
               else {
                  retval.append(".");
               }
            }//for
         }
         retval.append("\n");
      }//if
   }//for

   if (show_decode) {
      // do final char translations.
      String ending(30);
      int q = (i) % 16;
      int offset = i-q;
      retval.append("   ");
      for (int l = 0; l< 16-q; l++) {
         retval.append("   "); //space, where the hex would have gone.
      }
      //VLOG << "q: " << q << " offset: " << offset << " i: " << i << endl;
      for (int j = 0; j<q; j++) {
         if (isprint(msg[j + offset])) {
            ending.append(msg[j + offset]);
         }
         else {
            ending.append(".");
         }
      }//for
      retval.append(ending);
   }
}//hexDump
*/


//void IOBufferv::setLogFile(LogStream* dafile) { logfile = dafile; }

//LogStream* IOBufferv::getLogFile() { return logfile; }

void IOBufferv::ensureCapacity(int max_length) {
   if (getMaxLen() < max_length) {
      //cout << "Growing IOBufferv: " << this << " max_length: " << max_length << endl;
      unsigned char* tmp = new unsigned char[max_length + max_len];
      int len = getCurLen();
      peekBytes(tmp, len);
      if (iobuf) {
         delete[] iobuf;
      }
      iobuf = tmp;
      tail = 0;
      head = len;

      total_bytes += max_length; //keep our static statistics
      max_len += max_length;
   }//if
}//ensureCapacity
   
 
IOBufferv::IOBufferv() { //default constructor
   //log("In default const.\n");
   eof = 0;
   string_cnt++;
   iobuf = new unsigned char[200];
   head = 0;
   tail = 0;
   max_len = 200;
   cur_len = 0;

   total_bytes += 200;
} // constructor


IOBufferv::IOBufferv(const IOBufferv& S) {
   eof = 0;
   string_cnt++;
   iobuf = new unsigned char[S.max_len];
   memcpy(iobuf, S.iobuf, S.max_len);
   head = S.head;
   tail = S.tail;
   max_len = S.max_len;
   cur_len = S.cur_len;

   total_bytes += max_len;
} // constructor

IOBufferv& IOBufferv::operator=(const IOBufferv& S) {
   if (iobuf) {
      delete[] iobuf;
   }
   total_bytes -= max_len;

   iobuf = new unsigned char[S.max_len];
   memcpy(iobuf, S.iobuf, S.max_len);
   head = S.head;
   tail = S.tail;
   max_len = S.max_len;
   eof = S.eof;
   cur_len = S.cur_len;

   total_bytes += max_len;
   return *this;
} // constructor



IOBufferv::IOBufferv(const int my_len) {
   int m_len = my_len;
   string_cnt++;
   eof = 0;

   if (m_len > 100000) {
      m_len = 100000;
   }
   if (m_len < 200) {
      m_len = 200;
   }

   iobuf = new unsigned char[m_len];
   head = tail = 0;
   max_len = m_len;
   cur_len = 0;

   total_bytes += (max_len);
} // constructor


IOBufferv::~IOBufferv() {
   string_cnt--;
   total_bytes -= (max_len);
   if (iobuf) {
      delete[] iobuf;
   }
   iobuf = NULL;
} //destructor


string IOBufferv::toString() {
   ostringstream oss;
   oss << "max_len: " << max_len << "  head: " << head
       << "  tail: " << tail << "  cur_len: " << getCurLen()
       << endl;

   //hexDump((char*)(string), max_len, r2);

   //retval.append(r2);

   return oss.str();
}


string IOBufferv::toStringBrief() { 
   ostringstream oss;
   oss << "max_len: " << max_len << "  head: " << head
       << "  tail: " << tail << "  cur_len: " << getCurLen()
       << endl;

   //hexDump((char*)(iobuf), max_len, r2);

   //retval.append(r2);

   return oss.str();
}

int IOBufferv::getMaxContigFree() const {
   if (head == tail) { //if empty or full
      if (!isEmpty()) {
         return 0;
      }
      else {
         assert(head == 0); /** Code should always reset these to zero when possible
                             * to make sure that we have contig buffers as much as
                             * possible. */
         return max_len;
      }
   }
   else if (head > tail) {
      return max_len - head;
   }
   else {
      return tail - head - 1;
   }
}


int IOBufferv::getMaxContigUsed() const {
   if (isEmpty()) {
      return 0;
   }
   else if (head > tail) {
      return head - tail;
   }
   else {
      return max_len - tail;
   }
}


int IOBufferv::peekBytes(unsigned char* buf, int len_to_get) const {
   if (len_to_get > getCurLen()) {
      return -1;
   }
   else {
      if ((max_len - tail) >= len_to_get) {
         memcpy(buf, iobuf + tail, len_to_get);
      }
      else {
         int mlen = max_len - tail;
         memcpy(buf, iobuf + tail, mlen);
         memcpy(buf + mlen, iobuf, len_to_get - mlen);
      }
      return len_to_get;
   }
}//peekBytes


int IOBufferv::dropFromTail(int len) {
   int cl = getCurLen();

   if (cl == len) {
      head = tail = 0;
   }
   else if (len < cl) {
      if ((tail + len) < max_len) {
         tail += len;
      }
      else {
         tail = (len - (max_len - tail));
      }
   }
   else {
      assert("tried to drop more than we have" == "bad idea");
      return -1;
   }

   cur_len -= len;
   return len;
}
   

ssize_t IOBufferv::write(const int desc, int max_to_write) {
   ssize_t this_round = 0;
   ssize_t sofar = 0;

   if (desc == -1) {  //cleans up an error message or two.
      return -1;
   }//if

   if (getCurLen() == 0) {
      //if (IO_LOGFILE.ofLevel(DBG)) {
      //   IO_LOGFILE << "IOBufferv: Current length was zero in ::write(desc)\n";
      //}
      return 0;
   }

   int len;
   while (TRUE) {
      len = getMaxContigUsed();

      // Allow caller to cap amount written
      if (max_to_write > 0) {
         if (len > max_to_write) {
            len = max_to_write;
         }
      }

      if (len == 0) {
         if (getCurLen() == 0) {
            if (0) {//IO_LOGFILE.ofLevel(DBG)) {
               IO_LOGFILE << "IOBufferv::write:  max-contig is zero, sofar: " << sofar
                          << " cur_len: " << getCurLen() << "\n";
            }
         }
         else {
            if (1) { //IO_LOGFILE.ofLevel(LS_ERROR)) {
               IO_LOGFILE << "ERROR:  IOBufferv::write:  max-contig is zero, sofar: " << sofar
                          << " cur_len: " << getCurLen() << "\n";
               IO_LOGFILE << toString() << endl;
            }
            return -1;
         }
         return sofar;
      }
      else {
#ifdef __WIN32__
         this_round = ::send(desc, (char*)(iobuf + tail), len, 0);
         if (this_round < 0) { //some error happened
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
               IO_LOGFILE << "IOBufferv: WARNING: write err (NOT EAGAIN): "
                          << WSAGetLastError()
                          << " sofar: " << sofar << endl;
#else
         this_round = ::send(desc, iobuf + tail, len, 0);
         if (this_round < 0) { //some error happened
            if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK
               perror("IOBufferv.Write() err");
               //if (IO_LOGFILE.ofLevel(WRN)) {
               //   IO_LOGFILE << "IOBufferv: WARNING: write err (NOT EAGAIN): " << strerror(errno) 
               //              << " sofar: " << sofar << endl;
               //}
#endif
               if (sofar != 0)
                  return sofar;
               return this_round;
            }//if
            else {
               //IO_LOGFILE.log(WRN, "IOBufferv: WARNING: EAGAIN on write.\n"); //TODO: remove
               //if (IO_LOGFILE.ofLevel(DBG)) {
               //   IO_LOGFILE << "IOBufferv::write, got EAGAIN on write.\n";
               //}
               return sofar;
            }
         }//if
         else if (this_round == 0) { //all done, written to end of the buffer
            //if (IO_LOGFILE.ofLevel(DBG)) {
            //   IO_LOGFILE << "IOBufferv::write  Attempted to write: " << len
            //              << " wrote zero, sofar: " << sofar << endl;
            //}
            return sofar;
         }//if
         else { //wrote some, and all is well.
            sofar += this_round;
            dropFromTail(this_round);
         }//else
      }//else
   }//while
}//Write (to a descriptor)


/** May actually read more than max_to_read, but the buffer won't grow any
 * bigger than that... (it won't be made smaller if it's already bigger
 * of course.)
 */
ssize_t IOBufferv::read(const int desc, const int max_to_read,
                   const char* debug, ostream* os) {
   ssize_t sofar = 0;
   ssize_t this_round = 0;

   if (max_to_read <= 0) {
      return 0; //idiot exit
   }

   ensureCapacity(getCurLen() + max_to_read);

   ssize_t to_rd;
   ssize_t ctg;

   int rounds = 0;
   int breakout = FALSE;
   while ((rounds++ < 2) && (!breakout)) {
      //IO_LOGFILE << "Top of while loop, sofar: " << sofar << " max_to_read: " << max_to_read << endl;
      //IO_LOGFILE << toStringBrief() << endl;

      if (sofar >= max_to_read)
         return sofar;

      to_rd = max_to_read - sofar;
      ctg = getMaxContigFree();

      if (to_rd > ctg) {
         to_rd = ctg;
      }

      //IO_LOGFILE << "to_rd: " << to_rd << " ctg: " << ctg << endl;

#ifdef __WIN32__
      unsigned long avail_rd = 0xFFFFFFFF;
      ioctlsocket(desc, FIONREAD, &avail_rd);
      if (avail_rd == 0) {
         cout << " IOB:  About to read desc: " << desc << " avail_rd: " << avail_rd
                 << " to_rd: " << to_rd << " debug: " << debug << endl;
      }
      //if (to_rd > (int)(avail_rd)) {
      //   to_rd = avail_rd;
      //}
      //if (to_rd == 0) {
      //   this_round = -1;
      //   errno = EAGAIN;
      //}
      //else {
      this_round = ::recv(desc, (char*)(iobuf + head), to_rd, 0);
      //}
#else
      this_round = ::read(desc, iobuf + head, to_rd);
#endif

      // Odds are, if we can't read all we want this time, we definately won't read any
      // more next time.
      if (this_round <= to_rd) {
         breakout = TRUE;
      }
      //IO_LOGFILE << " this_round: " << this_round << endl;
      
      if (this_round > 0) {
         fakeAppend(this_round); //virtually add it to our buffer
         //IO_LOGFILE << "After fake_append: " << toStringBrief() << endl;

         sofar += this_round;
         if (os) {
            //Debugging mostly...
            const char* b = (const char*)(iobuf);
            os->write(b + head, this_round);
         }
      }//if
      else {
         if (this_round < 0) { //some error happened
#ifdef __WIN32__
            if (WSAGetLastError() != WSAEWOULDBLOCK) {
#else
            if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK
#endif
               //if (IO_LOGFILE.ofLevel(WRN)) {
               //   IO_LOGFILE << "IOBufferv: WARNING: recv/read err (NOT EAGAIN): " << strerror(errno)
               //              << " desc: " << desc << endl;
               //}
               if (sofar != 0) {
                  return sofar;
               }
               return this_round;
            }//if
            else {  //this means it read all in buffer
               return sofar;
            }
         }//if
         else {
            if (sofar == 0) {
               // We have managed to read nothing..that is end-of-file, so return -1
               eof = TRUE;
               return -1;
            }
            return sofar; // We read nothing this round, but we have read some since we entered the method.
         }//else
      }//else
      //IO_LOGFILE << "Bottom of while loop, sofar: " << sofar << " max_to_read: " << max_to_read << endl;
      //IO_LOGFILE << toStringBrief() << endl;
   }//while true

   return sofar;

}//Read (from a descriptor)


/** Peeks a single byte, deals with any buffer wrapping. */
unsigned char IOBufferv::charAt(int idx) {
   assert(idx <= getCurLen());
   if ((max_len - tail) >= idx) {
      return iobuf[tail + idx];
   }
   else {
      int mlen = max_len - tail;
      idx -= mlen;
      return iobuf[idx];
   }
}//charAt


/** Only does one call to recvfrom
 */
ssize_t IOBufferv::recvFrom(const int desc, const int max_to_read,
                            struct sockaddr *from, socklen_t *fromlen) {
   ssize_t this_round = 0;

   if (max_to_read <= 0) {
      return 0; //idiot exit
   }

   ensureCapacity(max_to_read);

   ssize_t ctg = getMaxContigFree();
   if (ctg < max_to_read) {
      //if (IO_LOGFILE.ofLevel(INF)) {
      //   IO_LOGFILE << "IOBufferv: WARNING:  not enough CONTIG room... (performance problem only)\n";
      //}
      unsigned char tmp_buf[max_to_read];
#ifdef __WIN32__
      this_round = ::recvfrom(desc, (char*)(tmp_buf), max_to_read, 0, from, fromlen);
#else
      this_round = ::recvfrom(desc, tmp_buf, max_to_read, 0, from, fromlen);
#endif

      if (this_round > 0) {
         append(tmp_buf, this_round);
      }//if
   }
   else {
#ifdef __WIN32__
      this_round = ::recvfrom(desc, (char*)(iobuf + head), max_to_read, 0, from, fromlen);
#else
      this_round = ::recvfrom(desc, iobuf + head, max_to_read, 0, from, fromlen);
#endif
      if (this_round > 0) {
         fakeAppend(this_round); //virtually add it to our buffer
      }//if
   }

   if (this_round <= 0) {
      if (this_round < 0) { //some error happened
#ifdef __WIN32__
         if (WSAGetLastError() != WSAEWOULDBLOCK) {
            IO_LOGFILE << "IOBufferv: WARNING: rcvfrom read err (NOT EAGAIN): "
                       << WSAGetLastError() << endl;
#else
         if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK
            //if (IO_LOGFILE.ofLevel(WRN)) {
            //   IO_LOGFILE << "IOBufferv: WARNING: rcvfrom read err (NOT EAGAIN): "
            //              << strerror(errno) << endl;
            //}
#endif
            return this_round;
         }//if
         else {  //this means it read all in buffer
            return 0;
         }
      }//if
      else {
         // We have managed to read nothing..that is end-of-file, so return -1
         return -1;
      }//else
   }//if there was some kind of error
   else {
      return this_round;
   }
}//recvFrom (from a descriptor)


void IOBufferv::purge() {
   total_bytes -= max_len;
   if (iobuf) {
      delete[] iobuf;
   }
   iobuf = new unsigned char[200];
   max_len = 200;
   head = tail = 0;
   cur_len = 0;
   total_bytes += max_len;
}//purge


void IOBufferv::clear() {
   head = tail = 0;
   cur_len = 0;
   eof = 0;
}//Clear


int IOBufferv::append(const unsigned char* source, int len) {
   ensureCapacity(getCurLen() + len);

   // Here, we know that we have the space, so lets copy it in...
   int mlen = 0;
   
   if ((head + len) >= max_len) {
      mlen = max_len - head;
      //will have to wrap it a bit
      memcpy(iobuf + head, source, mlen);
      head = 0;
   }
   
   // Copy the rest into the buffer starting at 'head'.
   memcpy(iobuf + head, source + mlen, len - mlen);
   head += (len - mlen);
   cur_len += len;
   //IO_LOGFILE << "IOBufferv::append, cur_len: " << cur_len << " len: " << len
   //           << " getCurLen() " << getCurLen() << endl;
   return len;
}//append


int IOBufferv::getCurLen() const {
   return cur_len;
}

// Data has already been coppied to the buffer..we just need to
// move the head pointer correctly.
int IOBufferv::fakeAppend(int len) {
   int mlen = 0;
   if ((head + len) >= max_len) {
      mlen = max_len - head;
      //will have to wrap it a bit
      head = 0;
   }

   head += (len - mlen);
   cur_len += len;
   return len;
}//append

//Do some internal checks to make sure we are in a sane state.
void IOBufferv::assertSanity() {
   if (getCurLen() == 0) {
      assert(tail == head);
      assert(tail == 0);
   }
   assert(getCurLen() <= getMaxLen());
}


//IOBufferv::operator const char*() const {
//   return iobuf;
//}//to char* operator overload

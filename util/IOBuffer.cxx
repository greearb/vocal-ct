// $Id: IOBuffer.cxx,v 1.1 2004/05/29 01:10:33 greear Exp $
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

#include "IOBuffer.hxx"
#include <errno.h>
#include <signal.h>
#include <string.h>

int IOBuffer::string_cnt = 0;
int IOBuffer::total_bytes = 0;

void vhexDump(const char* msg, int len, string& retval,
              bool show_decode = true, bool add_newlines = true);

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


void IOBuffer::ensureCapacity(int max_length) {
   if (getMaxLen() < max_length) {
      //cout << "Growing IOBuffer: " << this << " max_length: " << max_length << endl;
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
   
 
IOBuffer::IOBuffer() { //default constructor
   //log("In default const.\n");
   eof = 0;
   string_cnt++;
   iobuf = new unsigned char[200];
   head = 0;
   tail = 0;
   max_len = 200;

   total_bytes += 200;
} // constructor


IOBuffer::IOBuffer(const IOBuffer& S) {
   eof = 0;
   string_cnt++;
   iobuf = new unsigned char[S.max_len];
   memcpy(iobuf, S.iobuf, S.max_len);
   head = S.head;
   tail = S.tail;
   max_len = S.max_len;

   total_bytes += max_len;
} // constructor

IOBuffer& IOBuffer::operator=(const IOBuffer& S) {
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

   total_bytes += max_len;
   return *this;
} // constructor



IOBuffer::IOBuffer(const int my_len) {
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

   total_bytes += (max_len);
} // constructor


IOBuffer::~IOBuffer() {
   string_cnt--;
   total_bytes -= (max_len);
   if (iobuf) {
      delete[] iobuf;
   }
   iobuf = NULL;
} //destructor


string IOBuffer::toString() {
   char tmpb[256];
   snprintf(tmpb, 255, "max_len: %i  head: %i  tail: %i  cur_len: %i\n",
            max_len, head, tail, getCurLen());
   string retval(tmpb);
   string r2;

   vhexDump((char*)(iobuf), max_len, r2);

   retval.append(r2);

   return retval;
}


string IOBuffer::toStringBrief() {
   char tbuf[256];
   snprintf(tbuf, 255, "max_len: %i  head: %i  tail: %i  cur_len: %i\n",
           max_len, head, tail, getCurLen());
   return tbuf;
}

int IOBuffer::getMaxContigFree() const {
   if (head == tail) { //if empty.
      return max_len;
   }
   else if (head > tail) {
      return max_len - head;
   }
   else {
      return tail - head - 1;
   }
}


int IOBuffer::getMaxContigUsed() const {
   if (head == tail) {
      return 0;
   }
   else if (head > tail) {
      return head - tail;
   }
   else {
      return max_len - tail;
   }
}


int IOBuffer::getCurLen() const {
   if (head == tail) {
      return 0;
   }
   else if (head > tail) {
      return head - tail;
   }
   else {
      return max_len - (tail - head);
   }
}


/** Peeks a single byte, deals with any buffer wrapping. */
unsigned char IOBuffer::charAt(int idx) {
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

int IOBuffer::peekBytes(unsigned char* buf, int len_to_get) const {
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


int IOBuffer::dropFromTail(int len) {
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
      return len;
   }

   return -1;
}
   

int IOBuffer::write(const int desc, int max_to_write) {
   int this_round = 0;
   int sofar = 0;

   if (desc == -1) {  //cleans up an error message or two.
      return -1;
   }//if

   if (getCurLen() == 0) {
      //if (IO_LOGFILE.ofLevel(DBG)) {
      //   IO_LOGFILE << "IOBuffer: Current length was zero in ::write(desc)\n";
      //}
      return 0;
   }

   int len;
   while (true) {
      len = getMaxContigUsed();

      // Allow caller to cap amount written
      if (max_to_write > 0) {
         if (len > max_to_write) {
            len = max_to_write;
         }
      }

      if (len == 0) {
         if (getCurLen() == 0) {
            //if (IO_LOGFILE.ofLevel(DBG)) {
            //   IO_LOGFILE << "IOBuffer::write:  max-contig is zero, sofar: " << sofar
            //              << " cur_len: " << getCurLen() << "\n";
            //}
         }
         else {
            //if (IO_LOGFILE.ofLevel(ERROR)) {
            //   IO_LOGFILE << "ERROR:  IOBuffer::write:  max-contig is zero, sofar: " << sofar
            //              << " cur_len: " << getCurLen() << "\n";
            //   IO_LOGFILE << toString() << endl;
            //}
            return -1;
         }
         return sofar;
      }
      else {

#ifndef WIN32
         this_round = ::write(desc, iobuf + tail, len);
#else
         this_round = ::send(desc, iobuf + tail, len, 0);
#endif

         if (this_round < 0) { //some error happened
            if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK
               perror("IOBuffer.Write() err");
               //if (IO_LOGFILE.ofLevel(WRN)) {
               //   IO_LOGFILE << "IOBuffer: WARNING: write err (NOT EAGAIN): " << strerror(errno) 
               //              << " sofar: " << sofar << endl;
               //}
               if (sofar != 0)
                  return sofar;
               return this_round;
            }//if
            else {
               //IO_LOGFILE.log(WRN, "IOBuffer: WARNING: EAGAIN on write.\n"); //TODO: remove
               //if (IO_LOGFILE.ofLevel(DBG)) {
               //   IO_LOGFILE << "IOBuffer::write, got EAGAIN on write.\n";
               //}
               return sofar;
            }
         }//if
         else if (this_round == 0) { //all done, written to end of the buffer
            //if (IO_LOGFILE.ofLevel(DBG)) {
            //   IO_LOGFILE << "IOBuffer::write  Attempted to write: " << len
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
int IOBuffer::read(const int desc, const int max_to_read, ostream* os) {
   int sofar = 0;
   int this_round = 0;

   if (max_to_read <= 0) {
      return 0; //idiot exit
   }

   ensureCapacity(getCurLen() + max_to_read);

   int to_rd;
   int ctg;

   int rounds = 0;
   int breakout = false;
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



#ifndef WIN32
      this_round = ::read(desc, iobuf + head, to_rd);
#else
      this_round = ::recv(desc, iobuf + head, to_rd, 0);
#endif

      // Odds are, if we can't read all we want this time, we definately won't read any
      // more next time.
      // TODO: Verify this is a good performance thingie!!
      if (this_round < to_rd) {
         breakout = true;
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
            if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK, signal interrupted us
               //if (IO_LOGFILE.ofLevel(WRN)) {
               //   IO_LOGFILE << "IOBuffer: WARNING: read err (NOT EAGAIN): " << strerror(errno)
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
               eof = true;
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


/** Only does one call to recvfrom
 */
int IOBuffer::recvFrom(const int desc, const int max_to_read, struct sockaddr *from, socklen_t *fromlen) {
   int this_round = 0;

   if (max_to_read <= 0) {
      return 0; //idiot exit
   }

   ensureCapacity(max_to_read);

   int ctg = getMaxContigFree();
   if (ctg < max_to_read) {
      //if (IO_LOGFILE.ofLevel(INF)) {
      //   IO_LOGFILE << "IOBuffer: WARNING:  not enough CONTIG room... (performance problem only)\n";
      //}
      unsigned char tmp_buf[max_to_read];
      this_round = ::recvfrom(desc, tmp_buf, max_to_read, 0, from, fromlen);

      if (this_round > 0) {
         append(tmp_buf, this_round);
      }//if
   }
   else {
      this_round = ::recvfrom(desc, iobuf + head, max_to_read, 0, from, fromlen);

      if (this_round > 0) {
         fakeAppend(this_round); //virtually add it to our buffer
      }//if
   }

   if (this_round <= 0) {
      if (this_round < 0) { //some error happened
         if ((errno != EAGAIN) && (errno != EINTR)) { //== EWOULDBLOCK, signal interrupted us
            //if (IO_LOGFILE.ofLevel(WRN)) {
            //   IO_LOGFILE << "IOBuffer: WARNING: read err (NOT EAGAIN): " << strerror(errno) << endl;
            //}
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


void IOBuffer::purge() {
   total_bytes -= max_len;
   if (iobuf) {
      delete[] iobuf;
   }
   iobuf = new unsigned char[200];
   max_len = 200;
   head = tail = 0;
   total_bytes += max_len;
}//purge


void IOBuffer::clear() {
   head = tail = 0;
}//Clear


int IOBuffer::append(const unsigned char* source, int len) {
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
   return len;
}//append


// Data has already been coppied to the buffer..we just need to
// move the head pointer correctly.
int IOBuffer::fakeAppend(int len) {
   int mlen = 0;
   if ((head + len) >= max_len) {
      mlen = max_len - head;
      //will have to wrap it a bit
      head = 0;
   }

   head += (len - mlen);
   return len;
}//append


//IOBuffer::operator const char*() const {
//   return iobuf;
//}//to char* operator overload

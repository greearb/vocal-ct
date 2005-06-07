
// Released into the public domain. --Ben Greear Sep 6, 2003

#include <cpLog.h>

// 16/1/04 fpi
// WorkAround Win32
#ifdef WIN32
#include "global.h"
#include "misc.hxx"
#include <fstream>
#include <sstream>
#include "Data.hxx"
#include "TimeVal.hxx"
#include "VTime.hxx"

#define snprintf _snprintf

#else
#include "misc.hxx"
#include <sstream>
#include <iostream.h>
#include <fstream.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <Data.hxx>
#include <sys/select.h>
#include "TimeVal.hxx"
#include <stdio.h>

#endif

using namespace Vocal::TimeAndDate;

void debugMemUsage(const char* msg, const char* fname) {
   char cmd[256];
   cmd[255] = 0;

   snprintf(cmd, 255, "date >> %s\n", fname);
   system(cmd);

   snprintf(cmd, 255, "cat /proc/%d/status | tail +9 | head -9 >> %s\n", getpid(), fname);
   system(cmd);

	ofstream off (fname, ios::app);
   if (off) {
      off << "Message: " << msg << endl;
      off << "Data::_cnt: " << Data::getInstanceCount()
          << " Data::_total_buf: " << Data::getTotalBuff() << endl;
   }

   off << endl;
}

void vhexDump(const char* msg, int len, string& _retval,
             bool show_decode, bool add_newlines) {
   ostringstream retval;
   char buf[50];
   unsigned short tmp = 0;
   int i;
   for (i = 0; i<len; i++) {
      tmp = (unsigned short)(msg[i]);
      tmp &= 0xff; //mask out high bits (left over from signed-ness??)
      sprintf(buf, "%02hx ", tmp);
      retval << buf;
      if (add_newlines && ((i + 1) % 16 == 0)) {
         if (show_decode) {
            retval << ("   ");
            for (int j = i-15; j<=i; j++) {
               if (isprint(msg[j])) {
                  retval << (msg[j]);
               }
               else {
                  retval << ".";
               }
            }//for
         }
         retval << "\n";
      }//if
   }//for

   if (show_decode) {
      // do final char translations.
		std::ostringstream ending;
      int q = (i) % 16;
      int offset = i-q;
      retval << ("   ");
      for (int l = 0; l< 16-q; l++) {
         retval << ("   "); //space, where the hex would have gone.
      }
      for (int j = 0; j<q; j++) {
         if (isprint(msg[j + offset])) {
            ending << msg[j + offset];
         }
         else {
            ending << ".";
         }
      }//for
      retval << ending.str();
   }

   _retval = retval.str();
}//hexDump


int vsetPrio(int sk, uint16 tos, uint32 val, const char* dbg) {
   vsetTosHelper_priv(sk, tos);
   vsetPriorityHelper_priv(sk, val, dbg);
   return 0;
}


/* Returns actual priority that was set, or < 0 on error */
int vsetPriorityHelper_priv(int sk, uint32 val, const char* dbg) {
   cpLog(LOG_DEBUG_STACK, "Setting socket priority, dbg: %s  sk: %i to value: %lu\n",
         dbg, sk, (unsigned long)(val));
   if (setsockopt(sk, SOL_SOCKET, SO_PRIORITY,
                  (char*)&val, sizeof(val)) < 0) {
      cpLog(LOG_ERR, "ERROR:  Failed to set socket priority, sk: %i  val: %lu  err: %s\n",
            sk, (unsigned long)(val), strerror(errno));
      return -errno;
   }//if

   uint32 new_val = 0;
   socklen_t slt = sizeof(new_val);
   if (getsockopt(sk, SOL_SOCKET, SO_PRIORITY,
                  (char*)(&new_val), &slt) < 0) {
      cpLog(LOG_ERR, "ERROR:  Failed to get socket priority, sk: %i  val: %lu  err: %s\n",
            sk, (unsigned long)(val), strerror(errno));
      return -errno;
   }//if

   if (val != new_val) {
      cpLog(LOG_ERR, "ERROR:  Failed to set socket priority, val: %lu  new_val: %lu\n",
            (unsigned long)(val), (unsigned long)(new_val));
   }

   return new_val;
}//vsetPriorityHelper

int vsetTosHelper_priv(int sk, uint16 val) {
   if (setsockopt(sk, SOL_IP, IP_TOS, (char*)&val, sizeof(val)) < 0) {
      return -1;
   }//if

   int new_val = 0;
   socklen_t slt = sizeof(new_val);
   if (getsockopt(sk, SOL_IP, IP_TOS, (char*)(&new_val), &slt) < 0) {
      return -1;
   }//if
   return new_val;
}//vsetTosHelper



/*
void vusleep(int milliseconds) {
   TimeVal tv(0, milliseconds);
   struct timeval timeout;
   timeout.tv_usec = tv.tv_usec;
   timeout.tv_sec = tv.tv_sec;

   select(0, NULL, NULL, NULL, &timeout);
}
*/

// 16/1/04 fpi
// tbr
// todo: Win32 porting
// note: until now we do not need these functions
#ifndef WIN32
#ifndef LINK_WITH_LANFORGE_MISC

uint64 vgetCurMs() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return vtv_to_ms(tv);
}

uint64 vgetCurUs() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return vtv_to_us(tv);
}

uint64 vgetCurNs() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return vtv_to_ns(tv);
}


struct timeval vgetCurTv() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   return tv;
}


/** Convert from mili-seconds to struct timeval. */
struct timeval vms_to_tv(uint64 ms) {
   struct timeval retval;
   retval.tv_usec = (ms % 1000) * 1000;
   retval.tv_sec = ms / 1000;
   return retval;
}

/** Convert to miliseconds */
uint64 vtv_to_ms(const struct timeval& tv) {
   uint64 ms = tv.tv_usec / 1000;
   ms += (uint64)tv.tv_sec * (uint64)1000;
   return ms;
}

/** Convert to microseconds */
uint64 vtv_to_us(const struct timeval& tv) {
   uint64 us = tv.tv_usec;
   us += (uint64)tv.tv_sec * (uint64)1000000;
   return us;
}

/** Convert to nanoseconds */
uint64 vtv_to_ns(const struct timeval& tv) {
   uint64 us = ((uint64)tv.tv_usec) * 1000LLU;
   us += ((uint64)tv.tv_sec) * 1000000000LLU;
   return us;
}


int vparseInt(const char* s) {
   return strtol(s, NULL, 0); //should parse HEX, Octal, and Decimal.  If not decimal, must start with 0x
}


bool visNumber(const char* s) {
   char* tst = 0;
   long int l = strtol(s, &tst, 0); //should parse HEX, Octal, and Decimal.  If not decimal, must start with 0x
   if (l == 0) {
      if (s == tst) {
         return false;
      }
   }
   return true;
}//isNumber

string itoa(int i) {
   char buf[30];
   snprintf(buf, 30, "%d", i);
   return buf;
}

string itoa(unsigned long i) {
   char buf[30];
   snprintf(buf, 30, "%lu", i);
   return buf;
}

string itoa(uint16 i) {
   char buf[30];
   snprintf(buf, 30, "%hu", i);
   return buf;
}

#endif
#endif

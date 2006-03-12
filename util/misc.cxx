
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

#ifdef __WIN32__



uint64 vcalcEpocOffset() {
   SYSTEMTIME st;
   FILETIME ft;

   memset(&st, 0, sizeof(st));
   memset(&ft, 0, sizeof(ft));

   st.wYear = 1970;
   st.wMonth = 1;
   st.wDayOfWeek = 0;
   st.wDay = 1;
   st.wHour = 0;
   st.wMinute = 0;
   st.wSecond = 0;
   st.wMilliseconds = 0;

   if (!SystemTimeToFileTime(&st, &ft)) {
      cerr << "ERROR:  SystemTimeToFileTime failed, err: "
           << GetLastError() << endl;
   }

   uint64 t = ft.dwHighDateTime;
   t = t << 32;
   t |= ft.dwLowDateTime;

   cerr << "calcEpocOffset value: " << t << "  hi: " << ft.dwHighDateTime
        << " lo: " << ft.dwLowDateTime << endl;

   return t;
}

// Gets high resolution by spinning up to 15ms.  Don't call this often!!!
uint64 vgetRawCurMsSpin() {
   FILETIME tm;

   static uint64 epocOffset = vcalcEpocOffset();
   uint64 t_now;

   GetSystemTimeAsFileTime(&tm);
   uint64 t_start = tm.dwHighDateTime;
   t_start = t_start << 32;
   t_start |= tm.dwLowDateTime;
   while (1) {
      GetSystemTimeAsFileTime(&tm);
      t_now = tm.dwHighDateTime;
      t_now = t_now << 32;
      t_now |= tm.dwLowDateTime;

      if (t_now != t_start) {
         // Hit a boundary...as accurate as we are going to get.
         break;
      }
   }


   t_now -= epocOffset;

   // t is in 100ns units, convert to usecs
   t_now = t_now / 10000; //msecs

   return t_now;
}

uint64 vbaselineMs = 0;
uint32 vtickBaseline = 0;

void vresyncBaselineClock() {
   vbaselineMs = 0;
   struct timeval tv;
   vgettimeofday(&tv, NULL);
}

/* Correctly synched with the 'real' time/date clock, but only exact to
 * about 15ms.
 */
uint64 vgetCurMsFromClock() {
   // This has resolution of only about 15ms
   FILETIME tm;

   GetSystemTimeAsFileTime(&tm);
   uint64 t = tm.dwHighDateTime;
   t = t << 32;
   t |= tm.dwLowDateTime;

   // The Windows epoc is January 1, 1601 (UTC).
   static uint64 offset = vcalcEpocOffset();

   //mudlog << "file-time: " << t << " offset: " << offset
   //       << " normalized: " << (t - offset) << " hi: "
   //       << tm.dwHighDateTime << " lo: " << tm.dwLowDateTime
   //       << " calc-offset:\n" << calcEpocOffset() << "\n\n";

   t -= offset;

   // t is in 100ns units, convert to ms
   t = t / 10000;
   return t;
}

const char* vwinstrerror() {
   // To decode these, try:
   // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winsock/winsock/windows_sockets_error_codes_2.asp
   static char lfsrr[128];
   sprintf(lfsrr, "Error# %d", WSAGetLastError());
   return lfsrr;
}

#endif

/** Returns opinter to static memory, do NOT delete or modify result. */
const char* vtoStringIp(uint32 ip) {
   static char buf[50];
   sprintf(buf, "%i.%i.%i.%i",
           ((ip >> 24) & 0xFF),
           ((ip >> 16) & 0xFF),
           ((ip >> 8) & 0xFF),
           (ip & 0xFF));
   return buf;
}//toStringIp


int vtoIpString(const char* ip, uint32& retval) {
   retval = 0;

   if (strcmp(ip, "0.0.0.0") == 0) {
      return 0;
   }

   if (ip[0] == 0) {
      return 0;
   }

   uint64 now = vgetCurMs(); // Debugging only

#if 0
   struct addrinfo aiHints;
   struct addrinfo *aiList = NULL;

   // It seems it can't resolve IP addresses, or tries too hard to find a name,
   // or something.  Need to check for a.b.c.d
   retval = inet_addr(ip);
   if (retval != INADDR_NONE) {
      retval = ntohl(retval);
      return 0;
   }
   // Otherwise, try to resolve it below.


   //--------------------------------
   // Setup the hints address info structure
   // which is passed to the getaddrinfo() function
   memset(&aiHints, 0, sizeof(aiHints));
   aiHints.ai_family = AF_INET;
   aiHints.ai_socktype = SOCK_STREAM;
   aiHints.ai_protocol = IPPROTO_TCP;

   //--------------------------------
   // Call getaddrinfo(). If the call succeeds,
   // the aiList variable will hold a linked list
   // of addrinfo structures containing response
   // information about the host
   if ((retval = getaddrinfo(ip, NULL, &aiHints, &aiList)) != 0) {
      cpLog(LOG_ERR, "ERROR:  getaddrinfo failed, error: %s\n", VSTRERROR);
      return retval;
   }

   uint64 n2 = getCurMs();
   if (n2 - now > 10) {
      VLOG << "WARNING:  Took " << n2 - now << "ms to gethostbyname for host/ip -:"
           << ip << ":-\n";
   }

   if (aiList && aiList->ai_addr) {
      struct sockaddr_in* sin = (struct sockaddr_in*)(aiList->ai_addr);
      retval = ntohl(sin->sin_addr.s_addr);

      freeaddrinfo(aiList);
      return retval;
   }
   return -1;
#endif

   // It seems it can't resolve IP addresses, or tries too hard to find a name,
   // or something.  Need to check for a.b.c.d
   retval = inet_addr(ip);
   if (retval != INADDR_NONE) {
      retval = ntohl(retval);
      return 0;
   }

   // Otherwise, try to resolve it below.
   struct hostent *hp;
   hp = gethostbyname(ip);
   uint64 n2 = vgetCurMs();
   if (n2 - now > 10) {
      cpLog(LOG_ERR, "WARNING:  Took %dms to gethostbyname for host/ip: %s\n",
            n2 - now, ip);
   }
   if (hp == NULL) {
      return -1;
   }//if
   else {
      retval = ntohl(*((unsigned int*)(hp->h_addr_list[0])));
      if (retval != 0) {
         return 0;
      }
      else {
         return -1;
      }
   }
   return -1;
}



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
#ifndef __WIN32__
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
#else
   return 0;
#endif
}//vsetPriorityHelper

int vsetTosHelper_priv(int sk, uint16 val) {
#ifndef __WIN32__
   if (setsockopt(sk, SOL_IP, IP_TOS, (char*)&val, sizeof(val)) < 0) {
      return -1;
   }//if

   int new_val = 0;
   socklen_t slt = sizeof(new_val);
   if (getsockopt(sk, SOL_IP, IP_TOS, (char*)(&new_val), &slt) < 0) {
      return -1;
   }//if
   return new_val;
#else
   return -1;
#endif
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

#ifndef LINK_WITH_LANFORGE_MISC

int vgettimeofday(struct timeval* tv, void* null) {

   // Get baseline time, in seconds.
   if (vbaselineMs == 0) {
      vbaselineMs = vgetRawCurMsSpin();
      vtickBaseline = timeGetTime();
   }

   uint32 curTicks = timeGetTime();
   if (curTicks < vtickBaseline) {
      // Wrap!
      cerr << "milisecond tick counter wrapped, congratz on a long run!\n";
      vbaselineMs = vgetRawCurMsSpin();
      vtickBaseline = timeGetTime();
   }
   
   uint64 now_ms = (vbaselineMs + (curTicks - vtickBaseline));
   *tv = vms_to_tv(now_ms);
   return 0;
};

void vsleep(int secs) {
   Sleep(secs * 1000);
}

int vkill(int pid, int sig) {
   return 0;
}

int vflock(int pid, int lk) {
   return 0;
}

uint64 vgetCurMs() {
   struct timeval tv;
   vgettimeofday(&tv, NULL);
   return vtv_to_ms(tv);
}

uint64 vgetCurUs() {
   struct timeval tv;
   vgettimeofday(&tv, NULL);
   return vtv_to_us(tv);
}

uint64 vgetCurNs() {
   struct timeval tv;
   vgettimeofday(&tv, NULL);
   return vtv_to_ns(tv);
}


struct timeval vgetCurTv() {
   struct timeval tv;
   vgettimeofday(&tv, NULL);
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

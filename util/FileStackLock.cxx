// Released into the public domain. --Ben Greear June 7, 2004


#include "FileStackLock.hxx"
#include <cpLog.h>
#include "errno.h"


FileStackLock::FileStackLock(const string& fname, uint64 maxWait) {
   //int pid = getpid();
   int fd = -1;
   int flgs = LOCK_EX;
   struct itimerval value;
   struct itimerval ovalue;
   bool resetTimer = false;

   if (maxWait == 0) {
      flgs |= LOCK_NB;
   }
   else {
      // Use this to wake after a set amount of time if we cannot
      // get the lock by then...
      memset(&value, 0, sizeof(value));
      value.it_value = vms_to_tv(maxWait);
      //leave it_interval zero so that we do not re-start the timer
      if (setitimer(ITIMER_REAL, &value, &ovalue) < 0) {
         cpLog(LOG_ERR, "ERROR:  setitimer: %s\n", strerror(errno));
      }
      else {
         resetTimer = true;
      }
   }

   FILE* lck = fopen(fname.c_str(), "a");
   if (lck) {
      fd = fileno(lck);
      if (flock(fd, flgs) < 0) {
         locked = false;
      }
      else {
         //VLOG_INF(fprintf(lck, "%llu: %i has lock.\n", getCurMs(), pid));
         locked = true;
      }
   }
   else {
      cpLog(LOG_ERR, "ERROR:  Failed to open lck file -:%s:- error: %s\n",
            fname.c_str(), strerror(errno));
      locked = false;
   }

   if (resetTimer) {
      if (setitimer(ITIMER_REAL, &ovalue, NULL) < 0) {
         cpLog(LOG_ERR, "ERROR:  (re)setitimer: %s\n", strerror(errno));
      }
   }
}

FileStackLock::~FileStackLock() {
   if (lck) {
      //VLOG_INF(fprintf(lck, "%llu: %i gave up lock.\n", getCurMs(), pid));
      flock(fd, LOCK_UN); // Unlock it..
      fclose(lck); // Seems to free up the pointer...
      lck = NULL;
   }
}//destructor


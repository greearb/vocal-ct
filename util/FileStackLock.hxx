// Released into the public domain. --Ben Greear June 7, 2004


#include <sys/file.h>
#include <string>
#include <misc.hxx>

class FileStackLock {
protected:
   FILE* lck;
   int fd;
   bool locked;
public:
   FileStackLock(const string& fname, uint64 maxWaitMs);

   ~FileStackLock();

   bool isLocked() { return locked; }

};//FileStackLock



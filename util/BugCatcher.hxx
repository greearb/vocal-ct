

#ifndef __BUG_CATCHER_INC__
#define __BUG_CATCHER_INC__

#include <assert.h>
#include "RCObject.hxx"

class BugCatcher: public RCObject {
private:
   unsigned int magic;
   static unsigned int _cnt;

public:
   BugCatcher() { magic = 0x1234543; _cnt++; }
   BugCatcher(const BugCatcher& rhs) { magic = rhs.magic; _cnt++; }
   virtual ~BugCatcher() { assertNotDeleted(); magic = 0xdeadbeef; _cnt--; }

   virtual void assertNotDeleted() const {
      assert(magic != 0xdeadbeef);
   }

   static int getInstanceCount() { return _cnt; }
};

#endif

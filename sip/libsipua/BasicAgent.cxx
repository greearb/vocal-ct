

#include "BasicAgent.hxx"

using namespace Vocal::UA;

int BasicAgent::_cnt = 0;

BasicAgent::BasicAgent(unsigned long authId, const string& className)
      : myAuthId(authId),
        myDelFlag(false),
        class_name(className) {
   _cnt++;
   cpLog(LOG_ERR, "Creating basic-agent (id: %d className: %s  this: %p, _cnt: %d\n",
         authId, className.c_str(), this, _cnt);
}


BasicAgent::BasicAgent( const BasicAgent& src )
      : class_name(src.class_name)
{
   _cnt++;
   cpLog(LOG_ERR, "Copy-Creating basic-agent: %p, cnt: %d\n", this, _cnt);
   copyObj(src);
}


BasicAgent::~BasicAgent() {
   cpLog(LOG_ERR, "Deleting basic-agent, id: %d, this %p, cnt: %d\n",
         myAuthId, this, _cnt);
   _cnt--;
   if (myInvokee != 0) {
      // myInvokee points back to this, and we MUST have it cleaned
      // up before deleting this object, otherwise we crash in UaStateInCall
      // when handling a SIP_BYE (sometimes)
      myInvokee->assertNotDeleted();

      myInvokee->setControllerAgent(NULL); // Keep the invokee from accessing deleted mem
   }
}

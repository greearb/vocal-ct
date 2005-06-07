

#include "BasicAgent.hxx"

using namespace Vocal::UA;

int BasicAgent::_cnt = 0;

BasicAgent::BasicAgent(unsigned long authId, const string& className)
      : myAuthId(authId),
        class_name(className) {
   _cnt++;
   cpLog(LOG_DEBUG_STACK, "Creating basic-agent (id: %d className: %s  this: %p, _cnt: %d\n",
         authId, className.c_str(), this, _cnt);
}


BasicAgent::~BasicAgent() {
   assertNotDeleted();
   cpLog(LOG_DEBUG_STACK, "Deleting basic-agent, id: %d, this %p, cnt: %d\n",
         myAuthId, this, _cnt);
   if (myInvokee != 0) {
      // myInvokee points back to this, and we MUST have it cleaned
      // up before deleting this object, otherwise we crash in UaStateInCall
      // when handling a SIP_BYE (sometimes)
      myInvokee->assertNotDeleted();

      myInvokee->setControllerAgent(NULL); // Keep the invokee from accessing deleted mem
      myInvokee = NULL;
   }
   _cnt--;
}//destructor

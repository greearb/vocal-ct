#include "InviteMsg.hxx"
#include "SipTransceiver.hxx"

using namespace Vocal;


int main()
{
    Data                aData;
    const char*     szData=NULL;
     Data url;

     url = "sip:destination@192.168.5.130";
     Sptr<SipUrl> tourl = new SipUrl(url);
     int listenPort = 5060;
     int rtpPort = 3456;
     SipTransceiver x;
     LocalScopeAllocator lo;

     for(int i = 0; i < 10; i++) {
     InviteMsg invite(tourl, listenPort, rtpPort);
     aData = invite.encode();
     szData = aData.getData(lo);
     x.sendAsync(invite);
     }
     sleep(60);
}

#include "global.h"
#include <iostream> 
#include "SipAuthorization.cxx"
#include "Data.hxx"
#include "Verify.hxx"
#include "cpLog.h"
#include "RegisterMsg.hxx"
#include "SystemInfo.hxx"

char*msg_msg= "REGISTER sip:66.100.107.120 SIP/2.0\r\n\
To: <sip:1234@66.100.107.35>\r\n\
From: <sip:1234@66.100.107.35>\r\n\
Call-ID: -1273521907-1051736095@66.100.107.35\r\n\
CSeq: 10 REGISTER\r\n\
Via: SIP/2.0/UDP 66.100.107.35:5060\r\n\
User-Agent:  UbiquityUserAgent/4\r\n\
Contact: <sip:1234@66.100.107.35>\r\n\
Expires: 3600\r\n\
Content-Length: 0\r\n\
Authorization: Digest username=\"1234\", realm=\"66.100.107.120\", nonce=\"1011235448\", uri=\"sip:66.100.107.120\", algorithm=MD5, response=\"8a5165b024fda362ed9c1e29a7af0ef2\"\r\n\
\r\n";

Data data1("Digest realm=\"66.100.107.120\", username=\"1234\", nonce=\"1011235448\", uri=\"sip:66.100.107.120\", algorithm=MD5, response=\"8a5165b024fda362ed9c1e29a7af0ef2\"");

void 
test()
{
    string host_addr = theSystem.gethostAddress();

    ///Test header
    SipAuthorization aHeader(data1, host_addr);
    test_verify(aHeader.getTokenValue(REALM) == Data("66.100.107.120"));
    test_verify(aHeader.getTokenValue(USERNAME) == Data("1234"));
    test_verify(aHeader.getTokenValue(URI) == Data("sip:66.100.107.120"));
    test_verify(aHeader.getTokenValue("response") == Data("8a5165b024fda362ed9c1e29a7af0ef2"));
    test_verify(aHeader.getTokenValue("nonce") == Data("1011235448"));
    test_verify(aHeader.getTokenValue("algorithm") == Data("MD5"));
    test_verify(aHeader.getAuthScheme() == Data("Digest"));


    //Test within a message
    RegisterMsg rMsg(msg_msg, host_addr);
    RegisterMsg rMsg2("", host_addr);
    rMsg2 = rMsg;
    test_verify(rMsg == rMsg2);
    test_verify(rMsg.encode() == rMsg2.encode());
    SipAuthorization myAuth = rMsg.getAuthorization();
    test_verify(myAuth.getTokenValue("response") == Data("8a5165b024fda362ed9c1e29a7af0ef2"));
    test_verify(myAuth.getTokenValue(USERNAME) == Data("1234"));
    test_verify(myAuth.encode() == aHeader.encode());

}

int main()
{
    test();
    return test_return_code(12);
}


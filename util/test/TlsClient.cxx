#include <cstdio>
#include "Tcp_ClientSocket.hxx"
#include "TlsConnection.hxx"
#include "cpLog.h"
#include <stdio.h>

int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);
//    TcpClientSocket f("www.fortify.net", 443);
    TcpClientSocket f("localhost", 4000);

    char buf[4096];
	    

    try
    {
	f.connect();
    }
    catch (...) 
    {
	cout << "failed to connect\n";
	exit(-1);
    }

    try
    {
	string x = "STARTTLS\n";
	f.getConn().writeData(x);
	int y;
	f.getConn().readLine(buf, 4095, y);
	cout << "return: " << buf;
	f.getConn().readLine(buf, 4095, y);

	cout << "return: " << buf;

	TlsConnection tls(f.getConn());
	int err = tls.initTlsClient();
	if(err > 0)
	{
	    string out = "GET / HTTP/1.0\n\n";
	
	    tls.writeData(out);
	    
	    err = tls.readn(buf, 4095);
	    
	    buf[err] = '\0';
	    
	    printf("%s", buf);
	}
	tls.close();
    }
    catch(...)
    {
	cout << "no good!\n";
    }
    return 0;
}

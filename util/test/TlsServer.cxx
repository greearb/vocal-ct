#include <cstdio>

#include "Tcp_ServerSocket.hxx"
#include "TlsConnection.hxx"
#include "cpLog.h"
#include <stdio.h>

#define HOME "./"

#define CERTF  HOME "cert.pem"
#define KEYF  HOME  "key.pem"


int main()
{
    cpLogSetPriority(LOG_DEBUG_STACK);
    TcpServerSocket f(4000);
    char buf[4096];

    while(1)
    {
	Connection c;
	f.accept(c);

	int bytesRead;

	bool goTls = false;

#if 1
	while(1)
	{
	    int result = c.readLine(buf, 4095, bytesRead);
	    if(result <= 0)
	    {
		cpLog(LOG_DEBUG, "no tls");
		break;
	    }
	    else
	    {
		// check for start command
		if(strcmp(buf, "STARTTLS\n") == 0)
		{
		    goTls = true;
		    c.writeData((void*)"200 OK\n", strlen("200 OK\n"));
		    c.writeData((void*)"Content-Length: 0\n", strlen("Content-Length: 0\n"));
		    cpLog(LOG_DEBUG, "tls!");
		    break;
		}
	    }
	}
#endif
	goTls = true;

	if(goTls)
	{
	    TlsConnection tls(c);
	    
	    int err = tls.initTlsServer(CERTF, KEYF);
	    if(err > 0)
	    {
		
		err = tls.readn(buf, 4095);
		
		buf[err] = '\0';
		
		printf("got: %s\n", buf);
		string s = "HELLO, WORLD!\nThis is a test that i am writing people\ni hope it works now\n";
		tls.writeData(s);
	    }
	    else
	    {
		cpLog(LOG_ERR, "error initializing TLS server");
		cpLog(LOG_ERR, "errcode: %d", tls.getErrMsg(err).logData());
	    }
	    tls.close();
	
	}
	else
	{
	    c.close();
	}
    }
    return 0;
}

#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h> 
#include <arpa/inet.h>


using namespace std;

int main(int argc, char** argv)
{
        struct addrinfo hints;
        struct addrinfo *res;
        struct hostent* hent;
 
        // Setup structures
        memset(&hints, 0, sizeof(hints));

        char hostName[256];
        if (gethostname(hostName, sizeof(hostName)) == -1)
        {
           cerr << "Failed to get the host name:" << endl;
           return(-1);
        }
        hints.ai_flags = AI_CANONNAME;
        hints.ai_family = PF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        char* port = new char[25];
        sprintf(port, "%u", 80);
        int error = getaddrinfo(hostName, 0, &hints, &res);
        struct addrinfo* myItr = res;
        char hName[256];
        int myFlg = 0;
        while(myItr)
        {
            cerr << "Family:" << myItr->ai_family << endl;
            if(myItr->ai_family == PF_INET6)
            {
                myFlg |= 0x02;
            }
            else if(myItr->ai_family == PF_INET)
            {
                myFlg |= 0x01;
            }

            cerr << "Address Len:" << myItr->ai_addrlen << endl;
            cerr << "Protocol:" << myItr->ai_protocol << endl;
            error = getnameinfo(myItr->ai_addr, myItr->ai_addrlen, hName, 256, NULL, 0, NI_NUMERICHOST | NI_NUMERICSERV );
            cerr << "Host address:" << hName << endl;
            cerr << "Name:" << myItr->ai_canonname << endl;
            myItr = myItr->ai_next;
        }

        if((myFlg & 0x01) && (myFlg & 0x02))
        {
            cerr << "Dual-stack, IPv4 and IPv6" << endl;
        }
        else if((myFlg & 0x01))
        {
            cerr << "IPv4 support only" << endl;
        }
        else if(myFlg & 0x02)
        {
            cerr << "IPv6 support only" << endl;
        }

        freeaddrinfo(res);
}

#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>   
#include <errno.h>

#ifdef WIN32
#include <winsock2.h>
#include <stdlib.h>
#include <io.h>
#else

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <net/if.h>

#endif

#include "udp.h"
#include "stun.h"


using namespace std;


static void
computeHmac(char* hmac, const char* input, int length, const char* key, int keySize);

static bool 
stunParseAtrAddress( char* body, unsigned int hdrLen,  StunAtrAddress4& result )
{
   if ( hdrLen != 8 )
   {
      clog << "hdrLen wrong for Address" <<endl;
      return false;
   }
   result.pad = *body++;
   result.family = *body++;
   if (result.family == IPv4Family)
   {
      UInt16 nport;
      memcpy(&nport, body, 2); body+=2;
      result.ipv4.port = ntohs(nport);
      
      UInt32 naddr;
      memcpy(&naddr, body, 4); body+=4;
      result.ipv4.addr = ntohl(naddr);
      return true;
   }
   else if (result.family == IPv6Family)
   {
      clog << "ipv6 not supported" << endl;
   }
   else
   {
      clog << "bad address family: " << result.family << endl;
   }
   
   return false;
}

static bool 
stunParseAtrChangeRequest( char* body, unsigned int hdrLen,  StunAtrChangeRequest& result )
{
   clog << "hdr length = " << hdrLen << " expecting " << sizeof(result) << endl;
   
   if ( hdrLen != 4 )
   {
      clog << "Incorrect size for ChangeRequest" << endl;
      return false;
   }
   else
   {
      memcpy(&result.value, body, 4);
      result.value = ntohl(result.value);
      return true;
   }
}

static bool 
stunParseAtrError( char* body, unsigned int hdrLen,  StunAtrError& result )
{
   if ( hdrLen >= sizeof(result) )
   {
      clog << "head on Error too large" << endl;
      return false;
   }
   else
   {
      memcpy(&result.pad, body, 2); body+=2;
      result.pad = ntohs(result.pad);
      result.errorClass = *body++;
      result.number = *body++;
      
      result.sizeReason = hdrLen - 4;
      memcpy(&result.reason, body, result.sizeReason);
      result.reason[result.sizeReason] = 0;
      return true;
   }
}

static bool 
stunParseAtrUnknown( char* body, unsigned int hdrLen,  StunAtrUnknown& result )
{
   if ( hdrLen >= sizeof(result) )
   {
      return false;
   }
   else
   {
      if (hdrLen % 4 != 0) return false;
      result.numAttributes = hdrLen / 4;
      for (int i=0; i<result.numAttributes; i++)
      {
         memcpy(&result.attrType[i], body, 2); body+=2;
         result.attrType[i] = ntohs(result.attrType[i]);
      }
      return true;
   }
}


static bool 
stunParseAtrString( char* body, unsigned int hdrLen,  StunAtrString& result )
{
   if ( hdrLen >= STUN_MAX_STRING )
   {
      clog << "String is too large" << endl;
      return false;
   }
   else
   {
      if (hdrLen % 4 != 0)
      {
         clog << "Bad length string " << hdrLen << endl;
         return false;
      }
      
      result.sizeValue = hdrLen;
      memcpy(&result.value, body, hdrLen);
      result.value[hdrLen] = 0;
      return true;
   }
}


static bool 
stunParseAtrIntegrity( char* body, unsigned int hdrLen,  StunAtrIntegrity& result )
{
   if ( hdrLen != 20)
   {
      clog << "MessageIntegrity must be 20 bytes" << endl;
      return false;
   }
   else
   {
      memcpy(&result.hash, body, hdrLen);
      return true;
   }
}

bool
stunParseMessage( char* buf, unsigned int bufLen, StunMessage& msg)
{
   clog << "Received stun message: " << bufLen << " bytes" << endl;
   memset(&msg, 0, sizeof(msg));
   
   if(sizeof(StunMsgHdr) > bufLen)
   {
      clog << "Bad message" << endl;
      return false;
   }
   
   memcpy(&msg.msgHdr, buf, sizeof(StunMsgHdr));
   msg.msgHdr.msgType = ntohs(msg.msgHdr.msgType);
   msg.msgHdr.msgLength = ntohs(msg.msgHdr.msgLength);

   if (msg.msgHdr.msgLength + sizeof(StunMsgHdr) != bufLen)
   {
      clog << "Message header length doesn't match message size: " << msg.msgHdr.msgLength << " - " << bufLen << endl;
      return false;
   }

   char* body = buf + sizeof(StunMsgHdr);
   unsigned int size = msg.msgHdr.msgLength;

   //clog << "bytes after header = " << size << endl;
   
   while ( size > 0 )
   {
      // !jf! should check that there are enough bytes left in the buffer

      StunAtrHdr* attr = reinterpret_cast<StunAtrHdr*>(body);
	
      unsigned int attrLen = ntohs(attr->length);
      int atrType = ntohs(attr->type);
      
      //clog << "Found attribute type=" << AttrNames[atrType] << " length=" << attrLen << endl;
      if ( attrLen+4 > size ) 
      {
         clog << "claims attribute is larger than size of message " <<"(attribute type="<<atrType<<")"<< endl;
         return false;
      }

      body += 4; // skip the length and type in attribute header
      size -= 4;
      
      switch ( atrType )
      {
         case MappedAddress:
            msg.hasMappedAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.mappedAddress )== false )
            {
               clog << "problem parsing MappedAddress" << endl;
               return false;
            }
            else
            {
               clog << "MappedAddress = " << msg.mappedAddress.ipv4 << endl;
            }
            
            break;  

         case ResponseAddress:
            msg.hasResponseAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.responseAddress )== false )
            {
               clog << "problem parsing ResponseAddress" << endl;
               return false;
            }
            else
            {
               clog << "ResponseAddress = " << msg.responseAddress.ipv4 << endl;
            }
            break;  

         case ChangeRequest:
            msg.hasChangeRequest = true;
            if (stunParseAtrChangeRequest( body, attrLen, msg.changeRequest) == false)
            {
               clog << "problem parsing ChangeRequest" << endl;
               return false;
            }
            else
            {
               clog << "ChangeRequest = " << msg.changeRequest.value << endl;
            }
            break;

         case SourceAddress:
            msg.hasSourceAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.sourceAddress )== false )
            {
               clog << "problem parsing SourceAddress" << endl;
               return false;
            }
            else
            {
               clog << "SourceAddress = " << msg.sourceAddress.ipv4 << endl;
            }
            break;  

         case ChangedAddress:
            msg.hasChangedAddress = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.changedAddress )== false )
            {
               clog << "problem parsing ChangedAddress" << endl;
               return false;
            }
            else
            {
               clog << "ChangedAddress = " << msg.changedAddress.ipv4 << endl;
            }
            break;  
            
         case Username: 
            msg.hasUsername = true;
            if (stunParseAtrString( body, attrLen, msg.username) == false)
            {
               clog << "problem parsing Username" << endl;
               return false;
            }
            else
            {
               clog << "Username = " << msg.username.value << endl;
            }

            break;

         case Password: 
            msg.hasPassword = true;
            if (stunParseAtrString( body, attrLen, msg.password) == false)
            {
               clog << "problem parsing Password" << endl;
               return false;
            }
            else
            {
               clog << "Password = " << msg.password.value << endl;
            }
            break;

         case MessageIntegrity:
            msg.hasMessageIntegrity = true;
            if (stunParseAtrIntegrity( body, attrLen, msg.messageIntegrity) == false)
            {
               clog << "problem parsing MessageIntegrity" << endl;
               return false;
            }
            else
            {
               //clog << "MessageIntegrity = " << msg.messageIntegrity.hash << endl;
            }

            // read the current HMAC
            // look up the password given the user of given the transaction id 
            // compute the HMAC on the buffer
            // decide if they match or not
            break;

         case ErrorCode:
            msg.hasErrorCode = true;
            if (stunParseAtrError(body, attrLen, msg.errorCode) == false)
            {
               clog << "problem parsing ErrorCode" << endl;
               return false;
            }
            else
            {
               clog << "ErrorCode = " << int(msg.errorCode.errorClass) 
                    << " " << int(msg.errorCode.number) 
                    << " " << msg.errorCode.reason << endl;
            }
            
            break;

         case UnknownAttribute:
            msg.hasUnknownAttributes = true;
            if (stunParseAtrUnknown(body, attrLen, msg.unknownAttributes) == false)
            {
               clog << "problem parsing UnknownAttribute" << endl;
               return false;
            }
            break;

         case ReflectedFrom:
            msg.hasReflectedFrom = true;
            if ( stunParseAtrAddress(  body,  attrLen,  msg.reflectedFrom )== false )
            {
               clog << "problem parsing ReflectedFrom" << endl;
               return false;
            }
            break;  

         default:
            clog << "Unknown attribute: " << atrType << endl;
            if ( atrType <= 0x7FFF ) return false;
      }
	
      body += attrLen;
      size -= attrLen;
   }
    
   return true;
}

static char* 
encode16(char* buf, UInt16 data)
{
   UInt16 ndata = htons(data);
   memcpy(buf, reinterpret_cast<void*>(&ndata), sizeof(UInt16));
   return buf + sizeof(UInt16);
}

static char* 
encode32(char* buf, UInt32 data)
{
   UInt32 ndata = htonl(data);
   memcpy(buf, reinterpret_cast<void*>(&ndata), sizeof(UInt32));
   return buf + sizeof(UInt32);
}


static char* 
encode(char* buf, const char* data, unsigned int length)
{
   memcpy(buf, data, length);
   return buf + length;
}


static char* 
encodeAtrAddress4(char* ptr, UInt16 type, const StunAtrAddress4& atr)
{
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, 8);
   *ptr++ = atr.pad;
   *ptr++ = IPv4Family;
   ptr = encode16(ptr, atr.ipv4.port);
   ptr = encode32(ptr, atr.ipv4.addr);

   return ptr;
}

static char* 
encodeAtrChangeRequest(char* ptr, const StunAtrChangeRequest& atr)
{
   ptr = encode16(ptr, ChangeRequest);
   ptr = encode16(ptr, 4);
   ptr = encode32(ptr, atr.value);
   return ptr;
}

static char* 
encodeAtrError(char* ptr, const StunAtrError& atr)
{
   ptr = encode16(ptr, ErrorCode);
   ptr = encode16(ptr, 6 + atr.sizeReason);
   ptr = encode16(ptr, atr.pad);
   *ptr++ = atr.errorClass;
   *ptr++ = atr.number;
   ptr = encode(ptr, atr.reason, atr.sizeReason);
   return ptr;
}

static char* 
encodeAtrUnknown(char* ptr, const StunAtrUnknown& atr)
{
   ptr = encode16(ptr, UnknownAttribute);
   ptr = encode16(ptr, 2+2*atr.numAttributes);
   for (int i=0; i<atr.numAttributes; i++)
   {
      ptr = encode16(ptr, atr.attrType[i]);
   }
   return ptr;
}

static char* 
encodeAtrString(char* ptr, UInt16 type, const StunAtrString& atr)
{
   assert(atr.sizeValue % 4 == 0);
   
   ptr = encode16(ptr, type);
   ptr = encode16(ptr, atr.sizeValue);
   ptr = encode(ptr, atr.value, atr.sizeValue);
   return ptr;
}


static char* 
encodeAtrIntegrity(char* ptr, const StunAtrIntegrity& atr)
{
   ptr = encode16(ptr, MessageIntegrity);
   ptr = encode16(ptr, 20);
   ptr = encode(ptr, atr.hash, sizeof(atr.hash));
   return ptr;
}


unsigned int
stunEncodeMessage( const StunMessage& msg, char* buf, unsigned int bufLen, const StunAtrString& password)
{
   assert(bufLen >= sizeof(StunMsgHdr));
   char* ptr = buf;
   
   ptr = encode16(ptr, msg.msgHdr.msgType);
   char* lengthp = ptr;
   ptr = encode16(ptr, 0);
   ptr = encode(ptr, reinterpret_cast<const char*>(msg.msgHdr.id.octet), sizeof(msg.msgHdr.id));
   
   clog << "Encoding stun message: " << endl;
   if (msg.hasMappedAddress)
   {
      clog << "Encoding MappedAddress: " << msg.mappedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4 (ptr, MappedAddress, msg.mappedAddress);
   }
   if (msg.hasResponseAddress)
   {
      clog << "Encoding ResponseAddress: " << msg.responseAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ResponseAddress, msg.responseAddress);
   }
   if (msg.hasChangeRequest)
   {
      clog << "Encoding ChangeRequest: " << msg.changeRequest.value << endl;
      ptr = encodeAtrChangeRequest(ptr, msg.changeRequest);
   }
   if (msg.hasSourceAddress)
   {
      clog << "Encoding SourceAddress: " << msg.sourceAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, SourceAddress, msg.sourceAddress);
   }
   if (msg.hasChangedAddress)
   {
      clog << "Encoding ChangedAddress: " << msg.changedAddress.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ChangedAddress, msg.changedAddress);
   }
   if (msg.hasUsername)
   {
      clog << "Encoding Username: " << msg.username.value << endl;
      ptr = encodeAtrString(ptr, Username, msg.username);
   }
   if (msg.hasPassword)
   {
      clog << "Encoding Password: " << msg.password.value << endl;
      ptr = encodeAtrString(ptr, Password, msg.password);
   }
   if (msg.hasErrorCode)
   {
      clog << "Encoding ErrorCode: class=" 
           << int(msg.errorCode.errorClass)  
           << " number=" << int(msg.errorCode.number) 
           << " reason=" 
           << msg.errorCode.reason 
           << endl;
      
      ptr = encodeAtrError(ptr, msg.errorCode);
   }
   if (msg.hasUnknownAttributes)
   {
      clog << "Encoding UnknownAttribute: ???" << endl;
      ptr = encodeAtrUnknown(ptr, msg.unknownAttributes);
   }
   if (msg.hasReflectedFrom)
   {
      clog << "Encoding ReflectedFrom: " << msg.reflectedFrom.ipv4 << endl;
      ptr = encodeAtrAddress4(ptr, ReflectedFrom, msg.reflectedFrom);
   }
   if (password.sizeValue > 0)
   {
      clog << "HMAC with password: " << password.value << endl;

      StunAtrIntegrity integrity;
      computeHmac(integrity.hash, buf, int(ptr-buf) , password.value, password.sizeValue);
      ptr = encodeAtrIntegrity(ptr, integrity);
   }
   clog << endl;
   
   encode16(lengthp, UInt16(ptr - buf - sizeof(StunMsgHdr)));
   return int(ptr - buf);
}

int 
stunRand()
{
   // return 32 bits of random stuff
   assert( sizeof(int) == 4 );
   static bool init=false;
   if ( !init )
   { 
      init = true;
      
      UInt64 tick;
      
#if defined(WIN32) 
      volatile unsigned int lowtick=0,hightick=0;
      __asm
         {
            rdtsc 
               mov lowtick, eax
               mov hightick, edx
               }
      tick = hightick;
      tick <<= 32;
      tick |= lowtick;
#elif defined(__GNUC__) && ( defined(__i686__) || defined(__i386__) )
      asm("rdtsc" : "=A" (tick));
#elif defined (__SUNPRO_CC)	
      tick = gethrtime();//This is Not expensive Under solaris 8 & above but systemcall in solaris7
#elif defined(__MACH__)
      int fd=open("/dev/random",O_RDONLY);
      read(fd,&tick,sizeof(tick));
      close(fd);
#else
#     error Need some way to seed the random number generator 
#endif 
      int seed = int(tick);
#ifdef WIN32
      srand(seed);
#else
      srandom(seed);
#endif
   }

#ifdef WIN32
   assert( RAND_MAX == 0x7fff );
   int r1 = rand();
   int r2 = rand();

   int ret = (r1<<16) + r2;

   return ret;
#else
   return random(); 
#endif
}

#ifdef WIN32
static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
  strncpy(hmac,"hmac-not-implemented",20);
}
#else
#include <openssl/hmac.h>

static void
computeHmac(char* hmac, const char* input, int length, const char* key, int sizeKey)
{
   unsigned int resultSize=0;
   HMAC(EVP_sha1(), 
        key, sizeKey, 
        reinterpret_cast<const unsigned char*>(input), length, 
        reinterpret_cast<unsigned char*>(hmac), &resultSize);
   assert(resultSize == 20);
}
#endif


static void
toHex(const char* buffer, int bufferSize, char* output) 
{
   static char hexmap[] = "0123456789abcdef";

   const char* p = buffer;
   char* r = output;
   for (int i=0; i < bufferSize; i++)
   {
      unsigned char temp = *p++;
	   
      int hi = (temp & 0xf0)>>4;
      int low = (temp & 0xf);
      
      *r++ = hexmap[hi];
      *r++ = hexmap[low];
   }
   *r = 0;
}

void
stunCreateUserName(const StunAddress4& source, StunAtrString* username)
{
   UInt64 time = stunGetSystemTimeSecs();
   time -= (time % 20*60);
   UInt64 hitime = time >> 32;
   UInt64 lotime = time & 0xFFFFFFFF;
   
   char buffer[1024];
   sprintf(buffer,
	   "%08lx:%08lx:%08lx:", 
           UInt32(source.addr),
           UInt32(stunRand()),
           UInt32(lotime));
   assert( strlen(buffer) < 1024 );

   assert(strlen(buffer) + 41 < STUN_MAX_STRING);
   
   char hmac[20];
   char key[] = "Jason";
   computeHmac(hmac, buffer, strlen(buffer), key, strlen(key) );
   char hmacHex[41];
   toHex(hmac, 20, hmacHex );
   hmacHex[40] =0;

   strcat(buffer,hmacHex);
   
   int l = strlen(buffer);
   assert( l+1 < STUN_MAX_STRING );
   username->sizeValue = l;
   memcpy(username->value,buffer,l);
   username->value[l]=0;

   //clog << "computed username=" << username.value << endl;
}

void
stunCreatePassword(const StunAtrString& username, StunAtrString* password)
{
   char hmac[20];
   char key[] = "Fluffy";
   //char buffer[STUN_MAX_STRING];
   computeHmac(hmac, username.value, strlen(username.value), key, strlen(key));
   toHex(hmac, 20, password->value);
   password->sizeValue = 40;
   password->value[40]=0;

   //clog << "password=" << password->value << endl;
}


UInt64
stunGetSystemTimeSecs()
{
   UInt64 time=0;
#if defined(WIN32)  
   SYSTEMTIME t;
   GetSystemTime( &t );
   time = (t.wHour*60+t.wMinute)*60+t.wSecond; 
#else
   struct timeval now;
   gettimeofday( &now , NULL );
   //assert( now );
   time = now.tv_sec;
#endif
   return time;
}



ostream& operator<< ( ostream& strm, const UInt128& r )
{
   strm << int(r.octet[0]);
   for ( int i=1; i<16; i++ )
   {
      strm << ':' << int(r.octet[i]);
   }
    
   return strm;
}

ostream& 
operator<<( ostream& strm, const StunAddress4& addr)
{
   UInt32 ip = addr.addr;
   strm << ((int)(ip>>24)&0xFF) << ".";
   strm << ((int)(ip>>16)&0xFF) << ".";
   strm << ((int)(ip>> 8)&0xFF) << ".";
   strm << ((int)(ip>> 0)&0xFF) ;
  
   strm << ":" << addr.port;
  
   return strm;
}

static void
parseHostName( char* peerName,
                   UInt32& ip,
                   UInt16& portVal,
                   UInt16 defaultPort )
{
   in_addr sin_addr;
    
   char host[512];
   strncpy(host,peerName,512);
   host[512-1]='\0';
   char* port = NULL;

   int portNum = defaultPort;
   
   // pull out the port part if present.
   char* sep = strchr(host,':');
   
   if ( sep == NULL )
   {
      portNum = defaultPort;
   }
   else
   {
      *sep = '\0';
      port = sep + 1;
      // set port part
      
      char* endPtr=NULL;
      
      portNum = strtol(port,&endPtr,0);
      
      if ( endPtr != NULL )
      {
         if ( *endPtr != '\0' )
         {
            portNum = defaultPort;
         }
      }
   }
    
   assert( portNum >= 1024 );
   assert( portNum <= 65000 );

   // figure out the host part 
   struct hostent* h;
   
#ifdef WIN32
   assert( strlen(host) >= 1 );
   if ( isdigit( host[0] ) )
   {
	   // assume it is a ip address 
	   unsigned long a = inet_addr(host);
	   //cerr << "a=" << hex << a << endl;
		
	   ip = ntohl( a );
   }
   else
   {
	   // assume it is a host name 
	   h = gethostbyname( host );
 
   if ( h == NULL )
   {
	   int err = errno;
	   std::cerr << "error was " << err << std::endl;
	   assert( err != WSANOTINITIALISED );
	   assert(0);
	   ip = ntohl( 0x7F000001L );
   }
   else
   {
	   sin_addr = *(struct in_addr*)h->h_addr;
	   ip = ntohl( sin_addr.s_addr );
   }
  }

#else
   h = gethostbyname( host );
   if ( h == NULL )
   {
	   int err = errno;
	   std::cerr << "error was " << err << std::endl;
	   assert(0);
           ip = ntohl( 0x7F000001L );
   }
   else
   {
      sin_addr = *(struct in_addr*)h->h_addr;
      ip = ntohl( sin_addr.s_addr );
   }
#endif

   portVal = portNum;
}


void
stunParseServerName( char* name, StunAddress4& addr)
{
   assert(name);

   // TODO - put in DNS SRV stuff.

   parseHostName( name, addr.addr, addr.port, 3478); 
}

#define STUN_MAX_MESSAGE_SIZE 2048

static void
stunCreateErrorResponse(StunMessage& response, int cl, int number, const char* msg)
{
   response.msgHdr.msgType = BindErrorResponseMsg;
   response.hasErrorCode = true;
   response.errorCode.errorClass = cl;
   response.errorCode.number = number;
   strcpy(response.errorCode.reason, msg);
}

static void
stunCreateSharedSecretErrorResponse(StunMessage& response, int cl, int number, const char* msg)
{
   response.msgHdr.msgType = SharedSecretErrorResponseMsg;
   response.hasErrorCode = true;
   response.errorCode.errorClass = cl;
   response.errorCode.number = number;
   strcpy(response.errorCode.reason, msg);
}

static void
stunCreateSharedSecretResponse(const StunMessage& request, const StunAddress4& source, StunMessage& response)
{
   response.msgHdr.msgType = SharedSecretResponseMsg;
   response.msgHdr.id = request.msgHdr.id;

   response.hasUsername = true;
   stunCreateUserName( source, &response.username);

   response.hasPassword = true;
   stunCreatePassword( response.username, &response.password);
}


// This funtion takes a single message sent to a stun server, parses
// and constructs an apropriate repsonse - returns true if message is
// valid
bool
stunServerProcessMsg( char* buf,
                      unsigned int bufLen,
                      StunAddress4& from, 
                      StunAddress4& myAddr,
                      StunAddress4& altAddr, 
                      StunMessage* resp,
                      StunAddress4* destination,
                      StunAtrString* hmacPassword,
                      bool* changePort,
                      bool* changeIp,
                      bool verbose)
{
    
   // set up information for default response 

   memset( resp, 0 , sizeof(*resp) );
   
   *changeIp = false;
   *changePort = false;
   
   StunMessage req;
   bool ok = stunParseMessage( buf,bufLen, req);
   
   if (!ok)      // Complete garbage, drop it on the floor
   {
      if (verbose) clog << "Request did not parse" << endl;
      return false;
   }
   clog << "Request parsed ok" << endl;
   
   StunAddress4 mapped = req.mappedAddress.ipv4;
   StunAddress4 respondTo = req.responseAddress.ipv4;
   UInt32 flags = req.changeRequest.value;

   switch (req.msgHdr.msgType)
   {
      case SharedSecretRequestMsg:
         if(verbose) clog << "Received SharedSecretRequestMsg on udp. send error 433." << endl;
// !cj! - should fix so you know if this came over TLS or UDP
         stunCreateSharedSecretResponse(req, from, *resp);
         //stunCreateSharedSecretErrorResponse(*resp, 4, 33, "this request must be over TLS");
         return true;
         
      case BindRequestMsg:
         if (!req.hasMessageIntegrity)
         {
            if (verbose) clog << "BindRequest does not contain MessageIntegrity" << endl;
            
            if (0) // !jf! mustAuthenticate
            {
               if(verbose) clog << "Received BindRequest with no MessageIntegrity. Sending 401." << endl;
               stunCreateErrorResponse(*resp, 4, 1, "Missing MessageIntegrity");
               return true;
            }
         }
         else
         {
            if (!req.hasUsername)
            {
               if (verbose) clog << "No UserName. Send 432." << endl;
               stunCreateErrorResponse(*resp, 4, 32, "No UserName and contains MessageIntegrity");
               return true;
            }
            else
            {
               if (verbose) clog << "Validating username: " << req.username.value << endl;
               // !jf! could retrieve associated password from provisioning here
               if (strcmp(req.username.value, "test") == 0)
               {
                  if (0)
                  {
                     // !jf! if the credentials are stale 
                     stunCreateErrorResponse(*resp, 4, 30, "Stale credentials on BindRequest");
                     return true;
                  }
                  else
                  {
                     if (verbose) clog << "Validating MessageIntegrity" << endl;
                     // need access to shared secret

                     unsigned char hmac[20];
                     unsigned int hmacSize=20;
#ifndef WIN32
					 HMAC(EVP_sha1(), 
                          "1234", 4, 
                          reinterpret_cast<const unsigned char*>(buf), bufLen-20-4, 
                          hmac, &hmacSize);
                     assert(hmacSize == 20);
#endif

                     if (memcmp(buf, hmac, 20) != 0)
                     {
                        if (verbose) clog << "MessageIntegrity is bad. Sending " << endl;
                        stunCreateErrorResponse(*resp, 4, 3, "Unknown username. Try test with password 1234");
                        return true;
                     }

                     // need to compute this later after message is filled in
                     resp->hasMessageIntegrity = true;
                     assert(req.hasUsername);
                     resp->hasUsername = true;
                     resp->username = req.username; // copy username in
                  }
               }
               else
               {
                  if (verbose) clog << "Invalid username: " << req.username.value << "Send 430." << endl; 
               }
            }
         }
         
         // !jf! should check for unknown attributes here and send 420 listing the
         // unknown attributes. 
         
         if ( respondTo.port == 0 ) respondTo = from;
         if ( mapped.port == 0 ) mapped = from;
    
         *changeIp   = ( flags & ChangeIpFlag )?true:false;
         *changePort = ( flags & ChangePortFlag )?true:false;

         if (verbose)
         {
            clog << "Request is valid:" << endl;
            clog << "\t flags=" << flags << endl;
            clog << "\t changeIp=" << *changeIp << endl;
            clog << "\t changePort=" << *changePort << endl;
            clog << "\t from = " << from << endl;
            clog << "\t respond to = " << respondTo << endl;
            clog << "\t mapped = " << mapped << endl;
         }
    
         // form the outgoing message
         resp->msgHdr.msgType = BindResponseMsg;
         for ( int i=0; i<16; i++ )
         {
            resp->msgHdr.id.octet[i] = req.msgHdr.id.octet[i];
         }

         resp->hasMappedAddress = true;
         resp->mappedAddress.ipv4.port = mapped.port;
         resp->mappedAddress.ipv4.addr = mapped.addr;

         resp->hasSourceAddress = true;
         resp->sourceAddress.ipv4.port = (*changePort) ? altAddr.port : myAddr.port;
         resp->sourceAddress.ipv4.addr = (*changeIp) ? altAddr.addr : myAddr.addr ;
    
         resp->hasChangedAddress = true;
         resp->changedAddress.ipv4.port = altAddr.port;
         resp->changedAddress.ipv4.addr = altAddr.addr;

         if ( req.hasUsername && req.username.sizeValue > 0 ) 
         {
            // copy username in
            resp->hasUsername = true;
            memcpy( resp->username.value, req.username.value, req.username.sizeValue );
         }

         if ( req.hasMessageIntegrity & req.hasUsername )  
         {
            // this creates the password that will be used in the HMAC when then
            // messages is sent
            stunCreatePassword( req.username, hmacPassword );
         }
         
         if (req.hasUsername && (req.username.sizeValue > 64 ) )
         {
            UInt32 source;
            sscanf(req.username.value, "%lx", &source);
            resp->hasReflectedFrom = true;
            resp->reflectedFrom.ipv4.port = 0;
            resp->reflectedFrom.ipv4.addr = source;
         }

         destination->port = respondTo.port;
         destination->addr = respondTo.addr;

         return true;
         
      default:
         if (verbose) clog << "Unknown or unsupported request " << endl;
         return false;
   }

   assert(0);
   return false;
}
   
bool
stunInitServer(StunServerInfo& info, const StunAddress4& myAddr, const StunAddress4& altAddr)
{
   assert( myAddr.port != 0 );
   assert( altAddr.port!= 0 );
   assert( myAddr.addr  != 0 );
   assert( altAddr.addr != 0 );

   info.myAddr = myAddr;
   info.altAddr = altAddr;

   info.myFd = -1;
   info.altPortFd = -1;
   info.altIpFd = -1;
   info.altIpPortFd = -1;
   
   if ((info.myFd = openPort(myAddr.port, myAddr.addr)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      stunStopServer(info);
      return false;
   }
   clog << "Opened " << myAddr.addr << ":" << myAddr.port << " --> " << info.myFd << endl;

   if ((info.altPortFd = openPort(altAddr.port,myAddr.addr)) == INVALID_SOCKET)
   {
      clog << "Can't open " << myAddr << endl;
      stunStopServer(info);
      return false;
   }
   clog << "Opened " << myAddr.addr << ":" << altAddr.port << " --> " << info.altPortFd << endl;
   
   if ((info.altIpFd = openPort( myAddr.port, altAddr.addr)) == INVALID_SOCKET)
   {
      clog << "Can't open " << altAddr << endl;
      stunStopServer(info);
      return false;
   }
   clog << "Opened " << altAddr.addr << ":" << myAddr.port << " --> " << info.altIpFd << endl;;
   
   if ((info.altIpPortFd = openPort(altAddr.port, altAddr.addr)) == INVALID_SOCKET)
   {
      clog << "Can't open " << altAddr << endl;
      stunStopServer(info);
      return false;
   }
   clog << "Opened " << altAddr.addr << ":" << altAddr.port << " --> " << info.altIpPortFd << endl;;
   
   return true;
}

void
stunStopServer(StunServerInfo& info)
{
   if (info.myFd > 0) close(info.myFd);
   if (info.altPortFd > 0) close(info.altPortFd);
   if (info.altIpFd > 0) close(info.altIpFd);
   if (info.altIpPortFd > 0) close(info.altIpPortFd);
}


bool
stunServerProcess(StunServerInfo& info, bool verbose)
{
   char msg[udpMaxMessageLength];
   int msgLen = udpMaxMessageLength;
   
   msgLen = sizeof(msg);

   bool ok = false;
   bool recvAlt =false;
      
   fd_set fdSet; 
   unsigned int maxFd=0;
   FD_ZERO(&fdSet); 
   FD_SET(info.myFd,&fdSet); 
   if ( info.myFd >= maxFd ) maxFd=info.myFd+1;
   FD_SET(info.altPortFd,&fdSet); 
   if ( info.altPortFd >= maxFd ) maxFd=info.altPortFd+1;
   FD_SET(info.altIpFd,&fdSet);
   if (info.altIpFd>=maxFd) maxFd=info.altIpFd+1;
   FD_SET(info.altIpPortFd,&fdSet);
   if (info.altIpPortFd>=maxFd) maxFd=info.altIpPortFd+1;
   
   struct timeval tv;
   tv.tv_sec = 0;
   tv.tv_usec = 100000;

   int e = select( maxFd, &fdSet, NULL,NULL, &tv );
   if (e < 0)
   {
      clog << "Error on select: " << strerror(errno) << endl;
   }
   else if (e >= 0)
   {
      StunAddress4 from;

      if (FD_ISSET(info.myFd,&fdSet))
      {
         if (verbose) clog << "received on A1:P1" << endl;
         recvAlt = false;
         ok = getMessage( info.myFd, msg, &msgLen, &from.addr, &from.port );
      }
      else if (FD_ISSET(info.altPortFd, &fdSet))
      {
         if (verbose) clog << "received on A1:P2" << endl;
         recvAlt = false;
         ok = getMessage( info.altPortFd, msg, &msgLen, &from.addr, &from.port );
      }
      else if (FD_ISSET(info.altIpFd,&fdSet))
      {
         if (verbose) clog << "received on A2:P1" << endl;
         recvAlt = true;
         ok = getMessage( info.altIpFd, msg, &msgLen, &from.addr, &from.port );
      }
      else if (FD_ISSET(info.altIpPortFd, &fdSet))
      {
         if (verbose) clog << "received on A2:P2" << endl;
         recvAlt = true;
         ok = getMessage( info.altIpPortFd, msg, &msgLen, &from.addr, &from.port );
      }
      else
      {
         return true;
      }
    
      if ( !ok ) 
      {
         if ( verbose ) clog << "Get message did not return a valid message" <<endl;
         return true;
      }
      
      if ( verbose ) clog << "Got a request (len=" << msgLen << ") from " << from << endl;
      
      if ( msgLen <= 0 )
      {
         return true;
      }
      
      bool changePort = false;
      bool changeIp = false;

      StunMessage resp;
      StunAddress4 dest;
      StunAtrString hmacPassword;  
      hmacPassword.sizeValue = 0;
  
      ok = stunServerProcessMsg( msg, msgLen, from, 
                                 recvAlt ? info.altAddr : info.myAddr,
                                 recvAlt ? info.myAddr : info.altAddr, 
                                 &resp,
                                 &dest,
                                 &hmacPassword,
                                 &changePort,
                                 &changeIp,
                                 verbose );
      
      if ( !ok )
      {
         if ( verbose ) clog << "Failed to parse message" << endl;
         return true;
      }

      char buf[STUN_MAX_MESSAGE_SIZE];
      int len = STUN_MAX_MESSAGE_SIZE;
      

      len = stunEncodeMessage( resp, buf, len, hmacPassword );
      
      if ( dest.addr == 0 )  ok=false;
      if ( dest.port == 0 ) ok=false;
      
      if ( ok )
      {
         assert( dest.addr != 0 );
         assert( dest.port != 0 );

         bool sendAlt = false; // send on the default IP address 
         if ( recvAlt )  sendAlt = !sendAlt; // if received on alt, then flip logic
         if ( changeIp ) sendAlt = !sendAlt; // if need to change IP, then flip logic 

         if ( !changePort )
         {
            if ( !sendAlt )
            {
               sendMessage( info.myFd, buf, len, dest.addr, dest.port );
            }
            else
            {
               sendMessage( info.altIpFd, buf, len,dest.addr, dest.port );
            }
         }
         else
         {
            if ( !sendAlt )
            {
               sendMessage( info.altPortFd, buf, len,dest.addr, dest.port );
            }
            else
            {
               sendMessage( info.altIpPortFd, buf, len, dest.addr, dest.port );
            }
         }
      }
   }

   return true;
}

int 
stunFindLocalInterfaces(UInt32* addresses,int maxRet)
{
#ifdef WIN32
	return 0;
#else
   struct ifconf ifc;
   
   int s = socket( AF_INET, SOCK_DGRAM, 0 );
   int len = 100 * sizeof(struct ifreq);

   char buf[ len ];
   
   ifc.ifc_len = len;
   ifc.ifc_buf = buf;
   
   int e = ioctl(s,SIOCGIFCONF,&ifc);
   char *ptr = buf;
   int tl = ifc.ifc_len;
   int count=0;
  
   while ( (tl > 0) && ( count < maxRet) )
   {
      struct ifreq* ifr = (struct ifreq *)ptr;
      
      int si = sizeof(ifr->ifr_name) + sizeof(struct sockaddr);
      tl -= si;
      ptr += si;
      //char* name = ifr->ifr_ifrn.ifrn_name;
      //cerr << "name = " << name << endl;
 
      struct ifreq ifr2;
      ifr2 = *ifr;
      
      e = ioctl(s,SIOCGIFADDR,&ifr2);
      if ( e == -1 )
      {
         break;
      }
      
      //cerr << "ioctl addr e = " << e << endl;

      struct sockaddr a = ifr2.ifr_addr;
      struct sockaddr_in* addr = (struct sockaddr_in*) &a;

      UInt32 ai = ntohl( addr->sin_addr.s_addr );
      if (int((ai>>24)&0xFF) != 127)
      {
         addresses[count++] = ai;
      }

#if 1
      cerr << "Detected interface "
           << int((ai>>24)&0xFF) << "." 
           << int((ai>>16)&0xFF) << "." 
           << int((ai>> 8)&0xFF) << "." 
           << int((ai    )&0xFF) << endl;
#endif
   }

	close(s);

   return count;
#endif
}


static void
stunBuildReqSimple( StunMessage* msg,
                    const StunAtrString& username,
		    bool changePort, bool changeIp, unsigned int id=0 )
{
   assert( msg );
   memset( msg , 0 , sizeof(*msg) );
   
   msg->msgHdr.msgType = BindRequestMsg;

   for ( int i=0; i<16; i=i+4 )
   {
      assert(i+3<16);
      int r = stunRand();
      msg->msgHdr.id.octet[i+0]= r>>0;
      msg->msgHdr.id.octet[i+1]= r>>8;
      msg->msgHdr.id.octet[i+2]= r>>16;
      msg->msgHdr.id.octet[i+3]= r>>24;
   }

   if ( id != 0 )
   {
      msg->msgHdr.id.octet[0] = id; 
   }
   
   msg->hasChangeRequest = true;
   msg->changeRequest.value =(changeIp?ChangeIpFlag:0) | 
                             (changePort?ChangePortFlag:0);

   if ( username.sizeValue > 0 )
   {
      msg->hasUsername = true;
      msg->username = username;
   }
}


static void 
stunSendTest( Socket myFd, StunAddress4& dest, const StunAtrString& username, const StunAtrString& password, int testNum, bool verbose )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );

   bool changePort=false;
   bool changeIP=false;
   bool discard=false;
 
   switch (testNum)
   {
      case 1:
      case 10:
         break;
      case 2:
         changePort=true;
         changeIP=true;
         break;
      case 3:
         changePort=true;
         break;
      case 4:
         changeIP=true;
         break;
      case 5:
         discard=true;
         break;
      default:
         cerr << "Test " << testNum <<" is unkown\n";
         assert(0);
   }
   
   StunMessage req;
   memset(&req, 0, sizeof(StunMessage));
   
   stunBuildReqSimple( &req, username, 
		       changePort , changeIP , 
		       testNum );

   char buf[STUN_MAX_MESSAGE_SIZE];
   int len = STUN_MAX_MESSAGE_SIZE;
   
   len = stunEncodeMessage( req, buf, len, password );

   if ( verbose )
   {
      clog << "About to send msg of len " << len << " to " << dest << endl;
   }
   
   sendMessage( myFd,
                buf, len, 
                dest.addr, 
                dest.port );
}


void 
stunGetUserNameAndPassword(  const StunAddress4& dest, 
                             StunAtrString* username,
                             StunAtrString* password)
{ 
// !cj! This is totally bogus - need to make TLS connection to dest and get a
// username and password to use 
   stunCreateUserName(dest, username);
   stunCreatePassword(*username, password);
}


void 
stunTest( StunAddress4& dest, int testNum, bool verbose )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );

   Socket myFd = openPort();

   StunAtrString username;
   StunAtrString password;
   
   username.sizeValue = 0;
   password.sizeValue = 0;
   
#ifdef USE_TLS
   stunGetUserNameAndPassword( dest, username, password );
#endif

   stunSendTest( myFd, dest, username, password, testNum, verbose );
    
   char msg[STUN_MAX_MESSAGE_SIZE];
   int msgLen = STUN_MAX_MESSAGE_SIZE;

   StunAddress4 from;
   getMessage( myFd,
               msg,
               &msgLen,
               &from.addr,
               &from.port );
   
   StunMessage resp;
   memset(&resp, 0, sizeof(StunMessage));

   if ( verbose ) clog << "Got a response" << endl;
   bool ok = stunParseMessage( msg,msgLen, resp );
   
   if ( verbose )
   {
      clog << "\t ok=" << ok << endl;
      clog << "\t id=" << resp.msgHdr.id << endl;
      clog << "\t mappedAddr=" << resp.mappedAddress.ipv4 << endl;
      clog << "\t changedAddr=" << resp.changedAddress.ipv4 << endl;
      clog << endl;
   }
}


NatType
stunNatType( StunAddress4& dest, bool verbose )
{ 
   assert( dest.addr != 0 );
   assert( dest.port != 0 );
   
   //StunAddress4 mappedAddr;
   //StunAddress4 changedAddr;
   //UInt128 id;
    
   Socket myFd = openPort();
    
   bool respTestI=false;
   bool isNat=true;
   StunAddress4 testIchangedAddr;
   StunAddress4 testImappedAddr;
   bool respTestI2=false; 
   bool mappedIpSame = true;
   StunAddress4 testI2mappedAddr;
   StunAddress4 testI2dest=dest;
   bool respTestII=false;
   bool respTestIII=false;

   memset(&testImappedAddr,0,sizeof(testImappedAddr));

   StunAtrString username;
   StunAtrString password;
  
   username.sizeValue = 0;
   password.sizeValue = 0;

#ifdef USE_TLS 
   stunGetUserNameAndPassword( dest, username, password );
#endif

   stunSendTest( myFd, dest, username, password, 1, verbose );
   int count=0;
   while ( count++ < 10 )
   {
      struct timeval tv;
      fd_set fdSet; int fdSetSize;
      FD_ZERO(&fdSet); fdSetSize=0;
      FD_SET(myFd,&fdSet); 
      fdSetSize = myFd+1;
      tv.tv_sec=0;
      tv.tv_usec=100*1000; // 100 ms 

      int  err = select(fdSetSize, &fdSet, NULL, NULL, &tv);
      int e = errno;
      if ( err == SOCKET_ERROR )
      {
         // error occured
         cerr << "Error " << e << " " << strerror(e) << " in select" << endl;
      }
      else if ( err == 0 )
      {
         // timeout occured 
         if (!respTestI ) 
         {
            stunSendTest( myFd, dest, username, password, 1 ,verbose );
         }         
         if ((!respTestI2) && respTestI ) 
         {
            // check the address to send to if valid 
            if (  ( testI2dest.addr != 0 ) &&
                  ( testI2dest.port != 0 ) )
            {
               stunSendTest( myFd, testI2dest, username, password, 10  ,verbose);
            }
         }
         if (!respTestII )
         {
            stunSendTest( myFd, dest, username, password, 2 ,verbose );
         }   
         if (!respTestIII )
         {
            stunSendTest( myFd, dest, username, password, 3 ,verbose );
         }
      }
      else
      {
         //clog << "-----------------------------------------" << endl;
         assert( err>0 );
         // data is avialbe on some fd 
         if ( myFd!=INVALID_SOCKET ) if ( FD_ISSET(myFd,&fdSet) )
         {
            char msg[udpMaxMessageLength];
            int msgLen = udpMaxMessageLength;
            
            StunAddress4 from;

            getMessage( myFd,
                        msg,
                        &msgLen,
                        &from.addr,
                        &from.port );
            
            StunMessage resp;
            memset(&resp, 0, sizeof(StunMessage));
            
            stunParseMessage( msg,msgLen, resp );
                        
            if ( verbose )
            {
               clog << "Received message of type " << resp.msgHdr.msgType << endl;
            }
            
            switch( resp.msgHdr.id.octet[0] )
            {
               case 1:
                  testIchangedAddr.addr = resp.changedAddress.ipv4.addr;
                  testIchangedAddr.port = resp.changedAddress.ipv4.port;
                  testImappedAddr.addr = resp.mappedAddress.ipv4.addr;
                  testImappedAddr.port = resp.mappedAddress.ipv4.port;
                  respTestI=true;  
                  testI2dest.addr = resp.changedAddress.ipv4.addr;
                  //testI2dest.port = resp.changedAddress.ipv4.port;
                  break;
               case 2:
                  respTestII=true;
                  break;
               case 3:
                  respTestIII=true;
                  break;
               case 10:
                  testI2mappedAddr.addr = resp.mappedAddress.ipv4.addr;
                  testI2mappedAddr.port = resp.mappedAddress.ipv4.port;
                  respTestI2=true;
                  mappedIpSame = false;
                  if ( (testI2mappedAddr.addr  == testImappedAddr.addr ) &&
                       (testI2mappedAddr.port == testImappedAddr.port ))
                  { 
                     mappedIpSame = true;
                  }
			  
                  break;
            }
         }
      }
   }
   // see if we can bind to this address 
   //cerr << "try binding to " << testImappedAddr << endl;
   Socket s = openPort( 11000, testImappedAddr.addr );
   if ( s != INVALID_SOCKET )
   {
      close(s);
      isNat = false;
      //cerr << "binding worked" << endl;
   }
   else
   {
      isNat = true;
      //cerr << "binding failed" << endl;
   }

   if (verbose)
   {
      clog << "test I = " << respTestI << endl;
      clog << "test II = " << respTestII << endl;
      clog << "test III = " << respTestIII << endl;
      clog << "test I(2) = " << respTestI2 << endl;
      clog << "is nat  = " << isNat <<endl;
      clog << "mapped IP same = " << mappedIpSame << endl;
   }

   // implement logic flow chart from draft RFC
   if ( respTestI )
   {
      if ( isNat )
      {
         if (respTestII)
         {
            return StunTypeConeNat;
         }
         else
         {
            if ( mappedIpSame )
            {
               if ( respTestIII )
               {
                  return StunTypeRestrictedNat;
               }
               else
               {
                  return StunTypePortRestrictedNat;
               }
            }
            else
            {
               return StunTypeSymNat;
            }
         }
      }
      else
      {
         if (respTestII)
         {
            return StunTypeOpen;
         }
         else
         {
            return StunTypeSymFirewall;
         }
      }
   }
   else
   {
      return StunTypeBlocked;
   }

   assert(0);
   return StunTypeUnknown;
}


/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

// Local Variables:
// mode:c++
// c-file-style:"ellemtel"
// c-file-offsets:((case-label . +))
// indent-tabs-mode:nil
// End:



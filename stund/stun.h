#ifndef STUN_H
#define STUN_H

#include <iostream>

#define STUN_MAX_STRING 256

#define STUN_MAX_UNKNOWN_ATTRIBUTES 4

#define STUN_MAX_MESSAGE_SIZE 2048

#define STUN_PORT 3478

// define some basic types
typedef unsigned char  UInt8;
typedef unsigned short UInt16;
typedef unsigned int   UInt32;
#if defined( WIN32 )
typedef unsigned __int64 UInt64;
#else
typedef unsigned long long UInt64;
#endif
typedef struct { unsigned char octet[16]; }  UInt128;

/// define a structure to hold a stun address 
const UInt8  IPv4Family = 0x01;
const UInt8  IPv6Family = 0x02;

// define  flags  
const UInt32 ChangeIpFlag   = 4;
const UInt32 ChangePortFlag = 2;

// define  stun attribute
const UInt16 MappedAddress =    0x0001;
const UInt16 ResponseAddress =  0x0002;
const UInt16 ChangeRequest =    0x0003;
const UInt16 SourceAddress =    0x0004;
const UInt16 ChangedAddress =   0x0005;
const UInt16 Username =         0x0006;
const UInt16 Password =         0x0007;
const UInt16 MessageIntegrity = 0x0008;
const UInt16 ErrorCode =        0x0009;
const UInt16 UnknownAttribute = 0x000A;
const UInt16 ReflectedFrom =    0x000B;

// define types for a stun message 
const UInt16 BindRequestMsg               = 0x0001;
const UInt16 BindResponseMsg              = 0x0101;
const UInt16 BindErrorResponseMsg         = 0x0111;
const UInt16 SharedSecretRequestMsg       = 0x0002;
const UInt16 SharedSecretResponseMsg      = 0x0102;
const UInt16 SharedSecretErrorResponseMsg = 0x0112;

typedef struct 
{
      UInt16 msgType;
      UInt16 msgLength;
      UInt128 id;
} StunMsgHdr;


typedef struct
{
      UInt16 type;
      UInt16 length;
} StunAtrHdr;

typedef struct
{
      UInt16 port;
      UInt32 addr;
} StunAddress4;

typedef struct
{
      UInt8 pad;
      UInt8 family;
      StunAddress4 ipv4;
} StunAtrAddress4;


typedef struct
{
      UInt32 value;
} StunAtrChangeRequest;

typedef struct
{
      UInt16 pad; // all 0
      UInt8 errorClass;
      UInt8 number;
      char reason[STUN_MAX_STRING];
      UInt16 sizeReason;
} StunAtrError;

typedef struct
{
      UInt16 attrType[STUN_MAX_UNKNOWN_ATTRIBUTES];
      UInt16 numAttributes;
} StunAtrUnknown;

typedef struct
{
      char value[STUN_MAX_STRING];      
      UInt16 sizeValue;
} StunAtrString;


typedef struct
{
      char hash[20];
} StunAtrIntegrity;

typedef enum 
{
   HmacUnkown=0,
   HmacOK,
   HmacBadUserName,
   HmacUnkownUserName,
   HmacFailed,
} StunHmacStatus;

typedef struct
{
      StunMsgHdr msgHdr;

      bool hasMappedAddress;
      StunAtrAddress4  mappedAddress;

      bool hasResponseAddress;
      StunAtrAddress4  responseAddress;

      bool hasChangeRequest;
      StunAtrChangeRequest changeRequest;
      
      bool hasSourceAddress;
      StunAtrAddress4 sourceAddress;
      
      bool hasChangedAddress;
      StunAtrAddress4 changedAddress;
      
      bool hasUsername;
      StunAtrString username;
      
      bool hasPassword;
      StunAtrString password;

      bool hasMessageIntegrity;
      StunAtrIntegrity messageIntegrity;
      
      bool hasErrorCode;
      StunAtrError errorCode;
      
      bool hasUnknownAttributes;
      StunAtrUnknown unknownAttributes;

      bool hasReflectedFrom;
      StunAtrAddress4 reflectedFrom;
} StunMessage; 

// Define enum with different types of NAT 
typedef enum 
{
   StunTypeUnknown=0,
   StunTypeOpen,
   StunTypeConeNat,
   StunTypeRestrictedNat,
   StunTypePortRestrictedNat,
   StunTypeSymNat,
   StunTypeSymFirewall,
   StunTypeBlocked,
} NatType;

#ifdef WIN32
typedef SOCKET Socket;
#else
typedef int Socket;
#endif

typedef struct
{
      StunAddress4 myAddr;
      StunAddress4 altAddr;
      Socket myFd;
      Socket altPortFd;
      Socket altIpFd;
      Socket altIpPortFd;
}StunServerInfo;

bool
stunParseMessage( char* buf, unsigned int bufLen, StunMessage& message);

unsigned int
stunEncodeMessage( const StunMessage& message, char* buf, unsigned int bufLen, const StunAtrString& password);

void
stunCreateUserName(const StunAddress4& addr, StunAtrString* username);

void 
stunGetUserNameAndPassword(  const StunAddress4& dest, 
                             StunAtrString* username,
                             StunAtrString* password);

void
stunCreatePassword(const StunAtrString& username, StunAtrString* password);

int 
stunRand();

UInt64
stunGetSystemTimeSecs();

/// find the IP address of a the specified stun server - 
void 
stunParseServerName( char* serverName, StunAddress4& stunServerAddr);

/// return true if all is OK 
bool
stunInitServer(StunServerInfo& info, const StunAddress4& myAddr, const StunAddress4& altAddr);

void
stunStopServer(StunServerInfo& info);

/// return true if all is OK 
bool
stunServerProcess(StunServerInfo& info, bool verbose);

/// returns number of address found - take array or addres 
int 
stunFindLocalInterfaces(UInt32* addresses, int maxSize );

void 
stunTest( StunAddress4& dest, int testNum, bool verbose );

NatType
stunNatType( StunAddress4& dest, bool verbose );

/// prints a StunAddress
std::ostream& 
operator<<( std::ostream& strm, const StunAddress4& addr);

std::ostream& 
operator<< ( std::ostream& strm, const UInt128& r );


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
                      bool verbose);

#endif


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


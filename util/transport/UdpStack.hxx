#ifndef UDP_HXX
#define UDP_HXX

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

static const char* const UdpStackHeaderVersion =
    "$Id: UdpStack.hxx,v 1.5 2004/06/02 20:23:10 greear Exp $";

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/time.h>

#ifdef __vxworks
#include <selectLib.h>
#else
#include <sys/select.h>
#endif

#include <netdb.h>
#include "global.h"
#include <fstream>
#include <string>
#include <assert.h>
#include "NetworkAddress.h"


///
typedef enum
{
    inactive,
    sendonly,
    recvonly,
    sendrecv
} UdpMode;


// class UdpStackPrivateData;
// this class hold stuff that is likely to be a problem to port
class UdpStackPrivateData
{
public:
   UdpStackPrivateData() {
      localAddr = new sockaddr_storage();
      remoteAddr = new sockaddr_storage();
   }

   ~UdpStackPrivateData();

   /// File descriptor of socket
   int socketFd;
   
   /// Address of local computer that will receive or send the packets
   struct sockaddr_storage *localAddr;
   
   /** Address of remote computer that local computer receive from
    * or send to the packets
    * This address is set when doClient is called.
    * And later when setDestination() is called, this address will be the one
    * connect to. But disconnectPorts() won't remove this address.
    */
   struct sockaddr_storage *remoteAddr;

};



/** 
    A UDP transmitter and reciever.<p>

    This class functions to send and receive UDP packets over the
    network.  It is a simple abstraction of the standard facilities,
    in an slightly OO way.<p>

    By default, UdpStack prepares to both send and receive UDP packets.<p>

    <b>Receive Example:</b><p>

    <pre>
    UdpStack udpStack( 0, 5060 ); // listen on port 5060
    
    NetworkAddress sender;
    char buf[1024];
    
    // read a packet from the network, or timeout after 1 second
    int len = udpStack.receiveTimeout(buf, 1024, &sender, 1, 0);
    </pre>

    <p>
    <b>Send Example:</b><p>
    
    <pre>
    UdpStack udpStack( 0, 5060 ); // listen on port 5060
    
    NetworkAddress target;
    
    // ... stuff to set target here
    
    char buf[1024];
    strcpy(buf, "test");

    // read a packet from the network, or timeout after 1 second
    int len = udpStack.transmitTo(buf, strlen(buf) + 1, &target);
    </pre>

    <p>
    <b>Issues</b><p>

    Currently there is  no support for saying which interface a packet
    arrived on. I'm not sure why that would be useful but I think
    someone requested it once.
*/

class UdpStack: public RCObject
{
public:
   /** if local_ip is specified (not == ""), then we will attempt to 
    * bind to that local IP.  This allows one to specify the local interface
    * to use.  No ports can be specified and the stack will pick one, OR a minPort
    * can be specified and it will be used, OR a min and max can be
    * specified and the stack will use the first free port in that
    * range. The destiantion hostname and port may be set in the
    * destinationHost parameter.
    * If device_to_bind_to is not "",  then we'll attempt to bind to it
    * with SO_BINDTODEVICE
    */
   UdpStack ( const string& desired_local_ip,
              const string& device_to_bind_to, 
              const NetworkAddress* destinationHost = NULL,
              int localMinPort = -1,
              int localMaxPort = -1,
              UdpMode mode = sendrecv,
              bool log_flag = false,
              bool isMulticast = false);

   /** connectPorts() has to be called before using send()
    * It associates the local port to remote port
    * so that receive and send only from and to that remote port
    */
   void connectPorts();

   /// disconnectPorts() dissolve the port association
   void disconnectPorts();
   
   /// set the local ports
   void setLocal (const int minPort = -1, int maxPort = -1 );

   /// set the default destination
   void setDestination ( const NetworkAddress* host );

   /// set the default destination
   void setDestination ( const char* host, int port );

   /// Get the udp stack mode
   UdpMode getMode () {
      return mode;
   };

   /// Get the udp stack logFlag
   bool getLogFlag () {
      return logFlag;
   };

   /// Get the name for local
   string getLclName () const {
      return lclName;
   };

   /// Get the name for remote
   string getRmtName () const {
      return rmtName;
   };

   /// Get the port being used for recieving
   int getRxPort ();

   /// Get the port being used for transmitting
   int getTxPort ();

   /// Get the destination port
   int getDestinationPort () {
      return getRxPort();
   };

   ///
   NetworkAddress* getDestinationHost () const;


   /// Set the mode
   void setMode ( const UdpMode theMode ) {
      mode = theMode;
   };

   /// Set the logFlag
   void setLogFlag ( const bool theLogFlag ) {
      logFlag = theLogFlag;
   };

   /// Set the name to something logical - only used for errors
   void setLclName ( const string& theName ) {
      lclName = theName;
   };

   void setRmtName ( const string& theName ) {
      rmtName = theName;
   };


   /// Get the file descriptor for the socket - use with care or not at all
   int getSocketFD ();

   /// Add this stacks file descriptors to the the fdSet
   void addToFdSet ( fd_set* set );

   /// Find the max of any file descripts in this stack and the passed value
   int getMaxFD ( int prevMax);

   /// Check and see if this stacks file descriptor is set in fd_set
   bool checkIfSet ( fd_set* set );


   /** 
       Receive a datagram.
       Once connectPorts() is called, can't receive from other ports.
       
       @param buffer   buffer to write message into
       @param bufSize     maximum size of the buffer
       @param flags    Passed to sendto, ie MSG_DONTWAIT
       @return size of message received.
   */
   int receive ( const char* buffer,
                 const int bufSize,
                 int flags = 0);  // returns bytes read

   /** 
       Receive a datagram.
       Use receive if you do not need to be able to tell who this packet
       came from.
       
       @param buffer   buffer to write message into
       @param bufSize  maximum size of the buffer
       @param sender   address of the sender.
       @param flags    Flags passed to recvfrom, ie MSG_DONTWAIT
       @return size of message received.
       
   */
   int receiveFrom ( char* buffer,
                     const int bufSize,
                     NetworkAddress* sender,
                     int flags = 0);  // returns bytes read

   /** 
       Receive a datagram.
       Use receive if you do not need to be able to tell who this packet
       came from.
       
       @param buffer   buffer to write message into.
       @param bufSize     maximum size of the buffer.
       @param sender   the NetworkAdddress# of the sender.
       @param sec      number of seconds to wait.
       @param usec     number of microseconds to wait.
       
       @return size of message received, if successful, -1 for
       error, or 0 if the timeout was reached without receiving a
       message.
   */
   int receiveTimeout ( char* buffer,
                        const int bufSize,
                        NetworkAddress* sender,
                        int sec = 0,
                        int usec = 0);

   /** 
       Transmit a datagram.
       ConnectPorts() should be called before it is called.
       
       @param buffer   buffer to read message from
       @param length  number of bytes to send.
   */
   void transmit ( const char* buffer,
                   const int length );

   /** 
       Transmit a datagram.
       Use transmit if you always sent to the same person.
       
       @param buffer   buffer to read message from.
       @param length   number of bytes to send.
       @param dest     destination to send packet to.
       
       @return >0 if transmitted OK, or -errno if not OK.
   */
   /// Transmit a datagram
   int transmitTo ( const char* buffer,
                    const int length,
                    const NetworkAddress* dest );
   ///
   void joinMulticastGroup ( NetworkAddress group,
                             NetworkAddress* iface = NULL,
                             int ifaceInexe = 0 );
   ///
   void leaveMulticastGroup( NetworkAddress group,
                             NetworkAddress* iface = NULL,
                             int ifaceInexe = 0 );
   ///
   virtual ~UdpStack ();
   
   /// Get the number of bytes the stack has processed
   int getBytesTransmitted () const;

   /// Get the number of datagrams the stack has processed
   int getPacketsTransmitted () const;

   /// Get the number of bytes the stack has processed
   int getBytesReceived () const;

   /// Get the number of datagrams the stack has processed
   int getPacketsReceived () const;

   /** This can be used to eulate network packet loss by throwing
       out a certain percentage of the tranmitted packets */
   void emulatePacketLoss ( float probabilityOfLoss ) {
      packetLossProbability = probabilityOfLoss;
   };

   /**Set the mode to be blocking or non-blocking
    *flg=true (blocking)
    *flg=false (non-blocking), default beaviour is blocking
    *  Calls doSyncBlockingMode as needed.
    */
   int setModeBlocking(bool flg);

   // Set the bits on the socket.
   // This can be "" if was not specified during creation...just means we bind
   int doSyncBlockingMode();

   // to all interfaces.
   const string& getSpecifiedLocalIp() const { return desiredLocalIp; }

   string toString(); // Used for debugging.

private:
   UdpStack& operator= ( const UdpStack& x );
   UdpStack( const UdpStack& );
   UdpStack();

   void doServer ( int minPort,
                   int maxPort);

   void doClient ( const NetworkAddress* desName);
   
   int recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
                      struct sockaddr *sa, socklen_t *salenptr,
                      struct in6_pktinfo *pktp);
   /// name of receiver
   string lclName;

   /// name of transmitter
   string rmtName;

   /// probability of a given packet being thrown out by the stack
   float packetLossProbability;

   /// number of bytes the stack has received
   int numBytesReceived;

   /// number of datagrams the stack has received
   int numPacketsReceived;

   /// number of bytes the stack has transimitted
   int numBytesTransmitted;

   /// number of datagrams the stack has transimitted
   int numPacketsTransmitted;

   /// Mode
   UdpMode mode;


   /** Local IP address that we ended up bound to.
    */
   string curLocalIp; 

   /** If you wish to bind to a local IP, set that here.  If left
    * blank, the default local IP, as determined by the operating
    * system, will be used.
    */
   string desiredLocalIp;

   bool boundLocal; /* Have we bound ourselves to the local IP yet */

   /// flag for msg log
   bool logFlag;

   /// private data that will be a pain to port
   UdpStackPrivateData* data;

   ofstream* in_log;
   ofstream* out_log;

   int rcvCount ;
   int sndCount ;
   bool   blockingFlg;
   string localDev;
};


/// Class to hold all types of exceptions that occur in the UDP stack
class UdpStackException
{
    public:
        ///
        UdpStackException ( const string& description )
        : desc(description)
        {}
        ;

        string getDescription() const
        {
            return desc;
        }
    private:
        string desc;

        friend std::ostream& operator<< ( std::ostream& strm , const UdpStackException& e );
};

inline std::ostream&
operator<< ( std::ostream& strm , const UdpStackException& e )
{
    strm << e.desc;
    return strm;
}

//e exception class for when a transmited packet is renused
class UdpStackExceptionConectionRefused: public UdpStackException
{
    public:
        ///
        UdpStackExceptionConectionRefused( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when all ports all already in use
class UdpStackExceptionPortsInUse: public UdpStackException
{
    public:
        ///
        UdpStackExceptionPortsInUse ( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when a bogus hostname is given
class UdpStackExceptionBadHostname: public UdpStackException
{
    public:
        ///
        UdpStackExceptionBadHostname( const string& description )
        : UdpStackException(description)
        {}
        ;
};


/// exception class for when a bogus hostname is given
class UdpStackExceptionBadPort: public UdpStackException
{
    public:
        ///
        UdpStackExceptionBadPort( const string& description )
        : UdpStackException(description)
        {}
        ;
};



#endif

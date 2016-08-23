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

#include <sys/types.h>
#ifndef __MINGW32__
#include <sys/socket.h>
#include <netdb.h>
#endif

#include <unistd.h>
#include <sys/time.h>

#ifdef __vxworks
#include <selectLib.h>
#else
#ifndef __MINGW32__
#include <sys/select.h>
#endif
#endif

#include "global.h"
#include <fstream>
#include <string>
#include <assert.h>
#include "NetworkAddress.h"
#include <misc.hxx>
#include <list>
#include <Sptr.hxx>

///
typedef enum {
    inactive,
    sendonly,
    recvonly,
    sendrecv
} UdpMode;


class ByteBuffer : public BugCatcher {
private:
   // Not implemented, do not use.
   ByteBuffer& operator=(const ByteBuffer& rhs);
   ByteBuffer(const ByteBuffer& src);
   ByteBuffer();

protected:
   char* buf;
   int len;
   NetworkAddress* na;

public:
   ByteBuffer(const char* b, int ln, const NetworkAddress* n) {
      buf = (char*)(malloc(ln));
      memcpy(buf, b, ln);
      len = ln;
      na = NULL;
      if (n) {
         na = new NetworkAddress(*n);
      }
   }

   virtual ~ByteBuffer() {
      free(buf);
      if (na) {
         delete na;
      }
   }

   char* getBuf() { return buf; }
   int getLength() { return len; }
   NetworkAddress* getNetworkAddress() { return na; }
};//ByteBuffer
      


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

class UdpStack: public BugCatcher {
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
   UdpStack (const char* dbg, uint16 tos, uint32 priority,
              bool isBlocking, /* Are we a blocking or non-blocking socket? */
              const string& desired_local_ip,
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

   int setFds(fd_set* input_fds, fd_set* output_fds, fd_set* exc_fds,
              int& maxdesc, uint64& timeout, uint64 now);

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
                     struct sockaddr_in* sender,
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
                        struct sockaddr_in* sender,
                        int sec = 0,
                        int usec = 0);

   /** 
       Transmit a datagram.
       ConnectPorts() should be called before it is called.
       
       @param buffer   buffer to read message from
       @param length  number of bytes to send.
       
       This method will attempt to buffer data if we cannot send at this
       particular moment (EAGAIN).
   */
   int queueTransmit ( const char* buffer,
                       const int length );

   // Will not cache of flush backlog
   int doTransmit ( const char* buffer,
                     const int length );

   /** 
       Transmit a datagram.
       Use transmit if you always sent to the same person.
       
       @param buffer   buffer to read message from.
       @param length   number of bytes to send.
       @param dest     destination to send packet to.
       
       @return >0 if transmitted OK, or -errno if not OK.

       This method will attempt to buffer data if we cannot send at this
       particular moment (EAGAIN).
   */
   int queueTransmitTo ( const char* buffer,
                         const int length,
                         const NetworkAddress* dest );

   // Will not cache of flush backlog
   int doTransmitTo ( const char* buffer,
                      const int length,
                      const NetworkAddress* dest );

   int flushBacklog();

   int getBacklogMsgCount() { return sendBacklog.size(); }

   ///
   void joinMulticastGroup ( NetworkAddress group,
                             NetworkAddress* iface = NULL,
                             int ifaceInexe = 0 );
   ///
   void leaveMulticastGroup( NetworkAddress group,
                             NetworkAddress* iface = NULL,
                             int ifaceInexe = 0 );
   ///
   virtual ~UdpStack();
   
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

   int doServer ( int minPort, int maxPort);

   void doClient ( const NetworkAddress& desName);
   
   //int recvfrom_flags(int fd, void *ptr, size_t nbytes, int *flagsp,
   //                   struct sockaddr *sa, socklen_t *salenptr,
   //                   struct in6_pktinfo *pktp);

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

   /// File descriptor of socket
   int socketFd;
   
   /// Address of local computer that will receive or send the packets
   NetworkAddress localAddr;
   
   /** Address of remote computer that local computer receive from
    * or send to the packets
    * This address is set when doClient is called.
    * And later when setDestination() is called, this address will be the one
    * connect to. But disconnectPorts() won't remove this address.
    */
   NetworkAddress remoteAddr;

   /** If you wish to bind to a local IP, set that here.  If left
    * blank, the default local IP, as determined by the operating
    * system, will be used.
    */
   string desiredLocalIp;

   uint16 _tos;
   uint32 _skb_priority;

   bool boundLocal; /* Have we bound ourselves to the local IP yet */

   /// flag for msg log
   bool logFlag;

   ofstream* in_log;
   ofstream* out_log;

   int rcvCount ;
   int sndCount ;
   bool   blockingFlg;
   string localDev;
   string _dbg; // some identifying info for log messages.

   uint32 busy; //counter, number of times we tried to write and got EAGAIN
   uint32 drop_in_bklog; //Failed to send in the backlog
   uint32 rxbusy;

   // Msgs may queue here if we have no kernel buffers to send at the moment.
   list<Sptr<ByteBuffer> > sendBacklog;
};


#endif

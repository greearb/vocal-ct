#ifndef SDP2SESSION_HXX_
#define SDP2SESSION_HXX_

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

static const char* const Sdp2Session_hxx_Version =
    "$Id: Sdp2Session.hxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include "global.h"
#include "Data.hxx"
#include "Data.hxx"
#include <list>
#include <sys/types.h>
#include "support.hxx"
#include "Sdp2Media.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Time.hxx"
#include "Sdp2Attributes.hxx"
#include "Sdp2Bandwidth.hxx"
#include "Sdp2Encryptkey.hxx"
#include "Sdp2ZoneAdjustment.hxx"

#include "Sdp2Exceptions.hxx"



/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


enum SdpProtocolType
{
    SdpProtocolTypeSDP = 0,     // Conforms to RFC 2327
    SdpProtocolTypeNCS         // Conforms to additional NCS requirements
};


/** 
    This is the container which contains an entire Session Description Protocol (RFC 2327) session.
    <p>

    You can also look at the libmedia/SdpHandler.hxx for a real-life
    example of doing this.

    <p>
    <B>Usage</B>
    <p>
    <PRE>
    SdpSession sdpDesc;
    
    // v=0
    sdpDesc.setVersion(0);
    
    // this is responsible for c=IN IP4 10.0.0.1
    SdpConnection connection;
    connection.setUnicast("10.0.0.1"); // the 10.0.0.1
    connection.setAddressType(Vocal::SDP::AddressTypeIPV4); // the IP4
    sdpDesc.setConnection(connection);
    
    // the o= line is mostly automatically generated, but these two fields 
    // need to be filled in
    sdpDesc.setAddress("10.0.0.1");
    sdpDesc.setAddressType(Vocal::SDP::AddressTypeIPV4); // the IP4
    
    // the t= line is also automatically filled in
    
    // now, set the audio format.  this is responsible for the
    // m=audio 3456 RTP/AVP 0
    SdpMedia&#42; media = new SdpMedia();
    
    media->clearFormatList();
    // responsible for the audio
    media->setMediaType(Vocal::SDP::MediaTypeAudio); 
    // responsible for the 3456
    media->setPort( 3456 );
    // RTP/AVP
    media->setTransportType(Vocal::SDP::TransportTypeRTP);
    
    // here, set the payload type.  this 0 represends G.711 mu-law,
    // and is the 0 at the end of m= line above
    media->addFormat(0);
    
    // now, add a media object
    sdpDesc.addMedia(media);

    // declare the media attribute list pointer.  it creates an line 
    // a=sendrecv
    MediaAttributes&#42; mediaAttrib = new MediaAttributes();
    // mark as  sendrecv
    mediaAttrib->setsendrecv();
    media->setMediaAttributes(mediaAttrib);
    // glue it in

    // this section is responsible for a=rtpmap:0 PCMU/8000
    SdpRtpMapAttribute&#42; rtpMapAttrib = new SdpRtpMapAttribute();
    rtpMapAttrib->setPayloadType(0); // again, G.711 mu-law
    rtpMapAttrib->setEncodingName("PCMU"); 
    rtpMapAttrib->setClockRate(8000);
    mediaAttrib->addmap(rtpMapAttrib);

    sdpDesc.setSessionName("Vocal-Media-Lib");
    
    // print the final result
    cout << sdpDesc.encode() << endl;
    </PRE>

    Here is an example of the output of this program (see
    Sdp2Example.cxx for details):<p>
    <PRE>
    v=0
    o=- 3253665841 0 IN IP4 10.0.0.1
    s=Vocal-Media-Lib
    c=IN IP4 10.0.0.1
    t=3253665841 0
    m=audio 3456 RTP/AVP 0
    a=rtpmap:0 PCMU/8000
    a=sendrecv
    </PRE>
 */
class SdpSession
{
    public:
        ///
        SdpSession ();
        ///
        SdpSession (const SdpSession& x);

        ///
        ~SdpSession();

        ///
        bool isValidDescriptor()
        {
            return isValid;
        }

        ///
        void setProtocolType (SdpProtocolType protocol);
        ///
        SdpProtocolType getProtocolType ();
        /// Verify if this a well formed SDP according to protocol type
        bool verify (SdpProtocolType protocol = SdpProtocolTypeSDP);

        ///
        // Do we need this? Comment out to see what happens
        //    void setValidDescriptor(bool valid) { isValid = valid; }

        // No setProtocolVersion() is needed for now. It is always 0.
        ///
        int getProtocolVersion()
        {
            return protocolVersion;
        }

        ///
        void setUserName(char* name)
        {
            username = name;
            isValid = true;
        }
        ///
        void setUserName(const Data& name)
        {
            username = name;
            isValid = true;
        }
        ///
        Data getUserName()
        {
            return username;
        }

        ///
        void setSessionId(unsigned int session)
        {
            sessionId = session;
            isValid = true;
        }
        ///
        unsigned int getSessionId()
        {
            return sessionId;
        }

        ///
        void setVersion(unsigned int vers)
        {
            version = vers;
            isValid = true;
        }
        ///
        unsigned int getVersion()
        {
            return version;
        }

        // void setNetworkType (NetworkType ntwkype);
        ///
        NetworkType getNetworkType()
        {
            return networkType;
        }

        /// set the address type
        void setAddressType(AddressType addrType)
	{
		addressType = addrType;
	}

        ///
        AddressType getAddressType()
        {
            return addressType;
        }

        ///
        void setSessionName(char* name)
        {
            sessionName = name;
        }
        ///
        void setSessionName(const Data& name)
        {
            sessionName = name;
        }
        ///
        Data getSessionName()
        {
            return sessionName;
        }

        ///
        void setAddress(char* addr)
        {
            address = Data (addr);
        }
        ///
        void setAddress(const Data& addr)
        {
            address = addr;
        }
        ///
        Data getAddress()
        {
            return address;
        }

        ///
        void setSessionInfo(char* info)
        {
            sessionInfo = info;
        }
        ///
        void setSessionInfo(const Data& info)
        {
            sessionInfo = info;
        }
        ///
        Data getSessionInfo()
        {
            return sessionInfo;
        }

        ///
        void setURIInfo(char* uri)
        {
            uriInfo = uri;
        }
        ///
        void setURIInfo(const Data& uri)
        {
            uriInfo = uri;
        }
        ///
        Data getURIInfo()
        {
            return uriInfo;
        }

        ///
        void setEmailAddr(const char* emailAddr);
        ///
        void setEmailAddr(const Data& emailAddr);
        ///
        list < Data > getEmailList()
        {
            return emailList;
        }

        ///
        void setPhoneNum(const char* phoneNum);
        ///
        void setPhoneNum(const Data& phoneNum);
        ///
        list < Data > getPhoneList()
        {
            return phoneList;
        }


        ///
        void setConnection (const SdpConnection& conn);
        ///
        SdpConnection* getConnection();

        ///
        void setBandwidth (SdpBandwidth& bw);
        ///
        SdpBandwidth* getBandwidth ()
        {
            return bandwidth;
        }

        ///
        list < SdpTime > getSdpTimeList() const
        {
            return sdpTimeList;
        }

	void clearSdpTimeList();

        ///
        void addTime (const SdpTime& time);

        list < SdpZoneAdjustment > getZoneAdjustmentList() const
        {
            return zoneAdjustmentList;
        }
        ///
        void addZoneAdjustment (SdpZoneAdjustment& adjustment)
        {
            zoneAdjustmentList.push_back(adjustment);
        }

	    ///
        void setEncryptkey (const SdpEncryptkey& conn);
        ///
        SdpEncryptkey* getEncryptkey();

        ///
        SdpAttributes* getAttribute() const
        {
            return attribute;
        }
        ///
        void setAttribute (SdpAttributes* attrib)
        {
            attribute = attrib;
        }

        ///
        void addMedia (SdpMedia* media)
        {
            mediaList.push_back(media);
        }
        ///
        list < SdpMedia* > getMediaList() const
        {
            return mediaList;
        }

        /// Encode object into text Data
        Data encode();

        /// Reset all member variables so the object can be re-used
        void reset();

        /** Decode text Data into object
         *  return true, if able to decode successfully.
         */
        bool decode(Data buffer);

        /** Decode list of params into object
         *  return true, if able to decode successfully.
         */
        bool decode(list <Data>& parms);

        /// assignment operator
        SdpSession& operator= (const SdpSession& x);

#if 0
        /// equality operator -- not implemented
        bool operator == (const SdpSession& x) const;
#endif

        /// Clear all data in media list
        void flushMediaList();

        /**Implements INVITE(HOLD)
         * set the line "c="'s <connection address> to "0.0.0.0"
         */
        void setHold(); 	
        bool isHold();

    private:
        ///
        void encodeVersion (ostrstream& s);
        ///
        Data networkTypeString();
        ///
        Data addressTypeString();
        ///
        void encodeOrigin (ostrstream& s);
        ///
        void encodeSessionName (ostrstream& s);
        ///
        void encodeSessionInformation (ostrstream& s);
        ///
        void encodeURI (ostrstream& s);
        ///
        void encodeEmailAddress (ostrstream& s);
        ///
        void encodePhoneNumber (ostrstream& s);
        ///
        void encodeTime (ostrstream& s);
        ///
        void encodeTimeZoneAdjustment (ostrstream& s);
        ///
        void encodeMedia (ostrstream& s);

        /// Set this SDP according to the PacketCable NCS requirements
        void setNcs ();
        /// Check if this SDP has all the mandatory fields
        bool conformToSdp ();
        /// Check if this SDP conforms to the PacketCable NCS spec
        bool conformToNcs ();


    private:

        /// ostrstream buffer during encoding
        char buf[4096];
        /// set to true if this session descriptor is meant to be valid
        bool isValid;
        /** SDP Protocol Type indicates this SDP descriptor must conform to a
         *  specific protocol.
         *  Default is SDP which means no additional requirements.
         *  For example, NCS means this SDP conforms to Packet Cable NCS protocol
         */
        SdpProtocolType protocolType;


        /// Session Description data

        /// Protocol Version in "v=" line. Default is 0
        int protocolVersion;

        /** Origin
         *  o=<username> <session id> <version> <network type> <address type> <address>
         */
        /// <username> in "o=" line. Default is "-"
        Data username;
        /// <session id> in "o=" line. TODO: Use NTP time
        unsigned int sessionId;
        /// <version> in "o=" line. TODO: Use NTP time
        unsigned int version;
        /// <network type> in "o=" line. Default is IN
        NetworkType networkType;
        /// <address type> in "o=" line. Default is IP4
        AddressType addressType;
        /// <address> in "o=" line
        Data address;  // contains the address in the o field

        /// Session Name
        /// <session name> in "s=" line. Default is "-"
        Data sessionName;

        /// Session and Media Information - optional
        Data sessionInfo;

        //URI for more information "u=*" - optional
        Data uriInfo;

        //"e=*" email address list - optional
        list < Data > emailList;

        //"p=*" phone list - optional
        list < Data > phoneList;

        /// Connection Data
        /// Session level connection information("c=") - not required if include in all media
        SdpConnection* connection;

        /// Bandwidth
        /// Session level bandwidth information("b=") - optional
        SdpBandwidth* bandwidth;

        /// Times, Repeat Times and Time Zones
        list < SdpTime > sdpTimeList;
        //"t="
        /// Repeat times("r=") - optional, done in SdpTime
        ///"z=*" - optional
        list < SdpZoneAdjustment > zoneAdjustmentList;

        /// Encryption Key
        /// Session level encryption ket("k=") lines - optional
        SdpEncryptkey* encryptkey;

        /// Attributes
        /// Session level attribute("a=") lines - optional
        SdpAttributes* attribute;

        /// Media Announcements
        /// List of Media descriptions
        list < SdpMedia* > mediaList;
};

typedef SdpSession SessionDescriptor;

}

}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


#endif

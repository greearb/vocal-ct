#ifndef SDP2MEDIA_HXX_
#define SDP2MEDIA_HXX_

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

static const char* const Sdp2Media_hxx_Version =
    "$Id: Sdp2Media.hxx,v 1.1 2004/05/01 04:15:24 greear Exp $";

#include <strstream>
#include <cassert>
#include <list>
#include "Sdp2Externs.hxx"
#include "Sdp2Connection.hxx"
#include "Sdp2Bandwidth.hxx"
#include "Sdp2Attributes.hxx"
#include "Sdp2Encryptkey.hxx"

#include "cpLog.h"
#include "support.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{


/** Infrastructure in VOCAL related to logging.<br><br>
 */
namespace SDP
{


///
enum SdpMediaType
{
    MediaTypeUnknown,
    MediaTypeAudio,
    MediaTypeVideo
};


/** 
    SdpMedia describes the media lines in an SDP.
    <p>
    <b>Example</b>
    <pre>
    m=&lt;media> &lt;port> &lt;transport> &lt;fmt list> 
    </pre>
    In addition to the m= lines, this class holds the entire media block, 
    i.e. not just the "m=" line but all lines followed until next "m=" line.
    <p>

    A complete example of usage is available in the SdpSession class
    description.<p>

    @see SdpSession

  */
class SdpMedia
{

    public:
        ///
        SdpMedia();
        ///
        SdpMedia(list<Data>& mediaDetails);

        ///
        SdpMediaType getMediaType()
        {
            return mediaType;
        }
        ///
        Data getMediaTypeString()
        {
            return mediaTypeString;
        }
        ///
        void setMediaType( SdpMediaType newType );
        ///
        void setMediaType( Data newType );
        ///
        int getPort()
        {
            return port;
        }
        ///
        void setPort(int newport)
        {
            port = newport;
        }
        ///
        int getNumPorts()
        {
            return numPorts;
        }
        ///
        void setNumPorts(int newnumPorts)
        {
            numPorts = newnumPorts;
        }
        ///
        TransportType getTransportType()
        {
            return transportType;
        }
        ///
        void setTransportType(TransportType newtransport);
        ///

        void addFormat(int fmt)
        {
            formatList.push_back(fmt);
        }

        ///
        vector < int > * getFormatList() {
            if (formatList.size() > 0)
                return &formatList;
            else
                return NULL;
        }
        ///
        void clearFormatList()
        {
            formatList.clear();
        }
        ///
        MediaAttributes* getMediaAttributes()
        {
            return mediaAttrib;
        }

        // payload_type would be like the 97 in:  a=rtmap:97 speex/8000
        // Returns >= 0 on success, < 0 on failure.
        int getRtpmapAttributeValue(int payload_type, SdpRtpMapAttribute& rv);

        ///
        void setMediaAttributes(MediaAttributes* attributes)
        {
            mediaAttrib = attributes;
        };
        ///
        SdpConnection* getConnection()
        {
            return connection;
        }
        ///
        void setConnection(const SdpConnection& conn) 
        { 
            if (!connection)
            {
                connection = new SdpConnection;
            }
            *connection = conn;
        }
        ///
        SdpBandwidth* getBandwidth()
        {
            return bandwidth;
        }

        ///
        void setBandwidth(SdpBandwidth& bWidth)
        {
            delete bandwidth;
            Data aVal = bWidth.modifierString();
            aVal += ":";
            aVal += Data(bWidth.getValue());
            bandwidth = new SdpBandwidth(aVal);
        }
        ///
        SdpEncryptkey* getEncryptkey()
        {
            return encryptkey;
        }
        ///
        Data getMediaInfo()
        {
            return mediaInfo;
        }
        ///
        void setMediaInfo(const Data& info)
        {
            mediaInfo = info;
        }

        ///
        const SdpMedia& operator=(SdpMedia& media);

        ~SdpMedia();

        ///
        void encode (ostrstream& s);

    private:

	/// disable copy constructor
	SdpMedia(const SdpMedia&);
        ///
        Data transportTypeString();
        ///
        void encodeMediaAnnouncement (ostrstream& s);

        SdpMediaType mediaType;
        Data mediaTypeString;
        int port;
        int numPorts;
        TransportType transportType;     // RTP/AVP or UDP

        // Format lists must always be integers when in the SDP, so no reason to
        // let them be anything else here. --Ben
        vector < int > formatList;

        //maintain the MediaAttribute.
        MediaAttributes* mediaAttrib;

        //maintain the connection address in the c line.
        SdpConnection* connection;

        //maintain the bandwidth
        SdpBandwidth* bandwidth;

        //maintain the encryption key
        SdpEncryptkey* encryptkey;

        //maintain the media information
        Data mediaInfo;

        ///
        static const int MaxFormatIntValue;
};


}


}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */


#endif

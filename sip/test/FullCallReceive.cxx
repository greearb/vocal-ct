
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

static const char* const FullCallReceive_cxx_Version =
    "$Id: FullCallReceive.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";
#include "SipTransceiverFilter.hxx"
#include "InviteMsg.hxx"
#include "ByeMsg.hxx"
#include "AckMsg.hxx"
#include "StatusMsg.hxx"
#include "SipSdp.hxx"
#include "Sptr.hxx"

using namespace Vocal;
using namespace Vocal::SDP;

int main(int argc, char** argv)
{
    if (argc > 1)
    {
        cpLogSetPriority(LOG_DEBUG_STACK);
    }

    SipTransceiverFilter trans(argv[0], 5070);

    Sptr < SipMsgQueue > m;
    Sptr < StatusMsg > status;

    m = trans.receive();

    if (m == 0)
    {
        assert(0);
    }

    Sptr < InviteMsg > invite;
    invite.dynamicCast(m->back());

    if (invite == 0)
    {
        cout << "not an invite!" << endl;
        return -1;
    }

    // respond with the 100, then the 180


    StatusMsg trying(*invite, 100);

    cout << "sending 100" << endl;

    trans.sendReply(trying);


    StatusMsg ringing(*invite, 180);

    cout << "sending 180" << endl;

    trans.sendReply(ringing);


    sleep(1);

    StatusMsg inviteOk(*invite, 200);

    Sptr <SipUrl> myUrl = new SipUrl();
    myUrl->setUserValue( Data("bob") );
    myUrl->setHost( Data("127.0.0.1") );
    myUrl->setPort( Data(5070) );
    SipContact me;
    me.setUrl( myUrl );
    inviteOk.setNumContact( 0 );    // Clear
    inviteOk.setContact( me );

    // Create local SDP base on remote SDP

    Sptr<SipContentData> contData = inviteOk.getContentData(0);
    Sptr<SipSdp>  localSdp;
    
    localSdp.dynamicCast(contData);

//    SipSdp* localSdp = dynamic_cast < SipSdp* > ( inviteOk.getContentData(0) );
    if ( localSdp.getPtr() != 0 )
    {
        localSdp->setRtpPort( 4356 );

        int rtpPacketSize = 20;

        SdpSession sdpDesc = localSdp->getSdpDescriptor();
        list < SdpMedia* > mediaList;
        mediaList = sdpDesc.getMediaList();
        list < SdpMedia* > ::iterator mediaIterator = mediaList.begin();

        vector < int > * formatList = (*mediaIterator)->getFormatList();
        if ( formatList != 0 )
        {
            formatList->clear();
        }
        //using the default codec
        (*mediaIterator)->addFormat( 0 );

        MediaAttributes* mediaAttrib;
        LocalScopeAllocator lo;
        mediaAttrib = (*mediaIterator)->getMediaAttributes();
        if ( mediaAttrib != 0 )
        {
            vector < ValueAttribute* > * valueAttribList = mediaAttrib->getValueAttributes();
            vector < ValueAttribute* > ::iterator attribIterator = valueAttribList->begin();
            while ( attribIterator != valueAttribList->end() )
            {
                char* attribName = (*attribIterator)->getAttribute();
                if ( strcmp( attribName, "ptime" ) == 0 )
                {
                    rtpPacketSize = Data((*attribIterator)->getValue()).convertInt();
                    break;
                }
                attribIterator++;
            }
            mediaAttrib->flushValueAttributes();
            mediaAttrib->flushrtpmap();

            ValueAttribute* attrib = new ValueAttribute();
            attrib->setAttribute( "ptime" );
            attrib->setValue( Data( rtpPacketSize ).getData(lo) );

            //add the rtpmap attribute for the default codec
            SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
            rtpMapAttrib->setPayloadType( 0 );
            rtpMapAttrib->setEncodingName( "PCMU" );
            rtpMapAttrib->setClockRate( 8000 );

            mediaAttrib->addValueAttribute( attrib );
            mediaAttrib->addmap( rtpMapAttrib );
        }
        else
        {
            mediaAttrib = new MediaAttributes();
            assert(mediaAttrib);
            (*mediaIterator)->setMediaAttributes(mediaAttrib);

            // create the new value attribute object
            ValueAttribute* attrib = new ValueAttribute();
            // set the attribute and its value
            attrib->setAttribute("ptime");
            attrib->setValue( Data( "20" ).getData(lo) );

            //add the rtpmap attribute for the default codec
            SdpRtpMapAttribute* rtpMapAttrib = new SdpRtpMapAttribute();
            rtpMapAttrib->setPayloadType(0);
            rtpMapAttrib->setEncodingName("PCMU");
            rtpMapAttrib->setClockRate(8000);

            // add the value attribute just created to the media attribute object
            mediaAttrib->addValueAttribute(attrib);
            mediaAttrib->addmap(rtpMapAttrib);
        }
        localSdp->setSdpDescriptor(sdpDesc);
    }

    cout << "sending 200" << endl;

    trans.sendReply(inviteOk);

    cout << "sent 200" << endl;

    // get an ack

    m = trans.receive();

    assert(m != 0);



    if (m->back()->getType() != SIP_ACK)
    {
        cout << "not an ack!" << endl;
        return -1;
    }

    cout << "got ack!" << endl;

    m = trans.receive();

    if (m->back()->getType() != SIP_BYE)
    {
        cout << "not a bye!" << endl;
        return -1;
    }

    cout << "got bye!" << endl;

    Sptr < ByeMsg > bye;
    bye.dynamicCast(m->back());

    StatusMsg byeOk(*bye, 200);

    trans.sendReply(byeOk);




    cout << "call succeeded!" << endl;

    return 0;
}

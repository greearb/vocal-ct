/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted providd that the following conditions
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

static const char* const SipContentDataContainer_cxx_version =
    "$Id: SipContentDataContainer.cxx,v 1.1 2004/05/01 04:15:26 greear Exp $";

#include "global.h"
#include "SipContentDataContainer.hxx"
#include "SipOsp.hxx"
#include "SipIsup.hxx"
#include "SipSdp.hxx"
#include "SipHeader.hxx"
#include "SipUnknownContentData.hxx"
#include "SipContentData.hxx"

using namespace Vocal;

SipContentDataContainer::SipContentDataContainer(const string& _local_ip)
        : contentType(new SipContentType("", _local_ip, 0)),
	  contentLength(new SipContentLength("", _local_ip)),
	  contentDisposition(new SipContentDisposition("", _local_ip)),
	  mimeDataList(),
	  mimeData(),
	  isParsed(false),
	  actualBoundary(),
          local_ip(_local_ip)
{
    assert(mimeDataList.begin() == mimeDataList.end());
}


SipContentDataContainer::~SipContentDataContainer()
{
}


SipContentDataContainer::SipContentDataContainer( const SipContentDataContainer& rhs )
{
    copyRhs2This( rhs );
}


SipContentDataContainer&
SipContentDataContainer::operator=( const SipContentDataContainer& rhs )
{
    if ( &rhs != this )
    {
        copyRhs2This( rhs );
    }
    return *this;
}


void
SipContentDataContainer::copyRhs2This( const SipContentDataContainer& rhs )
{
    if( rhs.contentType != 0 )
    {
        contentType = new SipContentType( *rhs.contentType );
    }
    if( rhs.contentLength != 0 )
    {
        contentLength = new SipContentLength( *rhs.contentLength );
    }
    if( rhs.contentDisposition != 0 )
    {
        contentDisposition = new SipContentDisposition( *rhs.contentDisposition );
    }

    local_ip = rhs.local_ip;

    cpLog( LOG_DEBUG_STACK, "rhs mimeDataList size is %d",
                            rhs.mimeDataList.size() );
    mimeDataList.clear();
    for( vector< Sptr <SipContentData> >::const_iterator i = rhs.mimeDataList.begin();
         i != rhs.mimeDataList.end();
         ++i )
    {
        //TODO?: assert( *i != 0 ); and remove else part
        if( 0 != i->getPtr() )
        {
            mimeDataList.push_back( (*i)->duplicate() );
        }
        else
        {
            cpLog( LOG_ERR, "Null entry in mimeDataList" );
            // mimeDataList.push_back( 0 );
        }
    }

    mimeData = rhs.mimeData;
    isParsed = rhs.isParsed;
    actualBoundary = rhs.actualBoundary;
}


void
SipContentDataContainer::encode(Data* data) const
{
    
    Data tempData;
    
    if ( (isParsed) || (mimeDataList.size() > 0) )
    {
	Data parsedData;

	// call encode of SipContentData list
	vector< Sptr <SipContentData> >::const_iterator iter; 
    
	int len = 0;
    
	int numOfContentData = mimeDataList.size();
	if (numOfContentData > 1)
	{
	    iter = mimeDataList.begin();
 
	    int singleMimeLen;
    
	    while (iter != mimeDataList.end())
	    {
		singleMimeLen = 0;
    
		if  ((*iter) != 0)
		{
		    //add the boundary.
		    parsedData+= "--";
                    parsedData+= actualBoundary;
		    parsedData+= CRLF;
		    //this is a SipContentData item. call encode functions of SipContentData.
		    parsedData+= (*iter)->encodeHeaders();
		    parsedData+= CRLF;
		    parsedData+= (*iter)->encodeBody(singleMimeLen);
    
		    len+= singleMimeLen;
		    
		}
		++iter;
                                            
	    }
	    if (mimeDataList.size() > 1)
	    {
		//end with the boundary.
		
		parsedData+= "--";
		parsedData+= actualBoundary;
		parsedData+= CRLF;
	    }
	}
	else
	{
	    if(!mimeDataList.empty() && (mimeDataList[0] != 0))
	    {
		parsedData+= mimeDataList[0]->encodeBody(len);
	    }
	}
	tempData = parsedData;
	assert(contentLength != 0);

	//this needs to be called by the SipMsg, while encoding content length.
	contentLength->setLength(len); 
    }
    else
    {
	tempData = mimeData;
    }
    if (tempData.length())
    {
	//there is mime defined here.
	(*data) += CRLF;
	(*data) += tempData;
    }
    else
    {
	(*data) += CRLF;
    }
}
   
    
void
SipContentDataContainer::decode(const Data& data, 
				Sptr <SipContentLength> contLength, 
				Sptr <SipContentType> contType, 
				Sptr <SipContentDisposition> contDisp)
{
    contentLength = contLength;
    contentType = contType;
    contentDisposition = contDisp;
    
    //maintain string here.
    mimeData = data;
}
    
    
bool
SipContentDataContainer::parse() 
{
    bool success = false;
    //construct the SipContentData objects here.
    // xxx fixme
    if (contentType != 0)
    {
        // this is a multipart MIME (RFC 2046)
	if(contentType->getType() == "multipart") 
	{
	    //this is a multi-MIME.
	    Data boundary = contentType->getAttributeValue("boundary");
	    if (boundary.length())
	    {
		actualBoundary = "--";
		actualBoundary += boundary;

		success = parseMultiMime(actualBoundary);
	    }
	}
	else
	{
	    success = parseSingleMime(mimeData, 
                                      contentLength, 
                                      contentType, 
                                      contentDisposition);
	}
    }

    if(!success)
    {
	cpLog(LOG_DEBUG, "failed to parse MIME data when asked to");
    }

    isParsed = true;
    
    return success;
}
    
    
bool
SipContentDataContainer::parseSingleMime(Data singleMimeData, 
					 Sptr<SipContentLength> len, 
					 Sptr<SipContentType> type, 
					 Sptr<SipContentDisposition> disp)
{
    cpLog( LOG_DEBUG_STACK, "parseSingleMime %s", singleMimeData.logData() );
    Data subType;
    //get contentType. Defaults to text/plain.
    if (type != 0)
    {
	subType = type->getSubType();
    }
    else
    {
	subType = "plain";
    }
    Sptr <SipContentData> contentData;
    
    //parse singleMimeData, and add into ContentData object.
    if ( isEqualNoCase(subType, "SDP") )
    {
	contentData  = new SipSdp(singleMimeData, getLocalIp());
    }
    else if (isEqualNoCase(subType, "ISUP") )
    {
	//this is ISUP
	contentData = new SipIsup(singleMimeData, getLocalIp());
    }
    else if (isEqualNoCase(subType, "OSP") )
    {
	//this is OSP token
	contentData = new SipOsp(singleMimeData, getLocalIp());
    }
    else if (singleMimeData.length() > 0) // if length is 0, ignore
    {
        contentData = new SipUnknownContentData(singleMimeData, getLocalIp());
    }

    if(contentData != 0)
    {
	if (len != 0)
	{
	    contentData->setContentLength(len);
	}
	if (type != 0)
	{
	    contentData->setContentType(type);
	}
	if (disp != 0)
	{
	    contentData->setContentDisposition(disp);
	}
	//set this into the map.

	mimeDataList.push_back(contentData);
    }
    else
    {
	// xxx this is not an error if there is a null-bodied object
	// passed here ?
    }
    
    return true;  //need to find out error condition here.
}
    
    
bool
SipContentDataContainer::parseMultiMime(Data boundary)
{
    bool success = true;
    
    bool matchFail = true;
    Data mimeBoundary = mimeData.getLine(&matchFail);
    if ( (mimeBoundary.operator!=(boundary)) || ( matchFail ) )
    {
	success = false;
	return success;
    }
    else  //continue
    {
	while (mimeData.length())
        {
	    //get the string between this and the next boundary line.
	    Data singleMimeData;
            
	    //form the line that we need to search for.
	    Data boundaryLine = boundary;
	    boundaryLine+= CRLF;
            LocalScopeAllocator lo; 
	    int found = mimeData.match(boundaryLine.getData(lo), &singleMimeData, true);
    
	    if ( (found == NOT_FOUND) || (found == FIRST) )
	    {
                // this is the epilogue boundary, which is mandatory
		success = false; 
		break;  //break out of while loop.
	    }
	    // else continue
	    {
	        //get headers if any in the MIME section
		bool matchFailHeaders = true;
    
		Data nextLine = singleMimeData.getLine(&matchFailHeaders);
    
		Sptr <SipContentType> singleMimeContentType;
		Sptr <SipContentLength> singleMimeContentLength;
		Sptr <SipContentDisposition> singleMimeContentDisposition;
		bool dummyFlag;
    
		while  ( (nextLine.length()) && (!matchFailHeaders) )
		{
		    
		    Data headerValue = nextLine;
		    // decode this line of the message
		    Data headerName = headerValue.parse(": \t", &dummyFlag);
		    //we dont check for match failing, since these fields are not mandatory here.
		    headerName.lowercase();
		    
		    SipHeaderType  headerType = headerTypeDecode(headerName);

		    switch(headerType)
		    {
			case SIP_CONTENT_TYPE_HDR:
			{
			    singleMimeContentType 
                                = new SipContentType(headerValue, getLocalIp(), 0);
			}
			break;
			case SIP_CONTENT_LENGTH_HDR:
			{
			    singleMimeContentLength 
                                = new SipContentLength(headerValue, getLocalIp());
			}
			break;
			case SIP_CONTENT_DISPOSITION_HDR:
			{
			    singleMimeContentDisposition 
                                = new SipContentDisposition(headerValue, getLocalIp());
			}
			break;
			default:
			{
			}
			break;
		    } //end switch
		    //get the next line.
                    nextLine = singleMimeData.getLine(&matchFailHeaders);
		} //end while headers found
		//CRLF found now.
		//the remaining is content data.
		success 
                    = success && parseSingleMime(singleMimeData, 
                                                 singleMimeContentLength, 
                                                 singleMimeContentType, 
                                                 singleMimeContentDisposition);
	    }//else
	}// while mimeData.length()
	//parse based on boundary,
        //if you get contentlength, type, or disposition,
        //set in SipCOntentData.
    }//else found boundary.
    return success;
}
   

int
SipContentDataContainer::getNumContentData() 
{
    if (!isParsed)
    {
        //parse here
	parse();
    }
    return static_cast<int>(mimeDataList.size());
}


void
SipContentDataContainer::setNumContentData(int index)
{
    // precondition
    assert(index >= 0);

    if (!isParsed)
    {
        parse();
    }

    while(index < static_cast<int>(mimeDataList.size()) )
    {
        // reduce the number by trimming from the end
        mimeDataList.pop_back();
    }

    // postcondition
    assert(index >= static_cast<int>(mimeDataList.size()) );
}


Sptr <SipContentData>
SipContentDataContainer::getContentData(int index)
{
    if (!isParsed)
    {
        //parse here
	parse();
    }
    
    //return from list
    if (index >=  static_cast<int>(mimeDataList.size() ) || 
        (mimeDataList.size() == 0))
    {
	return 0;
    }
    else if (index < 0)
    {
	return mimeDataList[mimeDataList.size()-1];
    }
    else
    {
	return mimeDataList[index];
    }
}
    

// xxx there are two known bugs in this function:

// 1.  from an API perspective, this should have more consistent
// behavior if you pass an index that's out of range.  Either 

//     (a) you need to know what the range is, and you should assert
//     if you try to set something out of range

//     (b) you can use a big index, and it will either expand w/ null
//     items or append

// either is better than the current behavior (silently fail, unless
// the index is 0, in which case insert).

// 2.  the boundaries are not uniquely generated.  they need to be to
// work correctly.



void
SipContentDataContainer::setContentData(Sptr<SipContentData> contData, 
					int index)
{
    assert( contData != 0 );
    cpLog( LOG_DEBUG_STACK, "setContentData %d: %s",
                            index,
                            contData->encodeHeaders().logData() );
    int bodyLength;
    cpLog( LOG_DEBUG_STACK, "\n%s",
                            contData->encodeBody(bodyLength).logData() );

    cpLog( LOG_DEBUG_STACK, "mimeDataList size before parse is %d", mimeDataList.size() );

    //if the existing mimeData is not parsed yet, parse now.
    if  (!isParsed)
    {
	parse();
    }
    //set at index.
    int size = mimeDataList.size();
    
    cpLog( LOG_DEBUG_STACK, "mimeDataList size after parse is %d", size );
    if ( (index >= -1) && (index <= size) )
    {
        if (index == -1)
        {
            mimeDataList.push_back(contData);
        }
        else if ( index <= size )
        {
            vector< Sptr <SipContentData> >::iterator iter;
	    
            iter = mimeDataList.begin();
	    
            iter+=index;
            //insert it at index.
            mimeDataList.insert(iter, contData);      
	}
    }
    if ( (mimeDataList.size() == 2) && ( !actualBoundary.length()) ) //more than one item in mime list
    {
	//give a boundary if none exists before.
	actualBoundary = "unique-boundary-1"; // xxx need to generate this some how.
    }
}
    
    
void
SipContentDataContainer::removeContentData(int index)
{
    if (!isParsed)
    {
	parse();
    }
    if (index == -1)
    {
	mimeDataList.pop_back();
    }
    else if ( index <= static_cast<int>(mimeDataList.size())  && (index > -1) )
    {
	vector<Sptr <SipContentData> >::iterator iter;
 
        iter = mimeDataList.begin();
        iter+=index;
        if(iter != mimeDataList.end())
        {
            mimeDataList.erase(iter);
        }
    }

    if (mimeDataList.size() == 0)
    {          
	mimeDataList.clear();
    }

    if (mimeDataList.size() ==1)
    {
	//set boundary to ""
	actualBoundary = "";
    }
}         

    
Sptr <SipContentType>
SipContentDataContainer::getContentType()
{
    if (isParsed)
    {
        if(actualBoundary.length())
        {
            contentType->setTokenDetails("boundary", actualBoundary);
        }
    
	if ( (actualBoundary.length()) && 
	     (contentType->getType() != "multipart"))
	{
	    //set the content type.
	    contentType->setType("multipart");
	    contentType->setSubType("mixed");
	   
	}
#if 0
	else if ( (!actualBoundary.length()) &&
		  (contentType->getType() != "multipart"))
#endif
	else if ( (!actualBoundary.length()) )
	{
	    //get the type of object.
            if(mimeDataList.empty())
            {
                // there is nothing here -- so there is no contenttype
                contentType->setType("");
                contentType->setSubType("");
            }
            else
            {
                contentType->setType(mimeDataList[0]->getContentType()->getType());
                contentType->setSubType(mimeDataList[0]->getContentType()->getSubType());
            }
            contentType->clearToken("boundary");
	}
    }
    return contentType;
}

    
void SipContentDataContainer::setContentType(Sptr <SipContentType> content)
{
    contentType = content;
}

    
Sptr <SipContentLength> SipContentDataContainer::getContentLength()
{
    return contentLength;
}

    
void SipContentDataContainer::setContentLength(Sptr <SipContentLength>  content)
{
    contentLength = content;
}

    
void 
SipContentDataContainer::setContentDisposition(Sptr <SipContentDisposition> disp)
{
    contentDisposition = disp;
}
    
    
Sptr <SipContentDisposition> 
SipContentDataContainer::getContentDisposition()
{
    return contentDisposition;
}
    
bool
SipContentDataContainer::operator ==(const SipContentDataContainer& src)
{
    if (  (!src.isParsed) && (!isParsed) )
    {
	return (src.mimeData == mimeData);
    }
    else if ( ( src.isParsed ) && (!isParsed) )
    {
	//encode src
	Data srcEncodedBody;
	src.encode(&srcEncodedBody);
    
	//should we maintain a flag again, to say that the mimeData is 
	//here

	return (srcEncodedBody == mimeData);
    }
    else if ( (!src.isParsed) && (isParsed) )
    {
	//encode this.
	Data thisEncodedBody;
	encode(&thisEncodedBody);
    
	return (src.mimeData == thisEncodedBody);
    }
    else //both are parsed, run through list and compare.
    {
        SipContentDataContainer cData(src);
	int srcContentDatasize = cData.getNumContentData();
 
	if ( srcContentDatasize != getNumContentData())
	{
	    return false;
	}
	else
	{
	    //encode both and compare.
	    //optionally, we could also go over each in the list and
	    //call the == operator on that.
    
	    //encode this.
	    Data thisEncodedBody;
	    encode(&thisEncodedBody);
    
	    //encode src
	    Data srcEncodedBody;
	    src.encode(&srcEncodedBody);
    
	    return (thisEncodedBody == srcEncodedBody);
	}
    }         
}


void SipContentDataContainer::forceParse()
{
    parse();
}

/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#ifndef SIPMSG_HXX_
#define SIPMSG_HXX_

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

static const char* const SipMsgVersion 
= "$Id: SipMsg.hxx,v 1.2 2004/05/04 07:31:15 greear Exp $";

#include "Data.hxx"
#include "NetworkAddress.h"
#include "SipCallLeg.hxx"
#include "SipContentDataContainer.hxx"
#include "SipHeaderList.hxx"
#include "SipRawHeaderContainer.hxx"
#include "symbols.hxx"
#include "VException.hxx"


#if USE_HASH_MAP 
#if defined(__linux__)
#include <hash_map>
#else
#undef USE_HASH_MAP
#endif
#endif

namespace Vocal
{

class SipAccept;
class SipAcceptEncoding;
class SipAcceptLanguage;
class SipAllow;
class SipAlso;
class SipCallId;
class SipCSeq;
class SipContact;
class SipContentData;
class SipContentLanguage;
class SipContentLength;
class SipDate;
class SipDiversion;
class SipEncryption;
class SipExpires;
class SipFrom;
class SipMimeVersion;
class SipRecordRoute;
class SipReferTo;
class SipReferredBy;
class SipReplaces;
class SipRetryAfter;
class SipSubsNotifyEvent;
class SipSupported;
class SipTimestamp;
class SipTo;
class SipTransferTo;
class SipUserAgent;
class SipVia;

class SipParserException : public VException
{
    public:
	///
	SipParserException( const string& msg,
			    const string& file,
			    const int line,
			    const int error = 0 );
    
	string getName( void ) const;
};


typedef SipHeaderList <SipRecordRoute> SipRecordRouteList;
typedef SipHeaderList <SipVia> SipViaList;
typedef SipHeaderList <SipContact> SipContactList;
typedef SipHeaderList <SipDiversion> SipDiversionList;

/**
    This class is the base class for all SIP Messages. It is just a container
    class of all SIP Headers, common to SIP Requests and SIP responses. 
*/
class SipMsg: public BugCatcher
{
    public:

	///SipMsg constructor
        // local_ip cannot be "" here, must be the local IP we are bound to locally
        // or 'hostaddress' if we are not specifically bound.
	SipMsg(const string& local_ip, const char* class_name);

	///SipMsg copy constructor
	SipMsg(const SipMsg&);
    
	///SipMsg destructor
	virtual ~SipMsg();

        const string& getClassName() const { return className; }
        virtual bool isSipCommand() const = 0;
        virtual bool isStatusMsg() const = 0;

	///	
	SipMsg& operator=(const SipMsg& newSipMsg);
	///
	bool operator<(const SipMsg& src) const;
	///
	bool operator ==(const SipMsg& src) const;
	///
	//returns the type of the object in the sub classes.
	virtual Method getType() const = 0;

        /// copy an arbitrary header from the src message
        void copyHeader(const SipMsg& src, SipHeaderType type);

        /// checks to see if an arbitrary header exists in the message
        bool containsHeader(SipHeaderType type);

	/// get the number of Accept items 
	int getNumAccept() const;

	/// Get the i'th Accept item. If i is -1, it gets the last one 
	const SipAccept& getAccept( int i=-1) const;

	/** set or add another Accept item, if the index is -1, it is
	 * appended to the current list */
	void setAccept(const SipAccept& item, int index=-1);
    
	/** set or add another Accept item, if the index is -1, it is
	 * appended to the current list */
	void setAccept(const Data&item, int index=-1);

	/** Set number of Accept items.  If i is less than current
	    number then the extras are deleted. */
	void setNumAccept(int i);

	///returns the encoded accept list.
	Data encodeAcceptList() const;

	///compare the Accept objects in this SipMsg, to the src SipMsg object.
	bool compareAccept(const SipMsg& src) const; 

	///copies the accept list from srcObject to this object.
	void copyAcceptList(const SipMsg& srcObject);   

	///
	void flushAcceptList();

	///
	void addAccept(SipAccept*);
	///
	const vector<SipAccept*>& getAcceptList() const;

	/// get the number of AcceptEncoding items 
	int getNumAcceptEncoding() const;

	/// Get the i'th AcceptEncoding item. If i is -1, it gets the last one 
	const SipAcceptEncoding& getAcceptEncoding( int i=-1) const;

	/** set or add another AcceptEncoding item, if the index is -1,
	 * it is appended to the current list. */
	void setAcceptEncoding(const SipAcceptEncoding& item, int index=-1);
    
	/** set or add another AcceptEncoding item, if the index is
	 * -1, it is appended to the current list. */
	void setAcceptEncoding(const Data& item, int index=-1);

	/** Set number of AcceptEncoding items.If i is less than
	 * current number then the extras are deleted. */
	void setNumAcceptEncoding(int i);

	///return the entire acceptencoding list.
	const vector<SipAcceptEncoding*>& getAcceptEncodingList() const;

	///add another item into the acceptEncoding list.
	void addAcceptEncoding(SipAcceptEncoding* acceptEncoding);

	///returns the encoded acceptEncoding list.
	Data encodeAcceptEncodingList() const;
     
	///copies the acceptencoding list from srcObject to this object.
	void copyAcceptEncodingList(const SipMsg& srcObject);   

	///compare accept encoding items of this object to the src object
	bool compareAcceptEncoding(const SipMsg& src) const;    

	///
	void flushAcceptEncodingList();

	/// get the number of AcceptLanguage items 
	int getNumAcceptLanguage() const;
	/// Get the i'th AcceptLanguage item. If i is -1, it gets the last one 
	const SipAcceptLanguage& getAcceptLanguage( int i=-1) const;

	/** set or add another AcceptLanguage item, if the index is
	 * -1, it is appended to the current list */
	void setAcceptLanguage(const SipAcceptLanguage& item, int index=-1);
    
	/** set or add another AcceptLanguage item, if the index is
	 * -1, it is appended to the current list */
	void setAcceptLanguage(const Data& item, int index=-1);

	/** Set number of AcceptLanguage items. If i is less than
	 * current number then the extras are deleted. */
	void setNumAcceptLanguage(int i);
      
	/// returns the encoded accept language list.
	Data encodeAcceptLangList() const;
     
	/// copies the acceptlanguage list from srcObject to this object.
	void copyAcceptLangList(const SipMsg& srcObject);

	/// returns the entire acceptlanguage list.
	const vector<SipAcceptLanguage*>& getAcceptLangList() const;

	/// allows addition of another acceptlanguage object.
	void addAcceptLang(SipAcceptLanguage* sipacceptlang);

	/// removes all acceptlanguage objects from the list.
	void  flushAcceptLanguageList();

	/// compare acceptlanguage objects of this object to the src object
	bool compareAcceptLanguage(const SipMsg& src) const;   

	/// Get the current CSeq header 
	const SipCSeq& getCSeq() const;
	/// Set the CSeq header 
	void setCSeq( const SipCSeq& );
	///
	void setCSeq( const Data& );

        /// increments the cseq, if none there, 
        void incrementCSeq();

	/// Get the current CallId header 
	const SipCallId& getCallId() const;
	/// Set the CallId header 
	void setCallId( const SipCallId& );
	///
	void setCallId( const Data& );

	// ----------------- CallLeg Header Methods ------------------
	/// Get the current CallLeg header 
	SipCallLeg computeCallLeg() const;


	/**@name Contact Header Methods 
          */
        //@{

	/// get the number of Contact items 
	int getNumContact() const;

	/// Get the i'th Contact item. If i is -1, it gets the last one 
	const SipContact& getContact( int i=-1) const;

	/** set or add another Contact item, if the index is -1, it is
	 * appended to the current list */
	void setContact(const SipContact& item, int index=-1);

	/** set or add another Contact item, if the index is -1, it is
	 * appended to the current list */
	void setContact(const Data& item, int index=-1);

	/** Set number of Contact item. If i is less than current
	 * number then the extras are deleted. */
	void removeContact(int index = -1);
    
	/** Set number of Contact items. If i is less than current
	 * number then the extras are deleted. */
	void setNumContact(int i);

	/// remove all contact objects from the list.
	void flushContact();

	/// return the encoded contact list.
	Data encodeContact() const;

	/// copy contact object from srcObject to this object.
	void copyContact(const SipMsg& srcObject);

	/// return the entire contact list
	SipContactList getContactList() const;

	/// compare contact objects of this object to src object.
	bool compareContact(const SipMsg& src) const;   

	///return the contentlength object.
	const SipContentLength& getContentLength() const;

	///set the contentlength object as length.
	void setContentLength( const SipContentLength& length);
    
	///set the contentlength object as length.
	void setContentLength( const Data& length);

	/// get the number of ContentType items 
	const SipContentType& getContentType( ) const;

	/** set or add another ContentType item, if the index is -1,
	 * it is appended to the current list */
	void setContentType(const SipContentType& item);

	/// get the contentdisposition item
	const SipContentDisposition& getContentDisposition( ) const;

	///set ContentDisposition item
	void setContentDisposition(const SipContentDisposition& item);
    
	///set ContentDisposition item
	void setContentDisposition(const Data& item);
 
	/// get the number of ContentData items 
	int getNumContentData() const;

	/// Get the i'th ContentData item. If i is -1, it gets the last one 
	Sptr<SipContentData> getContentData( int i=-1) const;

	/** set or add another ContentData item, if the index is -1,
	 * it is appended to the current list */
	void setContentData(const SipContentData* item, int index=-1);

	/** Set number of ContentData items.If i is less than current
	 * number then the extras are deleted. */
	void setNumContentData(int i);

	/// return the encoded content data list.
	Data encodeContentDataList() const;

	///return the entire content data list.
	const vector<SipContentData*>& getContentDataList() const;

	///copy the contents of contentdata list from src to this object.
	void copyContentData(const SipMsg& src);

	///remove the content data object at location index.
	void removeContentData(int index = -1);
      
	/** compare the content data objects of this object to that
	 * contained in the src object. */
	bool compareContentData(const SipMsg& src) const;

        /** delete all of the current content datas */
        void flushContentData();
           
	/// Get the current Date header 
	const SipDate& getDate() const;

	///Set the Date header using SipDate
	void setDate( const SipDate& );
    
	/// Set the Date header using text data
	void setDate( const Data& textData);

	/// Get the current Encryption header 
	const SipEncryption& getEncryption() const;

	/// Set the Encryption header 
	void setEncryption( const SipEncryption& );
	/// Set the Encryption header using text data 
	void setEncryption( const Data& textData);
   
	/**@name SubsNotifyEvent Header Methods 
          */

	/// get the number of SubsNotifyEvent items 
	int getNumSubsNotifyEvent() const;

	/// Get the i'th SubsNotifyEvent item. If i is -1, it gets the last one 
	const SipSubsNotifyEvent& getSubsNotifyEvent( int i=-1) const;

	/** set or add another SubsNotifyEvent item, if the index is
	 * -1, it is appended to the current list */
	void setSubsNotifyEvent(const SipSubsNotifyEvent& item, int index=-1);
    
	/** set or add another SubsNotifyEvent item, if the index is
	 * -1, it is appended to the current list */
	void setSubsNotifyEvent(const Data& item, int index=-1);

	/// remove the SipSubsNotifyEvent from the list at location index.
	void removeSubsNotifyEvent(int index = -1);

	/** Set number of SubsNotifyEvent items.If i is less than
	 * current number then the extras are deleted. */
	void setNumSubsNotifyEvent(int i);

	///remove all items from SubsNotify list.
	void flushSubsNotifyEvent(); 

	///return the encoded subsnotify list.
	Data encodeSubsNotifyEvent() const;

	///copy the entire subs notify list from srcObject to this object.
	void copySubsNotifyEvent(const SipMsg& srcObject); 

	///compare subs notify objects of this object to the src object.
	bool compareSubsNotifyEvent(const SipMsg& src) const;

        //@}

	/**@name Expires Header Methods
          */
        //@{

	/// Get the current Expires header 
	const SipExpires& getExpires() const;

	/// Set the Expires header 
	void setExpires( const SipExpires& );
    
	void setExpires( const Data& );

        //@}

	/**@name From Header Methods 
          */
        //@{

	/// Get the current From header 
	const SipFrom& getFrom() const;

	/// Set the From header 
	void setFrom( const SipFrom& );

	/// Set the From header 
	void setFrom( const Data& textData);

        //@}

	/**@name Mime Version Methods
          */
        //@{

	///get the mime version
	const SipMimeVersion& getMimeVersion() const;
	/// Set the mimeversion  header 
	void setMimeVersion( const SipMimeVersion& );
	/// Set the mimeversion  header 
	void setMimeVersion( const Data& textData);

        //@}

	/**@name RecordRoute Header Methods 
          */
        //@{

	/// get the number of RecordRoute items 
	int getNumRecordRoute() const;

	/// Get the i'th RecordRoute item. If i is -1, it gets the last one 
	const SipRecordRoute& getRecordRoute( int i=-1) const;

	/** set or add another RecordRoute item, if the index is -1,
	 * it is appended to the current list */
	void setRecordRoute(const SipRecordRoute& item, int index=-1);

	/** set or add another RecordRoute item, if the index is -1,
	 * it is appended to the current list */
	void setRecordRoute(const Data&  item, int index=-1);

	/** Set number of RecordRoute items. If i is less than current
	 * number then the extras are deleted.  */
	void setNumRecordRoute(int i);     

	///copy the entire recordroute list from src, to this object.
	void copyRecordRoute(const SipMsg& src);

	///remove all record route items from the list.
	void flushrecordrouteList();

	///return the entire recordrout list.
	SipRecordRouteList getrecordrouteList() const;

	///add an item into the list.
	void addRecordRoute(SipRecordRoute* recordroute);

	///return the encoded record route list.
	Data  encodeRecordRouteList() const;

	///compare the record route objects of this object to the src object
	bool compareRecordRoute(const SipMsg& src) const;    

        /// highest in the command, refers to this element
        void recordRoutePushFront(const SipRecordRoute& item);
        ///Get the first route
        const SipRecordRoute& recordRouteFront() const;
        ///Get the last route
        const SipRecordRoute& recordRouteBack() const;
        ///Check if Record route header is empty
        bool recordRouteEmpty() const;

        //@}
        
	/**@name Supported Header Methods
          */
        //@{

	///return the number of Supported items.
	int getNumSupported() const;

	/// Get the i'th Supported item. If i is -1, it gets the last one
	const SipSupported& getSupported( int i=-1) const;

	/** set or add another Supported item, if the index is -1, it
	 * is appended to the current list */
	void setSupported(SipSupported item, int index=-1);
    
	/** set or add another Supported item, if the index is -1, it
	 * is appended to the current list */
	void setSupported(const Data&item, int index=-1);

	/** set number of Supported items.If i is less than current
	 * number then the extras are deleted. */
	void setNumSupported(int i);

	///copy all supported items from src, to this object.
	void copySupported(const SipMsg& src);

	/// remove all items from the supported list.
	void flushsupportedList();

	///return the entire supported list.
	const vector<SipSupported*>& getsupportedList() const;

	/// add another item in the supported list.
	void addSupported(SipSupported* supported);
      
	///return the encoded supported list.
	Data  encodeSupportedList() const;

	/** compare the Supported objects in this SipMsg, to the src
	 * SipMsg object. */
	bool compareSupported(const SipMsg& src) const;     

        //@}

	/**@name Content Language Header Methods
          */
        //@{

	///return the number of contentlanguage items.
	int getNumContentLanguage() const;

	/// Get the i'th ContentLanguage item. If i is -1, it gets the last one 
	const SipContentLanguage& getContentLanguage( int i=-1) const;

	/** set or add another Contentlanguage item, if the index is
	 * -1, it is appended to the current list */
	void setContentLanguage(const SipContentLanguage& item, int index=-1);
    
	/** set or add another Contentlanguage item, if the index is
	 * -1, it is appended to the current list  */
	void setContentLanguage(const Data& item, int index=-1);


	///copy the content language list from src, to this object.
	void copyContentLanguage(const SipMsg& src);

	///remove all items from content language list.
	void flushContentLanguageList();

	///return the entire content language list.
	const vector<SipContentLanguage*>& getContentLanguageList() const;

	///add another item into the content language list.
	void addContentLanguage(SipContentLanguage* ContentLanguage);

	/** Set number of ContentLanguage items.If i is less than
	 * current number then the extras are deleted.*/
	void setNumContentLanguage(int i);
	///return the encoded content language list.
	Data  encodeContentLanguageList() const;
	///
	bool compareContentLanguage(const SipMsg&) const;

        //@}

	//-----------Timestamp Header Methods ------------------
	/// Get the current Timestamp header 
	const SipTimestamp& getTimestamp() const;
	/// Set the Timestamp header 
	void setTimestamp( const SipTimestamp& );
	/// Set the Timestamp header 
	void setTimestamp( const Data& textData);
   

	/**@name To Header Methods
          */
        //@{

	/// Get the current To header 
	const SipTo& getTo() const;
	/// Set the To header 
	void setTo( const SipTo& );
	///
	void setTo( const Data& textData);

        //@}
 
	/**@name Via Header Methods
          */
        //@{

	/// get the number of Via items 
	int getNumVia() const;

	/// Get the i'th Via item. If i is -1, it gets the last one 
	const SipVia& getVia( int i=-1) const;

	///remove the Via item at location given by index.
	void removeVia(int index=-1);

	/** set or add another Via item, if the index is -1, it is
	 * appended to the current list */
	void setVia(const SipVia& item, int index=-1);

	/** set or add another Via item, if the index is -1, it is
	 * appended to the current list */
	void setVia(const Data& item, int index=-1);

	/** Set number of Via items. If i is less than current number
	 * then the extras are deleted. */
	void setNumVia(int i);

	///add another item into the via list.
	void addVia(SipVia* via);

	///copy the via list from the src to this object.
	void copyViaList(const SipMsg& src);

	///return the entire Via list.
	SipViaList getViaList() const;

	//Check for loop in Via list
	bool isLoopInViaList(const Data& myHost, int myPort, int limit) const;

	///delete the contents of the via list.
	void flushViaList();

	///return the encoded via list.
	Data encodeViaList() const;

	/** return if this object's via list objects are less than the
	 * src object's via list objects. */
	bool ltViaList(const SipMsg& src) const;

	///return true if both the objects contain identical via lists.
	bool compareViaList(const SipMsg& src) const;       
   
        /// front of the via list is the most recent one
        void viaPushFront(const SipVia& item);
        ///
        void viaPopFront();
        ///
        const SipVia& viaFront() const;
        ///
        bool viaEmpty() const;

        //@}
  
	// ----------------- Also Header Methods ------------------

	/// Get the current CallId header 
	const SipAlso& getAlso() const; 

	/// Set the CallId header 
	void setAlso( const SipAlso& ); 
    
	/// Set the CallId header 
	void setAlso( const Data& textData); 
    
	/**@name Diversion Methods
          */
        //@{
    
	///return the number of diversion objects in the list.
	int getNumDiversion() const;

	///remove the item given by location index.
	void removeDiversion(int index = -1);

	///add another item into the Diversion list.
	void addDiversion(SipDiversion* sipdiversion);

	///copy the diversion list from srcMsg to this object
	void copyDiversionList( const SipMsg& srcMsg); 

	///return the entire diversion list.
	SipDiversionList getDiversionList() const;

	///remove all items from the diversion list.
	void flushDiversionList();

	/** check if this object's diversion list objects are less
	 * than the src object's diversion list objects */
	bool ltDiversionList(const SipMsg& src) const;

	/// return the item at location given by index.
	const SipDiversion& getDiversion( int index=-1) const;

	/// set the item in location index in the diversion list.
	void setDiversion(const SipDiversion& item, int index=-1);

	/** set the number of diversion objects to be equal to
	 * index. If index is greater than the total number of objects
	 * already present, null diversion objects are introduced in
	 * between. */
	void setNumDiversion(int index);

	///return the encoded diversion list.
	Data encodeDiversionList() const;
    
	///
	bool compareDiversionList(const SipMsg& src) const;

        //@}
    
	/** this function is contained in every message sub class,
	 * such as Invite, Ack.. */
	virtual Data encode() const=0;

	/// encode the body of the message
	void encodeBody(Data* msg) const;

	/** this function is contained in SipCommand, and Status Msg
	 * to decode different headers.  data should not be a
	 * reference as it is modified in the decoding process */
	static SipMsg* decode( Data data, const string& local_ip);

	///
	void setVersion( const Data& version);
	///
	Data getVersion() const;
	///
	void setRetryAfter(const SipRetryAfter& newretry);
	///
	void setRetryAfter(const Data& newretry);
	///
	const SipRetryAfter& getRetryAfter() const;
	///
	bool isRetry() const;
	///
	const Data& getReceivedIPName() const;
	///
	void setReceivedIPName(const Data& ipName);
	/// 
	const Data& getReceivedIPPort();
	///
	void setReceivedIPPort(const Data& ipName);
    
	/**@name Transfer functions.
          */
        //@{

        ///
	const SipTransferTo& getTransferTo() const;

	///
	void setTransferTo(const SipTransferTo& transTo);
    
	///
	void setTransferTo(const Data& transTo);

        //@}

	/**@name Refer functions.
          */
        //@{

        ///
        bool hasReferredBy() const;
       
        /// 
	const SipReferredBy& getReferredBy() const;

	///
	void setReferredBy(const SipReferredBy& newreferredBy);
    
	///
	void setReferredBy(const Data& newreferredBy);
    
	///
	const SipReferTo& getReferTo() const;

	///
	void setReferTo(const SipReferTo& newreferTo);
    
	///
	void setReferTo(const Data& newreferTo);

        //@}

	///Replaces functions
	const SipReplaces& getReplaces() const;

	///
	void setReplaces(const SipReplaces& newreplaces);
    
	///
	void setReplaces(const Data& newreplaces);


	// ----------------- UserAgent Header Methods ------------------
        /// Get the current UserAgent header
        const SipUserAgent& getUserAgent() const;
        /// Set the UserAgent header
        void setUserAgent( const SipUserAgent& );
        /// 
        void setUserAgent( const Data& textData);

	/**@name Allow Header Methods
          */
        //@{

        /// get the number of Allow items
        int getNumAllow() const;

        /// Get the i'th Allow item. If i is -1, it gets the last one
        const SipAllow& getAllow( int i = -1) const;

        /** set or add another Allow item, if the index is -1, it is
	 * appended to the current list */
        void setAllow( const SipAllow& item, int index = -1);
    
        /** set or add another Allow item, if the index is -1, it is
	 * appended to the current list */
        void setAllow(const Data& item, int index = -1);

        /** Set number of Allow items.If i is less than current number
	 * then the extras are deleted. */
        void setNumAllow(int i);

        ///remove all the allow list items
        void flushAllowList();
    
        /** remove an item from allow list
            @param index the ith allow is to be removed
        */
        void removeAllow(int index );

        ///return the entire allow list
        const vector < SipAllow* > & getAllowList() const;

        ///add an item into the list.
        void addAllow(SipAllow* sipallow);

        ///copy allow from src to this object
        void copyAllowList(const SipMsg& src);

        ///return the encoded allow list
        Data encodeAllowList() const;

        ///compare the allow list of src object to this object.
        bool compareAllow(const SipMsg& src) const;     

        //@}

	//Parser functions.
	///
	static void removeSpaces(Data* data);
#if 0
	/** return the method given the first "word"
	 * (whitespace-delimited) of the message (either the method
	 * name or "SIP/2.0" for status msgs.
	 */
	static Method getMethod (const Data& method);
#endif

	/// 
	void sdpdecode(Data ghdata, Data contType=0);
	///
	Data  getMultipartBoundary(Data data); 
     
	///
	void singleMimeDecode(Data singleMimeData, Data contTypeData);  


	/** iterates through the contentData list, and gets the
	 * contentlength and adds it up to set length of the
	 * ContentLength object in SipMsg.
	 */
	void setContentLengthValue();   
    
	/** force the header / body to be parsed into objects
	 * immediately (overriding lazy parsing)
	 */
	void forceParse();

#if USE_HASH_MAP
	size_t hashfn() const;
#endif
        /**Check to see if message is a candidate for retransmission
         *Derived classes must define the behaviour
         */
        virtual bool toBeRetransmitted() const = 0;
        const SipRawHeaderContainer& getHeaderList() const { return myHeaderList; }

        ///
        bool nextHopIsAProxy() const { return myNextHopIsAProxy; };
        ///
        void setNextHopIsProxy(bool p) { myNextHopIsAProxy = p; };
        ///
        void setSendAddress(NetworkAddress &);
        ///
        const NetworkAddress& getSendAddress() const;
        ///
        ostream& brief(ostream& strm) const;
        ///
        string briefString() const;

        /// Make a copy of this and wrap it in a smart pointer
        virtual Sptr<SipMsg> clone();
        

        virtual const string& getLocalIp() const { return local_ip; }

        /** For debugging, can put this in logs, for instance */
        string toString() const;


    protected:

        string local_ip;
        /** Help ensure our up-casts are sane */
        string className;

	/// the version of SIP being used
	Data myVersion;
	/// the address the message has been received from
	NetworkAddress myReceivedAddress;
	/** the container holding all of the messages
          * the address the message has been sent to 
          */
        NetworkAddress mySendAddress;
        /// the container holding all of the messages
	SipRawHeaderContainer myHeaderList;
        /// the container holding mime list
        mutable SipContentDataContainer myMimeList;

	/** method to parse the headers and insert them into the HeaderList
            @return  returns true if there was an error
         */
	bool parseHeaders(Data& headers);
        ///
        void parseMime(Data mime);
	/** method to parse the message headers (other than the first line of
         *  the message) and the MIME bodies (if present)
         *
         *  @param msg   the message to be parsed (other than the first line)
         *
         *  @return returns true if there was an error 
        */
        bool parse(Data msg);

    private:

	bool retry;
	///
	Data version;
	/** maintained for authentication. This contains the received
	 * IP Name, set by the transceiver, and used by the Marshal
	 * for IP authentication. */
	Data recvdIPName;
	/** maintained for authentication. This contains the received
	 * IP port, set by the transceiver, and used by the Marshal
	 * for IP authentication */
	Data recvdIPPort;

	///  
	void mimeDecode( const Data& mimeData );

        ///
        bool myNextHopIsAProxy;

        SipMsg(); //Not Implemented.
};

ostream&
operator<<(ostream& s, const SipMsg &msg);


#if USE_HASH_MAP
struct hash<SipMsg>
{
	size_t operator()(const SipMsg& s) const ;
};
#endif

/// this is now a public function to copy a pointer to a smart pointer
Sptr<SipMsg> copyPtrtoSptr(SipMsg* sipMsg);  
 
} // namespace Vocal


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

#endif

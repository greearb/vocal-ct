#ifndef BUILDER_HXX
#define BUILDER_HXX

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


static const char* const Builder_hxx_Version = 
"$Id: Builder.hxx,v 1.2 2004/05/04 07:31:14 greear Exp $";


#include "Sptr.hxx"
#include "CallContainer.hxx"

namespace Vocal
{

class SipProxyEvent;
class SipTransceiver;
class Feature;

/** 
   Object Builder.
    <b> Usage of this Class </b>
    <P>
     Builder is a base class. It is passed Events by the WorkerThread
    for processing. It is a Feature container. Feature is a State container.
    State is an Operator container.

    It also contains a pointer to the proxy CallContainer.
    A builder object is used in constructing a HeartLessProxy/BasicProxy
    object.

 FooBuilder.hxx :<p>
<pre>

    class FooBuilder : public Vocal::Builder
    {
      public:
        FooBuilder();
        virtual ~FooBuilder();
        const char* const name() const;
    };
</pre>
 FooBuilder.cxx:
<pre>
    using namespace Vocal;

    FooBuilder::FooBuilder()
    {
      Sptr < FooFeature > FooFeature = new FooFeature;
      addFeature( FooFeature );
    }

    FooBuilder::~FooBuilder()
    {}

    const char* const
    FooBuilder::name() const
    {
     return "FooBuilder";
    }

</pre>
*/
class Builder: public BugCatcher
{
   public:


      /** Normal constructor initializes myCallContainer.
       */
      Builder();


      /** Virtual destructor
       */
      virtual ~Builder();


      /** First checks if the event is of SipEvent type. If so,
       *  it gets the CallInfo from the CallContainer and sets it in the
       *  SipEvent. It then passes the event to the Feature container
       *  process methods. After processing, it removes the call from 
       *  the CallContainer.
       */
      virtual void process( const Sptr < SipProxyEvent > event );


      /** Set the builder's callContainer
       */
      void setCallContainer(const Sptr < CallContainer > callContainer);

      void setSipStack(const Sptr < SipTransceiver > sipStack);
      

   protected:


      /** Method adds a feature to the container
       *  @param feature pointer to the feature to add
       *  @return void
       */
      void addFeature( const Sptr < Feature > feature );


      /** typedef list of Feature Sptr's
       */
      typedef list < Sptr < Feature > > FeatureContainer;


      /** FeatureContainer iterator. Yes, it's named poorly.
       */
      typedef FeatureContainer::iterator FeatureIter;
        

      /** Feature container object
       */
      FeatureContainer myFeatures;


      /** Pointer to the proxy CallContainer
       */
      Sptr < CallContainer > myCallContainer;

      ///
      Sptr < SipTransceiver > mySipStack;

   private:


      /** Suppress copying
       */
      Builder(const Builder &);

      /** Suppress copying
       */
      const Builder& operator=(const Builder &);
};
 
}

#endif // BUILDER_HXX

#ifndef SERIALIZE_HXX_
#define SERIALIZE_HXX_

/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2002 Vovida Networks, Inc.  All rights reserved.
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

static const char* const Serialize_hxx_Version = "$Id: Serialize.hxx,v 1.1 2004/05/01 04:15:33 greear Exp $";

#include "Data.hxx"


/** Infrastructure common to VOCAL.
 */
namespace Vocal 
{



/** Infrastructure in VOCAL related to Input/Output.<br><br>
 */
namespace IO
{


//uncomment the following line, to enable serialize 
// #define SERIALIZE       0 


/** usage 1:
 *
 *  Serialize::singleton("script_one.py w/ command line arguments", "w")
 *
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value");
 *
 *  usage 2 (changing the script):
 *
 *  Serialize::singleton("script_one1.py", "w")
 *
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value");
 *
 *  Serialize::singleton("script_two.py", "w")
 *  Serialize::singleton().addWrite("key", "value")
 *  Serialize::singleton().delWrite("value")
 *
 *  usage 3:
 *
 *  Serialize::singleton("script3.py w/ command line arguments", "r")
 *  char* actualBytesRead =  Serialize::singleton().read(readBuffer, numBytesToRead)
 *
 */



class SerializeFriend { };

class Serialize
{
    private:

    /**
     * name of the script file. Used in popen
     */
    static char*       myScriptFileName;
    /**
     *  readwritemode == "w" or "r" only. Used in popen
     */
    static char*       myReadWriteMode;
    /**
     */

    /**
     * fileptr returned by popen
     */
    static FILE*       myFilePtr;


    static Serialize*  self;

    /**
     */
    Serialize();

    /** to prevent compiler warning for having private destructor
     */
    friend class SerializeFriend;
    /**
     */
    virtual ~Serialize() {};
    /**
     */
    Serialize( Serialize &){};
    /**
     */
    Serialize& operator=( Serialize &);
    /**
     */
    bool validFile() const;

    /**
     */
    static void  doCpLog();

    public:

    /**
     */
    static Serialize& singleton( const char* scriptFileName = 0, const char* readWriteMode = "w" );

    /**
     */
    void addWrite( const Data& key, const Data& value );
    /**
     */
    void delWrite( const Data& key );
    /**
     */
    char* read(char* readBuffer, int numBytesToRead);

    /**
     */
    static void delSingleton();
};


} // namespace Transport
} // namespace Vocal




#endif // !defined(SERIALIZE_HXX_)

//End of File

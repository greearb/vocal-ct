
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
static const char* const FileDataStoreTest_cxx_Version =
    "$Id: FileDataStoreTest.cxx,v 1.1 2004/05/01 04:15:37 greear Exp $";
#include <iostream>
#include <cassert>

#include <FileDataStore.hxx>
#include <VIoException.hxx>
#include "cpLog.h"

int
main()
{
    cerr << "1" << endl;
    FileDataStore f(1000, "/tmp/t1");

    cerr << "2" << endl;
    f.addGroup("g1");
    assert( f.isGroup("g1") );
    assert( !f.isGroup("g1-no") );

    cerr << "3" << endl;
    f.putItem("g1", "i1", "g1i1");
    f.putItem("g1", "i2", "g1i2");
    f.putItem("g1", "i3", "g1i3");
    f.putItem("g1", "i4", "g1i4");

    cerr << "4" << endl;
    assert( f.isItem("g1", "i1") );
    assert( !f.isItem("g1", "i1-no") );

    cerr << "5" << endl;
    assert( f.getItem("g1", "i1") == "g1i1" );

    cerr << "6" << endl;
    f.removeItem("g1", "i2");
    assert( !f.isItem("g1", "i2") );

    cerr << "7" << endl;
    f.addGroup("g2");
    assert( f.isGroup("g2") );
    f.removeGroup("g2");
    assert( !f.isGroup("g2") );

    cerr << "8" << endl;
    StringList list;
    string v;
    list = f.listGroups();
    list.sort();
    assert( list.begin() != list.end() );
    v = *(list.begin());
    assert( v == "g1" );

    cerr << "9" << endl;
    list = f.listItems(string("g1"));
    list.sort();
    assert( list.begin() != list.end() );
    v = *(list.begin());
    assert( v == "i1" );

    cerr << "a" << endl;
    f.getItemSize("g1", "i1");
    cerr << "b" << endl;
    f.getItemTimeStamp("g1", "i1");
    TimeStamp t = 1;
    cerr << "c" << endl;
    f.putItem("g1", "i1", "g1i1", t);

    cerr << "d" << endl;
    f.addGroup("g3/g4");

    cerr << "e" << endl;

    try
    {
        f.putItem("g1-no", "i5", "g1i5");
        cerr << "f-no" << endl;
        assert(0);
    }
    catch (VIoException& f)
    {
        cerr << f.getDescription() << endl;
        cerr << "f-io" << endl;
    }
    catch (VException& f)
    {
        cerr << "f-exp" << endl;
    }

    cerr << "g" << endl;

    cerr << "Passed" << endl;
    return 0;
}

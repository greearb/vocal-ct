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


static const char* const deploy_cxx_Version =
    "$Id: deploy-hacked.cxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

using namespace std;
#include <sys/types.h>

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <fstream.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include <time.h>


#define LINK "ln -s %s /usr/local/vocal/tmp/mrtg/%s"

void conformance()
{
    cout << "deploy [-u] [-r <filename>] [-f <filename>]\n";
    cout << "\tu:\tperform upgrade.  Replace executable files. \n"
    << "\t\tRetain provisioning, network Management, billing etc... data\n"
    << "\tr:\tUser Rpm file for binaries.\n"
    << "\tf:\tRead system configuration from file.  Do not invoke auto-configuration\n\n"
    << "\tv:\tverbose - echo commands to stdout." << endl;

}

// vovida specific constants
const int DefaultSipPort = 5060;
const int DefaultRSSyncPort = 22002;
const int DefaultCdrPort = 22300;
const char DefaultPsMcHost[] = "224.0.0.101";
const char DefaultPsMcPort[] = "9001";
const char DefaultMcHost[] = "224.0.0.100";
const char DefaultMcPort[] = "9000";
const char DefaultMissedHb[] = "10";
const char DefaultHbInterval[] = "250";  //ms
const char DefaultPSPort[] = "6005";
const char DefaultPolicyPort[] = "3288";
const int DefaultPolicyPortInt = 3288;
const char DefaultH323Port[] = "22400";
const char ScriptFileLoc[] = "/usr/local/vocal/tmp/mkProvFiles.sh";
char h323MsIpPort [100];
const char DefaultLogLevel[] = "LOG_ERR";

const int MSuaPortOffset = 0;
const int MGWPortOffset = 1;
const int MConPortOffset = 2;
const int MInetPortOffset = 3;
const int FCbPortOffset = 4;
const int FFnabPortOffset = 5;
const int FCsPortOffset = 6;
const int FFacPortOffset = 7;
const int FCrPortOffset = 8;
const int FCidbPortOffset = 9;
const int JtapiPortOffset = 10;

const char RemoteProvDataDir[] = "/usr/local/vocal";
const char RemoteVocalLogDir[] = "/usr/local/vocal/log/";
const char RemoteVocalH323LogDir[] = "/usr/local/vocal/log/siph323csgw/";
#ifdef OS_SOLARIS
    const bool solarisOS = true;
    const char RemoteVocalStartupDir[] = "/etc/init.d/";
    const char RemoteEtcDir[] = "/etc";
#else
    const bool solarisOS = false;
    const char RemoteVocalStartupDir[] = "/etc/rc.d/init.d/";
    const char RemoteEtcDir[] = "/etc/rc.d";
#endif
const char RemoteVocalEtcDir[] = "/usr/local/vocal/etc/";
const char RemoteVocalBinDir[] = "/usr/local/vocal/bin/";
const char RemoteBillingDir[] = "/usr/local/vocal/billing/";
const char LocalVocalTmpDir[] = "/usr/local/vocal/tmp/";
const char LocalProvDataDir[] = "/usr/local/vocal/tmp/provisioning_data/";
const char LocalVocalEtcDir[] = "/usr/local/vocal/tmp/etc/";
const char LocalApacheConfigFile[] = "/usr/local/vocal/tmp/vocal_httpd.conf";
#ifdef OS_SOLARIS
    const char DefaultApacheConf1[] = "/etc/apache/httpd.conf";
    const char DefaultApacheConf2[] = "/etc/apache/httpd.conf";
#else
    const char DefaultApacheConf1[] = "/etc/httpd/conf/httpd.conf";
    const char DefaultApacheConf2[] = "/usr/local/apache/conf/httpd.conf";
#endif

// Config Related Defaults

const char defaultHostNamesFile[] = "./hosts";
const char defaultSwitchName[] = "classXswitch";
const char defaultBillingHost[] = "0.0.0.0";
const char defaultSecretKey[] = "testing123";
const char defaultGatewayIp[] = "0.0.0.0";
const char defaultGatewayNamesFile[] = "./Gateways";
const char defaultBinSourceDir[] = "/usr/local/vocal/stage/bin";
const char hashScript[] = "FileDataStoreWrapper";
const char NumberOfBins[] = "6";
const char PsRoot[] = "/usr/local/vocal/provisioning_data";
const char UserGroup[] = "Accounts";
const char sorScriptFileName[] = "/usr/local/vocal/tmp/startOnReboot";
const int maxFileNameSize = 512;
const string DefaultUser = "nobody";
string vsUser = DefaultUser;
bool upgrade = false;
bool configonly = false;
bool fuprovide = false;
bool userpm = false;
bool myTest = false;
bool usetar = false;
bool allinone = false;
bool verbose = false;
string psHost1 = "";
string psHost2 = "";

// TODO:  FIND REMOTE HTML DIR.  DON'T HARD CODE IT.
char RemoteHtmlDir[512];
char RemoteCgibinDir[512];
char VocalXmlSubDir[512] = "vocal" ;
const char HostNameToken1[] = "hostNameToken1";
const char HostNameToken2[] = "hostNameToken2";
const char CdrHostNameToken[] = "cdrNameToken";
const char DirToken[] = "dirToken";
const char CdrDirToken[] = "cdrDirectoryToken";
const char JarToken[] = "jarToken";
const char PortToken[] = "portToken";
const char UserSeedFile[] = "/usr/local/vocal/bin/provisioning/xml/UserSeed.xml";


// All-in-one config parameters

string GatewayIP;
string GatewayCode;
vector <string> SIPPhoneNumber;
vector <string> SIPPhoneIP;
char rlp[7];
char rcp[7];
char mcHost[512];
char mcPort[512];
char psMcHost[512];
char psMcPort[512];

// misc constants
const char nl[] = "\n";
const char sp[] = " ";
const char dq[] = "\"";

//Number of Vmsuas
const int NumSuaVm = 10;


// These are the functional units that can be loaded together.
// they are assined an enum value in inverse order.
#define ENUMMIN 1
#define NumOfFuncUnits 5
typedef enum {
    CDR_NET_VM = ENUMMIN,
    PS_PS,
    MS,
    FS_JS,
    RS,
    MAX_FU
} InstallFU_T;

typedef struct
{
    string hostName;
    InstallFU_T funcUnit;
    int port;
    vector < string > parms;
}
InstallUnit_T;

typedef enum {
    REDIRECT = ENUMMIN,
    PROVISIONING,
    MARSHAL_SUA,
    MARSHAL_GW,
    MARSHAL_CON,
    MARSHAL_INET,
    FEATURE_CB,
    FEATURE_FNAB,
    FEATURE_CS,
    FEATURE_FAC,
    FEATURE_VM,
    FEATURE_VM_SUA,
    FEATURE_CIDB,
    FEATURE_CR,
    JTAPI,
    CDR,
    POLICY,
    HBS
} serverType_T;


enum PackageType
{
    RPM,
    Tar,
    Generic
};

// this is similar to the linux "system" call except that it
// handls the "system"  error conditions and passes up the
// "cmd" errors to the user.
int mySystem(const char *cmd, bool silent = false)
{
    string myCmd;

    if (!cmd)
    {
        cout << "invalid command to mySystem" << nl;
    }

    if (verbose)
    {
	cout << "running command: " << cmd << endl;
    }

    myCmd.assign(cmd);
    myCmd.append("  > /dev/null 2>&1");

    int retVal = system(myCmd.c_str());
    if (retVal == 127)
    {
        cout << "execve() from /bin/sh failed during system call";
        return -1;
    }
    else if (retVal == -1)
    {
        perror("system command from shell failed: ");
    }

    if (retVal != 0 && !silent)
    {
        cout << "mySystem command failed " << cmd << nl;
    }

    return retVal;
}



// user entered data
string hostNamesFile;
string switchName;
string billingHost;
string billingSecretKey;
string binSourceDir;
vector < string > gatewayForMarshals;


int
makeDeployedServersFile(vector < InstallUnit_T > installConfig )
{
    vector < InstallUnit_T > ::iterator iv;
    char fileName[512];
    char ipStr[32];
    char portStr[32], portStr2[32], portStr3[32];
    struct in_addr inAddr;
    struct hostent *pHostent = NULL;

    memset(fileName, 0, sizeof(fileName));
    sprintf(fileName, "%s/deployedServers.xml", LocalProvDataDir);
    ofstream outFile(fileName);
    if (!outFile || outFile.bad())
    {
        cout << "can not open file " << fileName << nl;
        return false;
    }

    outFile << "<deployedServers>" << nl;

    outFile << "    <global" << nl
	    << "        multicastHost=" << dq <<mcHost<<dq <<nl
	    << "        multicastPort=" << dq << mcPort << dq << nl
	    << "    />" << nl;

    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
        pHostent = gethostbyname(iv->hostName.c_str());
        if (pHostent == NULL)
        {
            cout << "error in gethostbyname from makeDeployedServersFile\n";
            return false;
        }
        if (pHostent->h_addr_list[0])
        {
            memcpy(&inAddr, pHostent->h_addr_list[0], sizeof(unsigned long));
            memset(ipStr, 0 , sizeof(ipStr));
            strcpy(ipStr, inet_ntoa(inAddr));
        }
        else
        {
            cout << "error getting host ip in makeDeployedServersFile\n";
            return false;
        }

        switch (iv->funcUnit)
        {
            case CDR_NET_VM:
		// add in a cdr, fs voice mail (and vmsua), and heartbeat server
		// parms[0] = <cdrGroup1 | cdrGroup2>
		// parms[1] = billingHost
		// parms[2] = port
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port );
		memset(portStr2, 0, sizeof(portStr2));
		sprintf(portStr2, "%d", iv->port + 1);
		memset(portStr3, 0, sizeof(portStr3));
		// actually iv->port + NumSuaVm +1 -1
		sprintf(portStr3, "%d", iv->port + NumSuaVm );
		if (!allinone)
		{
		    outFile << "    <cdrServer" << nl
			    << "        host=" << dq << ipStr << dq << nl
			    << "        port=" << dq << iv->parms[2] << dq << nl
			    << "        radiusServerHost=" << dq << iv->parms[1] << dq << nl
			    << "        radiusSecretKey=" << dq << billingSecretKey << dq << nl
			    << "        billingDirPath=\"/usr/local/vocal/billing\"" << nl
			    << "        billForRingtime=\"true\"" << nl
			    << "    />" << nl;
		}
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"Voicemail\"" << nl
			<< "        uaVMHost=" << dq << ipStr << dq << nl
			<< "        uaVMFirstPort=" << dq << portStr2 << dq << nl
			<< "        uaVMLastPort=" << dq << portStr3 << dq << nl
			<< "    />" << nl;
		
		outFile << "    <heartbeatServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "    />" << nl;
		break;
		
            case PS_PS:
		// add in a policy server
		
		if (!allinone)
		{
		    outFile << "    <policyServer" << nl
			    << "        host=" << dq << ipStr << dq << nl
			    << "    />" << nl;
		}
		break;
		
            case MS:
		// add 1 of each type of ms
		// parms[0] = gateway ip
		// parms[1] = cdr server ip
		// parms[2] = cdr server port
		memset(portStr2, 0, sizeof(portStr2));
		sprintf(portStr2, "%d", DefaultSipPort );
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + MSuaPortOffset );
		outFile << "    <marshalServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"UserAgent\"" << nl
			<< "        gatewayHost=" << dq << iv->parms[0] << dq << nl
			<< "        gatewayPort=" << dq << portStr2 << dq << nl
			<< "    />" << nl;

		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + MGWPortOffset );
		outFile << "    <marshalServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"Gateway\"" << nl
			<< "        gatewayHost=" << dq << iv->parms[0] << dq << nl
			<< "        gatewayPort=" << dq << portStr2 << dq << nl
			<< "    />" << nl;

		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + MConPortOffset );
		outFile << "    <marshalServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
 			<< "        type=\"ConferenceBridge\"" << nl
 			<< "        gatewayHost=" << dq << iv->parms[0] << dq << nl
 			<< "        gatewayPort=" << dq << portStr2 << dq << nl
			<< "    />" << nl;

		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + MInetPortOffset );
		outFile << "    <marshalServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"Internetwork\"" << nl
			<< "    />" << nl;
		break;

            case FS_JS:
		// add 1 Jtapi and 1 of each non-vm Feature server
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FCbPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"CallBlocking\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FFnabPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"ForwardNoAnswerBusy\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FCsPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"CallScreening\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FFacPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"ForwardAllCalls\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FCrPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"CallReturn\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + FCidbPortOffset );
		outFile << "    <featureServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        type=\"CallerIdBlocking\"" << nl
			<< "    />" << nl;
		
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port + JtapiPortOffset );
		memset(portStr2, 0, sizeof(portStr2));
		sprintf(portStr2, "%d", iv->port + JtapiPortOffset + 1 );
		memset(portStr3, 0, sizeof(portStr3));
		sprintf(portStr3, "%d", iv->port + JtapiPortOffset + 2 );
		outFile << "    <jtapiServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        callingPort=" << dq << portStr << dq << nl
			<< "        calledPort=" << dq << portStr2 << dq << nl
			<< "        clientPort=" << dq << portStr3 << dq << nl
			<< "    />" << nl;
		break;
		
            case RS:
		// add 1 rs
		memset(portStr, 0, sizeof(portStr));
		sprintf(portStr, "%d", iv->port );
		memset(portStr2, 0, sizeof(portStr2));
		sprintf(portStr2, "%d", DefaultRSSyncPort );
		outFile << "    <redirectServer" << nl
			<< "        host=" << dq << ipStr << dq << nl
			<< "        port=" << dq << portStr << dq << nl
			<< "        syncPort=" << dq << portStr2 << dq << nl
			<< "    />" << nl;
		break;

            default:
                break;
        }
    }

    outFile << "</deployedServers>" << nl;
    outFile.close();
    return true;
}

int readGatewayFile(const char *inVal)
{
    char buff[512];
    ifstream inFile(inVal);
    if (!inFile || inFile.bad())
    {
        cout << " can not open file " << inFile;
        return false;
    }
    while (inFile.getline(buff, 512 -1))
    {
        if (buff[0] != '\0')
            gatewayForMarshals.push_back(buff);
    }
    inFile.close();
    return true;
}

// function to find any specified parameter in a file.
// the arguments are the filename with complete path, the parameter name to search for
// the parameter value delimiters, and the variable into which the parameter value
// should be transferred

bool findin(char *filer, char *stringer, char beginer, char ender, char *paramvalue, int sz)
{
    char searchstring[512];
    char tmpStr[512];
    bool foundStringer = false;

    ifstream searchfile(filer);
    if (!searchfile || searchfile.bad() )
    {
        cout << "    Can not locate httpd.conf in " << filer << "\n";
        return false;
    }

    memset(tmpStr, 0, sizeof(tmpStr));

    size_t position1, position2;

    while (searchfile.getline(searchstring, sizeof(searchstring)))
    {
        string sstring = searchstring;
        position1 = sstring.find(stringer);
        position2 = sstring.find(beginer);
        if ((position1 != string::npos) && (position2 != string::npos))
        {
            if (searchstring[0] != '#')
            {
	        foundStringer = true;
                position2 += 1;
                int inum = 0;
                while (searchstring[position2] != ender)
                {
                    tmpStr[inum] = searchstring[position2];
                    inum++;
                    position2++;
                }
            }
        }
    }
    memcpy(paramvalue, tmpStr, sz);
    return foundStringer;
}


bool HostnameIn(char *hosts, vector < string > HN)
{
    vector < string > ::iterator HNI;
    string HNIstr;
    for (HNI = HN.begin(); HNI < HN.end(); HNI++)
    {
        HNIstr = HNI->c_str();
        if (HNIstr == hosts) return true;
    }
    return false;
}


bool HostfileWrite(vector < InstallUnit_T > hostvector, char *path)
{
    ofstream configwrite(path);
    string hostfu;
    string hostn, hostfilestring, hName;
    vector < string > hN1;
    vector < InstallUnit_T > ::iterator hw;
    vector < InstallUnit_T > ::iterator hfw;
    time_t tt;
    time(&tt);
    char *dater = ctime(&tt);
    configwrite << "# Config file - Generated by <deploy> - " << dater << nl;
    for (hw = hostvector.begin(); hw < hostvector.end(); hw++)
    {
        hostn = hw->hostName;
        char hostncast[512];
        memset(hostncast, 0, sizeof(hostncast));
        sprintf(hostncast, "%s", hostn.c_str());
        if (!HostnameIn(hostncast, hN1))
        {
            hN1.push_back(hostn);
            configwrite << hostn << " ";
            for (hfw = hostvector.begin(); hfw < hostvector.end(); hfw++)
            {
                if (hostn == hfw->hostName)
                {
                    switch (hfw->funcUnit)
                    {
                        case (CDR_NET_VM):
                                    configwrite << "CDR_NET_VM ";
                        break;
                        case (RS):
                                    configwrite << "RS ";
                        break;
                        case (FS_JS):
                                    configwrite << "FS_JS ";
                        break;
                        case (PS_PS):
                                    configwrite << "PS_PS ";
                        break;
                        case (MS):
                                    configwrite << "MS ";
                        break;
                        default:
                        break;
                    }

                }
            }
            configwrite << "\n";
        }
    }
    configwrite.close();
    return true;
}

vector < InstallUnit_T > HostfileRead(char *hostfile)
{
    ifstream readfile(hostfile);
    vector < InstallUnit_T > hostvector;
    char readstring[512];
    if (!readfile || readfile.bad() )
    {
        cout << " Cannot Open Host File. Please check ....";
    }
    string hostfilestring, separatorstring, hoststring;
    memset(readstring, 0, sizeof(readstring));
    separatorstring = "";
    size_t i, ii;
    InstallUnit_T v;
    while (readfile.getline(readstring, sizeof(readstring)))
    {
        v.hostName = "";
        if ((readstring[0] != '#') && (readstring[0] != ' ') && (readstring[0] != '\n') && (readstring[0] != '\r') && (readstring[0] != '\0'))
        {

            hostfilestring.assign(readstring, sizeof(readstring));
            for (i = 0; i < hostfilestring.length(); i++)
            {
                if (readstring[i] != ' ')
                {
                    v.hostName += readstring[i];
                }
                else
                {

                    break;
                }
            }
            hoststring = v.hostName;


            for (ii = i + 1; ii < hostfilestring.length(); ii++)
            {
                if ((readstring[ii] == '\n') || (readstring[ii] == '\0') || (readstring[ii] == '\r') || (readstring[ii] == '0'))
                {
                    memset(readstring, 0, sizeof(readstring));
                    break;
                }

                if ((readstring[ii] != ' ') && (readstring[ii] != '\n') && (readstring[ii] != '\0') && (readstring[ii] != '\r') && (readstring[ii] != '0'))
                {

                    separatorstring.append(readstring, ii, 1);

                }
                if ((readstring[ii] == ' ') || (readstring[ii] == '\n') || (readstring[ii] == '\0') || (readstring[ii] == '\r') || (readstring[ii] != '0'))
                {
                    if (separatorstring == "CDR_NET_VM" )
                    {

                        v.funcUnit = CDR_NET_VM;
                        hostvector.push_back(v);

                        separatorstring = "";

                    }
                    else if (separatorstring == "PS_PS" )
                    {
                        v.funcUnit = PS_PS;
                        hostvector.push_back(v);
                        separatorstring = "";
                    }
                    else if (separatorstring == "MS" )
                    {
                        v.funcUnit = MS;
                        hostvector.push_back(v);
                        separatorstring = "";
                    }
                    else if (separatorstring == "FS_JS" )
                    {

                        v.funcUnit = FS_JS;
                        hostvector.push_back(v);
                        separatorstring = "";
                    }
                    else if (separatorstring == "RS" )
                    {
                        v.funcUnit = RS;
                        hostvector.push_back(v);
                        separatorstring = "";
                    }

                }

            }
        }
    }
    return hostvector;

}

// this function queries the user for data that is deployment specific.
// the results are place in the above global variables
int queryUser(bool upgrade, bool fuprovide, PackageType pkgType, bool allinone)
{
    char buf[512];
    
    
    if (!allinone)
    {
	if (!fuprovide)
	{
	    cout << nl << " Enter filename containing list of system (non-sua) hosts [" << defaultHostNamesFile << "]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		hostNamesFile.assign(defaultHostNamesFile);
	    }
	    else
	    {
		hostNamesFile.assign(buf);
	    }
	}
	if (pkgType == Generic)
	{
	    cout << nl << " Enter directory containing the executables to load [" << defaultBinSourceDir << "]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		binSourceDir.assign(defaultBinSourceDir);
	    }
	    else
	    {
		binSourceDir.assign(buf);
	    }
	}
	
	if (!upgrade)
	{
	    cout << nl << " Enter name of your classXswitch system [" << defaultSwitchName << "]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		switchName.assign(defaultSwitchName);
	    }
	    else
	    {
		switchName.assign(buf);
	    }
	    
	    cout << nl << " Enter hostname or ip of billing system [" << defaultBillingHost << "]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		billingHost.assign(defaultBillingHost);
	    }
	    else
	    {
		billingHost.assign(buf);
	    }
	    
	    cout << nl << " Enter billing SecretKey [" << defaultSecretKey << "]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		billingSecretKey.assign(defaultSecretKey);
	    }
	    else
	    {
		billingSecretKey.assign(buf);
	    }
	    
	    cout << nl << " Is there more than 1 Gateway for phones (y/n) [n]:" << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0 || !strcasecmp(buf, "n") || !strcasecmp(buf, "no") )
	    {
		cout << nl << "  Enter ip address for the gateway [" << defaultGatewayIp << "]: " << nl;
		cin.getline(buf, 512, '\n');
		if (strlen(buf) == 0)
		{
		    gatewayForMarshals.push_back(defaultGatewayIp);
		}
		else
		{
		    gatewayForMarshals.push_back(buf);
		}
	    }
	    else
	    {
		int retVal;
		cout << nl << " Enter file name containing list of gateways [" << defaultGatewayNamesFile << "]: " << nl;
		cin.getline(buf, 512, '\n');
		if (strlen(buf) == 0)
		{
		    retVal = readGatewayFile(defaultGatewayNamesFile);
		}
		else
		{
		    retVal = readGatewayFile(buf);
		}
		if (!retVal)
		{
		    cout << "Failure reading input Gateway file." << nl;
		    return false;
		}
		
	    }
	    
	    cout << nl << " Enter the user id who will own provisioning files and start pserver ["<<DefaultUser<<"]: " << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		vsUser = DefaultUser;
	    }
	    else
	    {
		vsUser = buf;
	    }
	    
	    cout << nl << " Enter multicast address for Heartbeat [" << DefaultMcHost << "]: " << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		memset(mcHost, 0, sizeof(mcHost));
		sprintf(mcHost, "%s", DefaultMcHost);
	    }
	    else
	    {
		memset(mcHost, 0, sizeof(mcHost));
		sprintf(mcHost, "%s", buf);
	    }
	    
	    cout << nl << " Enter multicast port for Heartbeat [" << DefaultMcPort << "]: " << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		memset(mcPort, 0, sizeof(mcPort));
		sprintf(mcPort, "%s", DefaultMcPort);
	    }
	    else
	    {
		memset(mcPort, 0, sizeof(mcPort));
		sprintf(mcPort, "%s", buf);
	    }
	    
	    cout << nl << " Enter multicast address for Provisioning Server [" << DefaultPsMcHost << "]: " << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    { 
		memset(psMcHost, 0, sizeof(psMcHost));
		sprintf(psMcHost, "%s", DefaultPsMcHost);
		
	    }
	    else
	    {
		memset(psMcHost, 0, sizeof(psMcHost));
		sprintf(psMcHost, "%s", buf);		    
	    }
	    cout << nl << " Enter multicast port for Provisioning Server [" << DefaultPsMcPort << "]: " << nl;
	    cin.getline(buf, 512, '\n');
	    if (strlen(buf) == 0)
	    {
		memset(psMcPort, 0, sizeof(psMcPort));
		sprintf(psMcPort, "%s", DefaultPsMcPort);
	    }
	    else
	    {
		memset(psMcPort, 0, sizeof(psMcPort));
		sprintf(psMcPort, "%s", buf);
	    }
	}
    }
    else
    {
	binSourceDir = ".";
	system ("hostname > allinonehost");
	hostNamesFile = "allinonehost";
    }
	
    return true;
}

// create directory "dir" on local host
// returns true on success; false on failure
int localMkdir(const char *dir)
{
    char sysCommand[512];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "mkdir -p %s", dir );
    if (mySystem(sysCommand) != 0)
    {
        cout << " can not create" << dir
        << " dir on host " << nl;
        return false;
    }
    return true;
}

// create directory "dir" on remote host "host"
// returns true on success; false on failure
int remoteMkdir (const char *host, const char *dir)
{
    char sysCommand[512];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "%s %s \"mkdir -p %s\"", rlp, host, dir );
    if (mySystem(sysCommand) != 0)
    {
        cout << " can not create" << dir << " dir on host " << host;
        return false;
    }
    return true;
}

// tests the write ability of the user for host in dir
// returns false on failure true on success
int remoteWriteTest(const char *host, const char *dir)
{
    char mySysCommand[512];
    int retVal;

    memset(mySysCommand, 0, sizeof(mySysCommand));
    sprintf(mySysCommand, "%s %s \"touch %swriteAccessTest\"", rlp, host, dir);
    retVal = mySystem(mySysCommand);
    
    if (retVal != 0)
    {
        cout << "No Write Access to dir " << dir <<
        " on host " << host;
        return false;
    }
    else
    {
        memset(mySysCommand, 0, sizeof(mySysCommand));
        sprintf(mySysCommand, "%s %s \"rm -f %swriteAccessTest\"", rlp, host, dir);
        retVal = mySystem(mySysCommand, true);
        if (retVal != 0)
        {
            cout << "can not remove temporary file " << dir << "writeAccessTest on host " << host;
            return false;
        }
    }
    return true;
}

// this function will return a copy of the vector "inVector" grouped
// by hostName
vector < InstallUnit_T > groupVectorByHostname( vector < InstallUnit_T > inVector)
{
    vector < InstallUnit_T > outVector;
    vector < InstallUnit_T > ::iterator iv;

    for (size_t i = 0; i < inVector.size(); i++)
    {
        if (outVector.empty())
        {
            outVector.push_back(inVector[i]);
        }
        else
        {
            for (iv = outVector.begin();
                    iv < outVector.end() && iv->hostName != inVector[i].hostName;
                    iv++);
            outVector.insert(iv, inVector[i]);
        }
    }
    return outVector;
}

int sortVectorbyFU ( InstallFU_T fu, vector < InstallUnit_T > &installConfig)
{
    vector < InstallUnit_T > installSwapVector;
    vector < InstallUnit_T > ::iterator isv;
    for (isv = installConfig.begin(); isv < installConfig.end(); isv++)
    {
        if (isv->funcUnit == fu)
        {
            installSwapVector.push_back(*isv);
        }
    }
    for (isv = installConfig.begin(); isv < installConfig.end(); isv++)
    {
        if (isv->funcUnit != fu)
        {
            installSwapVector.push_back(*isv);
        }
    }
    installConfig.swap(installSwapVector);
    return true;
}

// put sip port assignments into the system
// the first "real" server on a host will get the port 5060
// others will get 5061 5062 etc...
// Jtapi needs 3 ports so we will assign 4 to the FS_JS units
void assignSipPorts(vector < InstallUnit_T > &inVector)
{
    string lastHost("");
    vector < InstallUnit_T > ::iterator iv;
    int nextPort;
    
    sortVectorbyFU(MS, inVector);
    
    for (iv = inVector.begin(); iv < inVector.end(); iv++)
    {
        if (iv->hostName != lastHost)
        {
            nextPort = DefaultSipPort;
            lastHost = iv->hostName;
        }
        switch (iv->funcUnit)
        {
            case MS:
		iv->port = nextPort;
		nextPort = nextPort + 5;
		break;
            case RS:
		iv->port = nextPort;
		nextPort++;
		break;
            case FS_JS:
		iv->port = nextPort;
		nextPort = nextPort + 8;
		break;
            case CDR_NET_VM:
		iv->port = nextPort;	  
		nextPort = nextPort + NumSuaVm + 3 ;	    	   
		break;
            default :
		iv->port = 0;
		break;
        }
    }

}

// This function will put in the parms for some of the servers
// It will pass through the list for the CDR and then the Marshals
// This assumes there will be no more than 2 cdr servers in the system.
void assignParms(vector < InstallUnit_T > &inVector)
{
    vector < InstallUnit_T > ::iterator iv;
    int numCDRs = 0, numMSs = 0;
    char ipStr[32];
    string aParm, cdr1Ip, cdr2Ip;
    int cdr1Port, cdr2Port;
    div_t tmpDivt, msDivt;
    struct hostent *pHostent = NULL;
    struct in_addr inAddr;

    // cdr server
    for (iv = inVector.begin(); iv < inVector.end(); iv++)
    {
        if (iv->funcUnit == CDR_NET_VM)
        {
            numCDRs++;
            iv->parms.resize(3, " ");
            if (iv->hostName.c_str())
                pHostent = gethostbyname(iv->hostName.c_str());
            if (pHostent == NULL)
            {
                cout << "error in gethostbyname from assignParms.  Your results are incorrect.  KILL ME NOW !!!!!";
                return ;
            }
            if (pHostent->h_addr_list[0])
            {
                memcpy(&inAddr, pHostent->h_addr_list[0], sizeof(unsigned long));
                memset(ipStr, 0 , sizeof(ipStr));
                strcpy(ipStr, inet_ntoa(inAddr));
            }
            else
            {
                cout << "error getting host ip in assignParms.  Your results are incorrect. KILL ME NOW !!!!!!";
                return ;
            }
	    
	    if (!allinone)
	    {
		
		if (numCDRs == 1)
		{
		    iv->parms[0] = "cdrGroup1";
		    cdr1Ip.assign(ipStr);
		    cdr1Port = DefaultCdrPort;
		    char tmpstr[32];
		    sprintf(tmpstr, "%d", DefaultCdrPort);
		    iv->parms[2] = tmpstr;
		    
		}
		else
		{
		    iv->parms[0] = "cdrGroup2";
		    cdr2Ip.assign(ipStr);
		    cdr2Port = DefaultCdrPort + 1;
		    char tmpstr[32];
		    sprintf(tmpstr, "%d", DefaultCdrPort + 1);
		    iv->parms[2] = tmpstr;
		}
		iv->parms[1] = billingHost;
	    }
	}
	
	// marshals
	
        else if (iv->funcUnit == MS)
        {
            char tmpStr[32];
            memset(tmpStr, 0, sizeof(tmpStr));
	    
            if (strlen(h323MsIpPort) == 0)
            {
                struct hostent *pHostent = NULL;
                struct in_addr inAddr;
                pHostent = gethostbyname(iv->hostName.c_str());
                if (pHostent == NULL)
                {
                    cout << "error in gethostbyname from assignParms\n";
                    return ;
                }
                if (pHostent->h_addr_list[0])
                {
                    memcpy(&inAddr, pHostent->h_addr_list[0], sizeof(unsigned long));
                    memset(h323MsIpPort, 0 , sizeof(h323MsIpPort));
                    sprintf(h323MsIpPort, "%s:%d", inet_ntoa(inAddr), iv->port + MSuaPortOffset );
                }
                else
                {
                    cout << "error getting host ip in assignParms\n";
                    return ;
                }

            }

            iv->parms.resize(3, " ");
            numMSs++;
            msDivt = div(numMSs, gatewayForMarshals.size());
            iv->parms[0] = gatewayForMarshals[msDivt.rem];
            tmpDivt = div(numMSs, 2);
	    if (!allinone)
	    {
		if (numCDRs > 1 && tmpDivt.rem == 0)
		{
		    iv->parms[1] = cdr2Ip;
		    sprintf(tmpStr, "%d", cdr2Port);
		}
		else
		{
		    iv->parms[1] = cdr1Ip;
		    sprintf(tmpStr, "%d", cdr1Port);
		}
	    
		iv->parms[2].assign(tmpStr);
	    }
        }
    }
}



bool mkProvFiles(const char *scriptfilename, const char* provhost)
{
    ofstream outscript(scriptfilename);
    string javapath;
    char syster1[512];
    char syster2[512];
    memset (syster1, 0, sizeof(syster1));
    memset (syster2, 0, sizeof(syster2));
    sprintf(syster1, "%s %s \"ls /usr/jdk/bin/java\"", rlp, provhost);
    sprintf(syster2, "%s %s \"ls /usr/jdk1.2.2/bin/java\"", rlp, provhost);
    if (mySystem(syster1, true) == 0)
    {
        javapath = "/usr/jdk/bin/java";
    }
    else if (mySystem(syster2, true) == 0)
    {
        javapath = "/usr/jdk1.2.2/bin/java";
    }
    else
    {
        cout << "Java not found on " << provhost << " ! Might not create provisioning data ! \n";
        javapath = "java";

    }
    outscript << "#!/bin/bash" << nl
	      << "killall -9 pserver" << nl
	      << "cp -f /usr/local/vocal/provisioning_data/deployedServers.xml ." << nl
	      << RemoteVocalBinDir << "/pserver" << nl
	      << "export CLASSPATH=" << RemoteVocalBinDir << "/psClient.jar" << nl
	      << javapath << " vocal.data.DeployServers " << provhost << " 6005" << nl
	      << "for i in `ls -1 /usr/local/vocal/tmp/sipphoneusers` "<<nl
	      <<"do"<<nl
	      << "\t" << RemoteVocalBinDir<<"/"<<hashScript<<" -b "<<NumberOfBins<<" -r "
	      <<PsRoot<<" -d "<<UserGroup<<" -f $i"<<" -c \"`cat /usr/local/vocal/tmp/sipphoneusers/$i`\""<<nl
	      <<"done"<<nl;
    outscript.close();
    return true;
}


// This function will create the files for the /usr/local/vocal/etc directory
// right now we assume there is only 1 PS running on the system.
int makeLocalEtcFiles (vector < InstallUnit_T > installConfig, vector <string> hostN)
{
    vector < InstallUnit_T > ::iterator iv;
    vector < string > :: iterator hostNiv;
    string oldHostName;
    string psHostName, backupPsHostName;
    char outFileName[512];
    char portStr[32];
    ofstream *outFileP = NULL;
    int SuaVmPorts, psCount = 0;

    // find the pserver in the first loop, then create the file in the
    // second loop.  Only one pserver will be used at a time.
    //sort the vector by PS_PS functional unit, so that the pserver is started up first by vocalstart.

    sortVectorbyFU(PS_PS, installConfig);
    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
        if (iv->funcUnit == PS_PS)
        {
            if (psCount == 0)
            {
                psHostName = iv->hostName;
            }
            else
            {
                backupPsHostName = iv->hostName;
            }
            psCount++;
        }
    }
    if (psCount < 2)
    {
        backupPsHostName = psHostName;
    }

    for (hostNiv = hostN.begin(); hostNiv < hostN.end(); hostNiv ++ )
    { 

	memset(outFileName, 0, sizeof(outFileName));
	sprintf(outFileName, "%s/vocal.conf.%s", LocalVocalEtcDir, hostNiv->c_str());
	outFileP = new ofstream(outFileName);
	

	
	*outFileP << "VOCALUSER " << vsUser << nl
		  << "PSERVER " << psHostName << ":" << DefaultPSPort << nl
		  << "PSERVERBACKUP " << backupPsHostName << ":" << DefaultPSPort << nl
		  << "LOGLEVEL " << DefaultLogLevel << nl;
	
	for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
	{
	   
	    if (iv->hostName == hostNiv->c_str())
	    {
	
		switch (iv->funcUnit)
		{
		    case CDR_NET_VM:
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port);
			if (!allinone)
			{
			    *outFileP << "cdr" << nl;
			}
			
			*outFileP << "snmptrapd" << nl
				  << "netMgnt" << nl
				  << "vmserver" << nl
				  << "hbs" << nl
				  << "fsvm " << portStr << nl;
			SuaVmPorts = iv->port;
			
			if (!allinone)
			{
			    for (int ii = 1; ii <= NumSuaVm; ii++)
			    {
				SuaVmPorts++;
				memset(portStr, 0, sizeof(portStr));
				sprintf(portStr, "%d", SuaVmPorts);
				*outFileP << "uavm " << portStr << nl;
			    }
			    // if parms[2] == DefaultCdrPort put a h323 on this one.
			    
			    if (atoi(iv->parms[2].c_str()) == DefaultCdrPort)
			    {
				*outFileP << "siph323csgw " << DefaultH323Port << sp << h323MsIpPort << nl;
			    }
			}
			break;
			
		    case PS_PS:
			if (psHostName == backupPsHostName)
			{
			    *outFileP << "ps" << nl;
			}
			else
			{
			    *outFileP << "ps " << psMcHost << " " << psMcPort << nl;
			}
		
			if (!allinone)
			{
			    *outFileP << "policy" << nl;
			}
			break;
			
		    case MS:
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + MSuaPortOffset );
			*outFileP << "ms " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + MGWPortOffset );
			*outFileP << "ms " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + MInetPortOffset );
			*outFileP << "ms " << portStr << nl;
			
			if (!allinone)
			{
			    memset(portStr, 0, sizeof(portStr));
			    sprintf(portStr, "%d", iv->port + MInetPortOffset );
			    *outFileP << "inet_ms " << portStr << nl;
			}
			
			break;
			
		    case FS_JS:
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FCbPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FFnabPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FCsPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FFacPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FCrPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port + FCidbPortOffset );
			*outFileP << "fs " << portStr << nl;
			
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d %d",
				iv->port + JtapiPortOffset,
				iv->port + JtapiPortOffset + 1 );
			*outFileP << "js " << portStr << nl;
			
			break;
		    case RS:
			memset(portStr, 0, sizeof(portStr));
			sprintf(portStr, "%d", iv->port);
			*outFileP << "rs " << portStr << nl;
			
			break;
		    default:
			cout << "bad funcUnit in makeLocalEtcFiles" << nl;
			return false;
		}
	    }
	}
	
	*outFileP << "snmpd" << nl;
	outFileP->close();
	delete outFileP;
    }
    return true;
}




string printFU(InstallFU_T type)
{
    string retVal;
    switch (type)
    {
        case CDR_NET_VM:
	    if (!allinone)
	    {
		retVal = "CDR server, NetMgnt, snmptrapd, FS VoiceMail, sip-h323, heartbeat server";
	    }
	    else
	    {
		retVal = "NetMgnt, snmptrapd, FS VoiceMail, heartbeat server";
	    }
	    break;
        case PS_PS:
	    if (!allinone)
	    {
		retVal = "Provision Server, Policy Server";
	    }
	    else 
	    {
		retVal = "Provision Server, Policy Server";
	    }
	    break;
        case MS:
	    retVal = "Marshals";
	    break;
        case FS_JS:
	    retVal = "Feature Servers, JTAPI Server";
	    break;
        case RS:
	    retVal = "Redirect Server";
	    break;
        default:
	    retVal = "";
	    cout << "error in printFU input\n";
	    break;
    }
    return retVal;
}

vector < string > getHostNames(vector < InstallUnit_T > &installConfig)
{
    vector < string > hNames;
    vector < InstallUnit_T > ::iterator hiv;
    for (hiv = installConfig.begin(); hiv < installConfig.end(); hiv++)
    {
        char hostnamevar[512];
        memset(hostnamevar, 0, sizeof(hostnamevar));
        sprintf(hostnamevar, "%s", hiv->hostName.c_str());
        if (!HostnameIn(hostnamevar, hNames))
        {
            hNames.push_back(hiv->hostName);
        }
    }
    return hNames;
}


int parseApacheConfig(vector < InstallUnit_T > installConfig, char *RemoteHtmlDir, int sz, char *RemoteCgibinDir, int sz1)
{
    vector < InstallUnit_T > ::iterator iv;
    char buff[512];

    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
        if (iv->funcUnit == PS_PS || iv->funcUnit == CDR_NET_VM)
        {

            // Find DocumentRoot and ask if user would prefer to have the vocal pages elsewhere

            char apachename[512];
            cout << "  configuring Apache on " << iv->hostName.c_str() << nl;
            memset(apachename, 0, sizeof(apachename));
            sprintf(apachename, "%s root@%s:%s %s", rcp, iv->hostName.c_str(), DefaultApacheConf1, LocalApacheConfigFile );
            if (mySystem(apachename, true) != 0)
            {
                memset(apachename, 0, sizeof(apachename));
                sprintf(apachename, "%s root@%s:%s %s", rcp, iv->hostName.c_str(), DefaultApacheConf2, LocalApacheConfigFile );
                if (mySystem(apachename, true) != 0)
                {
                    cout << " Error : Can not find httpd.conf on " << iv->hostName.c_str() << nl
                    << " Please enter the fully-qualified file name: ";
                    cin.getline(buff, 512, '\n');
                    if (strlen(buff) == 0 )
                    {
                        cout << nl << "No remote webserver config done" << nl;
                        return 0;
                    }
                    else
                    {
                        memset(apachename, 0, sizeof(apachename));
                        sprintf(apachename, "%s root@%s:%s %s", rcp, iv->hostName.c_str(), buff, LocalApacheConfigFile );
                        if (mySystem(apachename, true) != 0)
                        {
                            cout << nl << " Can not find httpd conf there either.  The webservers will not function correctly";
                        }
                    }
                }
            }

            memset(RemoteHtmlDir, 0, sz);
            char LocalApacheConfigFileVar[512];
            memset(LocalApacheConfigFileVar, 0, sizeof(LocalApacheConfigFileVar));
            sprintf(LocalApacheConfigFileVar, "%s", LocalApacheConfigFile);
            if (!findin(LocalApacheConfigFileVar, "DocumentRoot", '"', '"', RemoteHtmlDir, sz))
            {
                memset(RemoteHtmlDir, 0, sz);
                cout << "No DocumentRoot variable found. Please enter your DocumentRoot directory:";
		cin.getline(RemoteHtmlDir, 512, '\n');
		if (strlen(RemoteHtmlDir) == 0 )
		{
		    cout << nl << "No remote webserver config done" << nl;
		    return 0;
		}
            }
            else 
	        if( verbose)
	        {
		    cout << "Apache DocumentRoot is " << RemoteHtmlDir << endl;
		}
            memset(RemoteCgibinDir, 0, sz1);
            if (!findin(LocalApacheConfigFileVar, "ScriptAlias /cgi-bin", '"', '"', RemoteCgibinDir, sz1))
            {
                memset(RemoteCgibinDir, 0, sz1);
                cout << "No cgi-bin directory found. Please enter your cgi-bin directory:";
		cin.getline(RemoteCgibinDir, 512, '\n');
		if (strlen(RemoteCgibinDir) == 0 )
		{
		    cout << nl << "No remote webserver config done" << nl;
		    return 0;
		}
            }
            else 
	        if( verbose)
	        {
		    cout << "Apache CGI-Bin Directory is " << RemoteCgibinDir << endl;
		}
        }
    }
    return 0;
}


bool tarfileGzipped (string tarfile)
{
    bool gzip = false;
    if (((tarfile.rfind("gz")) == (tarfile.size() - 2)) || ((tarfile.rfind("tgz")) == (tarfile.size()-3)))
    {
        gzip = true;
    }
    return gzip;
}

// Neetha's stuff

void createServerHtml(string hostName)
{
    FILE*  svr;
    FILE*  seedf;

    char bf[500];
    char nBuf[500];
    char tg[500];
    char systemCommand[1024];
    char serverName[100];
    size_t m,n;
    int reVal;
    string sipStat;
    string tempSipStat;
    string val;
    string va;
    string syLink;
    list<string> sipDat;
    list<string> sipLn;
	
    string hostn1;
    hostn1 = hostName;
	
    int hostfind = hostn1.find(".");
	
    hostn1 = hostn1.substr(0,(hostfind));
    // hostn1.erase(hostn1.find("."), ((hostn1.size()-hostfind)-1));

    
    tempSipStat = hostn1 + ".";
	
    sipStat = tempSipStat + "ack.html";
    syLink = hostn1 + "_sipAck";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "byte.html";
    syLink = hostn1 + "_sipStatsByte";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "cancel.html";
    syLink = hostn1 + "_sipStatsCancel ";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "invite.html";
    syLink = hostn1 + "_sipStatsInvite";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);
	
    sipStat = tempSipStat + "req.html";
    syLink = hostn1 + "_sipSummaryReq";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "reqfailclass.html";
    syLink = hostn1 + "_sipStatsReqFail ";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "res.html";
    syLink = hostn1 + "_sipSummaryResp";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);

    sipStat = tempSipStat + "successclass.html";
    syLink = hostn1 + "_sipStatsSuccessClass";
    sipDat.push_back(sipStat);
    sipLn.push_back(syLink);
    sprintf(systemCommand,LINK ,sipStat.c_str(),syLink.c_str());
    reVal = mySystem(systemCommand);
	
    strcpy(serverName,"/usr/local/vocal/tmp/mrtg/");
    strcat(serverName,hostn1.c_str());
    strcat(serverName,".html"); 
    svr = fopen( serverName,"w");
    if(!svr) {
	cout << "can't open input file/File may be existant";
	return;
    }
    else
    {
	seedf = fopen("/usr/local/vocal/tmp/mrtg/serverSeed.in","r");
	if(!seedf) 
	{
	    cout << "can't open input file";
	    return;
	}
	while(!feof(seedf))
	{
	    memset(bf, 0, sizeof(bf));
	    fgets(bf,512,seedf);

	    if(bf[0]=='$')
	    {
		for(m=1,n=0;m<strlen(bf);m++,n++)
		    nBuf[n] =  bf[m];
		nBuf[n] = '\0';
		fputs(nBuf,svr);
	    }
	    if(bf[0] == '@')
	    {
		if(!memcmp(bf,"@",strlen("@"))){

		    while(!sipDat.empty() && !sipLn.empty())
		    {
                        val = sipDat.front();
                        va = sipLn.front();
                        strcpy(tg,"<p><font color=\"#330000\"><a href=\"");
                        strcat(tg,val.c_str());
                        strcat(tg,"\"> ");
                        strcat(tg,va.c_str());
                        strcat(tg,"</a></font>");

                        fputs(tg,svr);
                        fputs("\n",svr);
                        sipDat.pop_front();
                        sipLn.pop_front();
		    }
		}
	    }
	}
    }
    fclose(svr);
    return;
}



void assignFuncUnits(vector < InstallUnit_T > &installConfig, vector < string > &hostNames, int numHosts, bool fuprovide, bool upgrade)
{
    InstallUnit_T aPair;
    div_t divRet;
    int i, j;
    InstallFU_T funcUnit;
    if ((!fuprovide) && (!upgrade))
    {
        if (numHosts <= NumOfFuncUnits)
        {
            for (funcUnit = (InstallFU_T)ENUMMIN;
                    funcUnit < MAX_FU;
                    funcUnit = (InstallFU_T)((int)funcUnit + 1))
            {
                aPair.funcUnit = funcUnit;
                divRet = div((int)funcUnit, numHosts);
                aPair.hostName = hostNames[divRet.rem];
                installConfig.push_back(aPair);
            }
        }
        else if (numHosts <= 2*NumOfFuncUnits)
        {
            // put 1 function unit on the first 5 hosts
            // put 1 function unit on the last 5 hosts.
            // the more heavy used applications will be on the ends so that
            // they have a greater chance of running alone on the host.
            for (i = numHosts - 1, j = 0, funcUnit = (InstallFU_T)((int)MAX_FU - 1);
                    funcUnit >= ENUMMIN;
                    i--, j++, funcUnit = (InstallFU_T)((int)funcUnit - 1))
            {
                aPair.funcUnit = funcUnit;
                aPair.hostName = hostNames[i];
                installConfig.push_back(aPair);

                // add the redundant Functional Unit
                aPair.funcUnit = funcUnit;
                aPair.hostName = hostNames[j];
                installConfig.push_back(aPair);
            }

        }
        else
        {
            // This time the number of servers is greater than the number of (FUs * 2) so
            // put in the redundant CDR_NET_VM & PS_PS (takes up 4 hosts)
            // then distribute the RS, MS, and FS equally between the remaining hosts.
            // todo:  weight the RS heaver than the MS and FS
            aPair.funcUnit = CDR_NET_VM;
            aPair.hostName = hostNames[0];
            installConfig.push_back(aPair);

            aPair.funcUnit = CDR_NET_VM;
            aPair.hostName = hostNames[1];
            installConfig.push_back(aPair);

            aPair.funcUnit = PS_PS;
            aPair.hostName = hostNames[2];
            installConfig.push_back(aPair);

            aPair.funcUnit = PS_PS;
            aPair.hostName = hostNames[3];
            installConfig.push_back(aPair);

            for (i = 4; i < numHosts; i++)
            {
                divRet = div(i, 3);
                switch ( divRet.rem )
                {
                    case 0:
                    aPair.funcUnit = MS;
                    break;
                    case 1:
                    aPair.funcUnit = RS;
                    break;
                    case 2:
                    aPair.funcUnit = FS_JS;
                    break;
                    default:
                    cout << "how did that happen";
                }
                aPair.hostName = hostNames[i];
                installConfig.push_back(aPair);
            }

        }


        installConfig = groupVectorByHostname(installConfig);


    }
}

string findCdr(vector<InstallUnit_T> installConfig)
{
   vector<InstallUnit_T>::iterator iv;
   string cdrHost;
   for (iv=installConfig.begin();iv<installConfig.end(); iv++)
   {
      if (iv->funcUnit == CDR_NET_VM)
      {
         cdrHost = iv->hostName.c_str();
      }
   }
   return cdrHost;
}


bool readAllInOneConfig (char *filename)
{
    
    ifstream AIOin(filename);
    char reader[512];
    char configParam[512];
    char configValue[512];
    int charCount=0, readCount=0, lineCount = 0;
    char AIOR[512];
    
    const string spacer = " ";
    
    memset(rlp,0, sizeof(rlp));
    memset(rcp, 0, sizeof(rcp));

    sprintf(rlp, "ssh");
    sprintf(rcp, "rcp");

    if (!AIOin || AIOin.bad())
    {
	cout << " Can not open file " << filename <<nl;
	cout      << "Please create the config file before installing." <<nl;
	return 0;
    }
    
    while (AIOin.getline(AIOR, sizeof(AIOR)))
    {
	memset(reader, 0, sizeof(reader));
	memset(configParam, 0, sizeof(configParam));
	memset(configValue, 0, sizeof(configValue));

	charCount = 0;
	readCount = 0;
	lineCount++;

	sprintf(reader,"%s", AIOR);

	if ((reader[0]!='#') && (reader[0]!='\n') && (reader[0] != '\0'))
	{
	    while ((reader[charCount]==' ')) 
	    {
		charCount = charCount+1;
	    }
	    
	    while ((reader[charCount]!= ' ') && (reader[charCount]!='\n') && (reader[charCount] != '\0') && reader[charCount] != '\t')
	    {
		configParam[readCount] = reader[charCount];
;
		readCount++;
		charCount++;
	    }

	    readCount = 0;
	    
	    while ((reader[charCount]==' ') || (reader[charCount]== '\t')) 
	    {
		charCount = charCount+1;
	    }
	    
	    if ((strcmp(configParam,"SIPPhoneNumber")==0) || (strcmp(configParam,"SIPPhoneIP")==0))
	    {
		while ((reader[charCount]!='\n') && (reader[charCount] != '\0'))
		{
		    if ((reader[charCount] != ' ') && (reader[charCount] != '\t'))
		    {
			configValue[readCount] = reader[charCount];

			readCount++;
		    }
		    charCount++;
		}
	    }
	    else
	    {
		while ((reader[charCount]!= ' ') && (reader[charCount]!='\n') && (reader[charCount] != '\0') && (reader[charCount] != '\t'))
		{
		    configValue[readCount] = reader[charCount];

		    readCount++;
		    charCount++;
		}
	    }

	    if (strcmp(configParam,"SoftswitchName") == 0)
	    {
	
		switchName = configValue;
	    }
	    
	    else if (strcmp(configParam,"PSTNGatewayIP") == 0)
	    {
		gatewayForMarshals.push_back(configValue);
	    }
	    
	    else if (strcmp(configParam,"PSTNAccessCode") == 0)
	    {
		GatewayCode = configValue;
	    }

	    else if (strcmp(configParam,"SIPPhoneNumber") == 0)
	    {
		SIPPhoneNumber.push_back(configValue);
	    }

	    else if (strcmp(configParam, "SIPPhoneIP") == 0)
	    {
		SIPPhoneIP.push_back(configValue);
	    }
	    
	    else if (strcmp(configParam, "HeartbeatMulticastIP") == 0)
	    {
		if ((strcmp(configValue, "") == 0) || (strcmp(configValue, " " ) == 0))
		{
		    memset(mcHost, 0, sizeof(mcHost));
		    sprintf(mcHost, "%s", DefaultMcHost);
		}
		else 
		{
		    memset(mcHost, 0, sizeof(mcHost));
		    sprintf(mcHost, "%s", configValue);
		}
	    }
	    
	    else if (strcmp(configParam, "HeartbeatMulticastPort") == 0)
	    {
		if ((strcmp(configValue, "") == 0) || (strcmp(configValue, " " ) == 0))
		{
		    memset(mcPort, 0, sizeof(mcPort));
		    sprintf(mcPort, "%s", DefaultMcPort);
		}
		else 
		{
		    memset(mcPort, 0, sizeof(mcPort));
		    sprintf(mcPort, "%s", configValue);
		}
	    }

	    else if (strcmp(configParam, "RemoteLoginProtocol") == 0)
	    {
		if (strcmp(configValue, "ssh") == 0)
		{
		    sprintf(rlp, "ssh");
		    sprintf(rcp, "scp");
		}
		else if (strcmp(configValue, "rsh") == 0)
		{
		    sprintf(rlp, "rsh");
		    sprintf(rcp, "rcp");
		}
		else if (strcmp(configValue, "ssh2") == 0)
		{
		    sprintf(rlp, "ssh2");
		    sprintf(rcp, "scp2");
		}
		
		else 
		{
		    cout << "Unsupported RemoteLoginProtocol -> " << configValue <<endl;
		    return false;
		}
	    }
	    
	    else if (strcmp(configParam, "VocalUser") == 0)
	    {
		vsUser = configValue;
	    }
		
	    else if (configParam != "")
	    {
		
		cout << "Invalid Configuration in Line Number : " << lineCount <<" : "<<reader <<endl;
		return false;
	    }
	}
    }
    
    if ((strcmp(switchName.c_str(), "") == 0) || (strcmp(switchName.c_str(), " ")==0))
    {
	switchName.assign(defaultSwitchName);
    }
    
    if (gatewayForMarshals.size() == 0)
    {
	gatewayForMarshals.push_back(defaultGatewayIp);
    }
    
    if (SIPPhoneNumber.size() != SIPPhoneIP.size())
    {
	cout << "Number of Phone Numbers does not correspond to number of Phone IPs."<< nl
	     << "Please check the config file again."<<nl;
	return false;
    }
    return true;
}

bool createUsers(char *phoneNumber, char *phoneIP, char *host)
{
    string seedFileRead;
    char temp[512];
    localMkdir("/usr/local/vocal/tmp/sipphoneusers");
    sprintf(temp, "/usr/local/vocal/tmp/sipphoneusers/%s", phoneNumber);

    ofstream userfile(temp);
    ifstream seedfile(UserSeedFile);
    
    if (!seedfile || seedfile.bad())
    {
	cout << " can not open file " << UserSeedFile << nl
	     << "Please check whether " << UserSeedFile << "exists." << nl;
	
	return false;
    }

    while (seedfile.getline(temp, sizeof(temp)))
    {
	seedFileRead = temp;
	
	if (seedFileRead.find("USERNAME")!= string::npos)
	{
	    seedFileRead.insert(seedFileRead.find("USERNAME"), phoneNumber);
	    seedFileRead.erase(seedFileRead.find("USERNAME"), strlen("USERNAME"));
	}
	
	if (seedFileRead.find("USERIP")!= string::npos)
	{
	    seedFileRead.insert(seedFileRead.find("USERIP"), phoneIP);
	    seedFileRead.erase(seedFileRead.find("USERIP"), strlen("USERIP"));
	}
	
	userfile << seedFileRead << nl;
    }
    
    

    return true;
}
	

bool makeStartOnRebootScript()
{
    
    ofstream sorScript(sorScriptFileName);
   
    sorScript << "#!/bin/bash" << nl
	      << "cp -f " << RemoteVocalBinDir <<"vocalstart "<<RemoteVocalStartupDir <<nl
	      << "j=75"<<nl
	      << "k=\"S\""<<nl
	      << "for i in `ls -d " << RemoteEtcDir <<"/rc3.d`" << nl
	      << "do" << nl
	      << "if ([ -d $i ] && [ ! -f $i/*vocalstart* ]); then" << nl
	      << "while [ -f $i/$k$j* ]; do" <<nl
	      << "j=`expr $j  + 1`"<<nl
	      << "done"<<nl
	      << "l=$i/$k"<<nl
	      << "l=$l$j"<<nl
	      << "l=$l\"vocalstart\""<<nl
	      << "ln -s "<<RemoteVocalStartupDir<<"/vocalstart $l" << nl
	      << "fi" <<nl
	      << "done"<<nl;
    
    sorScript.close();
    return true;
}

bool readHost(char *filename, vector <string> &hostName)
{
    char buff[512];
    ifstream inFile(filename);
    
    if (!inFile || inFile.bad())
    {
	cout << " can not open file " << hostNamesFile;
	return false;
    }
    
    memset(buff, 0, sizeof(buff));
    while (inFile.getline(buff, maxFileNameSize - 1))
    {
	if ((buff[0] != '\0') && (buff[0] != '#') && (buff[0] != '\n'))
	{
	    int remSpace = 0;
	    while ((buff[remSpace] == ' ') || (buff[remSpace] == '\t'))
	    {
		remSpace++;
	    }
	    if ((buff[remSpace]!= '\n') && (buff[remSpace]!='\0'))
	    {
		char readHost1[512];
		memset (readHost1, 0, sizeof(readHost1));
		int readHostIndex=0;
		
		while((buff[remSpace]!= '\n') 
		      && (buff[remSpace]!='\0') 
		      && (buff[remSpace] !='\t') 
		      && (buff[remSpace] != ' '))
		{
		    readHost1[readHostIndex] = buff[remSpace];
		    remSpace++;
		    readHostIndex++;
		}
		
		
		if (strcmp(readHost1, "RemoteLoginProtocol") == 0)
		{
		    memset(rlp, 0, sizeof(rlp));
		    while ((buff[remSpace] == ' ') || (buff[remSpace] == '\t'))
		    {
			remSpace++;
		    }
		    int rlpRead = 0;
		   
		    while((buff[remSpace]!= '\n') 
			  && (buff[remSpace]!='\0') 
			  && (buff[remSpace] !='\t') 
			  && (buff[remSpace] != ' '))
		    {
		  
			rlp[rlpRead] = buff[remSpace];
			remSpace++;
			rlpRead++;
		    }

		    if (strcmp(rlp, "ssh")== 0)
		    {
			sprintf(rcp, "scp");
		    }
		    if (strcmp(rlp, "ssh2")== 0)
		    {
			sprintf(rcp, "scp2");
		    }
		    if (strcmp(rlp, "rsh")==0)
		    {
			sprintf(rcp, "rcp");
		    }
		    
		    if ((strcmp(rlp, "")==0) || (strcmp(rlp, " ")==0))
		    {
			sprintf(rlp, "ssh");
		    }
		}
		
		else if (!HostnameIn(readHost1, hostName))
		{
		    hostName.push_back(readHost1);
		}
	    }
	}
	memset(buff, 0, sizeof(buff));
    }
    inFile.close();
    return true;
}
    
bool findPS(vector<InstallUnit_T> sysVector)
{
    vector <InstallUnit_T>::iterator sysVectorIter;
    for (sysVectorIter=sysVector.begin(); sysVectorIter < sysVector.end() ; sysVectorIter++)
    {
	if (sysVectorIter->funcUnit == PS_PS)
	{
	    if (strcmp(psHost1.c_str(), "") == 0)
	    {
		psHost1 = sysVectorIter->hostName;
	    }
	    else if (strcmp(psHost2.c_str(), "") == 0)
	    {
		psHost2 = sysVectorIter->hostName;
	    }
	}
    }
    return true;
}


enum RemoteSystemType
{
    Solaris,
    Linux
};




class InstallableFiles
{
    public:
        InstallableFiles() : myType(Generic) { }
        string name;
        PackageType type;
};


bool installOnRemoteServerRPM(string remoteSystem, RemoteSystemType sysType, 
                              InstallableFiles* local)
{
    char sysCommand[maxFileNameSize];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "%s %s %s:/tmp", 
            rcp, local->file.c_str(), remoteSystem->c_str());
    if (mySystem(sysCommand) != 0)
    {
        cout << "ERROR: can not create RPM on " 
             << remoteSystem->c_str() 
             << " in /tmp" 
             << "  System partially  deployed" 
             << nl;
    }
    else
    {
        memset(sysCommand, 0, sizeof(sysCommand));
        // in the case the user gives a full path name
        string rpmFileName 
            = local->file.substr(rpmfile.rfind("/") + 1);
        sprintf(sysCommand, 
                "%s %s \"rpm -U --force --nodeps --prefix %s /tmp/%s\"", 
                rlp, remoteSystem->c_str(), 
                RemoteVocalBinDir, rpmFileName.c_str());
        if (mySystem(sysCommand) != 0)
        {
            cout << "ERROR:   can not install RPMs on remote machine"
                 << remoteSystem->c_str() 
                 << "      System partially  deployed" 
                 << nl;
        }
    }
}		


bool installOnRemoteServerTar(string remoteSystem, RemoteSystemType sysType, 
                              InstallableFiles* local)
{
    char sysCommand[maxFileNameSize];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "%s %s %s:/tmp", rcp, tarfile.c_str(), remoteSystem->c_str());
    if (mySystem(sysCommand) != 0)
    {
        cout << "ERROR: can not create tar file on " 
             << remoteSystem->c_str() 
             << " in /tmp" 
             << "  System partially  deployed" 
             << nl;
    }
    else
    {
        string tarFileName = tarfile.substr(tarfile.rfind("/") + 1);
        memset(sysCommand, 0, sizeof(sysCommand));
        if (tarfileGzipped(tarfile))
        {
            sprintf(sysCommand, 
                    "%s %s '(cd %s; tar xzf /tmp/%s)'",
                    rlp, remoteSystem->c_str(), 
                    RemoteVocalBinDir, 
                    tarFileName.c_str());
        }
        else
        {
            sprintf(sysCommand, 
                    "%s %s '(cd %s; tar xf /tmp/%s)'",
                    rlp, 
                    remoteSystem->c_str(), 
                    RemoteVocalBinDir, 
                    tarFileName.c_str());
        }
        if (mySystem(sysCommand) != 0)
        {
            cout << "ERROR:   can not untar binary tar file on remote machine" 
                 << remoteSystem->c_str()
                 << "      System partially  deployed" 
                 << nl;
        }
			    
    }
}


bool installOnRemoteServerGeneric(string remoteSystem, RemoteSystemType sysType)
{
    char sysCommand[maxFileNameSize];
    memset(sysCommand, 0, sizeof(sysCommand));
    if (sysType == Solaris)
    {
        sprintf(sysCommand, "%s /usr/local/vocal/tmp/bin.tar %s:/tmp", 
                rcp, 
                remoteSystem->c_str() );
    }
    else
    {
        sprintf(sysCommand, "%s /usr/local/vocal/tmp/bin.tar.gz %s:/tmp", 
                rcp, 
                remoteSystem->c_str() );
    }

    if (mySystem(sysCommand) != 0)
    {
        cout << "ERROR: can not create binaries on " 
             << remoteSystem->c_str() 
             << "  System partially  deployed";
    }
    else
    {
        memset(sysCommand, 0, sizeof(sysCommand));
        if (sysType == Solaris)
        {
            sprintf(sysCommand, "%s %s '(cd %s; tar xf /tmp/bin.tar)'",
                    rlp, remoteSystem->c_str(), RemoteVocalBinDir);
        }
        else
        {
            sprintf(sysCommand, "%s %s '(cd %s; tar xzf /tmp/bin.tar.gz)'",
                    rlp, remoteSystem->c_str(), RemoteVocalBinDir);
        }
        if (mySystem(sysCommand) != 0)
        {
            cout << "ERROR:   can not untar binaries on remote machine" << remoteSystem->c_str()
                 << "      System partially  deployed" << nl;
        }
    }
}



bool installOnRemoteSystem(string remoteSystem, 
                           RemoteSystemType sysType, 
                           PackageType pkgType)
{
    switch (pkgType)
    {
    case RPM:
        return installOnRemoteServerRPM(remoteSystem, remoteSystemType);
        break;
    case Tar:
        return installOnRemoteServerTar(remoteSystem, remoteSystemType);
        break;
    case Generic:
        return installOnRemoteServerGeneric(remoteSystem, remoteSystemType);
        break;
    }
}


int main( int argc, const char **argv)
{
    char sysCommand[maxFileNameSize];
    char destFileName[maxFileNameSize];
    char buff1[512];
    char seeder[512];
    char HostConfig[512];
    char buff[maxFileNameSize];

    InstallableFiles installFile;

    vector < string > hostNames;  // list of the hostname to deploy the system
    vector < string > ::iterator aName;
    int numHosts = 0;
    vector < InstallUnit_T > installConfig;  // a list of the functional units (ie. servers) and the servers they go onto
    vector < InstallUnit_T > ::iterator iv;
    char tmpStr[1024];

    memset(h323MsIpPort, 0, sizeof(h323MsIpPort));
    memset(rlp, 0, sizeof(rlp));
    memset(rcp, 0, sizeof(rcp));
    
    sprintf(rlp, "ssh");
    sprintf(rcp, "scp");

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
                case 'u':
		    upgrade = true;
		    break;
                case 'f' :
		    fuprovide = true;
		    hostNamesFile.assign(argv[i + 1]);
		    if (hostNamesFile == "")
		    {
			cout << " No Host Filename Provided. Assuming Default !!\n";
			hostNamesFile.assign("./hosts");
		    }
		    break;
                case 'r' :
		    installFile.name.assign(argv[i + 1]);
		    installFile.type = RPM;
		    if (installFile.name == "")
		    {
			cout << " No RPM Filename Provided." << nl;
			conformance();
			return 0;
		    }
		    break;
                case 'T' :
		    installFile.name.assign(argv[i + 1]);
		    installFile.type = Tar;
		    if (installFile.name == "")
		    {
			cout << " No tar file provided." << nl;
			conformance();
			return 0;
		    }
		    break;
                case 't' :
		    myTest = true;
		    break;
		    
                case 'c' :
		    configonly = true;
		    break;
		case 'a' :
		    allinone = true;
		    if (!readAllInOneConfig("install.cfg"))
		    {
			return 0;
		    }
		    break;
		case 'v' :
		    verbose = true;
		    break;
                default:
		    conformance();
		    return 0;
            }
        }
    }

    if (!queryUser(upgrade, fuprovide, installFile.type, allinone))
    {
        cout << " System not deployed " << nl;
        return 0;
    }


    // read the host machine list
   
    if (fuprovide)
    {
	string oldhostname = "";
	char hostfilename[512];
	sprintf(hostfilename, "%s", hostNamesFile.c_str());
	installConfig = HostfileRead(hostfilename);
	hostNames = getHostNames(installConfig);
	
    }
    else
    {
	char tempHostFileName[512];
	memset(tempHostFileName, 0, sizeof(tempHostFileName));
	sprintf(tempHostFileName, "%s", hostNamesFile.c_str());

	readHost(tempHostFileName, hostNames);
#if 0
        if (!inFile || inFile.bad())
	{
	    cout << " can not open file " << hostNamesFile;
	    return 0;
	}
	memset(buff, 0, sizeof(buff));
	while (inFile.getline(buff, maxFileNameSize - 1))
	{
	    if ((buff[0] != '\0') && (buff[0] != '#') && (buff[0] != '\n'))
	    {
		int remSpace = 0;
		while ((buff[remSpace] == ' ') || (buff[remSpace] == '\t'))
		{
		    remSpace++;
		}
		if ((buff[remSpace]!= '\n') && (buff[remSpace]!='\0'))
		{
		    char readHost[512];
		    memset (readHost, 0, sizeof(readHost));
		    int readHostIndex=0;
		    while((buff[remSpace]!= '\n') 
			  && (buff[remSpace]!='\0') 
			  && (buff[remSpace] !='\t') 
			  && (buff[remSpace] != ' '))
		    {
			readHost[readHostIndex] = buff[remSpace];
			remSpace++;
			readHostIndex++;
		    }
		    if (!HostnameIn(readHost, hostNames))
		    {
			hostNames.push_back(readHost);
		    }
		}
            }                                
	}
	inFile.close(); 
#endif

    }
    
    
    

    numHosts = hostNames.size();
    if (!numHosts)
    {
        cout << "no hostname in hostname file " << hostNamesFile;
        return 0;
    }

    testRemoteInstallable();


    // create a provisioning scheme
    // There are 3 types of schemes based on number of servers
    // 1) 1-5 basic functionality.  1 of each functional unit, distributed evenly
    // 2) 6-10 redundancy. 2 of each functional unit, distributed evenly
    // 3) 11+ performance. add more MS/FS/RS to #3. keep other functional units at 2.
    // The functional units are:
    //    FS_JS
    //    RS
    //    3 MS (1-ua, 1-gateway, 1-conference)
    //    1 PS & 1 Policy Server
    //    1 CDR & 1 NetMgnt(+snmptrapd) & 1 FBVM
    // all hosts receive an snmpd

    // todo: better port assignment.  make sure every box has a 5060 then extra
    // things get different ports.

    assignFuncUnits(installConfig, hostNames, numHosts, fuprovide, upgrade);

    if (!upgrade)
    {
        assignSipPorts(installConfig);
        assignParms(installConfig);
    }

    cout << nl << "*********************************************";
    cout << nl << "Provisioning Configuration \n";
    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
        cout << iv->hostName 
             << sp 
             << printFU(iv->funcUnit) 
             << sp 
             << iv->port 
             << nl;
    }
    cout << "*********************************************" << nl;

    bool wantexec = true;

    if (!configonly)
    {
	if ( !allinone)
	{
	    if (!upgrade)
	    {
		cout << "Do you want to reload the  vocal softswitch executables and configuration data on the remote systems.  This could take a long time (y/n) [y]: " << nl;
	    }
	    else
	    {
		cout << "Do you want to reload the  vocal softswitch executables.  This could take a long time (y/n) [y]: " << nl;
	    }
	    
	    cin.getline(buff, 512, '\n');
	    if ((!strlen(buff) == 0) && (!strcasecmp(buff, "y")) && (!strcasecmp(buff, "yes") )) 
	    {
		cout << nl << "No remote software download done" << nl;
		wantexec = false;
	    }
}	
	    if ((strlen(buff) == 0 || strcasecmp(buff, "y") || strcasecmp(buff, "yes") ) || (allinone))
	    {
		wantexec = true;
		if (my (!userpm) && (!usetar))
		{
		    // create the local tmp directory
		    localMkdir( LocalVocalTmpDir);

		    cout << "create tar of bin files" << nl;
		    memset(sysCommand, 0, sizeof(sysCommand));
		    
		    if (solarisOS)
		    {
			sprintf(sysCommand, "cd %s; tar cvf /usr/local/vocal/tmp/bin.tar *", binSourceDir.c_str() );
		    }
		    else
		    {
			sprintf(sysCommand, "cd %s; tar cvzf /usr/local/vocal/tmp/bin.tar.gz *", binSourceDir.c_str() );
		    }
		    
		    if (system(sysCommand) != 0)
		    {
			cout << "can not make local tar of dir " << binSourceDir << "  System not deployed";
			return 0;
		    }
		    
		}
		
		// Make the startOnReboot script

                // and then deploy the
		// binaries and then run the script
		
		makeStartOnRebootScript();
		
		for (aName = hostNames.begin(); aName < hostNames.end(); aName++)
		{
		    cout << nl 
                         << " pushing software onto " 
                         << aName->c_str() 
                         << "  (this takes a while)" 
                         << nl;
		    
		    // if using rpms, copy and install RPMs. For lab use copy from the file system.


                    PackageType pkgType;

                    if (userpm)
                    {
                        pkgType = RPM;
                    }
                    else if (usetar)
                    {
                        pkgType = Tar;
                    }   
                    else
                    {
                        pkgType = Generic;
		    }

                    SystemType sysType;
                    sysType = Linux;
                    if(SolarisOS)
                    {
                        sysType = Solaris;
                    }

                    installOnRemoteSystem(aName, sysType, pkgType);


                    // i don't know what this does

		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s 'cp -f %s/.rev %s'", 
			    rlp, aName->c_str(), RemoteVocalBinDir, RemoteVocalEtcDir);
		    mySystem(sysCommand,true);


                    

                    // this copies the startOnReboot script to the
                    // remote host.  it should be part of the custom
                    // configuration tarball

		    
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s %s:/tmp",rcp,sorScriptFileName, aName->c_str());
		    if (mySystem(sysCommand) != 0)
		    {
			cout << "ERROR:   can not copy local file " << sorScriptFileName << " to remote machine" << aName->c_str()
			     << "      System partially  deployed" << nl;
		    }
		    
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s \"chmod +x %s; %s\"",
			    rlp, aName->c_str(), sorScriptFileName, sorScriptFileName);

		    if (mySystem(sysCommand)!=0)
		    {
			
		    }

                }
            }
            
    }





    if (!upgrade)
    {
        // delete all configuration files
        // create provisioning scheme, present it to the user
        // Create provisioning_data on pserver system
        // configure appache startup on pserver
        // put  /etc/rc.d/init.d/vocalstart into place
        // create /usr/local/vocal/etc/vocal.con based on provisioning scheme
        // copy /home/cvs/binary/bin.current/stage/install.sh to remote machine and run it.
        // reboot remote machine


    }
	    
    else	    
    { // if (!upgrade)
        // find current version on system then run the upgrade scripts
        // to bring it's configuration up to date
        cout << nl << "\t\tThank You!!!" << nl << nl;
    }
}


/* Local Variables: */
/* c-file-style: "stroustrup" */
/* indent-tabs-mode: nil */
/* c-file-offsets: ((access-label . -) (inclass . ++)) */
/* c-basic-offset: 4 */
/* End: */

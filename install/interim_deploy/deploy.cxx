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
    "$Id: deploy.cxx,v 1.1 2004/05/01 04:15:16 greear Exp $";

#include "deploy.h"



void conformance()
{
    cout << "deploy [-u] [-r <filename>] [-f <filename>]\n";
    cout << "\tu:\tperform upgrade.  Replace executable files. \n"
    << "\t\tRetain provisioning, network Management, billing etc... data\n"
    << "\tr:\tUser Rpm file for binaries.\n"
    << "\tf:\tRead system configuration from file.  Do not invoke auto-configuration\n\n"
    << "\tv:\tverbose - echo commands to stdout." << endl;

}


bool upgrade = false;
bool configonly = false;
bool fuprovide = false;
bool userpm = false;
bool myTest = false;
bool usetar = false;


// TODO:  FIND REMOTE HTML DIR.  DON'T HARD CODE IT.



char h323MsIpPort [100];









int readGatewayFile(const char *inVal, UserEnteredData userData)
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
            userData.gatewayForMarshals.push_back(buff);
    }
    inFile.close();
    return true;
}


string queryWithDefault(const char* prompt, const string& defaultValue)
{
    char buf[512];
    cout << nl 
	 << prompt 
	 << " ["
	 << defaultValue
	 << "]:"
	 << nl;
    cin.getline(buf, 512, '\n');
    if(strlen(buf) == 0)
    {
	return defaultValue;
    }
    else
    {
	return buf;
    }
}


bool queryYNWithDefault(const char* prompt, bool defaultValue)
{
    while(1)
    {
	char buf[512];
	cout << nl 
	     << prompt 
	     << " (y/n) ["
	     << (defaultValue ? "y" : "n")
	     << "]:"
	     << nl;
	cin.getline(buf, 512, '\n');
	
	if (strlen(buf) == 0)
	{
	    return defaultValue;
	}
	else if (!strcasecmp(buf, "n") || !strcasecmp(buf, "no") )
	{
	    return false;
	}
	else if (!strcasecmp(buf, "y") || !strcasecmp(buf, "yes") )
	{
	    return true;
	}
    }
}


// this function queries the user for data that is deployment specific.
// the results are place in the above global variables
int queryUser(
    bool upgrade, 
    bool fuprovide, 
    bool userpm, 
    bool usetar, 
    bool allinone,
    UserEnteredData userData
    )
{
    if (!allinone)
    {
	if (!fuprovide)
	{
	    userData.hostNamesFile = 
		queryWithDefault(
		    "Enter filename containing list of system (non-sua) hosts", 
		    defaultHostNamesFile);
	}

	if ((!userpm) && (!usetar))
	{
	    userData.binSourceDir =
		queryWithDefault(
		    "Enter directory containing the executables to load",
		    defaultBinSourceDir);
	}
	
	if (!upgrade)
	{
	    userData.switchName =
		queryWithDefault(
		    "Enter name of your classXswitch system",
		    defaultSwitchName);

	    userData.switchName =
		queryWithDefault(
		    "Enter hostname or IP of billing system",
		    defaultBillingHost);

	    userData.switchName =
		queryWithDefault(
		    "Enter billing SecretKey",
		    defaultSecretKey);

	    if(queryYNWithDefault("Is there more than 1 Gateway for phones", 
				  false))
	    {
		userData.gatewayForMarshals.push_back(
		    queryWithDefault("Enter ip address for the gateway",
				     defaultGatewayIp));
	    }
	    else
	    {
		int retVal;
		string gatewayFilename;
		gatewayFilename =
		    queryWithDefault(
			"Enter file name containing list of gateways",
			defaultGatewayNamesFile);

		retVal = readGatewayFile(gatewayFilename.c_str(), userData);

		if (!retVal)
		{
		    cout << "Failure reading input Gateway file." << nl;
		    return false;
		}
		
	    }
	    
	    userData.vsUser =
		queryWithDefault(
		    "Enter the user id who will own provisioning files and start pserver",
		    DefaultUser);



	    userData.mcHost =
		queryWithDefault(
		    "Enter multicast address for Heartbeat",
		    DefaultMcHost);

	    userData.mcPort =
		queryWithDefault(
		    "Enter multicast port for Heartbeat",
		    DefaultMcPort);

	    userData.psMcHost =
		queryWithDefault(
		    "Enter multicast address for Provisioning Sever",
		    DefaultPsMcHost);

	    userData.psMcPort =
		queryWithDefault(
		    "Enter multicast port for Provisioning Sever",
		    DefaultPsMcPort);
	}
    }
    else
    {
	userData.binSourceDir = ".";
	system ("hostname > allinonehost");
	userData.hostNamesFile = "allinonehost";
    }
	
    return true;
}


bool readAllInOneConfig (
    char *filename,
    UserEnteredData userData
    )
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
	
		userData.switchName = configValue;
	    }
	    
	    else if (strcmp(configParam,"PSTNGatewayIP") == 0)
	    {
		userData.gatewayForMarshals.push_back(configValue);
	    }
	    
	    else if (strcmp(configParam,"PSTNAccessCode") == 0)
	    {
		aioConfig.GatewayCode = configValue;
	    }

	    else if (strcmp(configParam,"SIPPhoneNumber") == 0)
	    {
		aioConfig.SIPPhoneNumber.push_back(configValue);
	    }

	    else if (strcmp(configParam, "SIPPhoneIP") == 0)
	    {
		aioConfig.SIPPhoneIP.push_back(configValue);
	    }
	    
	    else if (strcmp(configParam, "HeartbeatMulticastIP") == 0)
	    {
		if ((strcmp(configValue, "") == 0) || (strcmp(configValue, " " ) == 0))
		{
		    userData.mcHost = DefaultMcHost;
		}
		else 
		{
		    userData.mcHost = configValue;
		}
	    }
	    
	    else if (strcmp(configParam, "HeartbeatMulticastPort") == 0)
	    {
		if ((strcmp(configValue, "") == 0) || (strcmp(configValue, " " ) == 0))
		{
		    userData.mcPort = DefaultMcPort;
		}
		else 
		{
		    userData.mcPort = configValue;
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
		userData.vsUser = configValue;
	    }
		
	    else if (configParam != "")
	    {
		
		cout << "Invalid Configuration in Line Number : " << lineCount <<" : "<<reader <<endl;
		return false;
	    }
	}
    }
    
    if ((strcmp(userData.switchName.c_str(), "") == 0) || (strcmp(userData.switchName.c_str(), " ")==0))
    {
	userData.switchName.assign(defaultSwitchName);
    }
    
    if (userData.gatewayForMarshals.size() == 0)
    {
	userData.gatewayForMarshals.push_back(defaultGatewayIp);
    }
    
    if (aioConfig.SIPPhoneNumber.size() != aioConfig.SIPPhoneIP.size())
    {
	cout << "Number of Phone Numbers does not correspond to number of Phone IPs."<< nl
	     << "Please check the config file again."<<nl;
	return false;
    }
    return true;
}


int main( int argc, const char **argv)
{
    UserEnteredData userData;
    string rpmfile;
    string tarfile;
    bool allinone = false;

    vector < string > hostNames;  // list of the hostname to deploy the system
    int numHosts = 0;

    vector < InstallUnit_T > installConfig;  // a list of the functional units (ie. servers) and the servers they go onto


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
		    userData.hostNamesFile.assign(argv[i + 1]);
		    if (userData.hostNamesFile == "")
		    {
			cout << " No Host Filename Provided. Assuming Default !!\n";
			userData.hostNamesFile.assign("./hosts");
		    }
		    break;
                case 'r' :
		    userpm = true;
		    rpmfile.assign(argv[i + 1]);
		    if (rpmfile == "")
		    {
			cout << " No RPM Filename Provided." << nl;
			conformance();
			return 0;
		    }
		    break;
                case 'T' :
		    usetar = true;
		    tarfile.assign(argv[i + 1]);
		    if (tarfile == "")
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
		    if (!readAllInOneConfig("install.cfg", userData))
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

    if (!queryUser(upgrade, fuprovide, userpm, usetar, allinone, userData))
    {
        cout << " System not deployed " << nl;
        return 0;
    }


    if(!get_hostmachines(fuprovide, 
			 userData.hostNamesFile, 
			 installConfig, 
			 hostNames, 
			 numHosts))
    {
	return -1;
    }

    if(!test_remoteinstall(upgrade, myTest, numHosts, hostNames))
    {
	return -1;
    }

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

    bool wantexec;

    if(!install_executables(configonly, 
			    allinone, 
			    upgrade, 
			    userpm, 
			    usetar, 
			    userData.binSourceDir, 
			    hostNames, 
			    rpmfile, 
			    tarfile, 
			    wantexec))
    {
	return -1;
    }
    build_mrtg(installConfig, hostNames);

    char h323MsIpPort[512];

    print_config(installConfig, allinone);

    assign_units(installConfig, 
		 hostNames,  
		 h323MsIpPort,
		 userData.billingHost,
		 userData.gatewayForMarshals,
		 numHosts,
		 fuprovide,
		 upgrade, 
		 allinone);


    if (!upgrade)
    {
        // delete all configuration files
        // create provisioning scheme, present it to the user
        // Create provisioning_data on pserver system
        // configure appachee startup on pserver
        // put  /etc/rc.d/init.d/vocalstart into place
        // create /usr/local/vocal/etc/vocal.con based on provisioning scheme
        // copy /home/cvs/binary/bin.current/stage/install.sh to remote machine and run it.
        // reboot remote machine


	build_uavm( upgrade,  
		    allinone,  
		    wantexec,  
		    configonly,  
		    installConfig);

	if (!build_provdata(installConfig, 
			    hostNames, 
			    h323MsIpPort,
			    allinone, 
			    userData))
	{
	    return -1;
	}
	if(!install_provdata(installConfig, hostNames))
	{
	    return -1;
	}

	char vocalXmlSubDir[512] = "vocal" ;
	build_apache(allinone, 
		     installConfig, 
		     vocalXmlSubDir, 
		     userData.switchName);
	build_userdata(installConfig, hostNames, userData.vsUser);
	restart_boxes(allinone, installConfig);
	if(!run_userconfig(installConfig, allinone, vocalXmlSubDir ))
	{
	    return -1;
	}
    }
	    
    else	    
    { // if (!upgrade)
        // find current version on system then run the upgrade scripts
        // to bring it's configuration up to date
        cout << nl << "\t\tThank You!!!" << nl << nl;
    }
}

#include "deploy.h"


int
makeDeployedServersFile(
    vector < InstallUnit_T > installConfig, 
    bool allinone, 
    string mcHost, 
    string mcPort,
    string billingSecretKey
    )
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
	    << "        multicastHost=" << dq << mcHost << dq <<nl
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

// This function will create the files for the /usr/local/vocal/etc directory
// right now we assume there is only 1 PS running on the system.
int makeLocalEtcFiles (
    vector < InstallUnit_T > installConfig, 
    vector <string> hostN, 
    const char* h323MsIpPort,
    bool allinone, 
    string vsUser,
    string psMcHost,
    string psMcPort
)
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


int build_provdata(
    const vector < InstallUnit_T >& installConfig, 
    const vector < string > &hostNames, 
    const char* h323MsIpPort,
    bool allinone, 
    const UserEnteredData& myUserData
    )
{
    char sysCommand[maxFileNameSize];
    // create provisioning data save it to the proper files
    //      if (wantexec) {
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "rm -fr %s", LocalProvDataDir );
    if (mySystem(sysCommand) != 0)
    {
	cout << " can not remove directory tree " << LocalProvDataDir << "  System not deployed" << nl;
	return 0;
    }

    if (localMkdir(LocalProvDataDir) == false)
    {
	cout << "  System not deployed" << nl;
	return 0;
    }


    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "rm -fr %s", LocalVocalEtcDir );
    if (mySystem(sysCommand) != 0)
    {
	cout << " can not remove directory tree " << LocalVocalEtcDir << "  System not deployed" << nl;
	return 0;
    }

    if (localMkdir(LocalVocalEtcDir) == false)
    {
	cout << "  System not deployed" << nl;
	return 0;
    }

    if (makeDeployedServersFile(installConfig, 
				allinone, 
				myUserData.mcHost, 
				myUserData.mcPort,
				myUserData.billingSecretKey) == false)
    {
	cout << "system not deployed" << nl;
	return 0;
    }

    if (makeLocalEtcFiles(installConfig, hostNames, 
			  h323MsIpPort,
			  allinone, 
			  myUserData.vsUser, 
			  myUserData.psMcHost, 
			  myUserData.psMcPort) == false)
    {
	cout << "system not deployed" << nl;
	return 0;
    }
    return 1;
}

#include "deploy.h"

// this function will return a copy of the vector "inVector" grouped
// by hostName
vector < InstallUnit_T > groupVectorByHostname( 
    vector < InstallUnit_T > inVector
    )
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
void assignParms(
    vector < InstallUnit_T > &inVector, 
    bool allinone, 
    char* h323MsIpPort,
    string billingHost,
    vector <string> gatewayForMarshals
    )
{

    memset(h323MsIpPort, 0, sizeof(h323MsIpPort));

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


void     assign_units(
    vector < InstallUnit_T > &installConfig, 
    vector < string > &hostNames, 
    char* h323MsIpPort,
    const string& billingHost,
    const vector <string>& gatewayForMarshals,
    int numHosts, 
    bool fuprovide, 
    bool upgrade, 
    bool allinone
    )
{
    assignFuncUnits(installConfig, hostNames, numHosts, fuprovide, upgrade);
    if (!upgrade)
    {
        assignSipPorts(installConfig);
        assignParms(installConfig, 
		    allinone, 
		    h323MsIpPort, 
		    billingHost,
		    gatewayForMarshals
	    );
    }
}



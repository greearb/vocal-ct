#include "deploy.h"

string printFU(InstallFU_T type, bool allinone)
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

void print_config(const vector < InstallUnit_T >& installConfig, bool allinone)
{
     vector < InstallUnit_T > ::const_iterator iv;
    
    cout << nl << "*********************************************";
    cout << nl << "Provisioning Configuration \n";
    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
        cout << iv->hostName << sp << printFU(iv->funcUnit, allinone) << sp << iv->port << nl;

    }
    cout << "*********************************************" << nl;
}


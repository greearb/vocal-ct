#include "deploy.h"

int install_provdata(
    const vector < InstallUnit_T >& installConfig, 
    const vector < string > &hostNames
    )
{
    char sysCommand[maxFileNameSize];
    vector < InstallUnit_T > ::const_iterator iv;
    vector < string > ::const_iterator aName;

    // tar up the directory and ship it to it's destinations
    // cd /tmp; tar czf prov.tar.gz provisioning_data
    // use system so the user can see the status of the tar
    cout << "create tar of provisioning data" << nl;
    memset(sysCommand, 0, sizeof(sysCommand));
    if (solarisOS)
    {
	sprintf(sysCommand, "cd %s; cd ../; tar cvf /usr/local/vocal/tmp/pd.tar provisioning_data", LocalProvDataDir );
    }
    else
    {
	sprintf(sysCommand, "cd %s; cd ../; tar cvzf /usr/local/vocal/tmp/pd.tar.gz provisioning_data", LocalProvDataDir );
    }
    if (mySystem(sysCommand) != 0)
    {
	cout << "can not make local tar of dir " << LocalProvDataDir << "  System not deployed";
	return 0;
    }


    // scp /tmp/etc.tar.gz host:
    // ssh hostname '(cd /usr/local/vocal; tar xzf etc.tar.gz)'
    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
	if (iv->funcUnit == PS_PS)
	{

	    cout << "downloading provisioning configuration to " << iv->hostName.c_str() << nl;
	    memset(sysCommand, 0, sizeof(sysCommand));
	    if (solarisOS)
	    {
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/pd.tar %s:/tmp", rcp, iv->hostName.c_str());
	    }
	    else
	    {
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/pd.tar.gz %s:/tmp", rcp, iv->hostName.c_str());
	    }
	    if (mySystem(sysCommand, true) != 0)
	    {
		cout << "ERROR:  can not copy provisioning data to  " << iv->hostName.c_str() << "System partially  deployed" << nl;
	    }
	    else
	    {
		memset(sysCommand, 0, sizeof(sysCommand));
		if (solarisOS)
		{
		    sprintf(sysCommand, "%s %s '(cd %s; tar xf /tmp/pd.tar)'",
			    rlp, iv->hostName.c_str(), RemoteProvDataDir);
		}
		else
		{
		    sprintf(sysCommand, "%s %s '(cd %s; tar xzf /tmp/pd.tar.gz)'",
			    rlp, iv->hostName.c_str(), RemoteProvDataDir);
		}
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not untar /tmp/pd.tar.gz on remote machine" << iv->hostName.c_str()
			 << "      System partially  deployed" << nl;
		}
	    }
	}
    }

    for (aName = hostNames.begin(); aName < hostNames.end(); aName++)
    {
	cout << "Downloading etc startup configuration  to " << aName->c_str() << nl;
	sprintf(sysCommand, "%s %s/vocal.conf.%s %s:%s/vocal.conf",
		rcp, LocalVocalEtcDir, aName->c_str(), aName->c_str(), RemoteVocalEtcDir );
	if (mySystem(sysCommand) != 0)
	{
	    cout << "ERROR:   can not copy vocal.conf onto remote machine" << aName->c_str()
		 << "      System partially  deployed" << nl;
	}
    }
    return 1;
}

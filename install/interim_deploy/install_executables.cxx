#include "deploy.h"


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


bool tarfileGzipped (string tarfile)
{
    bool gzip = false;
    if (((tarfile.rfind("gz")) == (tarfile.size() - 2)) || ((tarfile.rfind("tgz")) == (tarfile.size()-3)))
    {
        gzip = true;
    }
    return gzip;
}


int install_executables(bool configonly, bool allinone, bool upgrade, bool userpm, bool usetar, string binSourceDir,     const vector < string >& hostNames, string rpmfile, string tarfile, bool& wantexec)
{
    vector < string > ::const_iterator aName;
    char sysCommand[maxFileNameSize];
    char buff[513];
    wantexec = true;

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
	    if ((!userpm) && (!usetar))
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
		
	    // Make the startOnReboot script and then deploy the binaries and then run the script
		
	    makeStartOnRebootScript();


	    // do the actual installation
	    for (aName = hostNames.begin(); aName < hostNames.end(); aName++)
	    {
		cout << nl << " pushing software onto " << aName->c_str() << "  (this takes a while)" << nl;
		    
		// if using rpms, copy and install RPMs. For lab use copy from the file system.
		    
		if (userpm)
		{
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s %s:/tmp", rcp, rpmfile.c_str(), aName->c_str());
		    if (mySystem(sysCommand) != 0)
		    {
			cout << "ERROR: can not create RPM on " << aName->c_str() << " in /tmp" << "  System partially  deployed" << nl;
		    }
		    else
		    {
			memset(sysCommand, 0, sizeof(sysCommand));
			// in the case the user gives a full path name
			string rpmFileName = rpmfile.substr(rpmfile.rfind("/") + 1);
			sprintf(sysCommand, "%s %s \"rpm -U --force --nodeps --prefix %s /tmp/%s\"", rlp, aName->c_str(), RemoteVocalBinDir, rpmFileName.c_str());
			if (mySystem(sysCommand) != 0)
			{
			    cout << "ERROR:   can not install RPMs on remote machine"
				 << aName->c_str() << "      System partially  deployed" << nl;
			}
		    }
		}
		else if (usetar)
		{
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s %s:/tmp", rcp, tarfile.c_str(), aName->c_str());
		    if (mySystem(sysCommand) != 0)
		    {
			cout << "ERROR: can not create tar file on " << aName->c_str() << " in /tmp" << "  System partially  deployed" << nl;
		    }
		    else
		    {
			string tarFileName = tarfile.substr(tarfile.rfind("/") + 1);
			memset(sysCommand, 0, sizeof(sysCommand));
			if (tarfileGzipped(tarfile))
			{
			    sprintf(sysCommand, "%s %s '(cd %s; tar xzf /tmp/%s)'",
				    rlp, aName->c_str(), RemoteVocalBinDir, tarFileName.c_str());
			}
			else
			{
			    sprintf(sysCommand, "%s %s '(cd %s; tar xf /tmp/%s)'",
				    rlp, aName->c_str(), RemoteVocalBinDir, tarFileName.c_str());
			}
			if (mySystem(sysCommand) != 0)
			{
			    cout << "ERROR:   can not untar binary tar file on remote machine" << aName->c_str()
				 << "      System partially  deployed" << nl;
			}
			    
		    }
		}
		else
		{
		    memset(sysCommand, 0, sizeof(sysCommand));
		    if (solarisOS)
		    {
			sprintf(sysCommand, "%s /usr/local/vocal/tmp/bin.tar %s:/tmp", rcp, aName->c_str() );
		    }
		    else
		    {
			sprintf(sysCommand, "%s /usr/local/vocal/tmp/bin.tar.gz %s:/tmp", rcp, aName->c_str() );
		    }
		    if (mySystem(sysCommand) != 0)
		    {
			cout << "ERROR: can not create binaries on " << aName->c_str() << "  System partially  deployed";
		    }
		    else
		    {
			memset(sysCommand, 0, sizeof(sysCommand));
			if (solarisOS)
			{
			    sprintf(sysCommand, "%s %s '(cd %s; tar xf /tmp/bin.tar)'",
				    rlp, aName->c_str(), RemoteVocalBinDir);
			}
			else
			{
			    sprintf(sysCommand, "%s %s '(cd %s; tar xzf /tmp/bin.tar.gz)'",
				    rlp, aName->c_str(), RemoteVocalBinDir);
			}
			if (mySystem(sysCommand) != 0)
			{
			    cout << "ERROR:   can not untar binaries on remote machine" << aName->c_str()
				 << "      System partially  deployed" << nl;
			}
		    }
		}
		    
		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -f %s/.rev %s'", 
			rlp, aName->c_str(), RemoteVocalBinDir, RemoteVocalEtcDir);
		mySystem(sysCommand,true);
		    
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
		else
		{
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s \"ls %s/NetMgnt\"", rlp, aName->c_str(), RemoteProvDataDir);
		    if (mySystem(sysCommand, true) != 0)
		    {
			memset(sysCommand, 0, sizeof(sysCommand));
			sprintf(sysCommand, "%s %s \"mkdir -p %s/NetMgnt\"", rlp,aName->c_str(), RemoteProvDataDir);
			if (mySystem(sysCommand) != 0 )
			{
			    cout << "ERROR : Can not make the NetMgnt directory on "
				 << aName->c_str() << nl << "\t System Partially Deployed!!!" << nl;
			}
		    }
			
		    memset(sysCommand, 0, sizeof(sysCommand));
		    sprintf(sysCommand, "%s %s \"cp -fR %s/mrtg/* %s/NetMgnt/\"", rlp, aName->c_str(), RemoteVocalBinDir, RemoteProvDataDir);
		    mySystem(sysCommand,true);
/*			{
			cout << "ERROR : Can not copy the mrtg directory into NetMgnt on "
			<< aName->c_str() << nl << "\t System Partially Deployed !!!" << nl;
			}
*/		
			
		}
	    }

	    // need to build the mrtg stuff here
		
	}
    }
    return 1;
}

#include "deploy.h"


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


void build_userdata(
    const vector < InstallUnit_T >& installConfig,
    const vector < string > &hostNames,
    const string& vsUser

    )
{
    char sysCommand[maxFileNameSize];
    // create user accounts.  todo:
    vector < InstallUnit_T > ::const_iterator iv;


    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
	memset(sysCommand, 0, sizeof(sysCommand));
	sprintf(sysCommand, "%s %s \"chmod -fR a+rw %s\"", rlp, iv->hostName.c_str(), RemoteProvDataDir);
	mySystem(sysCommand, true);
	    
	    

	if (iv->funcUnit == PS_PS)
	{
	    // to do: move user accounts out of the way

	    //make the script file and copy it over to the PS


	    if (!mkProvFiles(ScriptFileLoc, iv->hostName.c_str()))
	    {
		cout << "Can not make configuration files succesfully !! Please see README for details ! " << nl;
	    }
	    else
	    {
		if (mySystem("chmod +x /usr/local/vocal/tmp/mkProvFiles.sh") != 0)
		{
		    cout << "Can not change permissions on scriptfile mkProvFiles.sh....." << nl;
		}
		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/mkProvFiles.sh %s:%s", rcp, iv->hostName.c_str(), RemoteVocalBinDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << " Can not copy the script file to the Provisioning server" << nl;
		}
	    }

	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s \"cp -f %s/provisioning_data/deployedServers.xml %s\"", rlp, iv->hostName.c_str(), RemoteProvDataDir, RemoteProvDataDir);
	    if (mySystem(sysCommand) != 0)
	    {
		cout << " might erase deployedServers.xml" << nl;
	    }

	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s \"rm -rf %s/provisioning_data/*\"", rlp, iv->hostName.c_str(), RemoteProvDataDir);
	    if (mySystem(sysCommand) != 0)
	    {
		cout << " might not be able to clean up provisioning_data directory" << nl;
	    }

	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s \"cp -f %s/deployedServers.xml %s/provisioning_data\"", rlp, iv->hostName.c_str(), RemoteProvDataDir, RemoteProvDataDir);
	    if (mySystem(sysCommand) != 0)
	    {
		cout << " not able to copy deployedServers.xml back into provisioning_data directory" << nl;
	    }

	    vector<string>::iterator phonenumber;
	    vector<string>::iterator phoneip;

	    phoneip = aioConfig.SIPPhoneIP.begin();
	    for (phonenumber = aioConfig.SIPPhoneNumber.begin(); phonenumber < aioConfig.SIPPhoneNumber.end(); phonenumber++)
	    {
		char tempphonenumber[512];
		char tempphoneip[512];

		sprintf(tempphonenumber, "%s", phonenumber->c_str());
		sprintf(tempphoneip, "%s", phoneip->c_str());
		char allinonehost[512];
		memset(allinonehost, 0, sizeof(allinonehost));
		sprintf(allinonehost, "%s", hostNames[0].c_str());
		createUsers(tempphonenumber, tempphoneip, allinonehost);
		phoneip++;
	    }
                                                                                                                                
	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s \"%s/mkProvFiles.sh\"", rlp, iv->hostName.c_str(), RemoteVocalBinDir);
	    mySystem(sysCommand);
//                {
//                    cout << " Can not run the script file on the Provisioning server" << nl;
//                }

	    // to do: copy old users back to the prov data

	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s \"killall -9 pserver\"", rlp, iv->hostName.c_str());
	    mySystem(sysCommand, true);
		
	    memset(sysCommand, 0, sizeof(sysCommand)); 
	    sprintf(sysCommand, "%s %s \"chown -R %s %s \"", rlp, iv->hostName.c_str(), vsUser.c_str(), PsRoot);
	    mySystem(sysCommand);	
	}
    }
}

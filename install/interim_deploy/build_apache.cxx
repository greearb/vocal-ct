#include "deploy.h"


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

string psHost1 = "";
string psHost2 = "";

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


bool findPS(vector<InstallUnit_T> sysVector, bool allinone)
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


void build_apache(
    bool allinone, 
    vector < InstallUnit_T >& installConfig,
    char* VocalXmlSubDir,
    const string& switchName 
    )
{
    char buff[512];
    char buff1[512];
    
    char destFileName[512];
    vector < InstallUnit_T > ::iterator iv;
    char seeder[512];
    char RemoteHtmlDir[512];
    char RemoteCgibinDir[512];
    char tmpStr[512];
    char sysCommand[512];

    // configure apachee startup on pservers;
    if (!allinone)
    {
	cout << "Do you want to download webserver configuration.  Warning:  This will remove the old webserver config. (y/n) [y]:";
	cin.getline(buff, 512, '\n');
	if (!(strlen(buff) == 0 || !strcasecmp(buff, "y") || !strcasecmp(buff, "yes") ) )
	{
	    cout << nl << "No remote webserver config done" << nl;
	}
	   
    }
    if ((strlen(buff) == 0 || (strcasecmp(buff, "y") == 0) || (strcasecmp(buff, "yes") == 0) ) || (allinone))
    {
	    
	// assign the NetMgnt-ps linking.  This is done here and not in assignParms routine because
	// the user may want to change the location of servers at that time.
	// once we reach this part of the program, the server/FU have been assinged.
	// There are the same number of NetMgnt as PS.

	int psCount = 0, nmCount = 0;
	vector < InstallUnit_T > ::const_iterator iv2;
	for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
	{
	    if (iv->funcUnit == PS_PS)
	    {
		psCount++;
		for (iv2 = installConfig.begin(); iv2 < installConfig.end() ; iv2++)
		{
		    if (iv2->funcUnit == CDR_NET_VM )
		    {
			nmCount++;
			if (nmCount == psCount)
			{
			    iv->parms.push_back (iv2->hostName);
			}
		    }
		}
	    }
	}

	memset(seeder, 0, sizeof(seeder));
	sprintf(seeder, "/usr/local/vocal/tmp/webpages");
	if (!localMkdir(seeder))
	{
	    cout << "Can not create the webpages directory locally !!!" << nl
		 << "\tSystem Partially Deployed !!! " << nl;
	}



	parseApacheConfig(installConfig, RemoteHtmlDir, sizeof(RemoteHtmlDir), RemoteCgibinDir, sizeof(RemoteCgibinDir));
	    
	string cdrHost;
	cdrHost = findCdr(installConfig);
	findPS(installConfig, allinone);

	for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
	{

	    // create the html files
	    if ((iv->funcUnit == PS_PS)||(iv->funcUnit ==CDR_NET_VM))
	    {

		sprintf(VocalXmlSubDir, "%s", switchName.c_str());
		memset(buff1, 0, sizeof(buff1));
		sprintf(buff1, "%s/%s", RemoteHtmlDir, VocalXmlSubDir);
		if (remoteMkdir(iv->hostName.c_str(), buff1) == false)
		{
		    cout << "Error: Can not make Remote Html Directories " << iv->hostName.c_str() << sp << buff1 << nl;
		}
		time_t uts = time(NULL);
		char strUts[32];
		memset(strUts, 0, sizeof(strUts));
		sprintf(strUts, "%ld", (unsigned long)uts);

		// create index.html file
		memset(destFileName, 0, sizeof(destFileName));
		sprintf(destFileName, "/usr/local/vocal/tmp/%s_index.html", iv->hostName.c_str() );
		ofstream indexHtmlFile(destFileName);

		memset(destFileName, 0, sizeof(destFileName));

		memset(seeder, 0, sizeof(seeder));
		sprintf(seeder, "%s %s:%s/webpages/indexSeed.txt /usr/local/vocal/tmp/webpages", rcp, iv->hostName.c_str(), RemoteVocalBinDir);
		if (mySystem(seeder) != 0)
		{
		    cout << " Warning : indexseed.txt " << nl;
		}
		sprintf(destFileName, "/usr/local/vocal/tmp/webpages/indexSeed.txt");

		ifstream indexSeedFile(destFileName);
		while (indexSeedFile.getline(tmpStr, 512 - 1))
		{
		    string inString = tmpStr;
		    while (inString.find(HostNameToken1) != string::npos)
		    {
			inString.insert(inString.find(HostNameToken1), psHost1);
			inString.erase(inString.find(HostNameToken1), strlen(HostNameToken1));
		    }
		    if ((psHost1 == psHost2) || (strcmp(psHost2.c_str(), "") == 0))
		    { 
			while (inString.find(HostNameToken2) != string::npos)
			{
			    inString.erase(0, strlen(inString.c_str()));
			}
		    }
		    else
		    {
			while (inString.find(HostNameToken2) != string::npos)
			{
			    inString.insert(inString.find(HostNameToken2), psHost2);
			    inString.erase(inString.find(HostNameToken2), strlen(HostNameToken2));
			}
		    }
		    while (inString.find(DirToken) != string::npos)
		    {
			inString.insert(inString.find(DirToken), "cgi-bin");
			inString.erase(inString.find(DirToken), strlen(DirToken));
		    }
		    while (inString.find(CdrDirToken) != string::npos)
		    {
			inString.insert(inString.find(CdrDirToken), VocalXmlSubDir);
			inString.erase(inString.find(CdrDirToken), strlen(CdrDirToken));
		    }
		    while (inString.find(CdrHostNameToken) != string::npos)
		    {
			inString.insert(inString.find(CdrHostNameToken), cdrHost.c_str());
			inString.erase(inString.find(CdrHostNameToken), strlen(CdrHostNameToken));
		    }
		    indexHtmlFile << inString << nl;
		}
		indexHtmlFile.close();


		// copy html pages onto remote machines

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/%s_index.html %s:%s/%s/index.html",
			rcp, iv->hostName.c_str(), iv->hostName.c_str(), RemoteHtmlDir, VocalXmlSubDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy index.html onto remote machine " << iv->hostName
			 << "      System partially  deployed" << nl;
		}

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -f %s/webpages/provisioning.cgi %s'",
			rlp, iv->hostName.c_str(), RemoteVocalBinDir, RemoteCgibinDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy provisioning.cgi into cgi-bin directory on " << iv->hostName
			 << "      System partially  deployed" << nl;
		}

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -f %s/webpages/systemStatus.cgi %s'",
			rlp, iv->hostName.c_str(), RemoteVocalBinDir, RemoteCgibinDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy systemStatus.cgi into cgi-bin directory on " << iv->hostName
			 << "      System partially  deployed" << nl;
		}

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -f %s/webpages/user.cgi %s'",
			rlp, iv->hostName.c_str(), RemoteVocalBinDir, RemoteCgibinDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy user.cgi into cgi-bin directory on " << iv->hostName
			 << "      System partially  deployed" << nl;
		}


		// copy the jar and image files

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s/jarfiles", RemoteHtmlDir);
		remoteMkdir(iv->hostName.c_str(), sysCommand);

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -f %s/*.jar %s/jarfiles'", rlp, iv->hostName.c_str(), RemoteVocalBinDir, RemoteHtmlDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy *.jar files on remote machine " << iv->hostName
			 << "      System partially  deployed" << nl;
		}

		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'cp -fr %s/images %s/jarfiles'",
			rlp, iv->hostName.c_str(), RemoteVocalBinDir, RemoteHtmlDir);
		if (mySystem(sysCommand) != 0)
		{
		    cout << "ERROR:   can not copy image files on remote machine " << iv->hostName
			 << "      System partially  deployed" << nl;
		}

	    }
	    if (iv->funcUnit == CDR_NET_VM)
	    {
		sprintf(VocalXmlSubDir, "%s", switchName.c_str());
		memset(buff1, 0, sizeof(buff1));
		sprintf(buff1, "%s/%s", RemoteHtmlDir, VocalXmlSubDir);
		if (remoteMkdir(iv->hostName.c_str(), buff1) == false)
		{
		    cout << "Error: Can not make Remote Html Directories " << iv->hostName.c_str() << sp << buff1 << nl;
		}
		memset(sysCommand, 0, sizeof(sysCommand));
		sprintf(sysCommand, "%s %s 'rm -f %s/%s/NetMgnt; ln -s %s/NetMgnt/ %s/%s/NetMgnt'", rlp, iv->hostName.c_str(), RemoteHtmlDir, switchName.c_str(), RemoteProvDataDir, RemoteHtmlDir, switchName.c_str());
		if (mySystem(sysCommand, true) != 0)
		{
		    cout << "ERROR:   can not create link for netMgnt. System partially deployed "<<nl;
		    cout << "      command =  "<< sysCommand <<nl;
		}
	    }
	}
    }
}

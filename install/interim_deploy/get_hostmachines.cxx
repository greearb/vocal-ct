#include "deploy.h"

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





bool readHost(char *filename, vector <string> &hostName)
{
    char buff[512];
    ifstream inFile(filename);
    
    if (!inFile || inFile.bad())
    {
	cout << " can not open file " << filename;
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


int
get_hostmachines(bool fuprovide, 
		 string hostNamesFile, 
		 vector < InstallUnit_T >& installConfig,
		 vector < string >& hostNames, 
		 int& numHosts)
{
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
/*	if (!inFile || inFile.bad())
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
	inFile.close(); */
    }
    
    numHosts = hostNames.size();
    if (!numHosts)
    {
        cout << "no hostname in hostname file " << hostNamesFile;
        return 0;
    }
    return 1;
}


#include "deploy.h"


#define LINK "ln -s %s /usr/local/vocal/tmp/mrtg/%s"

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



int mrtgIndexing(vector < string > hostvector)
{
    char buf[500];
    char newBuf[500];
    char targ[500];
    size_t n, m;
    list < string > servList;

    FILE* serverSeedf;
    FILE* serverIndexf;
    string val;
    string repeat = "";
    string test, hostn2;

    vector < string > ::iterator siv;

    serverSeedf = fopen("/usr/local/vocal/tmp/mrtg/serverSeed.in", "r");
    if (!serverSeedf)
    {
        cout << "can't open input file";
        return 0;
    }
    serverIndexf = fopen("/usr/local/vocal/tmp/mrtg/serverIndex.html", "w");
    if (!serverIndexf)
    {
        cout << "can't open input file";
        return 0;
    }

    while (!feof(serverSeedf))
    {
        memset(buf, 0, sizeof(buf));
        fgets(buf, 512, serverSeedf);

        if (buf[0] == '$')
        {
            for (m = 1, n = 0; m < strlen(buf); m++, n++)
                newBuf[n] = buf[m];
            newBuf[n] = '\0';
            fputs(newBuf, serverIndexf);
        }
	
        if (buf[0] == '@')
        {
            if (!memcmp(buf, "@", strlen("@")))
            {

                for (siv = hostvector.begin(); siv < hostvector.end(); siv++)
		{
		    
		    hostn2 = siv->c_str();
		    int hostfind = hostn2.find(".");
		    hostn2 = hostn2.substr(0,(hostfind));
		    //		    hostn2.erase(hostfind, ((hostn2.size()-hostfind)-1));
                    servList.push_back(hostn2);
		}
                servList.sort();
                servList.unique();

                while (!servList.empty())
                {

                    val = servList.front();
                    strcpy(targ, "<p><font color=\"#330000\"><a href=\"");
                    strcat(targ, val.c_str());
                    strcat(targ, ".html\"> ");
                    strcat(targ, val.c_str());
                    strcat(targ, "</a></font>");

                    fputs(targ, serverIndexf);
                    fputs("\n", serverIndexf);

                    createServerHtml(val);
                    servList.pop_front();
                }
                servList.clear();
            }

        }
        if (buf[0] == '0')
        {
            return 0;
        }
    }
    fclose(serverSeedf);
    fclose(serverIndexf);

    return 1;
}


void build_mrtg(
    const vector < InstallUnit_T >& installConfig,
    const vector < string > &hostNames
    )
{
    char sysCommand[maxFileNameSize];
    vector < InstallUnit_T > ::const_iterator iv;

    localMkdir("/usr/local/vocal/tmp/mrtg");
		
    for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
    {
	if (iv->funcUnit == CDR_NET_VM)
	{
	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "%s %s:%s/NetMgnt/* /usr/local/vocal/tmp/mrtg/", rcp, iv->hostName.c_str(), RemoteProvDataDir);
	    mySystem(sysCommand, true);
			
	    mrtgIndexing(hostNames);
			
	    memset(sysCommand, 0, sizeof(sysCommand));
	    if (solarisOS)
	    {
		system("ls /usr/local/vocal/tmp/mrtg");
		sprintf(sysCommand, "(cd /usr/local/vocal/tmp/mrtg; tar cf mrtg.tar .)");
	    }
	    else
	    {
		sprintf(sysCommand, "(cd /usr/local/vocal/tmp/mrtg; tar czf mrtg.tar.gz .)");
	    }
	    mySystem(sysCommand, true);
			
	    memset(sysCommand, 0, sizeof(sysCommand));
	    if (solarisOS)
	    {
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/mrtg/mrtg.tar %s:/tmp", rcp, iv->hostName.c_str());
	    }
	    else
	    {
		sprintf(sysCommand, "%s /usr/local/vocal/tmp/mrtg/mrtg.tar.gz %s:/tmp", rcp, iv->hostName.c_str());
	    }
	    mySystem(sysCommand);
			
	    memset(sysCommand, 0, sizeof(sysCommand));
	    if (solarisOS)
	    {
		sprintf(sysCommand, "%s %s '(cd %s/NetMgnt; tar xf /tmp/mrtg.tar)'", rlp, iv->hostName.c_str(), RemoteProvDataDir);
	    }
	    else
	    {
		sprintf(sysCommand, "%s %s '(cd %s/NetMgnt; tar xzf /tmp/mrtg.tar.gz)'", rlp, iv->hostName.c_str(), RemoteProvDataDir);
	    }
	    mySystem(sysCommand);
			
	    memset(sysCommand, 0, sizeof(sysCommand));
	    if (solarisOS)
	    {
		sprintf(sysCommand, "rm -rf /usr/local/vocal/tmp/mrtg.tar");
	    }
	    else 
	    {
		sprintf(sysCommand, "rm -rf /usr/local/vocal/tmp/mrtg.tar.gz");
	    }
	    mySystem(sysCommand);
			
	    memset(sysCommand, 0, sizeof(sysCommand));
	    sprintf(sysCommand, "rm -rf /usr/local/vocal/tmp/mrtg/*");
	    mySystem(sysCommand);

	}
    }
}

#include "deploy.h"

int run_userconfig(
    const vector < InstallUnit_T >& installConfig, 
    bool allinone,
    const char* VocalXmlSubDir

    
    )
{    
    char sysCommand[maxFileNameSize];

    char buff[512];
    vector < InstallUnit_T > ::const_iterator iv;

    if (!allinone)
    {
	cout << "Do you want to add users on your new vocal classXswitch (y/n) [y]: " << nl;
	cin.getline(buff, 512, '\n');
	if (!(strlen(buff) == 0 || !strcasecmp(buff, "y") || !strcasecmp(buff, "yes")) )
	{
	    cout << nl << " Install completed." << nl;
	    return 0;
	}
    }
    else
    {
	/*vector<string>::iterator phonenumber;
	  vector<string>::iterator phoneip;
	    
	  phoneip = SIPPhoneIP.begin();
	  for (phonenumber = SIPPhoneNumber.begin(); phonenumber < SIPPhoneNumber.end(); phonenumber++)
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
	  }*/
	    
    }
	
    if ((strlen(buff) == 0 || strcasecmp(buff, "y") || strcasecmp(buff, "yes")) || (allinone))
    {
	for (iv = installConfig.begin(); iv < installConfig.end() && iv->funcUnit != PS_PS; iv++) ;
	if (iv == installConfig.end())
	{
	    cout << "internal error";
	    return 0;
	}
	char browserexec[512];
	memset (browserexec, 0, sizeof(browserexec));
	sprintf(browserexec, "netscape");
	    
	if (!allinone)
	{
	    cout << "Please specify which web browser to use [netscape] :" << nl;
	    cin.getline(buff, 512, '\n');
	    if (!(strlen(buff) == 0)
		|| !(strcasecmp(buff, "netscape"))
		|| !(strcasecmp(buff, "communicator"))
		|| !(strcasecmp(buff, "netscape communicator")))	
	    {
		sprintf(browserexec, "%s", buff);
	    }
	}
	    
	memset(sysCommand, 0, sizeof(sysCommand));
	sprintf(sysCommand, "%s http://%s/%s/index.html &", browserexec, iv->hostName.c_str(), VocalXmlSubDir);
	if (mySystem(sysCommand) != 0)
	{
	    cout << "ERROR: Can not open the vocal web pages from http://"
		 << iv->hostName.c_str() << "/" << VocalXmlSubDir << nl
		 << " Please open the file manually ! Thanks !!";
	}
	else
	{
	    cout << nl << "\tPlease remember to start the vocal system by running:\n\n" << nl
		 << nl << "\t\t/etc/rc.d/init.d/vocalstart start\n\n" << nl
		 << nl << "\t If your browser does not come up, please point your browser to :"<<nl
		 << nl << "\t http://"<<iv->hostName.c_str()<<"/" << VocalXmlSubDir << nl
		 << nl << "\t\tThank You!!!" << nl << nl;
		
	}
    }
    return 1;
}

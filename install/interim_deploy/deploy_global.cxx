#include "deploy.h"

char rlp[7];
char rcp[7];

AllInOneConfig aioConfig;

bool verbose = false;


bool HostnameIn(char *hosts, vector < string > HN)
{
    vector < string > ::iterator HNI;
    string HNIstr;
    for (HNI = HN.begin(); HNI < HN.end(); HNI++)
    {
        HNIstr = HNI->c_str();
        if (HNIstr == hosts) return true;
    }
    return false;
}


// this is similar to the linux "system" call except that it
// handls the "system"  error conditions and passes up the
// "cmd" errors to the user.
int mySystem(const char *cmd, bool silent = false)
{
    string myCmd;

    if (!cmd)
    {
        cout << "invalid command to mySystem" << nl;
    }

    if (verbose)
    {
	cout << "running command: " << cmd << endl;
    }

    myCmd.assign(cmd);
    myCmd.append("  > /dev/null 2>&1");

    int retVal = system(myCmd.c_str());
    if (retVal == 127)
    {
        cout << "execve() from /bin/sh failed during system call";
        return -1;
    }
    else if (retVal == -1)
    {
        perror("system command from shell failed: ");
    }

    if (retVal != 0 && !silent)
    {
        cout << "mySystem command failed " << cmd << nl;
    }

    return retVal;
}



// create directory "dir" on remote host "host"
// returns true on success; false on failure
int remoteMkdir (const char *host, const char *dir)
{
    char sysCommand[512];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "%s %s \"mkdir -p %s\"", rlp, host, dir );
    if (mySystem(sysCommand) != 0)
    {
        cout << " can not create" << dir << " dir on host " << host;
        return false;
    }
    return true;
}


// create directory "dir" on local host
// returns true on success; false on failure
int localMkdir(const char *dir)
{
    char sysCommand[512];
    memset(sysCommand, 0, sizeof(sysCommand));
    sprintf(sysCommand, "mkdir -p %s", dir );
    if (mySystem(sysCommand) != 0)
    {
        cout << " can not create" << dir
        << " dir on host " << nl;
        return false;
    }
    return true;
}


int sortVectorbyFU ( InstallFU_T fu, vector < InstallUnit_T > &installConfig)
{
    vector < InstallUnit_T > installSwapVector;
    vector < InstallUnit_T > ::iterator isv;
    for (isv = installConfig.begin(); isv < installConfig.end(); isv++)
    {
        if (isv->funcUnit == fu)
        {
            installSwapVector.push_back(*isv);
        }
    }
    for (isv = installConfig.begin(); isv < installConfig.end(); isv++)
    {
        if (isv->funcUnit != fu)
        {
            installSwapVector.push_back(*isv);
        }
    }
    installConfig.swap(installSwapVector);
    return true;
}

vector < string > getHostNames(vector < InstallUnit_T > &installConfig)
{
    vector < string > hNames;
    vector < InstallUnit_T > ::iterator hiv;
    for (hiv = installConfig.begin(); hiv < installConfig.end(); hiv++)
    {
        char hostnamevar[512];
        memset(hostnamevar, 0, sizeof(hostnamevar));
        sprintf(hostnamevar, "%s", hiv->hostName.c_str());
        if (!HostnameIn(hostnamevar, hNames))
        {
            hNames.push_back(hiv->hostName);
        }
    }
    return hNames;
}

#include "deploy.h"


// tests the write ability of the user for host in dir
// returns false on failure true on success
int remoteWriteTest(const char *host, const char *dir)
{
    char mySysCommand[512];
    int retVal;

    memset(mySysCommand, 0, sizeof(mySysCommand));
    sprintf(mySysCommand, "%s %s \"touch %swriteAccessTest\"", rlp, host, dir);
    retVal = mySystem(mySysCommand);
    
    if (retVal != 0)
    {
        cout << "No Write Access to dir " << dir <<
        " on host " << host;
        return false;
    }
    else
    {
        memset(mySysCommand, 0, sizeof(mySysCommand));
        sprintf(mySysCommand, "%s %s \"rm -f %swriteAccessTest\"", rlp, host, dir);
        retVal = mySystem(mySysCommand, true);
        if (retVal != 0)
        {
            cout << "can not remove temporary file " << dir << "writeAccessTest on host " << host;
            return false;
        }
    }
    return true;
}

int test_remoteinstall(bool upgrade, bool myTest, int numHosts, vector < string > hostNames)
{
    char sysCommand[maxFileNameSize];

    cout << "Testing configuration and permissions on local and remote systems" << nl;

    if (myTest == false)
    {
        // make sure all remote machines are up and we have write permission into the key directories
        for (int i = 0; i < numHosts; i++)
        {

            cout << "    Checking host " << hostNames[i] << nl;

            memset(sysCommand, 0, sizeof(sysCommand));
            sprintf(sysCommand, "ping -c1 %s > /dev/null", hostNames[i].c_str());
            if (mySystem(sysCommand) != 0)
            {
                cout << hostNames[i] << " is not responding.  System not deployed";
                return 0;
            }

            // test if we can write as root to a generic location
            if (remoteWriteTest( hostNames[i].c_str(), "/usr/local/") == false )
            {
                cout << "We do not have root write accesses to host " << hostNames[i] << " System not Deployed\n";
                cout << "Please configure system for access without password."<<nl;
                return 0;
            }



            if (upgrade == false)
            {
                // this directory should already exist.
                if (remoteWriteTest( hostNames[i].c_str(), RemoteVocalStartupDir) == false)
                {
                    cout << " System Not Deployed\n";
                    return 0;
                }

                if (remoteMkdir( hostNames[i].c_str(), RemoteVocalLogDir ) == false)
                {
                    cout << "System Not Deployed\n";
                    return 0;
                }

                if (remoteMkdir( hostNames[i].c_str(), RemoteVocalH323LogDir ) == false)
                {
                    cout << "System Not Deployed\n";
                    return 0;
                }

                if (remoteMkdir( hostNames[i].c_str(), RemoteBillingDir ) == false)
                {
                    cout << "System Not Deployed\n";
                    return 0;
                }

                if (remoteMkdir( hostNames[i].c_str(), RemoteVocalEtcDir) == false)
                {
                    cout << "System Not Deployed\n";
                    return 0;
                }

                if (remoteWriteTest( hostNames[i].c_str(), RemoteVocalEtcDir) == false)
                {
                    cout << " System Not Deployed\n";
                    return 0;
                }
                if (remoteMkdir( hostNames[i].c_str(), RemoteVocalBinDir) == false)
                {
                    cout << "System Not Deployed\n";
                    return 0;
                }

            }
            else
            {
                // don't clean it, just make sure we can write to it.  should already exist
                // on an upgrade situation
                if (remoteWriteTest( hostNames[i].c_str(), RemoteVocalBinDir) == false)
                {
                    cout << " System Not Deployed\n";
                    return 0;
                }
            }
        }
        cout << "All remote system configuration and permissions OK" << nl;

    }
    return true;
}

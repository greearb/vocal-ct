#include "deploy.h"

void build_uavm(bool upgrade, bool allinone, bool wantexec, bool configonly, const vector < InstallUnit_T >& installConfig)
{
     vector < InstallUnit_T > ::const_iterator iv;
    char destFileName[maxFileNameSize];
    char tmpStr[1024];


    // uavm stuff

    if ((!upgrade)&&(!allinone))
    {

        if ((wantexec) || (configonly))
        {
            char myCommand[512];
            char vmPortStr[32];

            for (iv = installConfig.begin(); iv < installConfig.end(); iv++)
            {

                if (iv->funcUnit == CDR_NET_VM)
                {
                    for (int j = 0; j < NumSuaVm; j++)
                    {
                        memset(vmPortStr, 0, sizeof(vmPortStr));
                        sprintf(vmPortStr, "%d", iv->port + j);

                        memset(destFileName, 0, sizeof(destFileName));
                        sprintf(destFileName, "/usr/local/vocal/tmp/uavm_%d.cfg", iv->port + j );
                        ofstream uavmOutFile(destFileName);

                        memset(destFileName, 0, sizeof(destFileName));
                        memset(myCommand, 0, sizeof(myCommand));
                        sprintf(myCommand, "%s %s:%s/voicemail/uavm_cfg.txt /usr/local/vocal/tmp/uavm_cfg.tmp", rcp, iv->hostName.c_str(), RemoteVocalBinDir);
                        if (mySystem(myCommand) != 0)
                        {
                            cout << " Warning: uavm_cfg.txt does not exist" << nl;
                        }
                        sprintf(destFileName, "/usr/local/vocal/tmp/uavm_cfg.tmp");

                        ifstream uavmInFile(destFileName);
                        while (uavmInFile.getline(tmpStr, 512 - 1))
                        {
                            string inString = tmpStr;
                            while (inString.find(PortToken) != string::npos)
                            {
                                inString.insert(inString.find(PortToken), vmPortStr);
                                inString.erase(inString.find(PortToken), strlen(PortToken));
                            }
                            uavmOutFile << inString << nl;
                        }
                        uavmInFile.close();
                        uavmOutFile.close();
                        // push file to the destination /usr/local/vocal/voicemail

                        memset (myCommand, 0, sizeof(myCommand));
                        sprintf(myCommand, "%s /usr/local/vocal/tmp/uavm_%d.cfg %s:%s/voicemail", rcp, iv->port + j, iv->hostName.c_str(), RemoteVocalBinDir);
                        if (mySystem(myCommand) != 0)
                            cout << "Warning: Can not copy the uavm config file to CDR  ! Voicemail will not work ! " << nl;

                    }
                }
            }
        }
    }
}

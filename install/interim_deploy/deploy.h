using namespace std;
#include <sys/types.h>

#include <iostream>
#include <string.h>
#include <vector>
#include <string>
#include <fstream.h>
#include <cstdlib>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <list>
#include <time.h>

//Number of Vmsuas
const int NumSuaVm = 10;


// These are the functional units that can be loaded together.
// they are assined an enum value in inverse order.
#define ENUMMIN 1
#define NumOfFuncUnits 5


typedef enum {
    CDR_NET_VM = ENUMMIN,
    PS_PS,
    MS,
    FS_JS,
    RS,
    MAX_FU
} InstallFU_T;

typedef struct
{
    string hostName;
    InstallFU_T funcUnit;
    int port;
    vector < string > parms;
}
InstallUnit_T;

typedef enum {
    REDIRECT = ENUMMIN,
    PROVISIONING,
    MARSHAL_SUA,
    MARSHAL_GW,
    MARSHAL_CON,
    MARSHAL_INET,
    FEATURE_CB,
    FEATURE_FNAB,
    FEATURE_CS,
    FEATURE_FAC,
    FEATURE_VM,
    FEATURE_VM_SUA,
    FEATURE_CIDB,
    FEATURE_CR,
    JTAPI,
    CDR,
    POLICY,
    HBS
} serverType_T;

struct UserEnteredData
{
    public:
// user entered data
	string hostNamesFile;
	string switchName;
	string billingHost;
	string billingSecretKey;
	string binSourceDir;
	vector < string > gatewayForMarshals;
	string vsUser;
	string mcHost;
	string mcPort;
	string psMcHost;
	string psMcPort;
};



struct AllInOneConfig
{ 
    public:
	string GatewayIP;
	string GatewayCode;
	vector <string> SIPPhoneNumber;
	vector <string> SIPPhoneIP;
	
};

#include "deploy_const.h"
#include "deploy_global.h"
#include "deploy_shared.h"


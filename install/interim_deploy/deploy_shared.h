bool HostnameIn(char *hosts, vector < string > HN);

int mySystem(const char *cmd, bool silent = false);

int remoteMkdir (const char *host, const char *dir);
int localMkdir(const char *dir);

int sortVectorbyFU ( InstallFU_T fu, vector < InstallUnit_T > &installConfig);

vector < string > getHostNames(vector < InstallUnit_T > &installConfig);

int get_hostmachines(bool fuprovide, string hostNamesFile, vector < InstallUnit_T >& installConfig, 		 vector < string >& hostNames, 		 int& numHosts  );

int test_remoteinstall(bool upgrade, bool myTest, int numHosts, vector < string > hostNames);

int install_executables(bool configonly, bool allinone, bool upgrade, bool userpm, bool usetar, string binSourceDir, const vector < string >& hostNames, string rpmfile, string tarfile, bool& wantexec);

void build_mrtg(    
    const vector < InstallUnit_T >& installConfig,
    const vector < string > &hostNames

    );

void     print_config(const vector < InstallUnit_T >& installConfig, 
		      bool allinone);
void     assign_units(
    vector < InstallUnit_T > &installConfig, 
    vector < string > &hostNames, 
    char* h323MsIpPort,
    const string& billingHost,
    const vector <string>& gatewayForMarshals,
    int numHosts, 
    bool fuprovide, 
    bool upgrade, 
    bool allinone
    );

void build_uavm(bool upgrade, bool allinone, bool wantexec, bool configonly, const vector < InstallUnit_T >& installConfig);

int build_provdata(
    const vector < InstallUnit_T >& installConfig, 
    const vector < string > &hostNames, 
    const char* h323MsIpPort,
    bool allinone, 
    const UserEnteredData& myUserData
    )
    ;
int 	install_provdata(
    const vector < InstallUnit_T >& installConfig,
    const vector < string > &hostNames
    );
void 	build_apache(
    bool allinone,
    vector < InstallUnit_T >& installConfig,
    char* VocalXmlSubDir,
    const string& switchName 
    );
void 	build_userdata(
    const vector < InstallUnit_T >& installConfig,
    const vector < string > &hostNames,
    const string& vsUser
    );
void 	build_hostconfig();
int 	restart_boxes(
    bool allinone,
    vector < InstallUnit_T >& installConfig
    );

int run_userconfig(
    const vector < InstallUnit_T >& installConfig, 
    bool allinone,
    const char* VocalXmlSubDir
    )
    ;

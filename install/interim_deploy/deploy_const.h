
// vovida specific constants
const int DefaultSipPort = 5060;
const int DefaultRSSyncPort = 22002;
const int DefaultCdrPort = 22300;
const char DefaultPsMcHost[] = "224.0.0.101";
const char DefaultPsMcPort[] = "9001";
const char DefaultMcHost[] = "224.0.0.100";
const char DefaultMcPort[] = "9000";
const char DefaultMissedHb[] = "10";
const char DefaultHbInterval[] = "250";  //ms
const char DefaultPSPort[] = "6005";
const char DefaultPolicyPort[] = "3288";
const int DefaultPolicyPortInt = 3288;
const char DefaultH323Port[] = "22400";
const char ScriptFileLoc[] = "/usr/local/vocal/tmp/mkProvFiles.sh";
const char DefaultLogLevel[] = "LOG_ERR";

const int MSuaPortOffset = 0;
const int MGWPortOffset = 1;
const int MConPortOffset = 2;
const int MInetPortOffset = 3;
const int FCbPortOffset = 4;
const int FFnabPortOffset = 5;
const int FCsPortOffset = 6;
const int FFacPortOffset = 7;
const int FCrPortOffset = 8;
const int FCidbPortOffset = 9;
const int JtapiPortOffset = 10;

const char RemoteProvDataDir[] = "/usr/local/vocal";
const char RemoteVocalLogDir[] = "/usr/local/vocal/log/";
const char RemoteVocalH323LogDir[] = "/usr/local/vocal/log/siph323csgw/";
#ifdef OS_SOLARIS
    const bool solarisOS = true;
    const char RemoteVocalStartupDir[] = "/etc/init.d/";
    const char RemoteEtcDir[] = "/etc";
#else
    const bool solarisOS = false;
    const char RemoteVocalStartupDir[] = "/etc/rc.d/init.d/";
    const char RemoteEtcDir[] = "/etc/rc.d";
#endif
const char RemoteVocalEtcDir[] = "/usr/local/vocal/etc/";
const char RemoteVocalBinDir[] = "/usr/local/vocal/bin/";
const char RemoteBillingDir[] = "/usr/local/vocal/billing/";
const char LocalVocalTmpDir[] = "/usr/local/vocal/tmp/";
const char LocalProvDataDir[] = "/usr/local/vocal/tmp/provisioning_data/";
const char LocalVocalEtcDir[] = "/usr/local/vocal/tmp/etc/";
const char LocalApacheConfigFile[] = "/usr/local/vocal/tmp/vocal_httpd.conf";
#ifdef OS_SOLARIS
    const char DefaultApacheConf1[] = "/etc/apache/httpd.conf";
    const char DefaultApacheConf2[] = "/etc/apache/httpd.conf";
#else
    const char DefaultApacheConf1[] = "/etc/httpd/conf/httpd.conf";
    const char DefaultApacheConf2[] = "/usr/local/apache/conf/httpd.conf";
#endif

// Config Related Defaults

const char defaultHostNamesFile[] = "./hosts";
const char defaultSwitchName[] = "classXswitch";
const char defaultBillingHost[] = "0.0.0.0";
const char defaultSecretKey[] = "testing123";
const char defaultGatewayIp[] = "0.0.0.0";
const char defaultGatewayNamesFile[] = "./Gateways";
const char defaultBinSourceDir[] = "/usr/local/vocal/stage/bin";
const char hashScript[] = "FileDataStoreWrapper";
const char NumberOfBins[] = "6";
const char PsRoot[] = "/usr/local/vocal/provisioning_data";
const char UserGroup[] = "Accounts";
const char sorScriptFileName[] = "/usr/local/vocal/tmp/startOnReboot";
const int maxFileNameSize = 512;
const string DefaultUser = "nobody";


const char HostNameToken1[] = "hostNameToken1";
const char HostNameToken2[] = "hostNameToken2";
const char CdrHostNameToken[] = "cdrNameToken";
const char DirToken[] = "dirToken";
const char CdrDirToken[] = "cdrDirectoryToken";
const char JarToken[] = "jarToken";
const char PortToken[] = "portToken";
const char UserSeedFile[] = "/usr/local/vocal/bin/provisioning/xml/UserSeed.xml";

// misc constants
const char nl[] = "\n";
const char sp[] = " ";
const char dq[] = "\"";

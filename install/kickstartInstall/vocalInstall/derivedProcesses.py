import string, sys, os, vocalConstant, psClass, random, re, socket

from baseProcess import *

class mss(baseProcess):
    "User Agent Marshal Process Class"
    def __init__(self, hName, pPort, msType="UserAgent"):
	self.port = pPort
	self.processName = "ms"
	self.hostName = hName
	self.params = []
	self.type = msType

    def process(self):
	mssDirs = baseVocalDirs()
	mssDirs.initDirs()
	
	self.calcParams()
	self.addToVocalConf()

	msvo = psClass.vocalObject()
	msvo.load(vocalConstant.ksVocalIdFile)
	
	mssXmlFile = "/tmp/" + self.hostName + ":" + self.port + ".xml"
	os.system("cp -f " + vocalConstant.mssSeedFile + " " + mssXmlFile)
	vUtils.replaceInFile(mssXmlFile, "MSHOSTIP", self.hostName)
	vUtils.replaceInFile(mssXmlFile, "MSPORT", self.port)
	vUtils.replaceInFile(mssXmlFile, "MSTYPE", self.type)
	vUtils.replaceInFile(mssXmlFile, "PSTNGATEWAYIP", msvo.pstnIP[0])
	vUtils.replaceInFile(mssXmlFile, "PSTNGATEWAYPORT", msvo.pstnPort[msvo.pstnIP[0]])
	
	vUtils.writeToPserver(mssXmlFile, "Marshals", mssXmlFile[5:])
	self.addToList("ListOfMarshalServers",vocalConstant.LOMSSeedFile, "marshalServers", "MS")
	
class msg(mss):
    "Gateway Marshal Process Class"
    def __init__(self, hName, pPort=vocalConstant.defaultMsgPort, msType = "Gateway"):
	mss.__init__(self, hName, pPort, msType)
	
    
class rs(baseProcess):
    "Redirect Server Process Class"
    def __init__(self, hName, pPort=vocalConstant.defaultRsPort, rsType = "Redirect"):
	self.port = pPort
	self.processName = "rs"
	self.hostName = hName
	self.params = []
        self.type = rsType
	
    def process(self):
	rsDirs = baseVocalDirs()
	rsDirs.initDirs()
	
	self.calcParams()
	self.addToVocalConf()
	
	rsXmlFile = "/tmp/" + self.hostName + ":" + self.port + ".xml"
	os.system("cp -f " + vocalConstant.rsSeedFile + " " + rsXmlFile)
	vUtils.replaceInFile(rsXmlFile, "RSHOSTIP", self.hostName)
	vUtils.replaceInFile(rsXmlFile, "RSPORT", self.port)
	
	vUtils.writeToPserver(rsXmlFile, "Redirects", rsXmlFile[5:])
	tmpFile = vUtils.readFromPserver("ListOfRedirectServers", "SystemConfiguration")
	if not(tmpFile):
		tmpFile = "/tmp/"+vocalConstant.LORSSeedFile[string.rfind(vocalConstant.LORSSeedFile, "/"):]
		vUtils.writeFile(tmpFile, "<redirectServers>\n</redirectServers>\n")
	
	LORSStr = vUtils.readFile(vocalConstant.LORSSeedFile)

	LORSStr = re.sub("RSHOSTIP", self.hostName, LORSStr)
	LORSStr = re.sub("RSPORT", self.port, LORSStr)
	
	vUtils.replaceInFile(tmpFile, "</redirectServers>", LORSStr+"\n</redirectServers>")
	
	vUtils.writeToPserver(tmpFile, "SystemConfiguration", "ListOfRedirectServers")

	os.system("rm -f " + tmpFile + ">/dev/null")

class snmptrapd(baseProcess):
    "snmptrapd Process Class"
    def __init__(self, hName):
	self.port = ""
	self.processName = "snmptrapd"
	self.hostName = hName
	self.params = []
    
    def process(self):
	snmptrapdDirs = nmVocalDirs()
	snmptrapdDirs.initDirs()
	
	self.addToVocalConf()

class hbs(baseProcess):
    def __init__(self, hName,pPort = "",hbsType = "Heartbeat"):
	self.port = pPort
	self.processName = "hbs"
	self.hostName = hName
	self.params = []
	self.type = hbsType

    def process(self):
	hbsDirs = baseVocalDirs()
	hbsDirs.initDirs()	
	
	#self.calcParams()
	self.addToVocalConf()
	hbsXmlFile = "/tmp/" + self.hostName + ":0.xml"
	os.system("cp -f " + vocalConstant.hbsSeedFile + " " + hbsXmlFile)
	vUtils.replaceInFile(hbsXmlFile, "HBSHOSTIP", self.hostName)
	vUtils.replaceInFile(hbsXmlFile, "HBSPORT", self.port)
	
	vUtils.writeToPserver(hbsXmlFile, "Heartbeats", hbsXmlFile[5:])
	
	self.addToList("ListOfHeartbeatServers", vocalConstant.LOHSSeedFile, "heartbeatServers", "HBS")

class netMgnt(baseProcess):
    "netMgnt Process Class"
    def __init__(self, hName):
	self.port = ""
	self.processName = "netMgnt"
	self.hostName = hName
	self.params = []
	
    def parseApacheConfig(self, fName):
	serverName = vUtils.getConfigParam(fName, "ServerName")
	documentRoot = vUtils.getConfigParam(fName, "DocumentRoot")
	cgiBinRoot = vUtils.getConfigParam(fName, "ScriptAlias /cgi-bin/")
	return serverName,documentRoot,cgiBinRoot

    def createLink(self, cFile, serName, lLink, lTitle):
	if not(os.path.isfile(cFile)):
	    os.system("cp -f " + vocalConstant.baseHtmlFile + " " + cFile)
	    
	ihStr = vUtils.readFile(cFile)

	lStr = vUtils.createHtmlLinkString(lLink,lTitle)
	ihStr = ihStr[0:string.find(ihStr, "</ul>")] + lStr + ihStr[string.find(ihStr, "</ul>"):]
	    
	vUtils.writeFile(cFile, ihStr)
	

    
    def apacheConfig(self):
	
	if not(os.path.isfile(vocalConstant.httpdConfFile)):
	    print "Apache config file "+vocalConstant.httpdConfFile+" not found. Not configuring "+self.hostName+" to display webpages."

	sName,docRoot,cgiRoot = self.parseApacheConfig(vocalConstant.httpdConfFile)

	if not(sName):
	    sName = socket.gethostname()
	    vUtils.appendFile(vocalConstant.httpdConfFile, "ServerName " + sName)
	   	    
	if not(docRoot):
	    docRoot = vocalConstant.defaultDocRoot
	    vUtils.appendFile(vocalConstant.httpdConfFile, "DocumentRoot " + docRoot)
	   
	if not(cgiRoot):
	    cgiRoot = vocalConstant.defaultCgiRoot
	    vUtils.appendFile(vocalConstant.httpdConfFile, "ScriptAlias /cgi-bin/ " + cgiRoot)


	vUtils.checkNMakeDirs(docRoot)
	vUtils.checkNMakeDirs(cgiRoot)

	os.system("cp -f " + vocalConstant.ksVocalWebDir + "/*.cgi " + cgiRoot)
	jarDir = docRoot + "/jarfiles"
	vUtils.checkNMakeDirs(jarDir)
	vUtils.checkNMakeDirs(docRoot+"/"+vocalConstant.switchName)
	
	os.system("cp -f "+ vocalConstant.ksVocalBinDir + "/*.jar " + jarDir)
        os.system("cp -fr "+ vocalConstant.ksVocalImagesDir + " "+ jarDir)	
	return sName, docRoot, cgiRoot

    def insertLink(self, serName, dRoot, cRoot):
	iFile = dRoot + "/" + vocalConstant.switchName + "/index.html"
	link = serName + "/cgi-bin/" + vocalConstant.nmCgiFile	    
	title = "Network Management on " + serName
	self.createLink(iFile, serName, link, title) 
	
    def process(self):
	netMgntDirs = nmVocalDirs()
	netMgntDirs.initDirs()

	self.addToVocalConf()
	
	nmHbs = hbs(self.hostName)
	nmHbs.process()
	
	nmSTrapd = snmptrapd(self.hostName)
	nmSTrapd.process()
	
	nmSerName, nmDocRoot, nmCgiRoot = self.apacheConfig()
	self.insertLink(nmSerName,nmDocRoot, nmCgiRoot)


class provisioning(netMgnt):
    def __init__(self, hName, pPort=vocalConstant.defaultPsPort):
	self.port = pPort
	self.processName = "ps"
	self.hostName = hName
	self.params = []	
	self.mainPserver = 0

    def calcParams(self):
	self.params.append(vocalConstant.defaultPServerMCIP)
	self.params.append(vocalConstant.defaultPServerMCPort)	
	
    def insertLink(self, serName,dRoot, cRoot):
	iFile = dRoot + "/" + vocalConstant.switchName + "/index.html"
	
	link = serName + "/cgi-bin/" + vocalConstant.psCgiFile	    
	title = "Provisioning on " + serName
	self.createLink(iFile, serName, link, title)

	link = serName + "/cgi-bin/" + vocalConstant.psUserCgiFile	    
	title = "User Preferences on " + serName
	self.createLink(iFile, serName, link, title)
	

    def process(self):
	psDirs = provVocalDirs()
	psDirs.initDirs()
	psVObj = psClass.vocalObject()
        psVObj.load(vocalConstant.ksVocalIdFile)

	os.system("chown -R "+psVObj.dict[self.hostName][0]+" "+vocalConstant.vocalProvDir)
	
	self.calcParams()
	self.addToVocalConf()

	psSerName, psDocRoot, psCgiRoot = self.apacheConfig()
	self.insertLink(psSerName, psDocRoot, psCgiRoot)
	
	
	if (psVObj.pServer == self.hostName):
	    self.mainPserver = 1
	else:
	    self.mainPserver = 0
	    
	if not(self.mainPserver):
	    pServerCheck = vUtils.hostUp(psVObj.bupServer, psVObj.bupServerPort, "GET Accounts foo")
	else:
	    pServerCheck = vUtils.hostUp(psVObj.pServer, psVObj.pServerPort, "GET Accounts foo")

	if not(pServerCheck):
	    print "Cannot write provisioning data without at least one Provisioning Server running."
	    return 0

class baseFs(rs):
    "Feature Server Base Process Class"
    def __init__(self, hName, fsType, pPort):
	self.port = pPort
	self.processame = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

    def addToLofs(self):
	tmpFile = vUtils.readFromPserver("ListOfFeatureServers", "SystemConfiguration")

	if not(tmpFile):
	    tmpFile = "/tmp/LOFS.xml"
	    vUtils.writeFile(tmpFile, "<featureServers>\n</featureServers>")

	tmpStr = vUtils.readFile(tmpFile)
	
	LOFSStr = vUtils.readFile(vocalConstant.LOFSSeedFile)

	LOFSStr = re.sub("FSHOSTIP", self.hostName, LOFSStr)
	LOFSStr = re.sub("FSPORT", self.port, LOFSStr)
	LOFSStr = re.sub("FSTYPE", self.type, LOFSStr)	

	findFsType = string.find(tmpStr, self.type+"Group")

	if (findFsType == -1):
	    vUtils.replaceInFile(tmpFile, "</featureServers>",LOFSStr+"\n</featureServers>")
	else:
	    tmpStr = tmpStr[0:string.find(tmpStr, "</serverGroup>", findFsType)]+LOFSStr[string.find(LOFSStr, "<server", string.find(LOFSStr,self.type+"Group")):string.rfind(LOFSStr, "</serverGroup>")]+ tmpStr[string.find(tmpStr, "</serverGroup>", findFsType):]
	    vUtils.writeFile(tmpFile, tmpStr)
	vUtils.writeToPserver(tmpFile, "SystemConfiguration", "ListOfFeatureServers")

    def process(self):
	fsDirs = baseVocalDirs()
	fsDirs.initDirs()

	self.calcParams()
	self.addToVocalConf()
	
	fsXmlFile = "/tmp/" + self.hostName + ":" + self.port + ".xml"
	os.system("cp -f " + vocalConstant.fsSeedFile + " " + fsXmlFile)
	vUtils.replaceInFile(fsXmlFile, "FSHOSTIP", self.hostName)
	vUtils.replaceInFile(fsXmlFile, "FSPORT", self.port)
	vUtils.replaceInFile(fsXmlFile, "FSTYPE", self.type)

	vUtils.writeToPserver(fsXmlFile, "Features", fsXmlFile[5:])

	self.addToList("ListOfFeatureServers",vocalConstant.LOFSSeedFile, "featureServers", "FS")

class fscb(baseFs):
    def __init__(self, hName, pPort, fsType="CallBlocking"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

class fsfnab(baseFs):
    def __init__(self, hName, pPort, fsType="ForwardNoAnswerBusy"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

class fscs(baseFs):
    def __init__(self, hName, pPort, fsType="CallScreening"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

class fsfac(baseFs):
    def __init__(self, hName, pPort, fsType="ForwardAllCalls"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

class fscib(baseFs):
    def __init__(self, hName, pPort, fsType="CallerIdBlocking"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

class fscr(baseFs):
    def __init__(self, hName, pPort, fsType="CallReturn"):
	self.port = pPort
	self.processName = "fs"
	self.hostName = hName
	self.type = fsType
	self.params = []

    def process(self):

	baseFs.process(self)
	vUtils.writeToPserver(vocalConstant.fscrCplFile, "Features_"+self.hostName+"_"+self.port+"_Called", "*69@vovida.com.cpl.xml")

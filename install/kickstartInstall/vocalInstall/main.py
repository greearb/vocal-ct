import directoryChecks, commands, vUtils, vocalConstant, string, os, re, socket, psClass

from derivedProcesses import *

def gcdCheck():
    
    print "Checking for GlobalConfigData on Provisioning Server....."

    gcdFile = vUtils.readFromPserver("GlobalConfigData", "SystemConfiguration")
    
    if not(gcdFile):
	print "Writing GlobalConfigData to Pserver"
	os.system("cp -f "+vocalConstant.gcdSeedFile+" /tmp")
	tmpGCDFile = "/tmp/"+vocalConstant.gcdSeedFile
	vUtils.replaceInFile(tmpGCDFile, "MULTICASTIP", vocalConstant.defaultMCIP)
	vUtils.replaceInFile(tmpGCDFile, "MULTICASTPORT", vocalConstant.defaultMCPort)
	vUtils.writeToPserver(tmpGCDFile, "SystemConfiguration", "GlobalConfigData")
    else:
	print "GlobalConfigData present on Provisioning Server."


def makeStartOnReboot():
	if (string.find(commands.getoutput("ls -1 "+vocalConstant.rebootLinkDir+"*"), "vocalstart") != -1):
		print "Link in "+vocalConstant.rebootLinkDir+" already exists"
		return 0
	l="90"
	c = "S"
	while (os.path.isfile(vocalConstant.rebootLinkDir+c+l+vocalConstant.vocalstartFile)):
		l = `int(l)+1`
		if (int(l) > 99):
			c = ord(c)+1
			l = "11"
	os.system("ln -s "+vocalConstant.sysInitDir+vocalConstant.vocalstartFile+" " +vocalConstant.rebootLinkDir+c+l+vocalConstant.vocalstartFile)
			

def kickstartVocal():
    mainVO = psClass.vocalObject()
    mainVO.load(vocalConstant.ksVocalIdFile)

    myName = vUtils.getIP(socket.gethostname())   
    print myName
    myUser = mainVO.dict[myName][0]
    if myUser in vocalConstant.vocalProcesses:
	print "Warning: VocalUser parameter is: "+myName+". If this is unintentional, please re-kickstart the machine."
    if os.path.isfile(vocalConstant.vocalConfFile):
        os.system("rm -f "+vocalConstant.vocalConfFile)
    checkDirs = directoryChecks.baseVocalDirs()
    checkDirs.initDirs()
    os.system("cp -rf "+vocalConstant.ksVocalBinDir+"/* "+vocalConstant.vocalBinDir)
    os.system("cp -rf "+vocalConstant.vocalBinDir+"/vocalstart "+vocalConstant.sysInitDir)

    makeStartOnReboot()

    pDict = mainVO.dict
    
    uPort = ""

    gcdCheck()
    vUtils.checkNWriteToPserver(vocalConstant.LOJSSeedFile,"SystemConfiguration", "ListOfJtapiServers")
    
    for ii in range(1,len(pDict[myName])):
	pSplit = string.split(pDict[myName][ii], ":")
	if (len(pSplit) == 2):
	    uProcess = pSplit[0]
	    uPort = pSplit[1]
            VCNGParam = vUtils.vocalPortInUse(uPort)
	    if VCNGParam:
		print "Warning: Specified port: "+uPort+" \n\tfor Process: "+uProcess+" is already assigned/in use.\n\tNot Processing....."
                continue
	elif (len(pSplit) > 2):
	    print "Fatal Error: Process "+uProcess+" for host "+myName+" cannot be resolved.\nExiting....."

	uProcess = pSplit[0]
	if not(uProcess in vocalConstant.vocalProcesses):
	    print "Fatal Error: Process "+uProcess+" for host "+myName+" cannot be resolved.\nExiting....."
	    return 0

	if (uProcess == "mss"):
	    if not(uPort):
		uPort =vocalConstant.defaultMssPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort, VCNGParam)
            psObj = mss(myName,uPort)
	    psObj.process()

	elif (uProcess == "msg"):
	    if not(uPort):
		uPort =vocalConstant.defaultMsgPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = msg(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "rs"):
	    if not(uPort):
		uPort =vocalConstant.defaultRsPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = rs(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "netMgnt"):
	 	    
	    psObj = netMgnt(myName)
	    psObj.process()
	    
	elif (uProcess == "ps"):
	    if not(uPort):
		uPort =vocalConstant.defaultPsPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = provisioning(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fscb"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSCBPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fscb(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fsfnab"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSFNABPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fsfnab(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fscs"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSCSPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fscs(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fsfac"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSFACPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fsfac(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fscr"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSCRPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fscr(myName,uPort)
	    psObj.process()
	    
	elif (uProcess == "fscib"):
	    if not(uPort):
		uPort =vocalConstant.defaultFSCIBPort 
	    uPort = vUtils.vocalCheckNGetPort(uPort)
	    
	    psObj = fscib(myName,uPort)
	    psObj.process()
	
	else:
	    print "Error: Process "+uProcess+" for host "+myName+" cannot be resolved."

	os.system("chmod -R a+rwx "+vocalConstant.vocalDir)
	    
	

	
kickstartVocal()

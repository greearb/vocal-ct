import vUtils, vocalConstant, string, os, re, socket, psClass

from derivedProcesses import *

def kickstartVocal():
    mainVO = psClass.vocalObject()
    mainVO.load(vocalConstant.ksVocalIdFile)

    myName = vUtils.getIP(socket.gethostname())   
    print myName
    myUser = mainVO.dict[myName][0]
    if myUser in vocalConstant.vocalProcesses:
	print "Warning: VocalUser parameter is: "+myName+". If this is unintentional, please re-kickstart the machine."

    os.system("cp -rf "+vocalConstant.ksVocalBinDir+"/* "+vocalConstant.vocalBinDir)
    os.system("cp -rf "+vocalConstant.vocalBinDir+"/vocalstart "+vocalConstant.sysInitDir)

    pDict = mainVO.dict
    
    uPort = ""

    for ii in range(1,len(pDict[myName])):
	pSplit = string.split(pDict[myName][ii], ":")
	if (len(pSplit) == 2):
	    uProcess = pSplit[0]
	    uPort = pSplit[1]
	    if vUtils.vocalPortInUse(uPort):
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
	    uPort = vUtils.vocalCheckNGetPort(uPort)
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

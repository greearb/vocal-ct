import string, sys, os, vocalConstant, psClass, random, re
from directoryChecks import *

class baseProcess:
    "Base Class for the Processes"
    def __init__(self, pPort):
	self.port = pPort
	self.processName = "base"
	self.hostName = "0.0.0.0"
	self.params = []
	self.type = "base"

    def addToList(self, psListFile, seedFile, listType, seedKey):
	pslf = vUtils.stripExt(psListFile)
	tmpFile = vUtils.readFromPserver(pslf, "SystemConfiguration")

	if not(tmpFile):
	    tmpFile = "/tmp/"+seedFile[string.rfind(seedFile, "/"):]
 	    vUtils.writeFile(tmpFile, "<"+listType+">\n</"+listType+">")
            tmpStr = vUtils.readFile(tmpFile)
            tmpSplit = []
	else:
	    tmpStr = vUtils.readFile(tmpFile)
	    tmpSplit = string.split(tmpStr)
	hostToken = 'host="'+self.hostName+'"'
	if not(self.port): portToken = 'port="0"'
	else: portToken = 'port="'+self.port+'"'
	for ii in range(0,len(tmpSplit)-1):
		if (string.split(tmpSplit[ii], ">")[0]	== hostToken) and (string.split(tmpSplit[ii+1],">")[0] == portToken):
			print "Server already exist in "+psListFile+"\nExiting without adding self to "+psListFile
			return 0
	
	listStr = vUtils.readFile(seedFile)

	listStr = re.sub(seedKey+"HOSTIP", self.hostName, listStr)
	listStr = re.sub(seedKey+"PORT", self.port, listStr)
	listStr = re.sub(seedKey+"TYPE", self.type, listStr)	

	findType = string.find(tmpStr, self.type+"Group")

	if (findType == -1):
	    vUtils.replaceInFile(tmpFile, "</"+listType+">",listStr+"\n</"+listType+">")
	else:
	    tmpStr = tmpStr[0:string.find(tmpStr, "</serverGroup>", findType)]+listStr[string.find(listStr, "<server", string.find(listStr,self.type+"Group")):string.rfind(listStr, "</serverGroup>")]+ tmpStr[string.find(tmpStr, "</serverGroup>", findType):]
	    vUtils.writeFile(tmpFile, tmpStr)
	vUtils.writeToPserver(tmpFile, "SystemConfiguration", pslf)

    
    def process(self):
	"This is the process method"
	return
    def calcParams(self):
	self.params.append(self.port)

    def addToVocalConf(self):
	if not(os.path.isfile(vocalConstant.vocalConfFile)):
	    vcFile = open(vocalConstant.vocalConfFile, 'w')
	    bpvObj = psClass.vocalObject()
	    bpvObj.load(vocalConstant.ksVocalIdFile)
	    vcFile.write("VOCALUSER "+ bpvObj.dict[self.hostName][0] + "\n")
	    
	    # Checks to see if the PSERVER defined in the identity.cfg file is itself
	    # If not, it writes the PSERVERBACKUP defined in the identity.cfg file as the PSERVER
	    # in vocal.conf. 
	    
	    if (socket.gethostbyname(socket.gethostname()) == bpvObj.pServer):
		vcFile.write("PSERVER " + bpvObj.pServer + ":" + bpvObj.pServerPort + "\n")		
		vcFile.write("PSERVERBACKUP " + bpvObj.bupServer + ":" + bpvObj.bupServerPort + "\n")
	    else:
		vcFile.write("PSERVER " + bpvObj.bupServer + ":" + bpvObj.bupServerPort + "\n")
		vcFile.write("PSERVERBACKUP " + bpvObj.pServer + ":" + bpvObj.pServerPort + "\n")		
		
	    vcFile.write("LOG_LEVEL " + vocalConstant.defaultLogLevel + "\n")
	    vcFile.write(self.processName + " ")
	    if (self.params):
		for ii in range(len(self.params)):
		    vcFile.write(self.params[ii] + " ")
	    vcFile.write("\n")
	    vcFile.write("snmpd\n")
	    vcFile.close()
	elif vUtils.processIn(vocalConstant.vocalConfFile, [self.processName] + self.params[:]):		
	    print "Process already present: "+`[self.processName]+self.params[:]`+". \n\tExiting without adding self to vocal.conf"
	   
	    return 0
	else:
            #print `self.processName`+" " + `self.params`
            #print `vUtils.processIn(vocalConstant.vocalConfFile, [self.processName]+self.params[:])`
	    vcFile = open(vocalConstant.vocalConfFile, 'a')
	    vcFile.write(self.processName + " ")
	    for ii in self.params:
		vcFile.write(ii + " ")
	    vcFile.write("\n")
	    vcFile.close()
    
   



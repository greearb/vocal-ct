# The Process-List & Process methods for each process in the process-list.


import string, vUtils, vocalConstant
	
# Class vocalObject contains a list of hosts, and a dictionary
# indexed by the host, which contains a list of all the processes
# on the host. The first element in the list is the VocalUser
# who will be starting up the processes. 
    
class vocalObject:
    "The Complete Vocal Setup"
    def __init__(self):
	self.hostList = []
	self.dict = {}
	self.pServer = ""
	self.pServerPort = "6005"
	self.bupServer = ""
	self.bupServerPort = "6005"
	self.pstnIP = []
	self.pstnPort = {}

    def load(self, hostFile):
	
	
	# Load list of PSTN Gateways
	tempDict = {}
	tempDictList = []
	tempDictList = vUtils.loadPs(hostFile, "PSTNGATEWAYS")
	if not(tempDictList):
	    tempDictList.append("0.0.0.0")
	tempDict["PSTNGATEWAYS"] = vUtils.loadPs(hostFile, "PSTNGATEWAYS")
	for ii in range(len(tempDict["PSTNGATEWAYS"])):
	    if (len(string.split(tempDict["PSTNGATEWAYS"][ii], ":")) == 2):
		if not(vUtils.isIn(self.pstnIP, string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0])):
		       self.pstnIP.append(string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0])
		       self.pstnPort[string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0]] = string.split(tempDict["PSTNGATEWAYS"][ii], ":")[1]
	    elif (len(string.split(tempDict["PSTNGATEWAYS"][ii], ":")) == 1):
		if not(vUtils.isIn(self.pstnIP, string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0])):
		       self.pstnIP.append(string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0])
		       self.pstnPort[string.split(tempDict["PSTNGATEWAYS"][ii], ":")[0]] = vocalConstant.defaultPSTNPort

	iFile = open(hostFile)
	iFileRead = iFile.readline() 
           
	while (iFileRead):
	    iFileSplit = string.split(iFileRead)
	    if (iFileSplit):
		if (iFileSplit[0][0] != '#'):	
		    if (len(string.split(iFileSplit[0], ".")) == 4):			
			hostName = iFileSplit[0]		    
			if not(vUtils.isIn(self.hostList, hostName)):
			    if not(vUtils.isIn(tempDict["PSTNGATEWAYS"], hostName)):
				   self.hostList.append(hostName)
		
	    iFileRead = iFile.readline()
    
	for x in range(len(self.hostList)):
	    self.dict[self.hostList[x]] = vUtils.loadPs(hostFile, self.hostList[x])
    
			
	iFile.close()
	iFile = open(hostFile)
	iFileRead = iFile.readline()
	while (iFileRead):
	    iFileSplit = string.split(iFileRead)
	    if (iFileSplit):
		if (iFileSplit[0][0] != '#') and (iFileSplit[0] == "PSERVER"):
		    self.pServer = string.split(iFileSplit[1], ":")[0]
		    if len(string.split(iFileSplit[1], ":")) == 2:
			self.pServerPort = string.split(iFileSplit[1], ":")[1]
		elif (iFileSplit[0][0] != '#') and (iFileSplit[0] == "PSERVERBACKUP"):
		    self.bupServer = string.split(iFileSplit[1], ":")[0]
		    if len(string.split(iFileSplit[1], ":")) == 2:
			self.bupServerPort = string.split(iFileSplit[1], ":")[1]		
	    iFileRead = iFile.readline()
	iFile.close()		    


	
	


#myList = vUtils.loadPs("myid.cfg", "192.168.1.65")
#print `myList`
#myVO = vocalObject()
#myVO.load("myid.cfg")
#print "process.dict: ",`myVO.dict`
#print "pserver: ",`myVO.pServer`
#print "pserver port: ",`myVO.pServerPort`
#print "backup-pserver:",`myVO.bupServer`
#print "backup-pserver port: ",`myVO.bupServerPort`
#print "list of PSTN GWs: ",`myVO.pstnIP`
#print "GWs dict: ",`myVO.pstnPort`

import string, re, socket, os, vocalConstant, psClass, random
# Small function to check whether a given object exists in a list.
# Returns false if the list is empty or if the element is not 
# present in it. 

def isIn(iiList, iiObj):
    if not(iiList):
	return 0
    for x in range(len(iiList)):
	if (iiObj == iiList[x]): return 1
    return 0
    


# Todo: Write an error-check function that checks the id file
#       for errors.		   


# Reads the id file and returns a list of all the processes for a given host.
# The first element in the list is the VocalUser.		    

def loadPs(hostFile, host):
    

    psLister = []
    idFile = open(hostFile)
    hostName = host
    idFileRead = idFile.readline()
    
    while (idFileRead):
	lineSplits = string.split(idFileRead)
	while (lineSplits) and (lineSplits[0] == host):	
	    while (lineSplits[0][0] == '#') or (lineSplits[-1] != '{'):
		
		idFileRead = idFile.readline()
		lineSplits = string.split(idFileRead)

		    
	    idFileRead = idFile.readline()
	    lineSplits = string.split(idFileRead)
		    		    
	    while (idFileRead) and ((lineSplits[0][0] == '#') or (lineSplits[-1] != '}')):
		
		if (lineSplits[0][0]!='#'):
		    if (len(string.split(idFileRead, "=")) >= 2) and (string.split(idFileRead)[0] == "VocalUser"):
			psLister.insert(0,string.split(idFileRead)[-1])
		    else:
			cs = string.split(idFileRead, ",")
			css = ''
			for j in range(len(cs)):			
			    css = css + ' ' + cs[j]
			
			
			psLister = psLister + string.split(css)
		idFileRead = idFile.readline()
		lineSplits = string.split(idFileRead)		
	idFileRead = idFile.readline()
    return psLister

# Written to check the vocal.conf file if a port is already in the file.
# Can be used generically to check whether a string is in a file.

def portIn(vConfFile, cPort):
    if not(os.path.isfile(vConfFile)): return 0
    vFile = open(vConfFile)
    vFileRead = vFile.readline()
    while (vFileRead):
	vFileSplit = string.split(vFileRead)
	if (vFileSplit):
	    if (vFileSplit[0][0] != '#'):
		for ii in range(len(vFileSplit)):
		    if (cPort == vFileSplit[ii]): return 1
	vFileRead = vFile.readline()
    return 0

# Written to check whether a process exists in the vocal.conf File.
# Arguments are the file and a list of strings, which are matched against 
# the split lines of the file.
	    
def processIn(vConfFile, pList):
    if not(os.path.isfile(vConfFile)): return 0
    vFile = open(vConfFile)
    vFileRead = vFile.readline()
    while (vFileRead):
	vFileSplit = string.split(vFileRead)
	if (vFileSplit == pList): 
	    vFile.close()
	    return 1
	vFileRead = vFile.readline()
    vFile.close()
    return 0

# Replaces a given string in a file with another specified string

def replaceInFile(hFile, fString, rString):
    riFile = open(hFile)
    riFileRead = riFile.read()
    riFile.close()
    riFileReadSub = re.sub(fString, rString, riFileRead)
    riFile = open(hFile, "w")
    riFile.write(riFileReadSub)
    riFile.close()

# Returns the Length of a file

def fileLen(hFile):
    hhFile = open(hFile)
    fileLength = len(hhFile.read())
    hhFile.close()
    return fileLength

# Checks whether a given string is in a file. More generic than "portIn"

def isinFile(hFile, hStr):
    iiFile = open(hFile)
    if (string.find(iiFile.read(), hStr) != -1): 
	iiFile.close()
	return 1
    else:
	iIFile.close()
	return 0

#Gets a host IP. Defaults it to loopback, if unresolved

def getIP(sName):
    if (len(string.split(sName,".")) == 4):
	    hIP = socket.gethostbyname(sName)
    else:
	try:
	    hIP = socket.gethostbyname(sName)
	except:
	    hIP = "127.0.0.1"
    return hIP
 


# Checks whether a given port is in use.

def portInUse(pPort):    
    si = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    try:
	hIP = socket.gethostbyname(socket.gethostname())
    except:
	print "Warning: Host IP not resolved. Using loopback IP."
	hIP = "127.0.0.1"
    try:
	si.bind(hIP, int(pPort))
    except:
	si.close()
	return 1
    else:
	si.close()
	return 0

def vocalCheckNGetPort(pPort, vParam = 0, cPort=vocalConstant.defaultPortNotUsed):
    qPort = pPort
    if vocalPortInUse(qPort, vParam):
	qPort = getVocalUnusedPort(cPort)
    return qPort

def getVocalUnusedPort(pPort):
    while portIn(vocalConstant.vocalConfFile, pPort) or portInUse(pPort):
	pPort = `int(pPort)+1`
    return pPort

def vocalPortInUse(pPort, vpPort=0):
    tempI = portIn(vocalConstant.vocalConfFile, pPort)
    tempJ = portInUse(pPort)
    if vpPort:
        if (tempI or tempJ):
	    return 1
    else:
	if (tempI):
            return 1
    return 0

def hostUp(huName, huPort, huMessage):

    try:
	pPort = int(huPort)
    except:
	raise portNotIntError("Second Argument to vUtils.hostUp does not resolve to an integer.")
    else:
	pass
    
    phName = socket.gethostbyname(huName)
    if (huMessage[-1]!= '\n'):
	huMessage = huMessage + "\n"
    
    try:
	hus = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	hus.connect(phName, pPort)
    except:
	hus.close()
	return ""
    else:
	hus.sendto(huMessage, (phName, pPort))
	retStr, sendAddr = hus.recvfrom(1024)
	hus.close()
	return retStr



# Gets the values of a given config parameter in a given config file.
# Only works when the config file is of the syntax:
# ConfigParam ConfigValue
# Returns null string if not found

def getConfigParam(cFile, cParam):
    if not (os.path.isfile(cFile)): return ""
    cpSplit = string.split(cParam)
    cpLen = len(cpSplit)
    gcFile = open(cFile)
    gcRead = gcFile.readline()    
    gcSplit = string.split(gcRead)
    quoted = ['"', "'"]
    cpEqual = 0

    while (gcRead): 
	if (gcSplit):
	    if (gcSplit[0][0] != '#'):
		for ii in range(cpLen):		    
		    if (gcSplit[ii] == cpSplit[ii]):
			cpEqual = 1
			pass
		    else:
			cpEqual = 0
			break
		if cpEqual:
		    if (gcSplit[cpLen][0] in quoted):
			return gcSplit[cpLen][1:-1]
		    else:
			return gcSplit[cpLen]
	gcRead = gcFile.readline()    
	gcSplit = string.split(gcRead)

    return ""


# Reads and returns the contents of a file.

def readFile(fFile):
    rFile = open(fFile)
    rFileStr = rFile.read()
    rFile.close()
    return rFileStr

# Writes a given string into a given file.

def writeFile(aFile, aStr):
    aaFile = open(aFile, "w")
    aaFile.write(aStr)
    aaFile.close()

# Appends a given file with a given string

def appendFile(aFile, aStr):
    aaFile = open(aFile, "a")
    if (aStr[-1] != "\n"): 
	aaFile.write(aStr + "\n")
    else:
	aaFile.write(aStr)	
    aaFile.close()


# Checks for the existence of specified directories, and creates them
# if they do nto exist

def checkNMakeDirs(vDir):
     if not(os.path.isdir(vDir)): os.system("mkdir -p "+vDir)

# Returns a string that would specify an HTML link for a given link and title.

def createHtmlLinkString(cLink, cTitle):
    return "<li><a href=\"http://"+cLink+"\">"+cTitle+"</a>\n"

# Returns 0 if the number is negative. Returns the number otherwise.

def noNegative(iNum):
    if (iNum >=0 ): return iNum
    else: return 0

def stripExt(fileStr, fileExt = ".xml"):
    if (fileStr[string.rfind(fileStr, "."):] == fileExt):
	return fileStr[0:string.rfind(fileStr, ".")]
    else:
	return fileStr

def psPutStr(psAcc, psFile, xFile):
    retStr = "PUT " + psAcc + " " + psFile + "\n"
    retStr = retStr + "Content-Length: " + `fileLen(xFile)` + "\n"
    return retStr

def psGetStr(psAcc, psFile):
     retStr = "GET " + psAcc + " " + psFile + "\n"
     return retStr

def beginXmlGroup(grStr):
    return "\t<"+grStr+"\n"

def dsXmlStr(cp, cv):
    return "\t\t"+cp+"=\""+cv+"\"\n"

def endXmlGroup(grpStr):
    return "\t/"+grpStr+">\n"

def selectPserver():
    svObj = psClass.vocalObject()
    svObj.load(vocalConstant.ksVocalIdFile)
    connServer = svObj.pServer
    connPort = string.atoi(svObj.pServerPort)
    
    if not(hostUp(connServer, connPort, "GET Accounts foo")):
	connServer = svObj.bupServer
	connPort = string.atoi(svObj.bupServerPort)
	if not(hostUp(connServer, connPort, "GET Accounts foo")):
	    print "Cannot open connection to either main or backup pserver. No provisioning data written."
	    connServer = ""
	    connPort = 0
    return connServer, connPort
	    
def writeToPserver(xmlFile, psGroup, psFile):
    print "writeToPserver " + xmlFile + " " + psGroup + " " + psFile
    connectServer, connectPort = selectPserver()	 
    psTuple = (connectServer, connectPort)
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    if not(connectServer): 
        print "Provisioning Server not running. No data written." 
	return ""
 
    s.connect(connectServer, connectPort)
    psXmlFile = stripExt(psFile)
    cmdStr = psPutStr(psGroup, psXmlFile, xmlFile)
    cmdStr = cmdStr + readFile(xmlFile)
    
    s.sendto(cmdStr, psTuple)
    statStr, fromAddr = s.recvfrom(1024)
    
    if (string.split(statStr)[0] == '200'):
	print "Write to provisioning server successful."
	return 1
    else:
	print "Write to provisioning server failed."
	return 0
    
def readFromPserver(xmlFile, psGroup):
    print "readFromPserver " + xmlFile + " " + psGroup 

    connectServer, connectPort = selectPserver()
    psTuple = (connectServer, connectPort)
    if not(connectServer): 
        print "Provisioning Server not running. No data read." 
	return ""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(connectServer, connectPort)
    
    psXmlFile = stripExt(xmlFile)
    s.sendto(psGetStr(psGroup, psXmlFile), psTuple)
    sockRead, pServerAddr = s.recvfrom(786432)
    
    if (string.split(sockRead)[0] == "200"):
	fileName = "/tmp/" + `random.randint(10000, 99999)`	     
	writeFile(fileName,sockRead[string.find(sockRead, "\n", string.find(sockRead, "\n") + 1) + 1:])	    
	return fileName
    else:
	print "Error while trying to read from Pserver: " + sockRead[0:string.find(sockRead, "\n", string.find(sockRead, "\n") + 1)]
	return ""

def checkNWriteToPserver(xmlFile, psGroup, psFile):
    print "Checking for "+psFile+" on Provisioning Server"
    tFile = readFromPserver(psFile, psGroup)
    if not tFile:
	print psFile+" not present on Provisioning Server. Writing to Provisioning Server..."
	writeToPserver(xmlFile, psGroup, psFile)
    else:
	print psFile+" present on Provisioning Server"

#Work in progress: To be finished later, once logistics are worked out
#def mkDeployedServersFile(dFile):
#    vuvo = psClass.vocalObject()
#    vuvo.load(vocalConstant.ksVocalIdFile)
#    initStr = "<deployedServers>\n\t<global\n\t\tmulticastHost=\""+vocalConstant.defaultPserverMCIP+"\"\n"
#    initStr = initStr+"\t\tmulticastPort=\""+vocalCosntant.defaultPserverMCPort+"\"\n"
#    initStr = initStr+"\t/>\n"
#    vUtils.writeFile(dFile, initStr)
#    for ii in vuvo.hostList:
#	for jj in range(1, len(vuvo.dict[ii])):
#	    if (vuvo.dict[ii][jj] == "mss"):
#		inStr = vUtils.beginXmlGroup("marshalServer")

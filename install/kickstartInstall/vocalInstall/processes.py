import string, sys, os, vocalConstant, psClass, random
from directoryChecks import *

class baseProcess:
    "Base Class for the Processes"
    def __init__(self, pPort):
	self.port = pPort
	self.processName = "base"
	self.hostName = "0.0.0.0"
	self.params = []
    
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
	    vcFile.write("VOCALUSER "+ bpvObj.dict(self.hostName)[0] + "\n")
	    vcFile.write("PSERVER " + bpvObj.pServer + ":" + bpvObj.pServerPort + "\n")
	    
	    vcFile.write("PSERVERBACKUP " + bpvObj.bupServer + ":" + bpvObj.bupServerPort + "\n")
	    vcFile.write("LOG_LEVEL " + vocalConstant.defaultLogLevel + "\n")
	    vcFile.write(self.processName + " ")
	    if (self.params):
		for ii in range(len(self.params)):
		    vcFile.write(self.params[ii] + " ")
	    vcFile.write("\n")
	    vcFile.write("snmpd\n")
	elif vUtils.processIn(vocalConstant.vocalConfFile, [self.processName] + self.params[:]):		
	    print "Process already present. Exiting without adding self to vocal.conf"
	    vcFile.close()
	    return 0
	else:
	    vcFile = open(vocalConstant.vocalConfFile, 'a')
	    vcFile.write(self.processName + " ")
	    for ii in self.params:
		vcFile.write(ii + " ")
	    vcFile.write("\n")
	vcFile.close()
    
   



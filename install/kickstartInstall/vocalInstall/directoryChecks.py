import string, sys, os, vUtils, vocalConstant

class baseVocalDirs:
    def __init__(self):
	self.dirList = []
	self.dirList.append("/usr/local/vocal")
	self.dirList.append("/usr/local/vocal/bin")
	self.dirList.append("/usr/local/vocal/log")
	self.dirList.append("/usr/local/vocal/etc")   
    
    def initDirs(self):
	for x in self.dirList:
	    vUtils.checkNMakeDirs(x)
    
class provVocalDirs(baseVocalDirs):
    def __init__(self):
	baseVocalDirs.__init__(self)
	self.dirList.append("/usr/local/vocal/provisioning_data")
	for ii in range(1,vocalConstant.psBins+1):
	    self.dirList.append("/usr/local/vocal/provisioning_data/" + chr(ord('A') + (ii/26)/26) + chr(ord('A')+(ii/26)%26) + chr(ord('A')+vUtils.noNegative((ii%26)-1))) 
	    
	    
class nmVocalDirs(baseVocalDirs):
    def __init__(self):
	baseVocalDirs.__init__(self)
	self.dirList.append("/usr/local/vocal/NetMgnt")

class cdrVocalDirs(baseVocalDirs):
    def __init__(self):
	baseVocalDirs.__init__(self)
	self.dirList.append("/usr/local/vocal/billing")
   

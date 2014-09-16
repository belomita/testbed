import os
import os.path
import sys
import zipfile


class Config:
	DumpFilePath = ''
	DumpTextPath = ''
	
class Util:
	@staticmethod
	def Log(msg):
		print(msg)

class DumpEntry:
	def __init__(self, zipfilename):
		self.ZipFileName = zipfilename
		self.DumpFileName = None
		self.LogFileName = None
		self.GetNameList()
		self.MacAddress = self.ParseMacAddress()
	def GetNameList(self):
		filepath = os.path.join(Config.DumpFilePath, self.ZipFileName)
		if filepath.endswith('.zip') and os.path.exists(filepath):
			namelist = zipfile.ZipFile(filepath).namelist()
			for name in namelist:
				if name.endswith('.dmp'):
					self.DumpFileName = name
				elif name.endswith('.log'):
					self.LogFileName = name
		else:
			Util.Log('DumpEntry.GetNameList(): invalid file path: ' + filepath)
	def ParseMacAddress(self):
		macAddress = ''
		parts = self.ZipFileName.split('_')
		if len(parts) >= 2: macAddress = parts[1]
		if macAddress == '': macAddress = 'no_mac_address'
		return macAddress
	def GetUniqDumpFileName(self):
		if self.MacAddress and self.DumpFileName:
			return self.MacAddress + '.' + self.DumpFileName
		return None
	def GetUniqLogFileName(self):
		if self.MacAddress and self.LogFileName:
			return self.MacAddress + '.' + self.LogFileName
		return None

class DumpProcesser:
	def __init__(self):
		self.DumpEntries = []
	def LoadEntries(self):
		files = os.listdir(Config.DumpFilePath)
		for filename in files:
			dumpEntry = DumpEntry(filename)
			self.DumpEntries.append(dumpEntry)
	def OutputEntries(self):
		for entry in self.DumpEntries:
			msg = '%s\n\t%s\n\t%s\n' % (entry.ZipFileName, entry.DumpFileName, entry.LogFileName)
			Util.Log(msg)
	def OutputUniqEntries(self):
		for entry in self.DumpEntries:
			msg = '%s\n\t%s\n\t%s\n' % (entry.ZipFileName, entry.GetUniqDumpFileName(), entry.GetUniqLogFileName())
			Util.Log(msg)

def Main():
	Config.DumpFilePath = 'Dumps'
	if len(sys.argv) > 1:
		Config.DumpFilePath = sys.argv[1]
	if not os.path.exists(Config.DumpFilePath):
		print('Usage: ' + sys.argv[0] + ' DumpFilePath')
		return
	dumpProcesser = DumpProcesser()
	dumpProcesser.LoadEntries()
	dumpProcesser.OutputEntries()
	dumpProcesser.OutputUniqEntries()
if __name__ == '__main__':
	Main()


















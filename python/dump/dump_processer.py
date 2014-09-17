import os
import os.path
import sys
import zipfile
import json


class Config:
	DumpFilePath = ''
	DumpTextPath = ''
	
class Util:
	@staticmethod
	def Log(msg):
		print(msg)

class DumpZipFile:
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
			Util.Log('DumpZipFile.GetNameList(): invalid file path: ' + filepath)
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

class DumpTextEntry:
	def __init__(self, textfile, zipfile):
		self.TextFileName = textfile
		self.DumpFileName = zipfile.DumpFileName
		self.ZipFileName  = zipfile.ZipFileName
		self.MacAddress   = zipfile.MacAddress
		self.CrashAddress = self.ParseCrashAddress()
		self.ProblemClass = None
		self.StackText    = None
	def ParseCrashAddress(self):
		return self.TextFileName[4:8]
	def ParseDumpText(self):
		filepath = os.path.join(Config.DumpTextPath, self.TextFileName)
		with open(filepath) as f:
			content = f.read()
			self.ProblemClass = self.ParseField(content, 'PRIMARY_PROBLEM_CLASS:  ', '\n')
			self.StackText = self.ParseField(content, 'STACK_TEXT:  \n', '\n\n')
			self.RefineStackText()
	def ParseField(self, content, prefix, postfix):
		startIndex = content.find(prefix)
		if startIndex > 0:
			startIndex += len(prefix)
			endIndex = content.find(postfix, startIndex)
			if endIndex > startIndex:
				field = content[startIndex : endIndex]
				return field
		return None
	def RefineStackText(self):
		if self.StackText != None:
			lines = self.StackText.split('\n')
			lines = [line.startswith('WARNING') and line or line[45:] for line in lines]
			self.StackText = '\t' +  '\n\t'.join(lines)

class DumpMergedEntry:
	def __init__(self, mergeKeyName, mergeKey):
		self.MergeKeyName = mergeKeyName
		self.MergeKey = mergeKey
		self.MergeCount = 0
		self.Entries = []
	def DoMerge(self, dumpTxtEntry):
		self.MergeCount += 1
		self.Entries.append(dumpTxtEntry)
	def GetResult(self):
		result = {}
		result[self.MergeKeyName] = self.MergeKey
		result['Count']           = self.MergeCount
		result['Entries']         = []
		for entry in self.Entries:
			dic = {
				'DumpFile'   : entry.DumpFileName,
				'ZipFile'    : entry.ZipFileName,
				'TxtFile'    : entry.TextFileName,
				'MacAddress' : entry.MacAddress,
				'ProblemClass' : str(entry.ProblemClass),
				'StackText'    : str(entry.StackText),
			}
			result['Entries'].append(dic)
		return result

class DumpProcesser:
	def __init__(self):
		self.DumpZipEntries = {}
		self.DumpTxtEntries = []
		self.CrashMergedEntries = None
		self.MacMergedEntries = None
		
	def LoadZipFiles(self):
		files = os.listdir(Config.DumpFilePath)
		files = [f for f in files if f.endswith('.zip')]
		for filename in files:
			dumpZipFile = DumpZipFile(filename)
			self.DumpZipEntries[dumpZipFile.DumpFileName] = dumpZipFile
		
	def LoadTextFiles(self):
		files = os.listdir(Config.DumpTextPath)
		files = [f for f in files if f.endswith('.dmp.txt')]
		for filename in files:
			dumpFileName = filename[0:-4]
			dumpZipFile = self.DumpZipEntries[dumpFileName]
			if dumpZipFile != None:
				dumpTxtFile = DumpTextEntry(filename, dumpZipFile)
				dumpTxtFile.ParseDumpText()
				self.DumpTxtEntries.append(dumpTxtFile)
		
	def MergeDumpTxtEntries(self):
		# merge by crash address
		self.CrashMergedEntries = self.MergeDumpTxtEntriesByKey(self.DumpTxtEntries, 'CrashAddress', lambda entry:entry.CrashAddress)
		self.MacMergedEntries   = self.MergeDumpTxtEntriesByKey(self.DumpTxtEntries, 'MacAddress',   lambda entry:entry.MacAddress)
		
	def MergeDumpTxtEntriesByKey(self, entries, keyName, keyFunc):
		merged = {}
		for entry in entries:
			key = keyFunc(entry)
			if not key in merged:
				merged[key] = DumpMergedEntry(keyName, key)
			merged[key].DoMerge(entry)
		mergedSorted = sorted(merged.values(), key=lambda entry:entry.MergeCount, reverse=True)
		return mergedSorted
	
	def OutputMergedResultAsJson(self, key, entries, filepath):
		head = 'function LoadStats() { \n'
		tail = '\n}\n'
		
		stat = {}
		stat['Total'] = len(self.DumpTxtEntries)
		stat['Type']   = key
		stat['ZipPath']  = Config.DumpFilePath
		stat['TextPath'] = Config.DumpTextPath
		stat['Entries'] = [entry.GetResult() for entry in entries]
		content = '\treturn ' + json.dumps(stat, indent=4)
		
		all = head + content + tail 
		with open(filepath, 'w') as f:
			f.write(all)
			f.flush()
	
	def OuputHtml(self, jsonFilePath, htmlTplFilePath, htmlFilePath):
		htmlContent = ''
		with open(htmlTplFilePath) as f:
			htmlContent = f.read()
			htmlContent = htmlContent % jsonFilePath
			with open(htmlFilePath, 'w') as outfile:
				outfile.write(htmlContent)
				outfile.flush()
	def AppendToHtmlList(self, zipPath, htmlFilePath, type):
		newContent = "<p><a href='%s'>%s Dump Analyze Result Sorted By %s</a></p>" % (os.path.basename(htmlFilePath), os.path.basename(zipPath), type) 
		content = ''
		with open('index.html') as file:
			content = file.read()
		with open('index.html', 'w') as file:
			file.write(content)
			file.write('\n')
			file.write(newContent)
		
	def OutputTextEntriesAsText(self, txtfile):
		sortedByCrash = sorted(self.DumpTxtEntries, key=lambda entry:entry.CrashAddress)
		with open(txtfile) as f:
			for entry in sortedByCrash:
				f.write('Crash Address: ' + entry.CrashAddress + '\n')
				f.write('    Dump File: ' + entry.DumpFileName + '\n')
				f.write('     Zip File: ' + entry.ZipFileName + '\n')
				f.write('  Mac Address: ' + entry.MacAddress + '\n')
				f.write('Problem Class: ' + str(entry.ProblemClass) + '\n')
				f.write('   Stack Text: \n' + str(entry.StackText) + '\n')
				f.write('\n')
	
def Main():
	Config.DumpFilePath = 'Dumps'
	Config.DumpTextPath = 'Texts'
	if len(sys.argv) < 4:
		print('Usage: ' + sys.argv[0] + ' DumpFilePath DumpTextPath CrashMergeFile MacMergeFile')
		return
	Config.DumpFilePath = os.path.basename(sys.argv[1])
	Config.DumpTextPath = os.path.basename(sys.argv[2])
	crashResultFile     = os.path.basename(sys.argv[3])
	macResultFile       = os.path.basename(sys.argv[4])
	
	dumpProcesser = DumpProcesser()
	dumpProcesser.LoadZipFiles()
	dumpProcesser.LoadTextFiles()
	dumpProcesser.MergeDumpTxtEntries()
	dumpProcesser.OutputMergedResultAsJson('CrashAddress', dumpProcesser.CrashMergedEntries, crashResultFile+".js")
	dumpProcesser.OutputMergedResultAsJson('MacAddress',   dumpProcesser.MacMergedEntries, macResultFile+".js")
	dumpProcesser.OuputHtml(crashResultFile+".js", 'dump_crash_tpl.html', crashResultFile)
	dumpProcesser.OuputHtml(macResultFile+".js",   'dump_mac_tpl.html', macResultFile)
	dumpProcesser.AppendToHtmlList(Config.DumpFilePath, crashResultFile, 'Crash Address')
	dumpProcesser.AppendToHtmlList(Config.DumpFilePath, macResultFile, 'Mac Address')
if __name__ == '__main__':
	Main()

	
















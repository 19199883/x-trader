#!/usr/bin/python
# -*- coding: UTF-8 -*-

import os
import sys
import logging

class Pos:
	def __init__(self):
		self.longPos = 0
		self.shortPos = 0

class PosSum:
	def __init__(self):
		self.posDict = {}
		os.chdir(sys.path[0])
		logging.basicConfig(filename='pos_sum.log',level=logging.DEBUG)

	def save(self):
		posFile = "pos_sum.pos"
		with open(posFile,'w') as f:
			for key in self.posDict:
				pos = self.posDict[key]
				f.write("{0};{1};{2}\n".format(key, pos.longPos, pos.shortPos))

	def getPosFromFile(self, posFile, pos):
		with open(posFile) as f:
		    for line in f:
				fields = line.split(';')
				pos.cont = fields[2]
				pos.longPos = int(fields[3])
				pos.shortPos = int(fields[4])

	def getStraPosFiles(self, posFiles):
		items = os.listdir(".")
		for names in items:
			if names.endswith(".pos") and names!="pos_sum.pos" and names!="stra_names.pos":
				posFiles.append(names)


	# positions for every strategy
	def sumPos(self):
		posFiles = []
		self.getStraPosFiles(posFiles)
		for posFile in posFiles:
			pos = Pos()
			self.getPosFromFile(posFile, pos)
			posItem = self.posDict.get(pos.cont)
			if not posItem:
				self.posDict[pos.cont] = Pos()
				posItem = self.posDict.get(pos.cont)
			posItem.longPos += pos.longPos
			posItem.shortPos += pos.shortPos

		cursor = 0
		straNamesfile = "stra_names.pos"
		with open(straNamesfile,'w') as f:
			for straFile in posFiles:
				f.write(straFile.split('.')[0])
				if cursor<len(posFiles)-1:
					f.write(",")
				cursor += 1



def main():
	posSum = PosSum()
	posSum.sumPos()
	posSum.save()

if __name__=="__main__":   
	main()

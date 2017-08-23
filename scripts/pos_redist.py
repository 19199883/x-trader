#!/usr/bin/python
# -*- coding: UTF-8 -*-

#
# prerequisite:
#   need delete position file of every starategy first
#
# redistribute postion for every startegy when sttrategy's settings changes.
#

import os
import csv
import sys
import xml.etree.ElementTree as ET
from datetime import date
import logging

# modify it according to account
stra_setting = './tools/stra_sett_dce_day137.csv'
pos_sum_file = './pos_sum.pos'
ex = "dl"

class Pos:
	def __init__(self):
		self.conf = ""
		self.longPos = 0
		self.shortPos = 0

class Strategy:
	def __init__(self,stra_id):
		self.stra_id = stra_id
		self.comm_no = ""
		self.pos_long = 0
		self.pos_short = 0
		self.stra_name = ""
		self.contract = ""

class Distributor:
	def __init__(self):
		# this field store strategy info, e.g. name,id,position, and so on
		self.straDict = {}
		self.posSumDict = {}
		os.chdir(sys.path[0])
		logging.basicConfig(filename='pos_dist.log',level=logging.DEBUG)


	# classify strategies by commodity No and then save
	# them into a dictionary whose key is No and whose value is a list
	# storing Strategy object.
	# strategy info is from stra_sett_[xxxx].csv file.
	#
	def classify(self):
		with open(stra_setting) as csvfile:
			reader = csv.reader(csvfile)
			stra_id = 10000000
			for row in reader:
				if stra_id == 10000000:
					stra_id = stra_id + 1
					# skip the first row, title row
					continue

				stra = Strategy(stra_id) 
				stra.stra_id = stra_id
				stra.comm_no = row[0]
				stra.stra_name = row[1]
				stra.contract = row[2]
				curList = None
				if not self.straDict.has_key(stra.comm_no):
					curList = []
					self.straDict[stra.comm_no] = curList
				else:
					curList = self.straDict[stra.comm_no]
				curList.append(stra)

				# new id
				stra_id = stra_id + 1

	# distribute position to every strategy.
	# the position is from pos_sum.pos file.
	def distribute(self):
		# read position info
		with open(pos_sum_file) as f:
		    for line in f:
				fields = line.split(';')
				pos = Pos()
				pos.cont = fields[0]
				pos.longPos = int(fields[1])
				pos.shortPos = int(fields[2])
				self.posSumDict[pos.cont] = pos

		# distribute position
		for contract in self.posSumDict.keys():
			found = False
			pos = self.posSumDict[contract]
			for commNo in self.straDict.keys():
				straList = self.straDict[commNo]
				if straList[0].contract==contract:
					found = True
					break
			if not found:
				posFile = "sc" + contract + "_offline.pos"
				with open(posFile,'w') as f:
					f.write("{0};{1};{2};{3};{4}".format( -1, "unkown", contract, pos.longPos, pos.shortPos))

		for commNo in self.straDict.keys():
			straList = self.straDict[commNo]
			firstStra = straList[0]
			count = len(straList)
			quotient_long = 0
			remainder_long = 0
			quotient_short = 0
			remainder_short = 0
			if self.posSumDict.has_key(firstStra.contract):
				posSum = self.posSumDict[firstStra.contract]
				quotient_long = posSum.longPos / count
				remainder_long = posSum.longPos % count
				quotient_short = posSum.shortPos / count
				remainder_short = posSum.shortPos % count

			# distribute position for first strategy of the given commodity No.
			firstStra.pos_long = quotient_long + remainder_long
			firstStra.pos_short = quotient_short + remainder_short
			# distribute position from the 2nd to last
			i = 1
			while i < len(straList):
				straList[i].pos_long = quotient_long
				straList[i].pos_short = quotient_short
				i += 1

	# respectively save position of every strategy 
	# to file which name is as my_exchange_fut_yyyymmdd.log
	def savePos(self):
		for commNo in self.straDict.keys():
			straList = self.straDict[commNo]
			for stra in straList:
				posFile = stra.stra_name + ".pos"
				with open(posFile,'w') as f:
					f.write("{0};{1};{2};{3};{4}".format(
									stra.stra_id,
									stra.stra_name, 
									stra.contract, 
									stra.pos_long, 
									stra.pos_short))

def main():
	dist = Distributor()
	dist.classify()
	dist.distribute()
	dist.savePos()

if __name__=="__main__":   
	main()

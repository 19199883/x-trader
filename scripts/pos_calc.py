#!/usr/bin/python
# -*- coding: UTF-8 -*-

# read my_exchange_fut_op_[yymmdd].log, calculate position for every strategy
# and then save these position to files.
#

import os
import sys
import xml.etree.ElementTree as ET
from datetime import date
import logging

class RecordT():
	plaOrd  = 'PlaceOrder'
	ordRes = 'OrderRespond'
	ordRtn = 'OrderReturn'
	traRtn = 'TradeReturn'

class SideT():
	buy  = '0'
	sell = '1'

class PosEfctT():
	open_pos = '0'
	close_pos =	'1'

class Strategy:
	def __init__(self,stra_id):
		self.stra_id = stra_id
		self.pos_long = 0
		self.pos_short = 0
		self.stra_name = ""
		self.contract = ""

class PosCalc:
	def __init__(self):
		# this field store strategy info, e.g. name,id,position, and so on
		self.straDict = {}
		os.chdir(sys.path[0])
		logging.basicConfig(filename='pos_calc.log',level=logging.DEBUG)

	# get log file name of my_exchange module 
	def getDataSrc(self):
		today = date.today()
		return "my_exchange_fut_op_" + today.strftime("%Y%m%d") + ".log"

	# respectively save position of every strategy 
	# to file which name is as my_exchange_fut_yyyymmdd.log
	def savePos(self):
		for key in self.straDict:
			stra = self.straDict[key]
			posFile = stra.stra_name + ".pos"
			with open(posFile,'w') as f:
				f.write("{0};{1};{2};{3};{4}".format(
								stra.stra_id,
								stra.stra_name, 
								stra.contract, 
								stra.pos_long, 
								stra.pos_short))

	# process a line of data frm my_exchange_fut_yyyymmdd.log
	def proc(self,line):
		if -1 != line.find(RecordT.traRtn):
			self.procTraRec(line)

	# process a line of traded record
	def procTraRec(self,line):
		fields = line.split(';')
		# serial number
		fld0 = fields[0]
		seri_no = fld0[fld0.find('serial_no: ')+11 : ]
		stra_id = seri_no[len(seri_no)-8 : ] 
		targStra = self.straDict.get(stra_id)

		# filled quantity
		fld2 = fields[2]
		filled_qty = fld2[fld2.find('business_volume: ')+17 : ]

		# stock code
		fld5 = fields[5]
		stock_code = fld5[fld5.find('stock_code: ')+12 : ]
		targStra.contract = stock_code

		# direction
		fld6 = fields[6]
		dire = fld6[fld6.find('direction: ')+11 : ]
		# position effect
		fld7 = fields[7]
		posEff = fld7[fld7.find('open_close: ')+12 : ]

		if posEff == PosEfctT.open_pos:
			if dire == SideT.buy:
				targStra.pos_long += int(filled_qty)	
			elif dire == SideT.sell:
				targStra.pos_short += int(filled_qty)	
			else:
				logging.error("error:undefined direction %d" ,dire)
		elif posEff == PosEfctT.close_pos:
			if dire == SideT.buy:
				targStra.pos_short -= int(filled_qty)	
			elif dire == SideT.sell:
				targStra.pos_long -= int(filled_qty)	
			else:
				logging.error("error:undefined direction %d" ,dire)
		else:
			logging.error("error:undefined posEff %d" ,posEff)

	# positions for every strategy
	def calc(self):
		dataSrc = self.getDataSrc()
		with open(dataSrc) as f:
		    for line in f:
		        self.proc(line)

	# load previous days's pssition into given strategy
	def load(self, stra):
		posFile = stra.stra_name + ".pos"
		if os.path.exists(posFile):
			with open(posFile,'r') as f:
				for line in f:
					fields = line.split(';')
					fld3 = fields[3]
					stra.pos_long = int(fld3)
					fld4 = fields[4]
					stra.pos_short = int(fld4)


	# build strategy objects by trasev.config and put them into 
	# strategies member field
	def buildStra(self):
		tree = ET.parse("trasev.config")
		root = tree.getroot()
		for straEn in root.findall('.//strategy'):
			stra_id = straEn.get('id')
			stra_name = straEn.get('model_file')
			stra = Strategy(stra_id) 
			stra.stra_id = stra_id
			stra.stra_name = stra_name
			symbol = straEn.find("./symbol")
			stra.contract = symbol.get("name")
			self.load(stra)
			self.straDict[stra_id] = stra

def main():
	posCalc = PosCalc()
	posCalc.buildStra()
	posCalc.calc()
	posCalc.savePos()

if __name__=="__main__":   
	main()

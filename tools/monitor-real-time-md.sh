#!/bin/bash

# the directory where this script file is.
function enter_cur_dir()
{
	 this_dir=`pwd`
	 dirname $0|grep "^/" >/dev/null
	 if [ $? -eq 0 ];then
		 this_dir=`dirname $0`
	 else
		 dirname $0|grep "^\." >/dev/null
		 retval=$?
		 if [ $retval -eq 0 ];then
			 this_dir=`dirname $0|sed "s#^.#$this_dir#"`
		 else
			 this_dir=`dirname $0|sed "s#^#$this_dir/#"`
		 fi
	 fi

	cd $this_dir
}

function deliverydaywarn()
{	
	warn_file="/home/u910019/tick-data/mc/deliveryday-warm.csv"
	if [[ -a $warn_file ]];then	
		message="$(cat ${warn_file})"
		if [[ -n $message ]];then 
			echo "交割日告警，如下合约即将进入交割日：${message}" | mail -s "交割日告警" 17199883@qq.com
			#echo "交割日告警，如下合约即将进入交割日：${message}" | mail -s "交割日告警" 3580771905@qq.com
		fi
	fi	
}	 

function ChangeMonthwarn()
{	
	warn_file="/home/u910019/tick-data/mc/mc-warm.csv"
	if [[ -a $warn_file ]];then	
		message="$(cat ${warn_file})"
		if [[ -n $message ]];then 
			echo "主力合约换月提醒，如下合约需要换月：${message}" | mail -s "主力合约换月提醒" 17199883@qq.com
			#echo "主力合约换月提醒，如下合约需要换月：${message}" | mail -s "主力合约换月提醒" 3580771905@qq.com
		fi
	fi	

	mc_2nd_warn_file="/home/u910019/tick-data/mc/mc-2nd-warm.csv"
	if [[ -a $mc_2nd_warn_file ]];then	
		message="$(cat ${mc_2nd_warn_file})"
		if [[ -n $message ]];then 
			echo "次主力合约换月提醒，如下合约需要换月：${message}" | mail -s "次主力合约换月提醒" 17199883@qq.com
			#echo "次主力合约换月提醒，如下合约需要换月：${message}" | mail -s "次主力合约换月提醒" 3580771905@qq.com
		fi
	fi	
}	
enter_cur_dir
deliverydaywarn
ChangeMonthwarn

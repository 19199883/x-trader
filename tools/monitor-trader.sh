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

function monitor()
{
	# 配置选项
	remoteip="-p 8015 u910019@1.193.38.91"
	interval=1
	targetdir="/home/u910019/medi/day211/x-zce/"
	targetfile="b.txt"	
	
	while true
	do		
		sleep 2s # $interval
		
		result=`ssh -p 8015 u910019@1.193.38.91 "find ${targetdir} -cmin $interval | grep ${targetfile}"`		
		if [[ -n $result ]];then				
			message=` ssh  $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[Compliance\]'" `
			message+=` ssh $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[TunnRptProducer\]'" `
			
			if [[ -n $message ]];then 
				title="交易错误：$targetdir"								
				echo "${message}" | mail -s "${title}" 17199883@qq.com
				
			fi
		fi
	done
}


enter_cur_dir
monitor

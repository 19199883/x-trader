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

##################
# param1: remoteip="
# param2: interval=1
# param3: targetdir=
# param4: targetfile
# param4: targetproc
###################
function monitor()
{
	# 配置选项
	remoteip=$1
	interval=$2
	targetdir=$3
	targetfile=$4
	targetproc=$5
	
	echo "remoteip:$remoteip"
	echo "interval:$interval"
	echo "targetdir:$targetdir"
	echo "targetfile:$targetfile"
	echo "targetproc:$targetproc"
	
	while true
	do		
		sleep 2s # $interval
		
		result=`ssh $remoteip "find ${targetdir} -cmin $interval | grep ${targetfile}"`		
		if [[ -n $result ]];then				
			message=` ssh  $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[Compliance\]'" `
			message+=` ssh $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[TunnRptProducer\]'" `
			
			if [[ -n $message ]];then 
				title="交易错误：$targetdir"								
				echo "${message}" | mail -s "${title}" 17199883@qq.com
				
			fi
		 fi
		 
		 # 监控进程
		 result=`ssh $remoteip "ps ux" | grep "${targetproc}" `		
		 echo $result
		 if [[ -z $result ]];then			
			title="${targetproc}异常退出！"								
			echo "" | mail -s "${title}" 17199883@qq.com						
		 fi
	done
}


enter_cur_dir

remoteip="-p 8015 u910019@1.193.38.91"
interval=1
targetdir="/home/u910019/medi/day211/x-zce/"
targetfile="b.txt"	
targetproc="x-day211"
monitor "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc"

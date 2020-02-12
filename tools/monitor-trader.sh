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
	interval=1
	target="x-trader.log"	
	
	while true
	do		
		sleep 2s
		
		result=`ssh -p 8012 u910019@1.193.38.91 "find /home/u910019/medi/day211/x-zce -cmin -$interval | grep ${target}"`
		echo $result
		if [[ -n $result ]];then			
			message=`cat $target | grep "ERROR \[Compliance\]"`
			message+=`cat $target | grep "ERROR \[TunnRptProducer\]"`
			
			if [[ -n $message ]];then 
				title="交易错误：`pwd`"
				echo $title
				echo $message
				echo "${message}" | mail -s "${title}" 17199883@qq.com
				
			fi
		fi
	done
}


enter_cur_dir
monitor

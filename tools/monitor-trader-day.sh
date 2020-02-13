#!/bin/bash

##################
# !!!!!!!!!!!!!!注意事项！！！！！
# 1. 该脚本必须在交易程序和行情程序启动后启动，
#    否则会产生错误报警信息，如"交易程序异常退出"
# 2. 该计划的时间依赖实盘的交易程序，行情程序的启停时间，
#     该脚本必须在交易程序和行情程序启动后启动；关闭前关闭
#
#
#######################################

# 如下定义该脚本要退出的时间范围
t_str1="02:25:00"
t_str2="08:30:00"
t_str3="14:59:00"
t_str4="20:30:00"
t1=`date -d "$t_str1" +%s`
t2=`date -d "$t_str2" +%s`
t3=`date -d "$t_str3" +%s`
t4=`date -d "$t_str4" +%s`

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

#########################
#
#
######################
function monitor_rt_md()
{
	echo "md"
}

function exit_script()
{
	t=`date +%s`
	echo "now: $t"
	if [[ ($t -gt $t1 && $t -lt $t2) ||  ($t -gt $t3 && $t -lt $t4) ]]; then
		echo "exit！！"
		exit
	fi	
}

##################
# param1: remoteip="
# param2: interval=1
# param3: targetdir=
# param4: targetfile
# param4: targetproc
###################
function monitor_trader()
{
	# 配置选项
	remoteip=$1
	interval=$2
	targetdir=$3
	targetfile=$4
	targetproc=$5	
	
	while true
	do	
		# 退出脚本		
		exit_script

		# TODO:
		sleep $interval
		
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

# TODO: 需要相应修改
remoteip="-p 8015 u910019@1.193.38.91"
interval=1
targetdir="/home/u910019/medi/day211/x-zce/"
targetfile="b.txt"	
targetproc="x-day211"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc"

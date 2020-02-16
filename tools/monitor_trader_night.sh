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

		sleep ${interval}m
		
		result=`ssh $remoteip "find ${targetdir} -cmin $interval | grep ${targetfile}"`		
		if [[ -n $result ]];then				
			message=` ssh  $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[Compliance\]'" `
			message+=` ssh $remoteip "cat ${targetdir}${targetfile} | grep 'ERROR \[TunnRptProducer\]'" `
			
			if [[ -n $message ]];then 
				title="交易错误：$targetdir"								
				# to wy
				echo "${message}" | mail -s "${title}" 17199883@qq.com
				# to hzp
				echo "${message}" | mail -s "${title}" 2684027160@qq.com
				
			fi
		 fi
		 
		 # 监控进程
		 result=`ssh $remoteip "ps ux" | grep "${targetproc}" `		
		 echo $result
		 if [[ -z $result ]];then			
			title="${targetproc}异常退出！"								
			# to wy
			echo "" | mail -s "${title}" 17199883@qq.com						
			# to hzp
			echo "" | mail -s "${title}" 2684027160@qq.com						
		 fi
	done
}


enter_cur_dir

#########################
# 根据情况设置合适值
#
##########################
interval=1  # minutes

############################
# 根据实盘进行设置
#
################################

############## dce start ##############
echo "------------JRdl-test2(dce_quote) u910028@101.231.3.117:44152--------"
remoteip="-p 44152 u910028@101.231.3.117"
targetdir="/home/u910028/medi/night115/x-dce/"
targetfile="x-trader.log"	
targetproc="x-night115"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------JRdl-test2(dce_quote) u910028@101.231.3.117:44152--------"
remoteip="-p 44152 u910028@101.231.3.117"
targetdir="/home/u910028/medi/night210/x-dce/"
targetfile="x-trader.log"	
targetproc="x-night210"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------JRdl-test3(dce_trade1) u910019@101.231.3.117:44153--------"
remoteip="-p 44153 u910019@101.231.3.117"
targetdir="/home/u910019/medi/night067/x-dce/"
targetfile="x-trader.log"	
targetproc="x-night067"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &
############## dce end ##############


############## shfe start ##############
echo "------------zjtest1(shfe_trade1) u910019@101.231.3.117:44163--------"
remoteip="-p 44163 u910019@101.231.3.117"
targetdir="/home/u910019/medi/night110/x-shfe/"
targetfile="x-trader.log"
targetproc="x-night110"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------zjtest1(shfe_trade1) u910019@101.231.3.117:44163--------"
remoteip="-p 44163 u910019@101.231.3.117"
targetdir="/home/u910019/medi/night169/x-shfe/"
targetfile="x-trader.log"
targetproc="x-night169"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------zjtest1(shfe_trade1) u910019@101.231.3.117:44163--------"
remoteip="-p 44163 u910019@101.231.3.117"
targetdir="/home/u910019/medi/night96/x-shfe/"
targetfile="x-trader.log"
targetproc="x-night96" 
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &
############## shfe end ##############

############## zce start ##############
echo "------------zz16f-hq(zce_quote) u910019@1.193.38.91:8015--------"
remoteip="-p 8015 u910019@1.193.38.91"
targetdir="/home/u910019/medi/night211/x-zce/"
targetfile="x-trader.log"	
targetproc="x-night211"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------zztest3(zce_trade1) u910019@1.193.38.91:8012--------"
remoteip="-p 8012 u910019@1.193.38.91"
targetdir="/home/u910019/medi/night21-00/x-zce/"
targetfile="x-trader.log"	
targetproc="x-night21-00"
monitor_trader "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &
############## zce end  ##############

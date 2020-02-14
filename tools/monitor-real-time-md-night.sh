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

###########################
# 休市时间
#
##############################
rest_str1="09:00:00"
rest_str2="10:15:00"
rest_str3="10:30:00"
rest_str4="11:30:00"
rest_str5="13:30:00"
rest_str6="15:00:00"
rest_str7="21:00:00"
rest_str8="23:00:00"
rest1=`date -d "$rest_str1" +%s`
rest2=`date -d "$rest_str2" +%s`
rest3=`date -d "$rest_str3" +%s`
rest4=`date -d "$rest_str4" +%s`
rest5=`date -d "$rest_str5" +%s`
rest6=`date -d "$rest_str6" +%s`
rest7=`date -d "$rest_str7" +%s`
rest8=`date -d "$rest_str8" +%s`

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

function exit_script()
{
	# TODO: debug 
	#t_str="20:32:00"
	#t=`date -d "$t_str" +%s`
	# TODO: commented for debuging
	t_str=`date +"%H:%M:%S"`
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
function monitor_rt_md()
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
		if [[ -z $result ]];then										
			t=`date +%s`
			echo "monitor now: $t"
			if [[ ($t -gt $rest1 && $t -lt $rest2) || \
				  ($t -gt $rest3 && $t -lt $rest4) || \
				  ($t -gt $rest5 && $t -lt $rest6) || \
				  ($t -gt $rest7 && $t -lt $rest8) ]]; then
				title="行情中断(如果是休市时间，请忽略)：${remoteip}-${targetdir}"	
				# to wy
				echo "" | mail -s "${title}" 17199883@qq.com			
				# to hzp
				echo "" | mail -s "${title}" 2684027160@qq.com			
			else
			  echo "休市时间: $t_str"
			fi
		 fi
		 
		 # 监控进程
		 result=`ssh $remoteip "ps ux" | grep "${targetproc}" `		
		 echo $result
		 if [[ -z $result ]];then			
			title="${targetproc}异常退出！(${remoteip}-${targetdir})"								
			# to wy
			echo "" | mail -s "${title}" 17199883@qq.com						
			# to hzp
			echo "" | mail -s "${title}" 2684027160@qq.com						
		 fi
	done
}


enter_cur_dir

#######################
# TODO: 需要相应修改
####################
interval=1 # minute

###################
# 如下与实盘部署相关
#
####################
echo "------------JRdl-test2(dce_quote) u910028@101.231.3.117:44152--------"
remoteip="-p 44152 u910028@101.231.3.117"
targetdir="/home/u910028/md/download/night/dcelv2_mktsvc/Data/"
targetfile="bestanddeepquote_`date +%Y%m%d`.dat"	
targetproc="down_md_ngt"
monitor_rt_md "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &

echo "------------JRdl-test3(dce_trade) u910019@101.231.3.117:44153--------"
remoteip="-p 44153 u910019@101.231.3.117"
targetdir="/home/u910019/md/download/night/dcelv2_mktsvc/Data/"
targetfile="bestanddeepquote_`date +%Y%m%d`.dat"	
targetproc="down_md_ngt"
monitor_rt_md "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &


echo "------------zjtest1(shfe_trade1) u910019@101.231.3.117:44163--------"
remoteip="-p 44163 u910019@101.231.3.117"
targetdir="/home/u910019/md/download/night/shfe_jr_mktsvc/Data/"
targetfile="my_shfe_md_`date +%Y%m%d`.dat"	
targetproc="down_md_ngt"
monitor_rt_md "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &


echo "------------zztest3(zce_trade1) u910019@1.193.38.91:8012--------"
remoteip="-p 8012 u910019@1.193.38.91"
targetdir="/home/u910019/md/download/night/mktdata/Data/"
targetfile="czce_level2_`date +%Y%m%d`.dat"	
targetproc="down_md_ngt"
monitor_rt_md "$remoteip" "$interval" "$targetdir" "$targetfile" "$targetproc" &


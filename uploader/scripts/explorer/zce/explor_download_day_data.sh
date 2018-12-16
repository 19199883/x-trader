#!/bin/bash
SUFFIX="day"
program_name="czce_tra_${SUFFIX}"

STRATEGY_DIR="./stra_log"
TICKDATA_DIR="./tickdata"

# obtain the directory where this script file locates.
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


STRA_LOG="/home/u910019/trade/day/backup/czce_tra_${SUFFIX}_`date +%y%m%d`.tar.gz"
TICK_DATA="/home/u910019/md/download/day/backup/czce_md_${SUFFIX}_`date +%y%m%d`.tar.gz"

echo $STRA_LOG
echo $TICK_DATA

scp -P 8012  "u910019@1.193.38.91:${STRA_LOG}" ${STRATEGY_DIR}
scp -P 8012  "u910019@1.193.38.91:${TICK_DATA}" ${TICKDATA_DIR}

#tar again to make compress format from gz to biz2
cd ${STRATEGY_DIR}
stra_src="czce_tra_${SUFFIX}_`date +%y%m%d`.tar.gz"
stra_dest="czce_tra_${SUFFIX}_`date +%y%m%d`.tar.bz2"
echo $stra_src
echo $stra_dest
tar --remove-files -xzf $stra_src
tar --remove-files -cvjf $stra_dest ./backup/log ./backup/*.log
rm $stra_src

cd ../${TICKDATA_DIR} 
data_src="czce_md_${SUFFIX}_`date +%y%m%d`.tar.gz"
data_dest="czce_md_${SUFFIX}_`date +%y%m%d`.tar.bz2"
echo $data_src
echo $data_dest
tar --remove-files -xzf $data_src
tar --remove-files -cvjf $data_dest ./backup
rm $data_src 


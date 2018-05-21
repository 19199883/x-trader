#!/bin/bash
SUFFIX="night"
program_name="sh_tra_${SUFFIX}"

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

STRA_LOG="/home/u910019/trade/night/backup/sh_stra_${SUFFIX}_`date +%y%m%d`.tar.gz"
STRA_LOG_GZ="sh_stra_${SUFFIX}_`date +%y%m%d`.tar.gz"
STRA_LOG_BJ2="sh_stra_${SUFFIX}_`date +%y%m%d`.tar.bz2"
scp  -P 44163  "u910019@101.231.3.117:${STRA_LOG}" ${STRATEGY_DIR}
cd ./stra_log
tar -xvzf $STRA_LOG_GZ 
rm $STRA_LOG_GZ
tar -cvjf $STRA_LOG_BJ2 ./backup
rm -r ./backup
cd ..


TICK_DATA="/home/u910019/md/download/night/backup/sh_md_${SUFFIX}_`date +%y%m%d`.tar.gz"
TICK_DATA_GZ="sh_md_${SUFFIX}_`date +%y%m%d`.tar.gz"
TICK_DATA_BZ2="sh_md_${SUFFIX}_`date +%y%m%d`.tar.bz2"
scp  -P 44163  "u910019@101.231.3.117:${TICK_DATA}" ${TICKDATA_DIR}
cd ./tickdata
tar -xvzf $TICK_DATA_GZ
rm $TICK_DATA_GZ
tar -cvjf $TICK_DATA_BZ2 ./backup
rm -r ./backup


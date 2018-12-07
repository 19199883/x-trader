#!/bin/bash
program_name=tdce100

# the directory where this script file is
function enter_cur_dir(){
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

function start(){
        ulimit -c unlimited
        export LD_LIBRARY_PATH=./:$LD_LIBRARY_PATH
        nohup ./$program_name &
}

enter_cur_dir
start


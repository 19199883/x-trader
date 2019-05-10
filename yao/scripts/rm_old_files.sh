# hold the latest two days's files

# the directory where this script file is.
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

function backup(){
	 backup_dir="./backup/"
	 OLD_IFS=$IFS
	 IFS=$'\n'
	 ln_cnt=0
	 for ln in `ls -lt "${backup_dir}" | grep ".*tar.gz" `
	 do  
	     col_cnt=0
	     IFS=' '
	     if [ $ln_cnt -gt 20 ]
	     then
		 for col in $ln
		 do
		     if [ $col_cnt -eq 8 ]
		     then
			 rm -f "${backup_dir}/${col}"
		     fi
		     col_cnt=$[$col_cnt+1]   
		 done
	     fi
	     ln_cnt=$[$ln_cnt+1]
	 done
	 IFS=$IFS.OLD
}	 

enter_cur_dir
backup

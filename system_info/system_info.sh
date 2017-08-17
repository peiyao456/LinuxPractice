#!/bin/bash
os_check()
{
	if [ -e /etc/redhat-release ];then
		REDHAT=`cat /etc/redhat-release | cut -d ' ' f1`
	else
		DEBIAN=`cat /etc/issue | cut -d ' ' f1`
	fi

	if [ REDHAT == "CentOS" -o REDHAT == "Red"];then
		P_M=yum
	elif [ REDHAT == "Ubuntu" -o REDHAT == "ubuntu"];then
		P_M=apt-get
	else
		exit 1
	fi
}

#if [ $LOGNAME != "root" ];then
#	echo "please use the root"
#	exit 1
#fi
#check vmstat is exit or not
if ! which vmstat &>/dev/null;then
	echo "vmstat is not exist,please install"
	sleep 1
	os_check
	$P_M install procps -y
	echo"-------------------------------"
fi
if ! which iostat &>/dev/null;then
	echo "iostat is not exist,please install"
	sleep 1
	os_check
	$P_M install sysstat -y
	echo "-----------------------------"
fi

while true;do
	select input in cpu_load disk_load disk_use disk_inode mem_use  tcp_status cpu_top10 mem_top10 traffic quit;do
		case $input in
			cpu_load) #CPU情况
				echo "-------------------"
				i=1
				while [[ $i -le 3 ]];do
					echo -e "\033[32m  参考值${i}\033[0m"
					UTIL=`vmstat | awk '{if(NR==3) print 100-$15"%"}'`
					USER=`vmstat | awk '{if(NR==3) print $13"%"}'`
					SYS=`vmstat | awk '{if(NR==3) print $14"%"}'`
					IOWAIT=`vmstat |awk '{if(NR==3) print $16"%"}'`
					echo "UTIL:$UTIL"
					echo "USER use:$USER"
					echo "System use:$SYS"
					echo "I/O wait:$IOWAIT"
					let i++
					sleep 1
				done
				echo "-------------------------"
				break
				;;
			disk_load)
				echo "--------------------------"
				i=1
				while [[ $i -le 3 ]];do
					echo -e "\033[32m  参考值${i}\033[0m"
					UTIL=`iostat -x -k | awk '/^[v|s]/{OFS=": ";print $1,$NF"%"}'`
					READ=`iostat -x -k | awk '/^[v|s]/{OFS=": ";print $1,$6"KB"}'`
					WRITE=`iostat -x -k | awk '/^[v|s]/{OFS=": ";print $1,$7"KB"}'`
					IOWAIT=`vmstat | awk '{if(NR==3) print $16"%"}'`
					echo -e "Util:"
					echo -e "${UTIL}"
					echo -e "IOWAIT: $IOWAIT"
					echo -e "Read/s :\n$READ"
					echo -e "Write/s: \n$WRITE"
					let i++
					sleep 1
				done
				echo "---------------------------"
				break
				;;
			disk_use) #磁盘利用率
				DISK_LOG=/tmp/disk_use.tmp
				DISK_TOTAL=`fdisk -l | awk '/^Disk.*bytes/&&/\/dev/{printf $2" ";printf "%d",$3,print "GB"}'`
				USE_RATE=`df -h | awk '/^\/dev/{printf int($5)}'`
				for i in $USE_RATE;do
					if [ $i -gt 90 ];then
						PART=`df -h | awk '{if(int($5)=='''$i''') print $6}'`
						echo "$PART=${i}%" >>DISK_LOG
					fi
				done
				echo "---------------------------"
				echo -e "Disk_total:\n ${DISK_TATAL}"
				if [ -f $DISK_LOG ];then
					echo "------------------------"
					cat $DISK_LOG
					echo "------------------------"
					rm -f $DISK_LOG
				else
					echo "------------------------"
					echo "disk_use no over 90%"
					echo "------------------------"
				fi
				break
				;;
			disk_inode) #磁盘的inode利用率
				INODE_LOG=/tmp/inode_use.tmp
				INODE_USE=`df -i | awk '/^\/dev/{print int($5)}'`
				for i in $INODE_USE;do
					if [ $i -gt 90 ];then
						PART=`df -h | awk '{if(int($5)=='''$i''') print $6}'`
						echo "$PART=${i}%" >> $INODE_LOG
					fi
				done
				#log file is exist or not
				if [ -f $INODE_LOG ];then
					echo "---------------------"
					cat $INODE_LOG
					echo "---------------------"
					rm -f $INDOE_LOG
				else
					echo "-----------------------"
					echo "inode no over 90%"
					echo "-----------------------"
				fi
				break
				;;
			mem_use) #内存的使用情况
				echo "------------------------"
				i=1
				while [[ $i -le 3 ]];do
					echo -e "\033[32m  参考值${i}\033[0m"
					MEM_TOTAL=`free -m | awk '{if(NR==2) printf "%.1f",$2/1024}END{print "G"}'`
					USE=`free -m | awk '{if(NR==3) printf "%.1f",$3/1024}END{print "G"}'`
					FREE=`free -m | awk '{if(NR==3) printf "%.1f",$4/1024}END{print "G"}'`
					CACHE=`free -m | awk '{if(NR==2) printf "%.1f",($6+$7)/1024}END{print "G"}'`
					echo "Mem_total: $MEM_TOTAL"
					echo "Mem_use: $USE"
					echo "Mem_free: $FREE"
					echo "Mem_cache: $CACHE"
					let i++
					sleep 1
				done
				break
				;;
			tcp_status)
				echo "-------------------------"
				COUNT=`netstat -antp | awk '{status[$6]++}END{for(i in status) print i,status[i]}'`
				echo -e "tcp connections status: \n $COUNT"
				echo "-------------------------"
				break
				;;
			cpu_top10)
				cd /tmp
				CPU_LOG=/tmp/cpu_top.tmp
				echo $CPU_LOG
				echo "-------------------------"
				i=1
				while [[ $i -le 3 ]];do
					ps aux | awk '{if($3>0.1){{printf "PID: " $2 " CPU: "$3 "%-->"}for(i=11;i<=NF;i++)if(i==NF)printf $i "\n";else printf $i}}' | sort -k4 -nr | head -10 > $CPU_LOG
					if [[ ! -n `cat $CPU_LOG` ]];then
						echo "no process is using cpu"
					else
						echo -e "\033[32m  参考值${i}\033[0m"
						cat $CPU_LOG
						> $CPU_LOG
					fi
					let i++
					sleep 1
				done
				break
				;;
			mem_top10)
				echo "----------------------------"
				MEM_LOG=/tmp/mem_top.tmp
				i=1
				while [[ $i -le 3 ]];do
					ps aux | awk '{if($4>0.1){{printf "PID: " $2 " Memory: "$3 "%-->"}for(i=11;i<=NF;i++)if(i==NF)printf $i "\n";else printf $i}}' | sort -k4 -nr | head -10 > $MEM_LOG
					if [[ -n `cat $MEM_LOG` ]];then
						echo -e "\033[32m  参考值${i}\033[0m"
						cat $MEM_LOG
						> $MEM_LOG
					else
						echo "no process using memory"
					fi
					let i++
					sleep 1
				done
				break
				;;
			traffic)
				while true;do
					read -p "please enter network name :" eth
					if [ `ifconfig | grep -c "\<$eth\>"` -eq 1 ];then
						break
					else
						echo "enter name error!"
					fi
				done
				echo "-------------------------"
				echo "In-------Out"
				i=1
				while [[ $i -le 3 ]];do
					OLD_IN=`ifconfig $eth | awk -F'[: ]+' '/bytes/{if(NR==8)print $4;else if(NR==5)print $6}'`
					OLD_OUT=`ifconfig $eth | awk -F'[: ]+' '/bytes/{if(NR==8)print $9;else if(NR==7)print $6}'`
					sleep 1
					NEW_IN=`ifconfig $eth | awk -F'[: ]+' '/bytes/{if(NR==8)print $4;else if(NR==5)print $6}'`
					NEW_OLD=`ifconfig $eth | awk -F'[: ]+' '/bytes/{if(NR==8)print $9;else if(NR==7)print $6}'`
					IN=`awk 'BEGIN{printf "%.1f\n",'$((${NEW_IN}-${OLD_IN}))'/1024/128}'`
					OUT=`awk 'BEGIN{printf "%.1f\n",'$((${NEW_OUT}-${OLD_OUT}))'/1024/128}'`
					echo "${IN}  MB/s  ${OUT}  MB/s"
					let i++
					sleep 1
				done
				echo "--------------------------"
				break
				;;
			quit)
				exit 0
				;;
			*)
				echo "please enter the number"
				break
				;;
		esac
	done
done

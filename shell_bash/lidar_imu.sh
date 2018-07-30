#! /bin/bash
ConfigPath=`pwd`/config.jcon
CarId=`grep "CarId" $ConfigPath | awk -F '=' '{print $2}'`
TfPath=`grep "LidarImuTf" $ConfigPath | awk -F '=' '{print $2}'`
UpdatePth=`pwd`/update.sh
Gibraltar=`grep "GibraltarFolder" $ConfigPath | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/lidar_imu/calibration_multi.sh
PfPth="$Gibraltar/platform/$CarId"
BagFolder=`grep "BagFolder" $ConfigPath | awk -F '=' 'NR==1 {print $2}'`
EstimateZList=`grep "ESTIMATE_Z_LIST" $ConfigPath | awk -F '=' '{print $2}'`
TypeStr=("hdl64" "vlp32_1" "vlp32_2" "radar")
ConfArr=""
LaserNum=""
TopicList=""

function CheckCalibration() {
	echo "CheckCalibration..."
	map_raw=`grep -n "topic map" $ConfigPath | awk -F ':' '{print $1}'`
	let "map_raw++"
	if [[ $map_raw == "" ]]; then
		echo "can't find config.jcon"
		exit;
	fi
	cat $ConfigPath | tail -n +$map_raw > xxxxxx20180726
	while read line
	do	
		key=`echo $line | awk -F '=' '{print $1}'`
		topic=`echo $line | awk -F '=' '{print $2}'`
		echo $key
		if [[ `cat $PfPth/velodyne_lidar.jcon | grep $key` == "" ]]; then
			continue
		fi
		laser_num=`echo $key | grep -Po "[0-9]+" | head -n 1`
		echo $laser_num,$topic
		LaserNum="$LaserNum $laser_num"
		TopicList="$TopicList $topic"
		index=0
		for((i=0;i<4;i++)); 
		do
			if [[ $key==${TypeStr[$i]} ]]; then
				let "configtype=i+1"
				ConfArr[$index]="$configtype"
				let "index++"
			fi	
		done
		if [[ `ls $BagFolder | grep -c .bag` != `rosbag info $BagFolder/*.bag | grep -c $topic` ]];then
			echo "Error bag!"
			exit;
		fi
	done < xxxxxx20180726
	rm -r xxxxxx20180726
	echo LaserNum=$LaserNum,TopicList=$TopicList
	LaserNum="\(${LaserNum}\)"
	TopicList="\(${TopicList} \)"
	sed -i "/GIBRALTAR_ROOT=/c GIBRALTAR_ROOT=$Gibraltar" $ClbPth
	sed -i "/DATA_FOLDER=/c DATA_FOLDER=$BagFolder" $ClbPth
	sed -i "/TOPIC_LIST=/c TOPIC_LIST=$TopicList" $ClbPth
	sed -i "/LASER_NUM=/c LASER_NUM=$LaserNum" $ClbPth
	sed -i "/ESTIMATE_Z_LIST=/c ESTIMATE_Z_LIST=$EstimateZList" $ClbPth
}

function Calibration() {
	echo "run calibration_multi.sh..."
	bash -c "$ClbPth | tee xxxxxx20180726" 
}

function Update() {
	echo "Update...	"
	conf_index=1;
	while [[ `cat xxxxxx20180726 | grep "finish"` == "" ]]; 
	do
		flag=`cat xxxxxx20180726 | grep Conf${conf_index} | awk -F "Conf${conf_index}:" '{print $2}'`
		if [[ $flag == "" ]]; then
			continue;
		fi
		let "lidar_type=conf_index-1"
		echo "ConArr[$lidar_type]=${ConfArr[$lidar_type]} "
		cmd="bash `pwd`/update.sh -t ${ConfArr[$lidar_type]} $flag"
		if [[ $lidar_type=="0" || $lidar_type=="1" ]]; then
			echo $flag > $TfPath
		fi
		echo $cmd;
		$cmd
		let "conf_index++"
		sleep 5
	done
	cali_pid=`grep -oE "[0-9]+(\ calibration_multi.cc)" xxxxxx20180726  | grep -oE "[0-9]+" | head -1`
	echo "cali_pid=$cali_pid and it will be killed."
	kill -9 $cali_pid
	rm -r xxxxxx20180726
}

CheckCalibration;
Calibration &
sleep 5
Update;
sleep 5
echo "Run lidar_imu.sh finish."

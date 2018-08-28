#! /bin/bash
#set -x
ConfigPath=`pwd`/config.jcon
echo 1
Gibraltar=`grep "GibraltarFolder" $ConfigPath | awk -F '=' '{print $2}'`
echo 2
EvlPth=$Gibraltar/tools/calibration/lidar_imu/evaluation_multi.sh
echo 3
CarId=`grep "CarId" $ConfigPath | awk -F '=' '{print $2}'`
echo 4
PfPth="$Gibraltar/platform/$CarId"
echo 5
BagFolder=`grep "BagFolder" $ConfigPath | awk -F '=' 'NR==1 {print $2}'`
echo 6
ConfigFolder=`cat $ConfigPath | grep ConfigFolder | awk -F '=' '{print $2}'`
echo 7
TypeStr=("hdl64" "vlp32_1" "vlp32_2" "radar")
echo 8
ConfArr=""
LaserNum=""
TopicList=""
echo 9
TfYamlPath=$gibraltar/tools/calibration/lidar_imu/multi_lidar.yaml
echo 10

function Init() {
  if [[  `cat lidar_sensor_name` == "" ]]; then
      echo "Error:Can't find lidar_sensor_name"
      exit
  fi
}

function CheckEvaluation() {
	echo "CheckEvaluation..."
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
	sed -i "/GIBRALTAR_ROOT=/c GIBRALTAR_ROOT=$Gibraltar" $EvlPth
	sed -i "/DATA_FOLDER=/c DATA_FOLDER=$BagFolder" $EvlPth
	sed -i "/TOPIC_LIST=/c TOPIC_LIST=$TopicList" $EvlPth
	sed -i "/LASER_NUM=/c LASER_NUM=$LaserNum" $EvlPth
	sed -i "/ConfigFolder=/c ConfigFolder=$ConfigFolder" $EvlPth
}

function Evaluation() {
	echo "run evaluation_multi.sh..."
	bash $EvlPth > xxxxxx20180726
}

function ShowEvaluation() {
	echo "===================================================================================="
	echo "Lidar evaluation... "
    while [[ `cat xxxxxx20180726 | grep "Finished"` == "" ]]; 
	do
    wait_for_finish=1
	done
    cat xxxxxx20180726 | grep "SCORE" > xxxxxx20180806
	i=1
	while read eva
	do
    echo ""
		cat lidar_sensor_name | awk "NR==$i"
    cat lidar_temp_tf | awk "NR==$i"
		echo $eva
		let "i++"
	done < xxxxxx20180806
    rm -r xxxxxx20180726
	rm -r xxxxxx20180806
	echo "===================================================================================="
	rm -r lidar_sensor_name
  rm -r lidar_temp_tf
}

CheckEvaluation;
Evaluation &
sleep 5
ShowEvaluation;
sleep 5
echo "Run lidar_evaluation.sh finish."

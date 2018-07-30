
#!/bin/bash

CnfPth=`pwd`/config.jcon
UpdatePth=`pwd`/update.sh
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/lidar_radar/calibration.sh
BagFolder=`cat $CnfPth | grep BagFolder | awk -F '=' 'NR==2 {print $2}'`
LidarImuTf=`cat $CnfPth | grep LidarImuTf | awk -F '=' '{print $2}'`
ConfigFolder=`cat $CnfPth | grep ConfigFolder | awk -F '=' '{print $2}'`
function CheckCalibration() {
	# source $Gibraltar/setup.bash
	bag_folder_raw=`cat $ClbPth | grep -n BAG_FOLDER | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$bag_folder_raw" == "" ]];then
		echo "fail to find keywords:BAG_FOLDER";
		exit;	
	fi
    sed -i "${bag_folder_raw}c BAG_FOLDER=${BagFolder}" $ClbPth

	gibraltar_raw=`cat $ClbPth | grep -n GIBRALTAR_ROOT | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$gibraltar_raw" == "" ]];then
		echo "fail to find keywords:GIBRALTAR_ROOT";
		exit;
	fi
	sed -i "${gibraltar_raw}c GIBRALTAR_ROOT=${Gibraltar}" $ClbPth

	conf_folder_raw=`cat $ClbPth | grep -n CONFIG_FOLDER | awk -F ':' 'NR==1 {print $1};'`	
	if [[ "$conf_folder_raw" == "" ]];then
		echo "fail to find keywords:CONFIG_FOLDER"
		exit;
	fi
	sed -i "${conf_folder_raw}c CONFIG_FOLDER=${ConfigFolder}" $ClbPth
	
	tf_input_raw=`cat $ClbPth | grep -n LIDAR_IMU_TF | awk -F ':' 'NR==1 {print $1};'`	
	if [[ "$tf_input_raw" == "" ]];then
		echo "fail to find keywords:"
		exit;
	fi
	sed -i "${tf_input_raw}c LIDAR_IMU_TF=${LidarImuTf}" $ClbPth
}

function Calibration() {
	#chmod +x $ClbPth
	bash -c "$ClbPth | tee xxxxxx"
}

function Update() {
	config=`cat xxxxxx | grep "CONF:" | awk -F "CONF:" 'END {print $2};'`
	#chmod +x $UpdatePth
	command=" -t 3 ${config}"
	echo "Run $UpdatePth$command"
	bash $UpdatePth$command
}

echo "" > xxxxxx
echo "CheckCalibration..."
CheckCalibration;
echo "Run Calibration.sh.................................................................................."
Calibration &

flag=true
while [[ "$flag" = "true" ]];
do
	str=`grep FINISH xxxxxx`
	if [ "$str" != "" ];then
		flag=false
		echo "Run Update.sh.................................................................................."
		Update
	fi
done
sleep 5
cali_pid=`grep -oE "[0-9]+(\ calibration.cc)" xxxxxx | grep -oE "[0-9]+" | head -1`
rm -rf xxxxxx
echo "cali_pid=$cali_pid and it will be killed."
kill -9 $cali_pid
echo "Run lidar_radar.sh finish."


#!/bin/bash

CnfPth=`pwd`/config.jcon
UpdatePth=`pwd`/update.sh
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/lidar_radar/calibration.sh
BagFolder=`cat $CnfPth | grep BagFolder | awk -F '=' '{print $2}'`
LidarImuTf=`cat $CnfPth | grep LidarImuTf | awk -F '=' '{print $2}'`
ConfigFolder=`cat $CnfPth | grep ConfigFolder | awk -F '=' '{print $2}'`
function CheckCalibration() {
	source $Gibraltar/setup.bash
	bagfolder_raw=`cat $ClbPth | grep -n BAG_FOLDER | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$bagfolder_raw" == "" ]];then
		echo "fail to find keywords:BAG_FOLDER";
		exit;	
	fi
        sed -i "${bagfolder_raw}c BAG_FOLDER=${BagFolder}" $ClbPth

	gibraltar_raw=`cat $ClbPth | grep -n GIBRALTAR_ROOT | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$gibraltar_raw" == "" ]];then
		echo "fail to find keywords:GIBRALTAR_ROOT";
		exit;
	fi
	sed -i "${gibraltar_raw}c GIBRALTAR_ROOT=${Gibraltar}" $ClbPth

	conffolder_raw=`cat $ClbPth | grep -n CONFIG_FOLDER | awk -F ':' 'NR==1 {print $1};'`	
	if [[ "$conffolder_raw" == "" ]];then
		echo "fail to find keywords:CONFIG_FOLDER"
		exit;
	fi
	sed -i "${conffolder_raw}c CONFIG_FOLDER=${ConfigFolder}" $ClbPth
	
	tfinput_raw=`cat $ClbPth | grep -n LIDAR_IMU_TF | awk -F ':' 'NR==1 {print $1};'`	
	if [[ "$tfinput_raw" == "" ]];then
		echo "fail to find keywords:"
		exit;
	fi
	sed -i "${tfinput_raw}c LIDAR_IMU_TF=${LidarImuTf}" $ClbPth
}

function Calibration() {
	chmod +x $ClbPth
	bash -c "$ClbPth | tee xxxxxx"
}

function Update() {
	config=`cat $ClbTxt | grep "conf:" | awk -F "conf:" 'END {print $2};'`
	echo $UpdatePth
	chmod +x $UpdatePth
	command=" -t 2 ${config}"
	echo $UpdatePth$command
	source $UpdatePth$command
}

echo "" > xxxxxx
echo CheckCalibration
CheckCalibration;
echo "Run Calibration..............................................................................................."
Calibration &

flag=true
while [[ "$flag" = "true" ]];
do
	str=`grep FINISH xxxxxx`
	if [ "$str" != "" ];then
		flag=false
		echo "Update Data.................................................................................."
		Update
	fi
done
sleep 5
echo "Finish........................................................................................................"
rm -rf xxxxxx

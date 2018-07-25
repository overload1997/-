#!/bin/bash
CnfPth=`pwd`/config.jcon
ClbTxt=`cat $CnfPth | grep LidarImuTf | awk -F '=' '{print $2}'`
UpdatePth=`pwd`/update.sh
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/lidar_imu/calibration.sh
CarId=`cat $CnfPth | grep CarId | awk -F '=' '{print $2}'`
PfPth="$Gibraltar/platform/$CarId"
Vlp32Flag=`cat $PfPth/velodyne_lidar.jcon | grep "vlp32_1"`
DataFolder=`cat $CnfPth | grep BagFolder | awk -F '=' 'NR==1 {print $2}'`


function CheckCalibration() {
	source $Gibraltar/setup.bash
	if [[ "$Vlp32Flag" != "" ]];then
		ClbPth=$Gibraltar/tools/calibration/lidar_imu/calibration32.sh;
	fi

	datafolder_raw=`cat $ClbPth | grep -n DATA_FOLDER | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$datafolder_raw" == "" ]];then
		echo "fail to find keywords:DATA_FOLDER";
		exit;	
	fi
        sed -i "${datafolder_raw}c DATA_FOLDER=${DataFolder}" $ClbPth

	gibraltar_raw=`cat $ClbPth | grep -n GIBRALTAR_ROOT | awk -F ':' 'NR==1 {print $1};'`
	if [[ "$gibraltar_raw" == "" ]];then
		echo "fail to find keywords:GIBRALTAR_ROOT";
		exit;
	fi
        sed -i "${gibraltar_raw}c GIBRALTAR_ROOT=${Gibraltar}" $ClbPth
}

function Calibration() {
	chmod +x $ClbPth
	bash -c "$ClbPth | tee $ClbTxt"
}

function Update() {
	config=`cat $ClbTxt | grep "conf" | awk -F "conf:" 'END {print $2};'`
	echo $UpdatePth
	chmod +x $UpdatePth
	command=" -t 0 ${config}"
	echo $UpdatePth$command
	source $UpdatePth$command
	echo $config > $ClbPth;
}

echo "" > $ClbTxt
echo CheckCalibration
CheckCalibration;
echo "Run Calibration..............................................................................................."
Calibration &

flag=true
while [[ "$flag" = "true" ]];
do
	str=`grep Publish $ClbTxt`
	# echo "in the while --------------------------------------------------------------------------------"
	if [ "$str" != "" ];then
		echo "------------------------- $str ----------------------------"
		flag=false
		echo "Update Data.................................................................................."
		Update
	fi
done
cali_pid=`grep -oE "[0-9]+(\ calibration.cc)" $ClbTxt  | grep -oE "[0-9]+" | head -1`
echo "cali_pid=$cali_pid ---------------------------------------------------------------------------------------------"
kill -9 $cali_pid
sleep 5
echo "Finish........................................................................................................"

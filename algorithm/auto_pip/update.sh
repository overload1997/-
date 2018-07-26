#! /bin/bash

CnfPth=`pwd`/config.jcon
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/manual/calibration.jcon
CarId=`cat $CnfPth | grep CarId | awk -F '=' '{print $2}'`
PfPth="$Gibraltar/platform/$CarId"
#TypeStr=("hdl64" "vlp32_1" "vlp32_2" "radar")
TypeFile=("velodyne_lidar.jcon" "velodyne_lidar.jcon" "velodyne_lidar.jcon" "delphi_esr_radar.jcon")
KeyWords=("imu2lidar" "vlp32_1_imu2lidar" "vlp32_2_imu2lidar" "esr_can_transform_data")
#used for updating lidar_config
ConfigArr=($3 $4 $5 $6 $7 $8 $9)
ConfigPre=("x:" "y:" "z:" "qx:" "qy:" "qz:" "qw:")
Type=-1
Path=""
Key=""

while getopts "t:" opt; do
	case $opt in
		t)Type=$OPTARG
		;;
		\?)Type=-1
		echo "Invalid option: -$OPTARG"
		;;
	esac
done

echo "Type is $Type"

function Init() {
	
        #find the path and the keywords
	if [ "$Type" == "-1" ];then
		echo "please enter the correct argument -t[0-hdl64;1-vlp32_1;2-vlp32_2]"
		exit;
	fi
	Path="$PfPth/${TypeFile[0]}"
	Key="${KeyWords[$Type]}"
	echo "Key is $Key"
}

function SetVerticalArgs() {
	echo "Path=$Path"
	#search the id of raw which contains the keyword
        StartRaw=`cat $Path | grep -n $Key | awk -F ':' '{print $1};'`
        let "EndRaw=$StartRaw+8"
        let "LineId=$StartRaw+1"
        #abstract the line which needs to be updated
        newconfig="\ \ \ \ \ \ quternion: True,"
        sed -i "${LineId}c $newconfig" $Path
        let "LineId++"  
        while [ $LineId -le $EndRaw ]
        do
                let "arg_index=${LineId}-${StartRaw}-2"  
                newconfig="\ \ \ \ \ \ ${ConfigPre[$arg_index]}\"${ConfigArr[$arg_index]}\""
                sed -i "${LineId}c $newconfig" $Path
                let "LineId++"
        done
}

function SetHorizontalArgs() {
	target_cali_node_raw=`cat $ClbPth | grep -n "radar_cali = Node" | awk -F ':' 'NR==1 {print $1};'`
        target_frame_raw=`cat $ClbPth | grep -A 10 "radar_cali = Node" | grep -n "target_frame" | awk -F ':' 'NR==1 {print $1};'`
        let "target_frame_raw+=target_cali_node_raw-1"
        newconfig="\ \ \ \ target_frame: \"delphi_esr\","
        sed -i "${target_frame_raw}c $newconfig" $ClbPth
        args="\ \ \ \ \ \ args: [\"${ConfigArr[0]}\", \"${ConfigArr[1]}\", \"${ConfigArr[2]}\", \"${ConfigArr[3]}\",\n\"${ConfigArr[4]}\", \"${ConfigArr[5]}\", \"${ConfigArr[6]}\", \"base_link\", \"hdl64\", \"100\"],"
        radar_cali_launch_raw=`cat $ClbPth| grep -n "radar_cali_launch = Launch" | awk -F ':' 'NR==1 {print $1};'`
        radar_cali_launch_args_raw=`cat $ClbPth | grep -A 10 "radar_cali_launch = Launch" | grep -n "args:" | awk -F ':' '{print $1}'`
        let "radar_cali_launch_args_raw+=radar_cali_launch_raw-1"
        let "firstraw=radar_cali_launch_args_raw"
        let "secondraw=radar_cali_launch_args_raw+1"
        sed -i "${firstraw},${secondraw}c $args" $ClbPth
}

function main() {
	Init;
	SetVerticalArgs;
	if [[ "$Type" == "0" || "$Type" == "1" ]];then
 		SetHorizontalArgs;
		echo "Horizontal"
	fi 
}

main

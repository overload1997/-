#! /bin/bash
CnfPth=`pwd`/config.jcon
Gibraltar=`grep "GibraltarFolder" $CnfPth | awk -F '=' '{print $2}'`
ClbPth=$Gibraltar/tools/calibration/manual/calibration.jcon
CarId=`grep "CarId" $CnfPth | awk -F '=' '{print $2}'`
PfPth="$Gibraltar/platform/$CarId"
TypeFile=("velodyne_lidar.jcon" "velodyne_lidar.jcon" "velodyne_lidar.jcon" "delphi_esr_radar.jcon")
KeyWords=("imu2lidar" "vlp32_1_imu2lidar" "vlp32_2_imu2lidar" "esr_can_transform_data")
Conf=($3 $4 $5 $6 $7 $8 $9)
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

function Init() {
        if [ "$Type" == "-1" ];then
                echo "please enter the correct argument -t[0-hdl64;1-vlp32_1;2-vlp32_2]"
                exit;
        fi
        Path="$PfPth/${TypeFile[$Type]}"
        Key="${KeyWords[$Type]}"
        echo "Key=$Key"
}

function SetVerticalArgs() {
        echo "Path=$Path"
        start_raw=`cat $Path | grep -n $Key | awk -F ':' '{print $1};'`
        let "end_raw=$start_raw+8"
        let "line_id=$start_raw+1"
        new_config="\ \ \ \ \ \ quternion: True,"
        sed -i "${line_id}c $new_config" $Path
        let "line_id++"  
        while [ $line_id -le $end_raw ]
        do
                let "arg_index=${line_id}-${start_raw}-2"  
                new_config="\ \ \ \ \ \ ${ConfigPre[$arg_index]}\"${Conf[$arg_index]}\""
                sed -i "${line_id}c $new_config" $Path
                let "line_id++"
        done
}

function SetHorizontalArgs() {
        target_cali_node_raw=`cat $ClbPth | grep -n "radar_cali = Node" | awk -F ':' 'NR==1 {print $1};'`
        target_frame_raw=`cat $ClbPth | grep -A 10 "radar_cali = Node" | grep -n "target_frame" | awk -F ':' 'NR==1 {print $1};'`
        let "target_frame_raw+=target_cali_node_raw-1"
        new_config="\ \ \ \ target_frame: \"delphi_esr\","
        sed -i "${target_frame_raw}c $new_config" $ClbPth
        pre_args="\ \ \ \ \ \ args: "
        suf_args=", \"base_link\", \"hdl64\", \"100\"],"
        args="[\"${Conf[0]}\", \"${Conf[1]}\", \"${Conf[2]}\", \"${Conf[3]}\",\n\"${Conf[4]}\", \"${Conf[5]}\", \"${Conf[6]}\""
        args="${pre_args}${args}${suf_args}"
        radar_cali_launch_raw=`cat $ClbPth | grep -n "radar_cali_launch = Launch" | awk -F ':' 'NR==1 {print $1};'`
        radar_cali_launch_args_raw=`cat $ClbPth | grep -A 10 "radar_cali_launch = Launch" | grep -n "args:" | awk -F ':' '{print $1}'`
        let "radar_cali_launch_args_raw+=radar_cali_launch_raw-1"
        let "first_raw=radar_cali_launch_args_raw"
        let "second_raw=radar_cali_launch_args_raw+1"
        sed -i "${first_raw},${second_raw}c $args" $ClbPth
}

function main() {
        Init;
        SetVerticalArgs;
        if [[ "$Type" == "0" || "$Type" == "1" ]];then
                SetHorizontalArgs;
                echo "Horizontal"
        fi 
}

echo "Type=$Type"
main

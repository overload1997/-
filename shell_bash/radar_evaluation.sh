#! /bin/bash
ConfigPath=`pwd`/config.jcon
Gibraltar=`grep "GibraltarFolder" $ConfigPath | awk -F '=' '{print $2}'`
EvlPth=$Gibraltar/tools/calibration/lidar_radar/evaluation.sh
CarId=`grep "CarId" $ConfigPath | awk -F '=' '{print $2}'`
PfPth="$Gibraltar/platform/$CarId"
BagFolder=`grep "BagFolder" $ConfigPath | awk -F '=' 'NR==1 {print $2}'`
ConfigFolder=`cat $ConfigPath | grep ConfigFolder | awk -F '=' '{print $2}'`

function CheckEvaluation() {
	echo "CheckEvaluation..."
	sed -i "/GIBRALTAR_ROOT=/c GIBRALTAR_ROOT=$Gibraltar" $EvlPth
	sed -i "/DATA_FOLDER=/c DATA_FOLDER=$BagFolder" $EvlPth
    sed -i "/ConfigFolder=/c ConfigFolder=$ConfigFolder" $EvlPth
}

function Evaluation() {
	echo "run radar_evaluation.sh..."
	bash $EvlPth > xxxxxx20180726 
}
function ShowEvaluation() {
	echo "===================================================================================="
	echo "Radar evaluation... "
    while [[ `cat xxxxxx20180726 | grep "Finished"` == "" ]]; 
	do
		waitflag=1
	done
    cat xxxxxx20180726 | grep "SCORE" > xxxxxx20180806
	i=1
	while read eva
	do
    	echo ""
        cat radar_sensor_name | awk "NR==$i"
        cat lidar_radar_temp_tf | awk "NR==$i"
		echo $eva
		let "i++"
	done < xxxxxx20180806
    rm -r xxxxxx20180726
	rm -r xxxxxx20180806
  rm -r lidar_radar_temp_tf

	echo "===================================================================================="
	#rm -r radar_sensor_name
}

CheckEvaluation;
Evaluation &
sleep 5
ShowEvaluation;
sleep 5
echo "Run radar_evaluation.sh finish."

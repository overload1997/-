#! /bin/bash
CnfPth=`pwd`/config.jcon
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
echo setup.bash
source $Gibraltar/setup.bash
echo "---------------------------------------------------------------------------------------------------------------------------------------"
echo lidar_imu
bash `pwd`/lidar_imu.sh
echo "---------------------------------------------------------------------------------------------------------------------------------------"
echo lidar_radar
source $Gibraltar/setup.bash
bash `pwd`/lidar_radar.sh
echo "---------------------------------------------------------------------------------------------------------------------------------------"
echo lidar_evaluation
bash `pwd`/lidar_evaluation.sh
echo radar_evaluation
echo "---------------------------------------------------------------------------------------------------------------------------------------"
bash `pwd`/radar_evaluation.sh
echo "---------------------------------------------------------------------------------------------------------------------------------------"
echo "all of the calibration are finished!"


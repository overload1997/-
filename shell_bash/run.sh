#! /bin/bash
Gibraltar=`cat $CnfPth | grep GibraltarFolder | awk -F '=' '{print $2}'`
bash $Gibraltar/setup.bash
bash `pwd`/lidar_imu.sh
bash `pwd`/lidar_radar.sh
echo "all of the calibration are finished!"


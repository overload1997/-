#!/bin/bash
set -e
GIBRALTAR_ROOT=/mnt/yclei/code/gibraltar
BAG_FOLDER=/mnt/yclei/data/calibration/radar/mkz003/all
CONFIG_FLODER=/mnt/yclei/data/calibration/radar/mkz003/configs/all
LIDAR_IMU_TF=/xxx.txt
OUT_FOLDER=$1

BAG_FILE=""
CONFIG_FILE=""

echo "Get Bag File"
for ONE_BAG in `ls $BAG_FLODER/*.bag`; do
  if [ $BAG_FILE ]; then
    BAG_FILE="$BAG_FILE,$ONE_BAG"
  else
    BAG_FILE="$ONE_BAG"
  fi
done

for ONE_CONFIG in `ls $CONFIG_FLODER/*.yaml`; do
  if [ $CONFIG_FILE ]; then
    CONFIG_FILE="$CONFIG_FILE,$ONE_CONFIG"
  else
    CONFIG_FILE="$ONE_CONFIG"
  fi
done

printf "BAG_FILE: \"$BAG_FILE\"\n"
printf "CONFIG_FILE: \"$CONFIG_FILE\"\n"

echo "Calibration..."
bazel build :all
$GIBRALTAR_ROOT/bazel-bin/tools/calibration/lidar_radar/calibration \
  --save_file /$OUT_FOLDER/calib_file.yaml \
  --bag_file $BAG_FILE \
  --lidar_topic /hdl64/velodyne_points \
  --radar_topic /delphi_esr/as_tx/radar_markers \
  --config_file $CONFIG_FILE

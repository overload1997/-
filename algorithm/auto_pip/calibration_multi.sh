#!/bin/bash
GIBRALTAR_ROOT=/mnt/yclei/code/gibraltar
DATA_FOLDER=/mnt/yclei/data/bags/2015.07.24/calibration/mkz011
TOPIC_LIST=(/hdl64/velodyne_packets /vlp32_2/velodyne_packets)
LASER_NUM=(32 32)
ESTIMATE_Z_LIST=(1.58 0.58)
LIDAR_CALIB_FILE=$GIBRALTAR_ROOT/platform/common/velodyne_lidar_VLP32db.yaml

printf "DataFolder: \"/\"\nOutputFolder: \"\"\nMask: False" > ./config.yaml
bazel build //tools/hd_map_dev/bag_to_pcd:all

HMV_FILES=""
echo "bag to pcd..."
for i in ${!TOPIC_LIST[@]}; do
  SAVE_FOLDER=$DATA_FOLDER/$i
  if [ ! -d $SAVE_FOLDER ]; then
    mkdir $SAVE_FOLDER
  fi
  
  if [ "${LASER_NUM[$i]}" = "32" ]; then
    for BAG_FILE in `ls $DATA_FOLDER/*.bag`; do
      BAG_FILE=`basename $BAG_FILE`
      POSE_FILE="${BAG_FILE:0:-4}.hmv"
      if [ ! -f $SAVE_FOLDER/$POSE_FILE ]; then
        $GIBRALTAR_ROOT/bazel-bin/tools/hd_map_dev/bag_to_pcd/bag_to_pcd \
          --bag_file $DATA_FOLDER/$BAG_FILE \
          --save_folder $SAVE_FOLDER \
          --topics ${TOPIC_LIST[$i]} \
          --pose_file $SAVE_FOLDER/$POSE_FILE \
          --pose_topic /localization/current_pose \
          --calib_file calib_identity.yaml \
          --lidar_calib_file $LIDAR_CALIB_FILE\
          --pcd_prefix $SAVE_FOLDER \
          --merge --unpack --demotion
      fi
    done
  else
    for BAG_FILE in `ls $DATA_FOLDER/*.bag`; do
      BAG_FILE=`basename $BAG_FILE`
      POSE_FILE="${BAG_FILE:0:-4}.hmv"
      if [ ! -f $SAVE_FOLDER/$POSE_FILE ]; then
        $GIBRALTAR_ROOT/bazel-bin/tools/hd_map_dev/bag_to_pcd/bag_to_pcd \
          --bag_file $DATA_FOLDER/$BAG_FILE \
          --save_folder $SAVE_FOLDER \
          --topics ${TOPIC_LIST[$i]} \
          --pose_file $SAVE_FOLDER/$POSE_FILE \
          --pose_topic /localization/current_pose \
          --calib_file calib_identity.yaml \
          --pcd_prefix $SAVE_FOLDER \
          --merge --demotion
      fi
    done
  fi
  
  if [ ! "$HMV_FILES" ]; then
    HMV_FILES=`ls $SAVE_FOLDER/*.hmv`
  else
    HMV_FILES="$HMV_FILES `ls $SAVE_FOLDER/*hmv`"
  fi
done

ESTIMATE_Z=""
for Z in ${ESTIMATE_Z_LIST[@]}; do
  if [ ! "$ESTIMATE_Z" ]; then
    ESTIMATE_Z=$Z
  else
    ESTIMATE_Z="$ESTIMATE_Z,$Z"
 fi
done

echo "$HMV_FILES"
echo "Calibration..."
bazel build :all
../../../bazel-bin/tools/calibration/lidar_imu/calibration_multi \
  --save_file /$DATA_FOLDER/calib_file.yaml \
  --data_folder $DATA_FOLDER \
  --estimate_z_list $ESTIMATE_Z \
  --fixz \
  $HMV_FILES

#include "tools/gen_roi/gen_roi.h"

#include "gflags/gflags.h"

DEFINE_string(label_roi_file,
              "/home/host/data/1x1_map/semantic_map/roi1121_v2/", "");
DEFINE_string(map_dir,
              "/home/host/data/1x1_map/semantic_map/"
              "1023_hotel_0.02_pcd_tilemap_roi_with_pcd/tilemap/",
              "");

DEFINE_string(pcd_file, "", "It is used when pcd is not from tilemap");
DEFINE_string(pcd_roi_file, "/home/host/data/1x1_map/gen_roi_1130/roi1121_v2.pcd",
              "");
DEFINE_string(out_map_file,
              "/home/host/data/1x1_map/gen_roi_1130/ground_1x1_1121_v2.pcd",
              "");
DEFINE_string(out_ground_file,
              "/home/host/data/1x1_map/gen_roi_1130/ground_1x1_1121_v2.txt",
              "");
DEFINE_string(
    out_ground_filtered_file,
    "/home/host/data/1x1_map/gen_roi_1130/ground_1x1_1121_filtered_v2.pcd",
    "");
DEFINE_double(ground_filter_size, 0.2, "");
DEFINE_double(median_filter_size, 10, "");
DEFINE_double(y_flip, 1, "1 or -1");
int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  gen_roi::GenRoi node(FLAGS_map_dir, FLAGS_label_roi_file, FLAGS_y_flip);
  //node.Run(FLAGS_pcd_file, FLAGS_pcd_roi_file, FLAGS_out_map_file,
  //          FLAGS_out_ground_file, FLAGS_out_ground_filtered_file,
  //          FLAGS_ground_filter_size, FLAGS_median_filter_size);
  // node.FilterGroundFiles();
  node.CheckGround(FLAGS_out_ground_file);
  return 0;
}

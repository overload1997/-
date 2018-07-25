#include "tools/curbroi/gen_roi.h"

#include "gflags/gflags.h"

DEFINE_string(label_curb_file,
              "/media/local_6T/map/semantic_map/s_map_07-19-17-07-08.rio",
              "");
DEFINE_string(label_roi_file, "not used", "");
DEFINE_string(map_dir, "/media/local_6T/map/tilemap/tilemap-0523/final",
              "");

DEFINE_string(pcd_file, "", "It is used when pcd is not from tilemap");
DEFINE_string(pcd_roi_file,
              "/media/local_6T/map/ROI/gz_roi.pcd", "");
DEFINE_string(out_map_file,
              "/media/local_6T/map/ROI/gz.pcd", "");
DEFINE_string(out_ground_file,
              "/media/local_6T/map/ROI/gz.txt", "");
DEFINE_double(ground_filter_size, 0.2, "");
DEFINE_double(median_filter_size, 5, "");
DEFINE_double(y_flip, -1, "1 or -1");
int main(int argc, char **argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  curb_roi::GenRoi node(FLAGS_map_dir, FLAGS_label_roi_file,
                        FLAGS_label_curb_file, FLAGS_y_flip);
  node.WriteCurbRoi(FLAGS_pcd_file, FLAGS_pcd_roi_file, FLAGS_out_map_file,
                    FLAGS_out_ground_file, FLAGS_ground_filter_size,
                    FLAGS_median_filter_size);
  return 0;
}

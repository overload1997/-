#include "tools/gen_roi/gen_roi.h"

#include "gflags/gflags.h"

DEFINE_string(label_roi_file, "/home/host/data/1x1_map/semantic_map/roi/", "");
DEFINE_string(map_dir, "/home/host/data/1x1_map/semantic_map/0.02_pcd/tilemap/", "");

int main(int argc, char **argv){
  //zq_tools::timer_helper::Timer time_node;
  //time_node.name("mainTime");
  //time_node.start();

  std::unique_ptr<gen_roi::GenRoi> node(
      new gen_roi::GenRoi(FLAGS_map_dir, FLAGS_label_roi_file));
  //node->ShowRoiPng();
  //node->ShowCSloop(-550, -100, 550, 1000);
  //node->ShowCSloopRoi(-600, -400, 600, 1000);
  node->ShowCSloopRoi();
  //node->EistimateGround();

  //time_node.stop();
  //std::cout << time_node.toString() << std::endl;
  return 0;
}

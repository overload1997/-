#pragma once

#include <iostream>
#include "tools/hd_map_dev/tilemap/map_reader.h"
#include "s_loop/src/map/semantic_map/interface.h"
#include <pcl/filters/voxel_grid.h>


//#include "s_loop/src/perception/common/pcl_util.h"
//#include "s_loop/src/perception/detector/lane/line_finder.h"
//#include "Eigen/Core"
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/property_tree/ptree.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/objdetect/objdetect.hpp>
//#include <unordered_map>
//#include <vector>
//#include "glog/logging.h"
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
//#include "third_party/json/json.hpp"
//#include "tf_conversions/tf_eigen.h"
//
//#include "experimental/zhiqian/zq_tools/timer_helper.h"
//#include "experimental/zhiqian/zq_tools/memory.h"
//#include "s_loop/src/perception/common/ficp/simple_icp.h"
//#include "s_loop/src/perception/common/ficp/type_definition.h"
//#include <pcl/visualization/pcl_visualizer.h>

namespace gen_roi{
  int pcd_viewer_count_ = 0;
  pcl::visualization::PCLVisualizer::Ptr
      pcd_viewer_(new pcl::visualization::PCLVisualizer("Debug Viewer"));
  void ShowPointCloud(const pcl::PointCloud<pcl::PointXYZI>::Ptr in_cloud,
                      const int r, const int g, const int b,
                      const std::string& viewer_id_string,
                      const int viewer_count,
                      pcl::visualization::PCLVisualizer::Ptr debug_viewer) {
    pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
        new pcl::PointCloud<pcl::PointXYZRGB>);
    for (const auto& pt : in_cloud->points) {
      pcl::PointXYZRGB cpt;
      cpt.x = pt.x;
      cpt.y = pt.y;
      cpt.z = pt.z;
      cpt.r = r;
      cpt.g = g;
      cpt.b = b;
      rgb_cloud->push_back(cpt);
    }
    std::ostringstream viewer_id;
    viewer_id << viewer_id_string << viewer_count;
    debug_viewer->addPointCloud(rgb_cloud, viewer_id.str(), 0);
    std::cout << "add points " << rgb_cloud->points.size() << "to viewer"
              << std::endl;
}
class GenRoi{
  std::vector<std::vector<cv::Point2f>> opencv_polygons_;
  hdmap::MapReader map_reader_;


 public:
  explicit GenRoi(const std::string& map_dir, const std::string& label_roi_file)
      : map_reader_(map_dir) {
    std::cout << map_dir << std::endl;
    std::cout << label_roi_file << std::endl;
    semantic_map::Interface::LoadROI(label_roi_file, &opencv_polygons_);
  }
  void ShowRoiPng(){
    std::cout << "show png" << std::endl;
    //map_reader_.Visualize();
    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
    for (size_t i = 0; i < 5; i++) {
      double min_x = 1000000000000000000;
      double max_x = -1000000000000000000;
      double min_y = 1000000000000000000;
      double max_y = -1000000000000000000;
      for(const auto& p: opencv_polygons_[i]){
        std::cout << p.x << " " << p.y << " ";
        min_x = std::min(double(p.x), min_x);
        min_y = std::min(double(-1 * p.y), min_y);
        max_x = std::max(double(p.x), max_x);
        max_y = std::max(double(-1 * p.y), max_y);
      }
      std::cout << std::endl;
      std::cout << min_x << " " << min_y << " " << max_x << " " << max_y << std::endl;
      cv::Mat mat;
      //map_reader_.Query(min_x, min_y, max_x, max_y, &mat);
      //cv::imshow("image" , mat);
      //cv::waitKey(0);
      pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
      map_reader_.Query(min_x, min_y, max_x, max_y, cloud.get());

      pcd_viewer_->setBackgroundColor(0, 0, 0);
      pcd_viewer_->addCoordinateSystem(1.0);
      pcd_viewer_->initCameraParameters();
      ShowPointCloud(cloud, 255, 5, 5, "roi", pcd_viewer_count_++, pcd_viewer_);
      pcd_viewer_->spin();
    }
  }
  void ShowCSloop(const int map_min_x, const int map_min_y, const int map_max_x,
                  const int map_max_y) {
    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
    cv::Mat mat;
    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, &mat);
    cv::imshow("image", mat);
    cv::waitKey(0);

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1, cloud.get());

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
    voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
    voxel_grid_filter.setInputCloud(cloud);
    voxel_grid_filter.filter(*cloud_f);

    pcd_viewer_->setBackgroundColor(0, 0, 0);
    pcd_viewer_->addCoordinateSystem(1.0);
    pcd_viewer_->initCameraParameters();
    ShowPointCloud(cloud_f, 255, 5, 5, "roi", pcd_viewer_count_++, pcd_viewer_);
    pcd_viewer_->spin();
  }

  void ShowCSloopRoi(const int map_min_x, const int map_min_y,
                     const int map_max_x, const int map_max_y) {
    cv::Mat mat;

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1, cloud.get());

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
    voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
    voxel_grid_filter.setInputCloud(cloud);
    voxel_grid_filter.filter(*cloud_f);

    std::vector<std::vector<cv::Point2f>> polygons;
    ShrinkPolygonToROI(map_min_x, map_min_y, map_max_x, map_max_y, &polygons);

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
        new pcl::PointCloud<pcl::PointXYZI>);
    int cnt=0;
    for (const auto& pt : cloud_f->points) {
      if (cnt % 1000 == 0) {
        std::cout << cnt *1.0 /  cloud_f->points.size() << std::endl;
      }
      for (const auto& polygon : polygons) {
        if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, -pt.y), false) >=
            0) {
          cloud_roi_ptr->push_back(pt);
          break;
        }
      }
      cnt++;
    }

    pcd_viewer_->setBackgroundColor(0, 0, 0);
    pcd_viewer_->addCoordinateSystem(1.0);
    pcd_viewer_->initCameraParameters();
    ShowPointCloud(cloud_roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++, pcd_viewer_);
    pcd_viewer_->spin();
  }

  void ShowCSloopRoi() {
    cv::Mat mat;

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    map_reader_.Query(map_reader_.map_range(), 2, cloud.get());

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
    voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
    voxel_grid_filter.setInputCloud(cloud);
    voxel_grid_filter.filter(*cloud_f);

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
        new pcl::PointCloud<pcl::PointXYZI>);
    int cnt=0;
    for (const auto& pt : cloud_f->points) {
      if (cnt % 100000 == 0) {
        LOG(INFO) << cnt *1.0 /  cloud_f->points.size() << std::endl;
      }
      for (const auto& polygon : opencv_polygons_) {
        if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, -pt.y), false) >=
            0) {
          cloud_roi_ptr->push_back(pt);
          break;
        }
      }
      cnt++;
    }

    //pcd_viewer_->setBackgroundColor(0, 0, 0);
    //pcd_viewer_->addCoordinateSystem(1.0);
    //pcd_viewer_->initCameraParameters();
    //ShowPointCloud(cloud_roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++, pcd_viewer_);
    //pcd_viewer_->spin();
    pcl::io::savePCDFile("/home/host/data/1x1_map/sematic_map_roi0922.pcd", *cloud_roi_ptr,
                         true);
  }

  void EistimateGround() {
    LOG(INFO) << map_reader_.map_range();

    double min_x = map_reader_.map_range().min_x;
    double min_y = map_reader_.map_range().min_y;
    double max_x = map_reader_.map_range().max_x;
    double max_y = map_reader_.map_range().max_y;
    // resolution 0.2; will try smaller later
    double resolution = 0.2;
    const int dim_x = (max_x - min_x) / resolution + 1;
    const int dim_y = (max_y - min_y) / resolution + 1;
    std::vector<std::vector<double>> map_sum(dim_x,
                                             std::vector<double>(dim_y, 0));
    std::vector<std::vector<int>> map_cnt(dim_x, std::vector<int>(dim_y, 0));
    std::vector<std::vector<double>> map_avg(dim_x,
                                             std::vector<double>(dim_y, 0));
    // depth 2; will try smaller later

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    map_reader_.Query(map_reader_.map_range(), 2, cloud.get());
    LOG(INFO) << "EstimateGround";
    for (const auto& p : cloud->points) {
      const int i = (p.x - min_x) / resolution;
      const int j = (-p.y - min_y) / resolution;
      if (i < 0 || j < 0 || i >= dim_x || j >= dim_y) {
        LOG(ERROR) << "Incorrect point range" << p.x << " " << -p.y << " ";
        return;
      }
      map_sum[i][j] += p.z;
      map_cnt[i][j]++;
    }
    LOG(INFO) << "average";
    for (int i = 0; i < dim_x; i++) {
      LOG(INFO) << "roi check: " << i << " / " << dim_x;
      for (int j = 0; j < dim_y; j++) {
        int cnt=0;
        for (int x = -2; x <= 2; x++) {
          for (int y = -2; y <= 2; y++) {
            if (i + x >= 0 && i + x < dim_x && j + y >= 0 && j + y < dim_y) {
              cnt += map_cnt[i + x][j + y];
            }
          }
        }
        if(cnt ==0){
          continue;
        }
        
        bool in_roi = false;

        for (const auto& polygon : opencv_polygons_) {
          if (cv::pointPolygonTest(polygon,
                                   cv::Point2f(i * resolution + min_x,
                                               -(j * resolution + min_y)),
                                   false) >= 0) {
            in_roi = true;
            break;
          }
        }
        if(!in_roi){
          map_cnt[i][j] = 0;
          map_sum[i][j] = 0;
          continue;
        }
      }
    }

    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map_cnt[i][j] == 0) continue;
        double sum = 0;
        int cnt = 0;
        for (int x = -2; x <= 2; x++) {
          for (int y = -2; y <= 2; y++) {
            if (i + x >= 0 && i + x < dim_x && j + y >= 0 && j + y < dim_y) {
              sum += map_sum[i + x][j + y];
              cnt += map_cnt[i + x][j + y];
            }
          }
        }
        map_avg[i][j] = sum / cnt;
      }
    }

    std::ofstream of;
    of.open("/home/host/data/1x1_map/ground_1x1_0920.txt");

    of << min_x << " " << min_y << " " << resolution << " " << dim_x << " "
       << dim_y << std::endl;
    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map_cnt[i][j]) {
          of << i * dim_y + j << " " << map_avg[i][j] << std::endl;
        }
      }
    }
    of.close();

    pcl::PointCloud<pcl::PointXYZ>::Ptr map_save(
        new pcl::PointCloud<pcl::PointXYZ>);
    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map_cnt[i][j]) {
          pcl::PointXYZ p;
          p.x = i * resolution + min_x;
          p.y = j * resolution + min_y;
          p.z = map_avg[i][j];
          map_save->push_back(p);
        }
      }
    }
    pcl::io::savePCDFile("/home/host/data/1x1_map/ground_1x1_0920.pcd",
                         *map_save, true);
    // cv::Mat mat;

    // pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
    //    new pcl::PointCloud<pcl::PointXYZI>);
    // map_reader_.Query(map_reader_.map_range(), 2, cloud.get());

    // pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
    //    new pcl::PointCloud<pcl::PointXYZI>);
    // pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
    // voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
    // voxel_grid_filter.setInputCloud(cloud);
    // voxel_grid_filter.filter(*cloud_f);

    // pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
    //    new pcl::PointCloud<pcl::PointXYZI>);
    // int cnt=0;
    // for (const auto& pt : cloud_f->points) {
    //  if (cnt % 10000 == 0) {
    //    std::cout << cnt *1.0 /  cloud_f->points.size() << std::endl;
    //  }
    //  for (const auto& polygon : opencv_polygons_) {
    //    if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, -pt.y), false) >=
    //        0) {
    //      cloud_roi_ptr->push_back(pt);
    //      break;
    //    }
    //  }
    //  cnt++;
    //}

    ////pcd_viewer_->setBackgroundColor(0, 0, 0);
    ////pcd_viewer_->addCoordinateSystem(1.0);
    ////pcd_viewer_->initCameraParameters();
    ////ShowPointCloud(cloud_roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++,
    /// pcd_viewer_);
    ////pcd_viewer_->spin();
    // pcl::io::savePCDFile("/home/host/data/sematic_map_roi0913.pcd",
    // *cloud_roi_ptr,
    //                     true);
  }
  void ShrinkPolygonToROI(const int map_min_x, const int map_min_y,
                     const int map_max_x, const int map_max_y, std::vector<std::vector<cv::Point2f>>* polygons) {
    for(const auto &vec:opencv_polygons_){
      if (vec.size() < 3) continue;
      int min_x = vec[0].x;
      int min_y = vec[0].y;
      int max_x = vec[0].x;
      int max_y = vec[0].y;
      for(const auto p : vec){
        min_x = std::min(int(p.x), min_x);
        min_y = std::min(int(p.y), min_y);
        max_x = std::max(int(p.x), max_x);
        max_x = std::max(int(p.y), max_y);
      }
      int ov_min_x = std::max(int(min_x), map_min_x);
      int ov_min_y = std::max(int(min_y), map_min_y);
      int ov_max_x = std::min(int(max_x), map_max_x);
      int ov_max_y = std::min(int(max_y), map_max_y);
      if(ov_min_x >= ov_max_x || ov_min_y >= ov_max_y){
        continue;
      }
      polygons->push_back(vec);
    }
  }
};
}
//    double min_x = 100000, min_y = 1000000, max_x = -10000000, max_y = -1000000;
//    for(const auto& vec:car_poses_){
//      min_x = std::min(min_x, vec[1]);
//      min_y = std::min(min_y, vec[2]);
//      max_x = std::max(max_x, vec[1]);
//      max_y = std::max(max_y, vec[2]);
//    }
//    whole_min_x_ = min_x - max_distance_;
//    whole_min_y_ = min_y - max_distance_;
//    const int dim_x = (max_x - min_x + max_distance_ * 2) / resolution_ + 1;
//    const int dim_y = (max_y - min_y + max_distance_ * 2) / resolution_ + 1;
//    whole_img_ = cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    whole_lidar_= cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    whole_lidar_range_ = cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    icp_.setMinNumberCorrespondences(1);
//    icp_.setMaxCorrespondenceDistance(1);
//    icp_.setMaximumIterations(50);
//    icp_.setEuclideanFitnessEpsilon(0.0001);
//    pcd_viewer_->setBackgroundColor(0, 0, 0);
//    pcd_viewer_->addCoordinateSystem(1.0);
//    pcd_viewer_->initCameraParameters();
//    read_file_n("/home/host/workspace/scripts/colormap.txt", 3, &colormap_);
//  };
//
//  ~LidarLane() {}
//
//  Mat blur(const Mat& input_image, int kernel_size) {
//    Mat smoothed_image;
//    cv::blur(input_image, smoothed_image, Size(kernel_size, kernel_size));
//    return smoothed_image;
//  }
//
//  Mat canny(const Mat& input_image, double low_threshold, double high_threshold,
//            int kernel_size) {
//    Mat edges;
//    cv::Canny(input_image, edges, low_threshold, high_threshold, kernel_size);
//    return edges;
//  }
//  Mat houghDetector(const Mat& binary_image, const Mat& original_image) {
//    LineFinder ld;
//    ld.setLineLengthAndGap(160, 60);
//    ld.setMinVote(10);
//    // image_to_process at this point should be Binary picture
//    Mat output_image_unfiltered = original_image.clone();
//    ld.findLines(binary_image);
//    Mat output_image = original_image.clone();
//    ld.drawDetectedLines(output_image);
//    return output_image;
//  }
//  // explicit LidarLane(Scalar_t max_distance) : max_distance_(max_distance) {
//  //  static_assert(std::is_same<PointSource_t, PointXYZIRTf>::value,
//  //                "Only pcl::PointXYZIRTf type supported for source");
//  //  static_assert(std::is_same<PointTarget_t, PointXYZIRTf>::value,
//  //                "Only pcl::PointXYZIRTf type supported for source");
//  //}
//  // void GroundShow(const typename pcl::PointCloud<PointSource_t>::Ptr input)
//  // {
//  void GroundShow(const std::string& in_str) {
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(in_str, *in_ptr);
//    PointCloudPtr filtered_ptr(new PointCloud);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    float max_i = -10000, min_i = 10000;
//    // float max_x= -10000, min_x= 10000;
//    // float max_y= -10000, min_y= 10000;
//    for (const auto& p : in_ptr->points) {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.7) {
//        // std::cout << p.x << " " << p.y << " " << p.z << " " << p.intensity
//        //          << " " << p.timestamp << " " << p.ring << std::endl;
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        max_i = max_i < p.intensity ? p.intensity : max_i;
//        min_i = min_i > p.intensity ? p.intensity : min_i;
//        // max_x = max_x < p.x ? p.x : max_x;
//        // min_x = min_x > p.x ? p.x : min_x;
//        // max_y = max_y < p.y ? p.y : max_y;
//        // min_y = min_y > p.y ? p.y : min_y;
//        // std::cout << max_i << " " << min_i << std::endl;
//        filtered_ptr->push_back(pt);
//        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//      }
//    }
//    // std::cout << max_i << " " << min_i << std::endl;
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(xyz_ptr);
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//    const unsigned int dim_x = 2 * max_distance_ / resolution_ + 1;
//    const unsigned int dim_y = 2 * max_distance_ / resolution_ + 1;
//
//    // std::cout << dim_x << " " << dim_y << std::endl;
//    cv::Mat img = cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    // img.create(dim_x, dim_y, CV_8UC1);
//    // std::cout << "Initial Image "  << std::endl;
//    // cv::imshow("png map", img);
//    // cv::waitKey();
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        pointIdxRadiusSearch.clear();
//        pointRadiusSquaredDistance.clear();
//        const double x = i * resolution_ - max_distance_;
//        const double y = j * resolution_ - max_distance_;
//        pcl::PointXYZ p(x, y, 0);
//        // std::cout << x << " " << y << std::endl;
//        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                pointRadiusSquaredDistance) > 0) {
//          std::vector<double> tmp_v;
//          double ring = filtered_ptr->points[pointIdxRadiusSearch[0]].ring;
//          for (const auto idx : pointIdxRadiusSearch) {
//            if (filtered_ptr->points[idx].ring == ring) {
//              tmp_v.push_back(filtered_ptr->points[idx].intensity);
//            }
//          }
//          SetImg(tmp_v, &(img.at<uchar>(i, j)));
//        }
//      }
//    }
//    // cv::imshow("png map", img);
//    cv::imwrite(in_str + ".png", img);
//    // cv::waitKey();
//    // std::cout << " blur " << std::endl;
//    // cv::Mat img1 = blur(img, 3);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//    // std::cout << " canny" << std::endl;
//    // img1 = canny(img1, 30, 150, 3);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//    // std::cout << " houghDetector" << std::endl;
//    // img1 = houghDetector(img1, img);
//    // cv::imwrite("hdmap_use_intensity.png", img1);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//  }
//
//  void GroundShowZ(const std::string& in_str) {
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(in_str, *in_ptr);
//    PointCloudPtr filtered_ptr(new PointCloud);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    for (const auto& p : in_ptr->points) {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.7) {
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        filtered_ptr->push_back(pt);
//        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//      }
//    }
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(xyz_ptr);
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//    const unsigned int dim_x = 2 * max_distance_ / resolution_ + 1;
//    const unsigned int dim_y = 2 * max_distance_ / resolution_ + 1;
//    cv::Mat img = cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        pointIdxRadiusSearch.clear();
//        pointRadiusSquaredDistance.clear();
//        const double x = i * resolution_ - max_distance_;
//        const double y = j * resolution_ - max_distance_;
//        pcl::PointXYZ p(x, y, 0);
//        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                pointRadiusSquaredDistance) > 0) {
//          std::vector<double> tmp_v;
//          for (const auto idx : pointIdxRadiusSearch) {
//            tmp_v.push_back(filtered_ptr->points[idx].z);
//          }
//          SetImg(tmp_v, &(img.at<uchar>(i, j)));
//        }
//      }
//    }
//    cv::imwrite(in_str + "_z.png", img);
//    // cv::imshow("png map", img);
//    // cv::waitKey();
//    // std::cout << " blur " << std::endl;
//    // cv::Mat img1 = blur(img, 3);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//    // std::cout << " canny" << std::endl;
//    // img1 = canny(img1, 30, 150, 3);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//    // std::cout << " houghDetector" << std::endl;
//    // img1 = houghDetector(img1, img);
//    // cv::imwrite("hdmap_use_intensity.png", img1);
//    // cv::imshow("png map", img1);
//    // cv::waitKey();
//  }
//
//  void Pcd2Png(const std::string& in_str) {
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(in_str, *in_ptr);
//    PointCloudPtr filtered_ptr(new PointCloud);
//    float max_i = -10000, min_i = 10000;
//    for (const auto& p : in_ptr->points) {
//      // if (std::abs(p.x) < max_distance_ && std::abs(p.y) < 3 && p.z < -1.0)
//      // {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.0) {
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        max_i = max_i < p.intensity ? p.intensity : max_i;
//        min_i = min_i > p.intensity ? p.intensity : min_i;
//        filtered_ptr->push_back(pt);
//      }
//    }
//    // std::cout << max_i << " " << min_i << std::endl;
//    const unsigned int dim_x = 2 * max_distance_ / resolution_ + 1;
//    const unsigned int dim_y = 2 * max_distance_ / resolution_ + 1;
//    vector<vector<float>> vec_map(dim_x, vector<float>(dim_y, 0));
//    vector<vector<int>> vec_count(dim_x, vector<int>(dim_y, 0));
//    for (const auto& p : filtered_ptr->points) {
//      const int i = (p.x + max_distance_) / resolution_;
//      const int j = (p.y + max_distance_) / resolution_;
//      vec_map[i][j] += (p.intensity - min_i) / (max_i - min_i);
//      vec_count[i][j]++;
//    }
//
//    max_i = -10000, min_i = 10000;
//    cv::Mat img = cv::Mat(dim_x, dim_y, CV_8UC1);
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        vec_map[i][j] /= vec_count[i][j];
//        max_i = max_i < vec_map[i][j] ? vec_map[i][j] : max_i;
//        min_i = min_i > vec_map[i][j] ? vec_map[i][j] : min_i;
//      }
//    }
//    // std::cout << max_i << " " << min_i << std::endl;
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        img.at<uchar>(i, j) =
//            static_cast<uchar>(255 * (vec_map[i][j] - min_i) / (max_i - min_i));
//      }
//    }
//    cv::imwrite("pcd.png", img);
//    cv::imshow("png map", img);
//    cv::waitKey();
//  }
//
//  bool SearchPoseWithTime(const std::vector<std::vector<double>>& poses,
//                          const double query_time, Eigen::Matrix4d* mat4d) {
//    double minV = 1;
//    int idx = -1;
//    for (size_t i = 0; i < poses.size(); i++) {
//      if (poses[i][0] - query_time < -0.1) {
//        continue;
//      }
//      if (poses[i][0] - query_time > 0.1) {
//        break;
//      }
//      if (minV > std::fabs(poses[i][0] - query_time)) {
//        minV = std::fabs(poses[i][0] - query_time);
//        idx = i;
//      }
//    }
//    if (idx != -1) {
//      const auto& vec = poses[idx];
//      Eigen::Quaternion<double> tmp_quat(vec[4], vec[5], vec[6], vec[7]);
//      Eigen::Matrix3d tmp_mat3d = tmp_quat.toRotationMatrix();
//
//      for (int i = 0; i < 3; i++) {
//        for (int j = 0; j < 3; j++) {
//          (*mat4d)(i, j) = tmp_mat3d(i, j);
//        }
//      }
//      (*mat4d)(0, 3) = vec[1];
//      (*mat4d)(1, 3) = vec[2];
//      (*mat4d)(2, 3) = vec[3];
//      (*mat4d)(3, 3) = 1;
//
//      return true;
//    }
//    return false;
//  }
//
//  void GroundLaneLabel(const std::string& in_str) {
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(in_str, *in_ptr);
//
//
//    PointCloudPtr filtered_ptr(new PointCloud);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    for (const auto& p : in_ptr->points) {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.0) {
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        filtered_ptr->push_back(pt);
//        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//      }
//    }
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(xyz_ptr);
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//    const unsigned int dim_x = 2 * max_distance_ / resolution_ + 1;
//    const unsigned int dim_y = 2 * max_distance_ / resolution_ + 1;
//
//    cv::Mat img = cv::Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//    // img.create(dim_x, dim_y, CV_8UC1);
//    // std::cout << "Initial Image "  << std::endl;
//    // cv::imshow("png map", img);
//    // cv::waitKey();
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        pointIdxRadiusSearch.clear();
//        pointRadiusSquaredDistance.clear();
//        const double x = i * resolution_ - max_distance_;
//        const double y = j * resolution_ - max_distance_;
//        pcl::PointXYZ p(x, y, 0);
//        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                pointRadiusSquaredDistance) > 0) {
//          double sum = 0;
//          for (const auto idx : pointIdxRadiusSearch) {
//            sum += filtered_ptr->points[idx].intensity;
//          }
//          img.at<uchar>(i, j) = sum / pointIdxRadiusSearch.size();
//          // std::vector<double> tmp_v;
//          // double ring = filtered_ptr->points[pointIdxRadiusSearch[0]].ring;
//          // for (const auto idx : pointIdxRadiusSearch) {
//          //  if (filtered_ptr->points[idx].ring == ring) {
//          //    tmp_v.push_back(filtered_ptr->points[idx].intensity);
//          //  }
//          //}
//          // SetImg(tmp_v, &(img.at<uchar>(i, j)));
//        }
//      }
//    }
//    // std::cout << in_str.substr(59, 20) << std::endl;
//
//    Eigen::Matrix4d car_pose_matrix = Eigen::Matrix4d::Identity();
//    if (!SearchPoseWithTime(car_poses_, std::stod(in_str.substr(59, 20)),
//                            &car_pose_matrix)) {
//      LOG(ERROR) << "cannot find pose for " << in_str;
//      return;
//    }
//
//    std::cout << in_str.substr(59, 20) << std::endl;
//    for (const auto& vec : gt_lane_[in_str.substr(59, 20)]) {
//      // std::cout << " -------------" << std::endl;
//      //std::cout << vec[0] << " " << vec[1] << " " << vec[2] << " " << vec[3]
//      //          << std::endl;
//
//      int gap_x = std::abs(vec[2]-vec[0]);
//      int gap_y = std::abs(vec[3]-vec[1]);
//
//      if(gap_x < gap_y){
//        float slope = gap_x * 1.0 / gap_y;
//        float start_y = 0, end_y = 0, start_x = 0;
//        if (vec[1] < vec[3]) {
//          start_y = vec[1];
//          end_y = vec[3];
//          start_x = vec[0];
//        } else {
//          start_y = vec[3];
//          end_y = vec[1];
//          start_x = vec[2];
//        }
//        for (size_t j = start_y; j <= end_y; j++) {
//          const int i = slope * (j - start_y) + start_x;
//          for (int ii = -1; ii <= 1; ii++) {
//            for (int jj = -1; jj <= 1; jj++) {
//              img.at<uchar>(i + ii, j + jj) = 255;
//
//              const double x = (i + ii) * resolution_ - max_distance_;
//              const double y = (j + jj) * resolution_ - max_distance_;
//              pcl::PointXYZ p(x, y, 0);
//              if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                      pointRadiusSquaredDistance) > 0) {
//                for (const auto idx : pointIdxRadiusSearch) {
//                  Eigen::Vector4d vec(filtered_ptr->points[idx].x,
//                                      filtered_ptr->points[idx].y,
//                                      filtered_ptr->points[idx].z, 1);
//                  vec = car_pose_matrix * lidar_imu_matrix_ * vec;
//                  if (vec(0) < whole_min_x_ || vec(1) < whole_min_y_) {
//                    std::cout << "out of min range!" << std::endl;
//                    std::cout << vec << std::endl;
//                    std::cout << whole_min_x_ << " " << whole_min_y_
//                              << std::endl;
//                    continue;
//                  }
//                  const int wi = (vec(0) - whole_min_x_) / resolution_;
//                  const int wj = (vec(1) - whole_min_y_) / resolution_;
//                  if (wi >= whole_img_.rows || wj >= whole_img_.cols) {
//                    std::cout << "out of max range!" << std::endl;
//                    std::cout << wi << " " << wj << std::endl;
//                    continue;
//                  }
//                  whole_img_.at<uchar>(wi, wj) = 255;
//                }
//              }
//            }
//          }
//        }
//      } else {
//        float slope = (vec[3] - vec[1]) * 1.0 / (vec[2] - vec[0]);
//        float start_x = 0, end_x = 0, start_y = 0;
//        if (vec[0] < vec[2]) {
//          start_x = vec[0];
//          end_x = vec[2];
//          start_y = vec[1];
//        } else {
//          start_x = vec[2];
//          end_x = vec[0];
//          start_y = vec[3];
//        }
//        for (int i = start_x; i <= end_x; i++) {
//          const int j = slope * (i - start_x) + start_y;
//          for (int ii = -1; ii <= 1; ii++) {
//            for (int jj = -1; jj <= 1; jj++) {
//              img.at<uchar>(i + ii, j + jj) = 255;
//
//              const double x = (i + ii) * resolution_ - max_distance_;
//              const double y = (j + jj) * resolution_ - max_distance_;
//              pcl::PointXYZ p(x, y, 0);
//              if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                      pointRadiusSquaredDistance) > 0) {
//                for (const auto idx : pointIdxRadiusSearch) {
//                  Eigen::Vector4d vec(filtered_ptr->points[idx].x,
//                                      filtered_ptr->points[idx].y,
//                                      filtered_ptr->points[idx].z, 1);
//                  vec = car_pose_matrix * lidar_imu_matrix_ * vec;
//                  if (vec(0) < whole_min_x_ || vec(1) < whole_min_y_) {
//                    std::cout << "out of min range!" << std::endl;
//                    std::cout << vec << std::endl;
//                    std::cout << whole_min_x_ << " " << whole_min_y_
//                              << std::endl;
//                    continue;
//                  }
//                  const int wi = (vec(0) - whole_min_x_) / resolution_;
//                  const int wj = (vec(1) - whole_min_y_) / resolution_;
//                  if (wi >= whole_img_.rows || wj >= whole_img_.cols) {
//                    std::cout << "out of max range!" << std::endl;
//                    std::cout << wi << " " << wj << std::endl;
//                    continue;
//                  }
//                  whole_img_.at<uchar>(wi, wj) = 255;
//                }
//              }
//            }
//          }
//        }
//      }
//      // for (int i = -2; i <= 2; i++) {
//      //  for (int j = -2; j <= 2; j++) {
//      //    img.at<uchar>(i + vec[1], j + vec[0]) = 255;
//      //    img.at<uchar>(i + vec[3], j + vec[2]) = 255;
//      //  }
//      //}
//    }
//
//    // cv::imwrite("pcd.png", img);
//    Mat combine(std::max(whole_img_.size().height, img.size().height),
//                whole_img_.size().width + img.size().width, CV_8UC1);
//    Mat left_roi(combine, cv::Rect(0, 0, img.size().width, img.size().height));
//    img.copyTo(left_roi);
//    Mat right_roi(combine,
//                  cv::Rect(img.size().width, 0, whole_img_.size().width,
//                           whole_img_.size().height));
//    whole_img_.copyTo(right_roi);
//
//    cv::namedWindow("image",cv::WINDOW_NORMAL);
//    cv::resizeWindow("image", 1920,1080);
//    cv::imshow("image", combine);
//    //cv::imshow("local map", combine);
//    //cv::imshow("local map", img);
//    //cv::imshow("global map", whole_img_);
//    cv::waitKey();
//  }
//
//  void GroundLaneLabelPcd(const std::string& in_str) {
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(in_str, *in_ptr);
//    PointCloudPtr filtered_ptr(new PointCloud);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    for (const auto& p : in_ptr->points) {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.0) {
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        filtered_ptr->push_back(pt);
//        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//      }
//    }
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(xyz_ptr);
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//
//    Eigen::Matrix4d car_pose_matrix = Eigen::Matrix4d::Identity();
//    if (!SearchPoseWithTime(car_poses_, std::stod(in_str.substr(59, 20)),
//                            &car_pose_matrix)) {
//      LOG(ERROR) << "cannot find pose for " << in_str;
//      return;
//    }
//
//    for (const auto& vec : gt_lane_[in_str.substr(59, 20)]) {
//      if (vec[2] - vec[0] == 0) continue;
//      float slope = (vec[3] - vec[1]) * 1.0 / (vec[2] - vec[0]);
//      float start_x = 0, end_x = 0, start_y = 0;
//      if (vec[0] < vec[2]) {
//        start_x = vec[0];
//        end_x = vec[2];
//        start_y = vec[1];
//      } else {
//        start_x = vec[2];
//        end_x = vec[0];
//        start_y = vec[3];
//      }
//      for (int i = start_x; i <= end_x; i++) {
//        const int j = slope * (i - start_x) + start_y;
//        const double x = i * resolution_ - max_distance_;
//        const double y = j * resolution_ - max_distance_;
//        pcl::PointXYZ p(x, y, 0);
//        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                pointRadiusSquaredDistance) > 0) {
//          for (const auto idx : pointIdxRadiusSearch) {
//            Eigen::Vector4d vec(filtered_ptr->points[idx].x,
//                                filtered_ptr->points[idx].y,
//                                filtered_ptr->points[idx].z, 1);
//            vec = car_pose_matrix * lidar_imu_matrix_ * vec;
//          }
//        }
//      }
//    }
//  }
//  void EigenMatrix2Pose(const Eigen::Matrix4d& mat4d) {
//    tf::Matrix3x3 tf_mat;
//    double r, p, y;
//    tf_mat.setValue(mat4d(0, 0), mat4d(0, 1), mat4d(0, 2), mat4d(1, 0),
//                    mat4d(1, 1), mat4d(1, 2), mat4d(2, 0), mat4d(2, 1),
//                    mat4d(2, 2));
//    tf_mat.getRPY(r, p, y);
//    std::cout << mat4d(0, 3) << " " << mat4d(1, 3) << " " << mat4d(2, 3) << " "
//              << r * 180 / 3.1415926 << " " << p * 180 / 3.1415926 << " "
//              << y * 180 / 3.1415926 << std::endl;
//  }
//  void EigenMatrix2Pose(const Eigen::Matrix4d& mat4d,
//                        vector<vector<double>>* vec) {
//    tf::Matrix3x3 tf_mat;
//    double r, p, y;
//    tf_mat.setValue(mat4d(0, 0), mat4d(0, 1), mat4d(0, 2), mat4d(1, 0),
//                    mat4d(1, 1), mat4d(1, 2), mat4d(2, 0), mat4d(2, 1),
//                    mat4d(2, 2));
//    tf_mat.getRPY(r, p, y);
//    r *= (180/3.1415926);
//    p *= (180/3.1415926);
//    y *= (180/3.1415926);
//    vec->push_back({mat4d(0, 3), mat4d(1, 3), mat4d(2, 3), r, p, y});
//  }
//
//  void Pose2EigenMatrix(const vector<double>& vec,
//                        Eigen::Matrix4d* mat4d) {
//    Eigen::AngleAxisd rotation_x(vec[3], Eigen::Vector3d::UnitX());
//    Eigen::AngleAxisd rotation_y(vec[4], Eigen::Vector3d::UnitY());
//    Eigen::AngleAxisd rotation_z(vec[5], Eigen::Vector3d::UnitZ());
//
//    Eigen::Translation3d translation(vec[0], vec[1], vec[2]);
//
//    *mat4d = (translation * rotation_z * rotation_y * rotation_x).matrix();
//  }
//  void MapPngToPcd(const PointCloudPtr in_ptr,
//                   const vector<vector<double>>& label,
//                   vector<PointCloudPtr>* out) {
//    PointCloudPtr filtered_ptr(new PointCloud);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    PointCloudPtr point_ptr(new PointCloud);
//    for (const auto& p : in_ptr->points) {
//      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//          p.z < -1.0) {
//        Point pt;
//        pt.x = p.x;
//        pt.y = p.y;
//        pt.z = p.z;
//        pt.intensity = p.intensity;
//        pt.ring = p.ring;
//        filtered_ptr->push_back(pt);
//        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//      }
//    }
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(xyz_ptr);
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//    const unsigned int dim_x = 2 * max_distance_ / resolution_ + 1;
//    const unsigned int dim_y = 2 * max_distance_ / resolution_ + 1;
//    for (size_t i = 0; i < dim_x; ++i) {
//      for (size_t j = 0; j < dim_y; ++j) {
//        pointIdxRadiusSearch.clear();
//        pointRadiusSquaredDistance.clear();
//        const double x = i * resolution_ - max_distance_;
//        const double y = j * resolution_ - max_distance_;
//        pcl::PointXYZ p(x, y, 0);
//        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//                                pointRadiusSquaredDistance) > 0) {
//          for(const auto& idx : pointIdxRadiusSearch){
//            point_ptr->push_back(filtered_ptr->points[idx]);
//          }
//        }
//      }
//    }
//    out->push_back(point_ptr);
//  }
//
//  void PngLabel2PixelMap(const vector<vector<double>>& png_labels,
//                         const Eigen::Matrix4d& car_pose,
//                         const Eigen::Matrix4d& adjust,
//                         vector<Eigen::Vector4d>* points_map,
//                         double* car_x, double * car_y) {
//    points_map->clear();
//    zq_tools::timer_helper::Timer time_pngpixel;
//    time_pngpixel.name("time_pngpixel");
//    time_pngpixel.start();
//
//    Eigen::Matrix4d lidar_world = Eigen::Matrix4d::Identity();
//    lidar_world = adjust * car_pose * lidar_imu_matrix_;
//    *car_x = lidar_world(0,3);
//    *car_y = lidar_world(1,3);
//    for (size_t iLabel = 0; iLabel < png_labels.size(); iLabel++) {
//
//      const auto& vec = png_labels[iLabel];
//      int gap_x = std::abs(vec[2]-vec[0]);
//      int gap_y = std::abs(vec[3]-vec[1]);
//
//      //std::cout << "label id " << iLabel << std::endl;
//      //for(const auto& it : vec){
//      //  std::cout << it << " " ;
//      //}
//      //std::cout << std::endl;
//      if(gap_x < gap_y){
//        float slope = gap_x * 1.0 / gap_y;
//        float start_y = 0, end_y = 0, start_x = 0;
//        if (vec[1] < vec[3]) {
//          start_y = vec[1];
//          end_y = vec[3];
//          start_x = vec[0];
//        } else {
//          start_y = vec[3];
//          end_y = vec[1];
//          start_x = vec[2];
//        }
//        for (size_t j = start_y; j <= end_y; j++) {
//          const double i = slope * (j - start_y) + start_x;
//          const double x = i * resolution_ - max_distance_;
//          const double y = j * resolution_ - max_distance_;
//          Eigen::Vector4d p(x, y, -1.9, 1);
//          p = lidar_world * p;
//          points_map->push_back(p);
//      //const int ii = (p(0) - whole_min_x_) / resolution_;
//      //const int jj = (p(1) - whole_min_y_) / resolution_;
//      //whole_lidar_.at<uchar>(ii, jj) = 255;
//        }
//      } else {
//        float slope = (vec[3] - vec[1]) * 1.0 / (vec[2] - vec[0]);
//        float start_x = 0, end_x = 0, start_y = 0;
//        if (vec[0] < vec[2]) {
//          start_x = vec[0];
//          end_x = vec[2];
//          start_y = vec[1];
//        } else {
//          start_x = vec[2];
//          end_x = vec[0];
//          start_y = vec[3];
//        }
//        for (int i = start_x; i <= end_x; i++) {
//          const double j = slope * (i - start_x) + start_y;
//          const double x = i * resolution_ - max_distance_;
//          const double y = j * resolution_ - max_distance_;
//          Eigen::Vector4d p(x, y, -1.9, 1);
//          p = lidar_world * p;
//          points_map->push_back(p);
//      //const int ii = (p(0) - whole_min_x_) / resolution_;
//      //const int jj = (p(1) - whole_min_y_) / resolution_;
//      //whole_lidar_.at<uchar>(ii, jj) = 255;
//        }
//      }
//      //cv::imshow("whole", whole_lidar_);
//      //cv::waitKey(0);
//    }
//    time_pngpixel.stop();
//    //std::cout << time_pngpixel.toString() << std::endl;
//  }
//
//  void ValidIdx(const Mat& in_mat, const int i, const int j, const int rows,
//                const int cols, const std::unordered_map<int, int> current,
//                const int dist, std::unordered_map<int, int>* next) {
//    if(i<0 || j < 0 || i >= rows || j>=cols || in_mat.at<uchar>(i,j)) {
//      return;
//    }
//    const int idx = i * cols + j;
//    if(current.find(idx) == current.end()){
//      (*next)[idx] = dist;
//    }
//  }
//  void CreateDistanceMask(const Mat& in_mat, Mat* out_mat) {
//    if (in_mat.rows != out_mat->rows || in_mat.cols != out_mat->cols) {
//      std::cout << " CreateDistanceMask Error " << std::endl;
//      return;
//    }
//
//    pcl::PointCloud<pcl::PointXYZ>::Ptr label_xyz(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//
//    for (int i = 0; i < in_mat.rows; i++) {
//      for (int j = 0; j < in_mat.cols; j++) {
//        if (in_mat.at<uchar>(i, j) == 255) {
//          label_xyz->points.emplace_back(i, j, 0);
//        }
//      }
//    }
//    const int K=1;
//    kdtree.setInputCloud(label_xyz);
//    std::vector<int> pointIdxNKNSearch(K);
//    std::vector<float> pointNKNSquaredDistance(K);
//
//    for (int i = 0; i < in_mat.rows; i++) {
//      for (int j = 0; j < in_mat.cols; j++) {
//        kdtree.nearestKSearch(pcl::PointXYZ(i, j, 0), K, pointIdxNKNSearch,
//                              pointNKNSquaredDistance);
//        double dist = std::accumulate(pointNKNSquaredDistance.begin(),
//                                      pointNKNSquaredDistance.end(), 0.0);
//        dist = std::sqrt(dist / K);
//        //dist = 10 * 255 * dist /
//        //       (in_mat.rows * in_mat.rows + in_mat.cols * in_mat.cols);
//        dist = dist > 255 ? 255 : dist;
//        ////dist = 2 * 255 * std::sqrt(dist) / std::max(in_mat.rows, in_mat.cols) /1.4142;
//        //std::cout << dist << std::endl;
//        (*out_mat).at<uchar>(i,j) = static_cast<uchar>(dist);
//      }
//    }
//    //even slower !!
////    if (in_mat.rows != out_mat->rows || in_mat.cols != out_mat->cols) {
////      std::cout << " CreateDistanceMask Error " << std::endl;
////      return;
////    }
////    std::unordered_map<int, int> current;
////    for (int i = 0; i < in_mat.rows; i++) {
////      for (int j = 0; j < in_mat.cols; j++) {
////        if (in_mat.at<uchar>(i, j) == 255) {
////          (*out_mat).at<uchar>(i, j) = 1;
////          current[i * in_mat.cols + j] = 1;
////        }
////      }
////    }
////
////    std::unordered_map<int, int> next;
////    for(const auto & it:current){
////      const int i = it.first / in_mat.cols;
////      const int j = it.first % in_mat.cols;
////      ValidIdx(*out_mat, i, j - 1, in_mat.rows, in_mat.cols, current,
////               it.second + 1, &next);
////      ValidIdx(*out_mat, i, j + 1, in_mat.rows, in_mat.cols, current,
////               it.second + 1, &next);
////      ValidIdx(*out_mat, i - 1, j, in_mat.rows, in_mat.cols, current,
////               it.second + 1, &next);
////      ValidIdx(*out_mat, i + 1, j, in_mat.rows, in_mat.cols, current,
////               it.second + 1, &next);
////    }
////
////    const int total_size = in_mat.rows * in_mat.cols;
////    int cnt = 0;
////    while(!next.empty()){
////      cnt +=current.size();
////      std::cout << cnt << " / " << total_size << " = " << cnt * 1.0 / total_size << std::endl;
////      std::cout << current.size() << " " << next.size() << std::endl;
////      current = next;
////      next.clear();
////      for (const auto& it : current) {
////        const int i = it.first / in_mat.cols;
////        const int j = it.first % in_mat.cols;
////        ValidIdx(*out_mat, i, j - 1, in_mat.rows, in_mat.cols, current,
////                 it.second + 1, &next);
////        ValidIdx(*out_mat, i, j + 1, in_mat.rows, in_mat.cols, current,
////                 it.second + 1, &next);
////        ValidIdx(*out_mat, i - 1, j, in_mat.rows, in_mat.cols, current,
////                 it.second + 1, &next);
////        ValidIdx(*out_mat, i + 1, j, in_mat.rows, in_mat.cols, current,
////                 it.second + 1, &next);
////      }
////    }
//    //// very stupid solution; needs to be improved;
//
//    //for (int i = 0; i < in_mat.rows; i++) {
//    //  std::cout << i << " / " << in_mat.rows << std::endl;
//    //  for (int j = 0; j < in_mat.cols; j++) {
//    //    bool setFlag = false;
//    //    int radius = 0;
//    //    double minV = std::max(in_mat.rows, in_mat.cols);
//    //    while (!setFlag) {
//    //      for (int ii = i - radius; ii <= i + radius; ii++) {
//    //        for (int jj = j - radius; jj <= j + radius; jj++) {
//    //          if (ii < 0 || jj < 0 || ii >= in_mat.rows || jj >= in_mat.cols)
//    //            continue;
//    //          if (in_mat.at<uchar>(ii, jj) == 255) {
//    //            int dist = (ii - i) * (ii - i) + (jj - j) * (jj - j);
//    //            if (dist == 0) {
//    //              minV = 0;
//    //              setFlag = true;
//    //            } else {
//    //              minV = std::min(minV, std::sqrt(dist));
//    //              setFlag = true;
//    //            }
//    //          }
//    //        }
//    //      }
//    //      if (setFlag) {
//    //        (*out_mat).at<uchar>(i, j) = static_cast<uchar>(minV);
//    //      } else {
//    //        radius++;
//    //      }
//    //    }
//    //  }
//    //}
//  }
//
//  void OptimizePngInitial(const double car_x, const double car_y,
//                          const vector<Eigen::Vector4d>& points_map) {
//    for (const auto& p : points_map) {
//      //std::cout << p(0) << " " << p(1) << std::endl;
//      const int i = (p(0) - whole_min_x_) / resolution_;
//      const int j = (p(1) - whole_min_y_) / resolution_;
//      if (i < 0 || j < 0 || i > whole_lidar_.rows || j >= whole_img_.cols) {
//        std::cout << "out of max range" << std::endl;
//        std::cout << p(0) << " " << p(1) << std::endl;
//      } else {
//        whole_lidar_.at<uchar>(i, j) = 255;
//        icp_map_->points.emplace_back(p(0), p(1), 0);
//      }
//    }
//    std::cout << "Initializing whole_lidar image "  << std::endl;
//    //cv::namedWindow("whole", cv::WINDOW_NORMAL);
//    //cv::resizeWindow("whole", 1920, 1080);
//    //cv::imshow("whole", whole_lidar_);
//    //cv::waitKey(0);
//
//    const int min_i = (car_x - max_distance_ - whole_min_x_) / resolution_;
//    const int min_j = (car_y - max_distance_ - whole_min_y_) / resolution_;
//    const int max_i = (car_x + max_distance_ - whole_min_x_) / resolution_;
//    const int max_j = (car_y + max_distance_ - whole_min_y_) / resolution_;
//
//    for (int i = min_i; i < max_i; i++) {
//      for (int j = min_j; j < max_j; j++) {
//        const double x = i * resolution_ + whole_min_x_ - car_x;
//        const double y = j * resolution_ + whole_min_y_ - car_y;
//        if (x * x + y * y < max_distance_ * max_distance_) {
//          whole_lidar_range_.at<uchar>(i, j) = 255;
//        }
//      }
//    }
//  }
//
////  void FindOverlap(const double car_x, const double car_y, const Mat& in_lidar,
////                   Mat* crop, Mat* mask) {
////    // both whole_lidar_ and in_lidar are in map frame
////    // overlap constrains:  car position, map cover range
////
////    const int crop_center_i = (car_x - whole_min_x_) / resolution_;
////    const int crop_center_j = (car_x - whole_min_x_) / resolution_;
////
////    
////
////    // find overlap world map
////    int crop_min_i = whole_lidar_.rows;
////    int crop_min_j = whole_lidar_.cols;
////    int crop_max_i = -1;
////    int crop_max_j = -1;
////
////    for (int i = 0; i < whole_lidar_.rows; i++) {
////      for (int j = 0; i < whole_lidar_.cols; j++) {
////        if (whole_lidar_range_.at<uchar>(i, j)) {
////          const int x = i * resolution_ + whole_min_x_ - car_x;
////          const int y = j * resolution_ + whole_min_y_ - car_y;
////          if (x * x + y * y < max_distance_ * max_distance_ * 1.44) {
////            crop_min_i = std::min(crop_min_i, i);
////            crop_min_j = std::min(crop_min_j, j);
////            crop_max_i = std::max(crop_max_i, i);
////            crop_max_j = std::max(crop_max_j, j);
////          }
////        }
////      }
////    }
////
////    Mat crop(whole_lidar_,
////             cv::Rect(crop_min_i, crop_min_j, crop_max_i - crop_min_i,
////                      crop_max_j - crop_min_j));
////
////    // find overlap matching mask
////    vector<Eigen::Vector4d> point_mask;
////    for (const auto& p : points_map) {
////      const int i = (p(0) - whole_min_x_) / resolution_;
////      const int j = (p(1) - whole_min_y_) / resolution_;
////      if (i < 0 || j < 0 || i > whole_lidar_.rows || j >= whole_img_.cols) {
////        std::cout << "out of max range" << std::endl;
////        std::cout << p[0] << " " << p[1] << std::endl;
////      } else if (whole_lidar_range_.at<uchar>(i, j)) {
////        point_mask.push_back(p);
////      }
////    }
////
////    double min_x = 100000, min_y = 1000000, max_x = -10000000, max_y = -1000000;
////    for (const auto& p : point_mask) {
////      min_x = std::min(min_x, p(0));
////      min_y = std::min(min_y, p(1));
////      max_x = std::max(max_x, p(0));
////      max_y = std::max(max_y, p(1));
////    }
////    min_x = min_x - max_distance_;
////    min_y = min_y - max_distance_;
////    const size_t dim_x = (max_x + max_distance_ - min_x) / resolution_ + 1;
////    const size_t dim_y = (max_y + max_distance_ - min_y) / resolution_ + 1;
////    Mat mask = Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
////    Mat mask_dist_map = Mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
////    // loop to minimize hausdorff distance
////    for (const auto& p : point_mask) {
////      const int i = (p(0) - min_x) / resolution_;
////      const int j = (p(1) - min_y) / resolution_;
////      mask.at<uchar>(i, j) = 255;
////    }
////
////    Mat combine(std::max(crop.size().height, mask.size().height),
////                crop.size().width + mask.size().width, CV_8UC1);
////    Mat left_roi(combine,
////                 cv::Rect(0, 0, mask.size().width, mask.size().height));
////    mask.copyTo(left_roi);
////    Mat right_roi(combine, cv::Rect(mask.size().width, 0, crop.size().width,
////                                    crop.size().height));
////    crop.copyTo(right_roi);
////
////    cv::namedWindow("image", cv::WINDOW_NORMAL);
////    cv::resizeWindow("image", 1920, 1080);
////    cv::imshow("image", combine);
////    cv::waitKey();
////  }
//
//  void OptimizePngFollowing(const double car_x, const double car_y,
//                            const vector<Eigen::Vector4d>& points_map,
//                            double* cost, Eigen::Matrix4d* adjust) {
//    pcl::PointCloud<pcl::PointXYZ>::Ptr icp_input(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    
//    //for(int i=0; i<whole_lidar_.rows; i++){
//    //  for(int j=0; j<whole_lidar_.cols; j++){
//    //    if(whole_lidar_.at<uchar>(i,j)==255){
//    //      const int x = i * resolution_ + whole_min_x_;
//    //      const int y = j * resolution_ + whole_min_y_;
//    //      icp_map->points.emplace_back(x,y,0);
//    //    }
//    //  }
//    //}
//
//
//    for(const auto& vec:points_map){
//      icp_input->points.emplace_back(vec(0), vec(1), 0);
//    }
//
//    pcl::PointCloud<pcl::PointXYZ>::Ptr map(new pcl::PointCloud<pcl::PointXYZ>);
//    for(const auto & p:icp_map_->points){
//      const double dist =
//          (p.x - car_x) * (p.x - car_x) + (p.y - car_y) * (p.y - car_y);
//      if(dist < 4*max_distance_ * max_distance_){
//        map->push_back(p);
//      }
//    }
//
//    //pcl::PointCloud<pcl::PointXYZ>::Ptr f_map(
//    //    new pcl::PointCloud<pcl::PointXYZ>);
//    //pcl::PointCloud<pcl::PointXYZ>::Ptr f_icp_input(
//    //    new pcl::PointCloud<pcl::PointXYZ>);
//
//    //pcl::VoxelGrid<pcl::PointXYZ> voxel_grid_filter;
//    //voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//    //voxel_grid_filter.setInputCloud(map);
//    //voxel_grid_filter.filter(*f_map);
//
//    //voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//    //voxel_grid_filter.setInputCloud(icp_input);
//    //voxel_grid_filter.filter(*f_icp_input);
//
//    //icp_.setInputTarget(f_map);
//    //icp_.setInputSource(f_icp_input);
//    icp_.setInputTarget(map);
//    icp_.setInputSource(icp_input);
//
//    pcl::PointCloud<pcl::PointXYZ>::Ptr out(new pcl::PointCloud<pcl::PointXYZ>);
//
//
//    icp_.align(out, Eigen::Matrix4f::Identity(), 0);
//    //icp_.align(out, Eigen::Matrix4f::Identity(), 2);
//
//    *adjust = icp_.finalTransMatrix().cast<double>();
//
//    *cost = icp_.cost();
//
//     pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
//        new pcl::PointCloud<pcl::PointXYZRGB>);
//     for (const auto& pt : map->points) {
//      pcl::PointXYZRGB cpt;
//      cpt.x = pt.x;
//      cpt.y = pt.y;
//      cpt.z = pt.z;
//      cpt.r = 255;
//      cpt.g = 0;
//      cpt.b = 0;
//      rgb_cloud->push_back(cpt);
//    }
//     pcd_viewer_->addPointCloud(rgb_cloud, "1", 0);
//     pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud2(
//        new pcl::PointCloud<pcl::PointXYZRGB>);
//     for (const auto& pt : icp_input->points) {
//      pcl::PointXYZRGB cpt;
//      cpt.x = pt.x;
//      cpt.y = pt.y;
//      cpt.z = pt.z;
//      cpt.r = 0;
//      cpt.g = 255;
//      cpt.b = 0;
//      rgb_cloud2->push_back(cpt);
//    }
//     pcd_viewer_->addPointCloud(rgb_cloud2, "2", 0);
//     pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud3(
//        new pcl::PointCloud<pcl::PointXYZRGB>);
//     for (const auto& pt : out->points) {
//      pcl::PointXYZRGB cpt;
//      cpt.x = pt.x;
//      cpt.y = pt.y;
//      cpt.z = pt.z;
//      cpt.r = 0;
//      cpt.g = 0;
//      cpt.b = 255;
//      rgb_cloud3->push_back(cpt);
//    }
//     pcd_viewer_->addPointCloud(rgb_cloud3, "3", 0);
//    //pcd_viewer_->addPointCloud(icp_map);
//    //pcd_viewer_->addPointCloud(icp_input);
//    //pcd_viewer_->addPointCloud(out);
//     //pcd_viewer_->spin();
//     pcd_viewer_->removePointCloud("1", 0);
//     pcd_viewer_->removePointCloud("2", 0);
//     pcd_viewer_->removePointCloud("3", 0);
//     //    zq_tools::timer_helper::Timer time_optimize;
//     //    time_optimize.name("OptimizePngFollowing");
//     //    time_optimize.start();
//     //    double min_x = 100000000, min_y = 100000000,
//     //           max_x = -10000000, max_y = -10000000;
//     //
//     //    for (const auto& vec : points_map) {
//     //      min_x = std::min(min_x, vec[0]);
//     //      min_y = std::min(min_y, vec[1]);
//     //      max_x = std::max(max_x, vec[0]);
//     //      max_y = std::max(max_y, vec[1]);
//     //    }
//     //
//     //    //std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
//     //    << std::endl;
//     //    const int dim_x = (max_x - min_x) / resolution_ + 1;
//     //    const int dim_y = (max_y - min_y) / resolution_ + 1;
//     //    Mat in_mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//     //    for (const auto& vec : points_map) {
//     //      const int i = (vec[0] - min_x) / resolution_;
//     //      const int j = (vec[1] - min_y) / resolution_;
//     //      in_mat.at<uchar>(i, j) = 255;
//     //    }
//     //    //cv::namedWindow("in_mat", cv::WINDOW_NORMAL);
//     //    //cv::resizeWindow("in_mat", 1920, 1080);
//     //    //cv::imshow("in_mat", in_mat);
//     //    //cv::waitKey(0);
//     //    //cv::destroyWindow("in_mat");
//     //    ////in_mat.release();
//     //
//     //    //// 1. compute overlap
//     //    ////FindOverlap(car_x, car_y, in_mat, &crop, &mask);
//     //
//     //    //Mat overlap_crop(whole_lidar_.rows, whole_lidar_.cols, CV_8UC1,
//     //    //                 Scalar::all(0));
//     //    //Mat overlap_mask(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//     //
//     //    int min_i = (car_x - max_distance_ - min_x) / resolution_;
//     //    int min_j = (car_y - max_distance_ - min_y) / resolution_;
//     //    int max_i = (car_x + max_distance_ - min_x) / resolution_;
//     //    int max_j = (car_y + max_distance_ - min_y) / resolution_;
//     //    min_i = std::max(min_i, 0);
//     //    min_j = std::max(min_j, 0);
//     //    max_i = std::min(max_i, dim_x);
//     //    max_j = std::min(max_j, dim_y);
//     //
//     //    int crop_min_i = max_i, crop_min_j = max_j, crop_max_i = min_i,
//     //        crop_max_j = min_j;
//     //    for(int i=min_i; i<max_i; i++){
//     //      for(int j=min_j; j<max_j; j++){
//     //        const int wi = i + (min_x - whole_min_x_) / resolution_;
//     //        const int wj = j + (min_y - whole_min_y_) / resolution_;
//     //        if(whole_lidar_range_.at<uchar>(wi,wj)){
//     //          // overlap found
//     //          //overlap_crop.at<uchar>(wi,wj) = 1;
//     //          //overlap_mask.at<uchar>(i,j) = 1;
//     //          crop_min_i = std::min(crop_min_i, i);
//     //          crop_min_j = std::min(crop_min_j, j);
//     //          crop_max_i = std::max(crop_max_i, i);
//     //          crop_max_j = std::max(crop_max_j, j);
//     //        }
//     //      }
//     //    }
//     //    //std::cout << crop_min_i << " " << crop_min_j << " " << crop_max_i
//     //    <<
//     //    " " << crop_max_j << std::endl;
//     //    const int crop_dim_i = crop_max_i - crop_min_i + 1;
//     //    const int crop_dim_j = crop_max_j - crop_min_j + 1;
//     //    //Mat crop(crop_dim_i, crop_dim_j, CV_8UC1, Scalar::all(0));
//     //    //Mat mask(crop_dim_i, crop_dim_j, CV_8UC1, Scalar::all(0));
//     //    cv::Rect roi_mask =
//     //        cv::Rect(crop_min_j, crop_min_i, crop_dim_j, crop_dim_i);
//     //    Mat mask = in_mat(roi_mask);
//     //    //cv::imshow("mask", mask);
//     //    //cv::waitKey(0);
//     //
//     //    const int roi_x = crop_min_i + (min_x - whole_min_x_) / resolution_;
//     //    const int roi_y = crop_min_j + (min_y - whole_min_y_) / resolution_;
//     //    //std::cout << roi_x << " " << roi_y << " " << crop_max_i << " " <<
//     //    crop_max_j << std::endl;
//     //    cv::Rect roi_whole = cv::Rect(roi_y, roi_x, crop_dim_j, crop_dim_i);
//     //    Mat crop = whole_lidar_(roi_whole);
//     //    //cv::imshow("crop", crop);
//     //    //cv::waitKey(0);
//     //
//     //
//     //
//     //    //Mat combine(std::max(crop.size().height, mask.size().height),
//     //    //            crop.size().width + mask.size().width, CV_8UC1);
//     //    //Mat left_roi(combine,
//     //    //             cv::Rect(0, 0, mask.size().width,
//     //    mask.size().height));
//     //    //mask.copyTo(left_roi);
//     //    //Mat right_roi(combine, cv::Rect(mask.size().width, 0,
//     //    crop.size().width,
//     //    //                                crop.size().height));
//     //    //crop.copyTo(right_roi);
//     //
//     //    //cv::namedWindow("image", cv::WINDOW_NORMAL);
//     //    //cv::resizeWindow("image", 960, 540);
//     //    //cv::imshow("image", combine);
//     //    //cv::waitKey();
//     //
//     //    // ICP compute cost and tra
//     //
//     //    // overlap check will make sure crop and mask have same size
//     //    *cost = 0;
//     //    for(int i=0; i<mask_dist_map.rows; i++){
//     //      for(int j=0; j<mask_dist_map.cols; j++){
//     //        *cost += static_cast<double>(mask_dist_map.at<uchar>(i, j)) /
//     //        255.0 +
//     //                 static_cast<double>(crop.at<uchar>(i, j)) / 255.0;
//     //      }
//     //    }
//     //    time_optimize.stop();
//     //    std::cout << time_optimize.toString() << std::endl;
//  }
//  void AdjustEigenMatrix(const double x, const double y, const double z,
//                         const double roll, const double pitch,
//                         const double yaw, Eigen::Matrix4d* out0) {
//    zq_tools::timer_helper::Timer time_adjust;
//    time_adjust.name("time_adjust");
//    time_adjust.start();
//    auto& out = *out0;
//
//    tf::Matrix3x3 tf_mat;
//    double base_r, base_p, base_y;
//    tf_mat.setValue(out(0, 0), out(0, 1), out(0, 2), out(1, 0), out(1, 1),
//                    out(1, 2), out(2, 0), out(2, 1), out(2, 2));
//    tf_mat.getRPY(base_r, base_p, base_y);
//
//    Eigen::AngleAxisd rotation_x(base_r + roll, Eigen::Vector3d::UnitX());
//    Eigen::AngleAxisd rotation_y(base_p + pitch, Eigen::Vector3d::UnitY());
//    Eigen::AngleAxisd rotation_z(base_y + yaw, Eigen::Vector3d::UnitZ());
//
//    Eigen::Translation3d translation(out(0, 3) + x, out(1, 3) + y,
//                                     out(2, 3) + z);
//
//    *out0 = (translation * rotation_z * rotation_y * rotation_x).matrix();
//    time_adjust.stop();
//    //std::cout << time_adjust.toString() << std::endl;
//  }
//  void AddPointToWhole(const double car_x, const double car_y,
//                       const vector<Eigen::Vector4d>& points_map) {
//
//    //whole_lidar_ = cv::Scalar(0);
//
//
//    for (const auto& p : points_map) {
//      //std::cout << p(0) << " " << p(1) << std::endl;
//      const int i = (p(0) - whole_min_x_) / resolution_;
//      const int j = (p(1) - whole_min_y_) / resolution_;
//      if (i < 0 || j < 0 || i > whole_lidar_.rows || j >= whole_img_.cols) {
//        std::cout << "out of max range" << std::endl;
//        std::cout << p(0) << " " << p(1) << std::endl;
//      } else {
//        whole_lidar_.at<uchar>(i, j) = 255;
//      }
//      icp_map_->points.emplace_back(p(0), p(1), 0);
//    }
//    const int min_i = (car_x - max_distance_ - whole_min_x_) / resolution_;
//    const int min_j = (car_y - max_distance_ - whole_min_y_) / resolution_;
//    const int max_i = (car_x + max_distance_ - whole_min_x_) / resolution_;
//    const int max_j = (car_y + max_distance_ - whole_min_y_) / resolution_;
//
//    for (int i = min_i; i < max_i; i++) {
//      for (int j = min_j; j < max_j; j++) {
//        const double x = i * resolution_ + whole_min_x_ - car_x;
//        const double y = j * resolution_ + whole_min_y_ - car_y;
//        if (x * x + y * y < max_distance_ * max_distance_) {
//          whole_lidar_range_.at<uchar>(i, j) = 255;
//        }
//      }
//    }
//  }
//  void OptimizePng(const vector<vector<vector<double>>>& png_labels,
//                   const vector<Eigen::Matrix4d>& car_pose_matrices,
//                   vector<Eigen::Matrix4d>* adjust_pose_matrices) {
//    // TODO Split whole set to into groups will speed up convergence?
//    const size_t N = png_labels.size();
//    std::cout << N << " images To be Optimized in PNG level" << std::endl;
//    //if (N != car_pose_matrices.size()) {
//    //  std::cout << "Incorrect number of png labels and poses" << std::endl;
//    //  return;
//    //}
//    //if(adjust_poses->empty()){
//    //  adust_poses->resize(N);
//    //}
//
//    //First try just optimize hausdorff distance
//    
//    vector<Eigen::Vector4d> points_map;
//    ///------ optimize cost loop ----- 
//    // apply adjust to first png
//    
//    std::cout << "Initializing " << std::endl;
//    std::vector<double> all_cost;
//    Eigen::Matrix4d initial_adjust = Eigen::Matrix4d::Identity();
//    double car_x, car_y;
//    cv::namedWindow("whole", cv::WINDOW_NORMAL);
//    cv::resizeWindow("whole", 1920, 1080);
//    PngLabel2PixelMap(png_labels[0], car_pose_matrices[0], initial_adjust,
//                      &points_map, &car_x, &car_y);
//    OptimizePngInitial(car_x, car_y, points_map);
//    all_cost.push_back(0);
//    adjust_pose_matrices->push_back(Eigen::Matrix4d::Identity());
//
//    cv::imshow("whole", whole_lidar_);
//    cv::waitKey(0);
//    for(size_t i=1; i<N; i++){
//      std::cout << "---- Processing:  " << i << "----- " << std::endl;
//      std::cout << file_names_[i] << std::endl;
//      Eigen::Matrix4d adjust = Eigen::Matrix4d::Identity();
//      double cost = 0;
//      PngLabel2PixelMap(png_labels[i], car_pose_matrices[i], adjust,
//                        &points_map, &car_x, &car_y);
//      OptimizePngFollowing(car_x, car_y, points_map, &cost, &adjust);
//
//      PngLabel2PixelMap(png_labels[i], car_pose_matrices[i], adjust,
//                        &points_map, &car_x, &car_y);
//      AddPointToWhole(car_x, car_y, points_map);
//      cv::imshow("whole", whole_lidar_);
//      //cv::imwrite("whole_" + std::to_string(i) + ".png", whole_lidar_);
//      cv::waitKey(0);
//      ////cv::imshow("whole", whole_lidar_range_);
//      ////cv::waitKey(0);
//      all_cost.push_back(cost);
//      adjust_pose_matrices->push_back(adjust);
//      std::cout << "adjust" << std::endl;
//      EigenMatrix2Pose(adjust);
//      std::cout << cost << std::endl;
//    }
//    cv::imshow("whole", whole_lidar_);
//    cv::waitKey();
//  }
//
//
//
//
//  //void OptimizePngFollowing(const double car_x, const double car_y,
//  //                          const vector<Eigen::Vector4d>& points_map,
//  //                          double* cost, Eigen::Matrix4d* adjust) {
//  //  pcl::PointCloud<pcl::PointXYZ>::Ptr icp_input(
//  //      new pcl::PointCloud<pcl::PointXYZ>);
//  //  
//  //  //for(int i=0; i<whole_lidar_.rows; i++){
//  //  //  for(int j=0; j<whole_lidar_.cols; j++){
//  //  //    if(whole_lidar_.at<uchar>(i,j)==255){
//  //  //      const int x = i * resolution_ + whole_min_x_;
//  //  //      const int y = j * resolution_ + whole_min_y_;
//  //  //      icp_map->points.emplace_back(x,y,0);
//  //  //    }
//  //  //  }
//  //  //}
//
//
//  //  for(const auto& vec:points_map){
//  //    icp_input->points.emplace_back(vec(0), vec(1), 0);
//  //  }
//
//  //  pcl::PointCloud<pcl::PointXYZ>::Ptr map(new pcl::PointCloud<pcl::PointXYZ>);
//  //  for(const auto & p:icp_map_->points){
//  //    const double dist =
//  //        (p.x - car_x) * (p.x - car_x) + (p.y - car_y) * (p.y - car_y);
//  //    if(dist < 4*max_distance_ * max_distance_){
//  //      map->push_back(p);
//  //    }
//  //  }
//
//  //  //pcl::PointCloud<pcl::PointXYZ>::Ptr f_map(
//  //  //    new pcl::PointCloud<pcl::PointXYZ>);
//  //  //pcl::PointCloud<pcl::PointXYZ>::Ptr f_icp_input(
//  //  //    new pcl::PointCloud<pcl::PointXYZ>);
//
//  //  //pcl::VoxelGrid<pcl::PointXYZ> voxel_grid_filter;
//  //  //voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//  //  //voxel_grid_filter.setInputCloud(map);
//  //  //voxel_grid_filter.filter(*f_map);
//
//  //  //voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//  //  //voxel_grid_filter.setInputCloud(icp_input);
//  //  //voxel_grid_filter.filter(*f_icp_input);
//
//  //  //icp_.setInputTarget(f_map);
//  //  //icp_.setInputSource(f_icp_input);
//  //  icp_.setInputTarget(map);
//  //  icp_.setInputSource(icp_input);
//
//  //  pcl::PointCloud<pcl::PointXYZ>::Ptr out(new pcl::PointCloud<pcl::PointXYZ>);
//
//
//  //  icp_.align(out, Eigen::Matrix4f::Identity(), 0);
//  //  //icp_.align(out, Eigen::Matrix4f::Identity(), 2);
//
//  //  *adjust = icp_.finalTransMatrix().cast<double>();
//
//  //  *cost = icp_.cost();
//
//  //   pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
//  //      new pcl::PointCloud<pcl::PointXYZRGB>);
//  //   for (const auto& pt : map->points) {
//  //    pcl::PointXYZRGB cpt;
//  //    cpt.x = pt.x;
//  //    cpt.y = pt.y;
//  //    cpt.z = pt.z;
//  //    cpt.r = 255;
//  //    cpt.g = 0;
//  //    cpt.b = 0;
//  //    rgb_cloud->push_back(cpt);
//  //  }
//  //   pcd_viewer_->addPointCloud(rgb_cloud, "1", 0);
//  //   pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud2(
//  //      new pcl::PointCloud<pcl::PointXYZRGB>);
//  //   for (const auto& pt : icp_input->points) {
//  //    pcl::PointXYZRGB cpt;
//  //    cpt.x = pt.x;
//  //    cpt.y = pt.y;
//  //    cpt.z = pt.z;
//  //    cpt.r = 0;
//  //    cpt.g = 255;
//  //    cpt.b = 0;
//  //    rgb_cloud2->push_back(cpt);
//  //  }
//  //   pcd_viewer_->addPointCloud(rgb_cloud2, "2", 0);
//  //   pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud3(
//  //      new pcl::PointCloud<pcl::PointXYZRGB>);
//  //   for (const auto& pt : out->points) {
//  //    pcl::PointXYZRGB cpt;
//  //    cpt.x = pt.x;
//  //    cpt.y = pt.y;
//  //    cpt.z = pt.z;
//  //    cpt.r = 0;
//  //    cpt.g = 0;
//  //    cpt.b = 255;
//  //    rgb_cloud3->push_back(cpt);
//  //  }
//  //   pcd_viewer_->addPointCloud(rgb_cloud3, "3", 0);
//  //  //pcd_viewer_->addPointCloud(icp_map);
//  //  //pcd_viewer_->addPointCloud(icp_input);
//  //  //pcd_viewer_->addPointCloud(out);
//  //   //pcd_viewer_->spin();
//  //   pcd_viewer_->removePointCloud("1", 0);
//  //   pcd_viewer_->removePointCloud("2", 0);
//  //   pcd_viewer_->removePointCloud("3", 0);
//  //   //    zq_tools::timer_helper::Timer time_optimize;
//  //   //    time_optimize.name("OptimizePngFollowing");
//  //   //    time_optimize.start();
//  //   //    double min_x = 100000000, min_y = 100000000,
//  //   //           max_x = -10000000, max_y = -10000000;
//  //   //
//  //   //    for (const auto& vec : points_map) {
//  //   //      min_x = std::min(min_x, vec[0]);
//  //   //      min_y = std::min(min_y, vec[1]);
//  //   //      max_x = std::max(max_x, vec[0]);
//  //   //      max_y = std::max(max_y, vec[1]);
//  //   //    }
//  //   //
//  //   //    //std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
//  //   //    << std::endl;
//  //   //    const int dim_x = (max_x - min_x) / resolution_ + 1;
//  //   //    const int dim_y = (max_y - min_y) / resolution_ + 1;
//  //   //    Mat in_mat(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//  //   //    for (const auto& vec : points_map) {
//  //   //      const int i = (vec[0] - min_x) / resolution_;
//  //   //      const int j = (vec[1] - min_y) / resolution_;
//  //   //      in_mat.at<uchar>(i, j) = 255;
//  //   //    }
//  //   //    //cv::namedWindow("in_mat", cv::WINDOW_NORMAL);
//  //   //    //cv::resizeWindow("in_mat", 1920, 1080);
//  //   //    //cv::imshow("in_mat", in_mat);
//  //   //    //cv::waitKey(0);
//  //   //    //cv::destroyWindow("in_mat");
//  //   //    ////in_mat.release();
//  //   //
//  //   //    //// 1. compute overlap
//  //   //    ////FindOverlap(car_x, car_y, in_mat, &crop, &mask);
//  //   //
//  //   //    //Mat overlap_crop(whole_lidar_.rows, whole_lidar_.cols, CV_8UC1,
//  //   //    //                 Scalar::all(0));
//  //   //    //Mat overlap_mask(dim_x, dim_y, CV_8UC1, Scalar::all(0));
//  //   //
//  //   //    int min_i = (car_x - max_distance_ - min_x) / resolution_;
//  //   //    int min_j = (car_y - max_distance_ - min_y) / resolution_;
//  //   //    int max_i = (car_x + max_distance_ - min_x) / resolution_;
//  //   //    int max_j = (car_y + max_distance_ - min_y) / resolution_;
//  //   //    min_i = std::max(min_i, 0);
//  //   //    min_j = std::max(min_j, 0);
//  //   //    max_i = std::min(max_i, dim_x);
//  //   //    max_j = std::min(max_j, dim_y);
//  //   //
//  //   //    int crop_min_i = max_i, crop_min_j = max_j, crop_max_i = min_i,
//  //   //        crop_max_j = min_j;
//  //   //    for(int i=min_i; i<max_i; i++){
//  //   //      for(int j=min_j; j<max_j; j++){
//  //   //        const int wi = i + (min_x - whole_min_x_) / resolution_;
//  //   //        const int wj = j + (min_y - whole_min_y_) / resolution_;
//  //   //        if(whole_lidar_range_.at<uchar>(wi,wj)){
//  //   //          // overlap found
//  //   //          //overlap_crop.at<uchar>(wi,wj) = 1;
//  //   //          //overlap_mask.at<uchar>(i,j) = 1;
//  //   //          crop_min_i = std::min(crop_min_i, i);
//  //   //          crop_min_j = std::min(crop_min_j, j);
//  //   //          crop_max_i = std::max(crop_max_i, i);
//  //   //          crop_max_j = std::max(crop_max_j, j);
//  //   //        }
//  //   //      }
//  //   //    }
//  //   //    //std::cout << crop_min_i << " " << crop_min_j << " " << crop_max_i
//  //   //    <<
//  //   //    " " << crop_max_j << std::endl;
//  //   //    const int crop_dim_i = crop_max_i - crop_min_i + 1;
//  //   //    const int crop_dim_j = crop_max_j - crop_min_j + 1;
//  //   //    //Mat crop(crop_dim_i, crop_dim_j, CV_8UC1, Scalar::all(0));
//  //   //    //Mat mask(crop_dim_i, crop_dim_j, CV_8UC1, Scalar::all(0));
//  //   //    cv::Rect roi_mask =
//  //   //        cv::Rect(crop_min_j, crop_min_i, crop_dim_j, crop_dim_i);
//  //   //    Mat mask = in_mat(roi_mask);
//  //   //    //cv::imshow("mask", mask);
//  //   //    //cv::waitKey(0);
//  //   //
//  //   //    const int roi_x = crop_min_i + (min_x - whole_min_x_) / resolution_;
//  //   //    const int roi_y = crop_min_j + (min_y - whole_min_y_) / resolution_;
//  //   //    //std::cout << roi_x << " " << roi_y << " " << crop_max_i << " " <<
//  //   //    crop_max_j << std::endl;
//  //   //    cv::Rect roi_whole = cv::Rect(roi_y, roi_x, crop_dim_j, crop_dim_i);
//  //   //    Mat crop = whole_lidar_(roi_whole);
//  //   //    //cv::imshow("crop", crop);
//  //   //    //cv::waitKey(0);
//  //   //
//  //   //
//  //   //
//  //   //    //Mat combine(std::max(crop.size().height, mask.size().height),
//  //   //    //            crop.size().width + mask.size().width, CV_8UC1);
//  //   //    //Mat left_roi(combine,
//  //   //    //             cv::Rect(0, 0, mask.size().width,
//  //   //    mask.size().height));
//  //   //    //mask.copyTo(left_roi);
//  //   //    //Mat right_roi(combine, cv::Rect(mask.size().width, 0,
//  //   //    crop.size().width,
//  //   //    //                                crop.size().height));
//  //   //    //crop.copyTo(right_roi);
//  //   //
//  //   //    //cv::namedWindow("image", cv::WINDOW_NORMAL);
//  //   //    //cv::resizeWindow("image", 960, 540);

//  //   //    //cv::waitKey();
//  //   //
//  //   //    // ICP compute cost and tra
//  //   //
//  //   //    // overlap check will make sure crop and mask have same size
//  //   //    *cost = 0;
//  //   //    for(int i=0; i<mask_dist_map.rows; i++){
//  //   //      for(int j=0; j<mask_dist_map.cols; j++){
//  //   //        *cost += static_cast<double>(mask_dist_map.at<uchar>(i, j)) /
//  //   //        255.0 +
//  //   //                 static_cast<double>(crop.at<uchar>(i, j)) / 255.0;
//  //   //      }
//  //   //    }
//  //   //    time_optimize.stop();
//  //   //    std::cout << time_optimize.toString() << std::endl;
//  //}
//  //void AdjustEigenMatrix(const double x, const double y, const double z,
//  //                       const double roll, const double pitch,
//  //                       const double yaw, Eigen::Matrix4d* out0) {
//  //  zq_tools::timer_helper::Timer time_adjust;
//  //  time_adjust.name("time_adjust");
//  //  time_adjust.start();
//  //  auto& out = *out0;
//
//  //  tf::Matrix3x3 tf_mat;
//  //  double base_r, base_p, base_y;
//  //  tf_mat.setValue(out(0, 0), out(0, 1), out(0, 2), out(1, 0), out(1, 1),
//  //                  out(1, 2), out(2, 0), out(2, 1), out(2, 2));
//  //  tf_mat.getRPY(base_r, base_p, base_y);
//
//  //  Eigen::AngleAxisd rotation_x(base_r + roll, Eigen::Vector3d::UnitX());
//  //  Eigen::AngleAxisd rotation_y(base_p + pitch, Eigen::Vector3d::UnitY());
//  //  Eigen::AngleAxisd rotation_z(base_y + yaw, Eigen::Vector3d::UnitZ());
//
//  //  Eigen::Translation3d translation(out(0, 3) + x, out(1, 3) + y,
//  //                                   out(2, 3) + z);
//
//  //  *out0 = (translation * rotation_z * rotation_y * rotation_x).matrix();
//  //  time_adjust.stop();
//  //  //std::cout << time_adjust.toString() << std::endl;
//  //}
//  //void AddPointToWhole(const double car_x, const double car_y,
//  //                     const vector<Eigen::Vector4d>& points_map) {
//
//  //  //whole_lidar_ = cv::Scalar(0);
//
//
//  //  for (const auto& p : points_map) {
//  //    //std::cout << p(0) << " " << p(1) << std::endl;
//  //    const int i = (p(0) - whole_min_x_) / resolution_;
//  //    const int j = (p(1) - whole_min_y_) / resolution_;
//  //    if (i < 0 || j < 0 || i > whole_lidar_.rows || j >= whole_img_.cols) {
//  //      std::cout << "out of max range" << std::endl;
//  //      std::cout << p(0) << " " << p(1) << std::endl;
//  //    } else {
//  //      whole_lidar_.at<uchar>(i, j) = 255;
//  //    }
//  //    icp_map_->points.emplace_back(p(0), p(1), 0);
//  //  }
//  //  const int min_i = (car_x - max_distance_ - whole_min_x_) / resolution_;
//  //  const int min_j = (car_y - max_distance_ - whole_min_y_) / resolution_;
//  //  const int max_i = (car_x + max_distance_ - whole_min_x_) / resolution_;
//  //  const int max_j = (car_y + max_distance_ - whole_min_y_) / resolution_;
//
//  //  for (int i = min_i; i < max_i; i++) {
//  //    for (int j = min_j; j < max_j; j++) {
//  //      const double x = i * resolution_ + whole_min_x_ - car_x;
//  //      const double y = j * resolution_ + whole_min_y_ - car_y;
//  //      if (x * x + y * y < max_distance_ * max_distance_) {
//  //        whole_lidar_range_.at<uchar>(i, j) = 255;
//  //      }
//  //    }
//  //  }
//  //}
//
//  bool PointOnLine(const vector<double>& p, const vector<double>& two_point,
//                   const double threshold) {
//    if (p.size() != 2 || two_point.size() != 4) {
//      std::cout << " Incorrect data size for PointOnLine" << std::endl;
//      std::cout << p.size() << " " << two_point.size() << std::endl;
//      return false;
//    }
//    double x1 = two_point[0] * resolution_ - max_distance_,
//           y1 = two_point[1] * resolution_ - max_distance_,
//           x2 = two_point[2] * resolution_ - max_distance_,
//           y2 = two_point[3] * resolution_ - max_distance_;
//    
//    double A = y1 - y2;
//    double B = x2 -x1;
//    double C = x1*y2-x2*y1;
//    double norm = A * A + B * B;
//    double x0 = (B *(B * p[0] - A*p[1]) - A * C) / norm;
//    double y0 = (A *(-B * p[0] + A*p[1]) - B * C) / norm;
//    double x_min = std::min(x1, x2);
//    double x_max = std::max(x1, x2);
//    double y_min = std::min(y1, y2);
//    double y_max = std::max(y1, y2);
//    if (x0 < x_min - threshold || x0 > x_max + threshold ||
//        y0 < y_min - threshold || y0 > y_max + threshold) {
//      return false;
//    }
//    double dist = std::fabs(A * p[0] + B * p[1] + C) / std::sqrt(norm);
//    //if(dist < threshold){
//    //  std::cout << p[0] << " " << p[1] << std::endl;
//    //  std::cout << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
//    //  std::cout << A << " " << B << " " << C << std::endl;
//    //  std::cout << x0 << " " << y0 << std::endl;
//    //}
//    return dist < threshold;
//    //double dist =
//    //    std::fabs((y2 - y1) * p[0] - (x2 - x1) * p[1] + x2 * y1 - y2 * x1);
//    //double norm = std::sqrt((y2 - y1) * (y2 - y1) + (x2 - x1) * (x2 - x1));
//    ////std::cout << dist << "\t" << norm << "\t" << dist / norm << std::endl;
//    //dist /= norm;
//    //double d1 =
//    //    std::sqrt((p[0] - x1) * (p[0] - x1) + (p[1] - y1) * (p[1] - y1));
//    //double d2 =
//    //    std::sqrt((p[0] - x2) * (p[0] - x2) + (p[1] - y2) * (p[1] - y2));
//    //if (dist > d1 + threshold || dist > d2 + threshold) {
//    //  return false;
//    //}
//    //return dist< threshold;
//  }
//  void PngLabel2LidarPoints(const PointCloudPtr in_pcd,
//                            const vector<vector<double>>& png_labels,
//                            PointCloudPtr out_pcd) {
//    out_pcd->clear();
//    for(const auto &p : in_pcd->points){
//      for(const auto & two_point: png_labels){
//        if (PointOnLine({p.x, p.y}, two_point, 0.1)) {
//          out_pcd->push_back(p);
//          //std::cout << p.x << " " << p.y << std::endl;
//          //std::cout << two_point[0] * resolution_ - max_distance_ << " "
//          //          << two_point[1] * resolution_ - max_distance_ << " "
//          //          << two_point[2] * resolution_ - max_distance_ << " "
//          //          << two_point[3] * resolution_ - max_distance_ << std::endl;
//          //ShowPointCloud(out_pcd, 255, 5, 5, "roi", pcd_viewer_count_++,
//          //               pcd_viewer_);
//          //pcd_viewer_->spin();
//          //RemovePointCloud("roi", --pcd_viewer_count_, pcd_viewer_);
//          break;
//        }
//      }
//    }
//  }
//void RemovePointCloud(const std::string& viewer_id_string,
//                      const int viewer_count,
//                      pcl::visualization::PCLVisualizer::Ptr debug_viewer) {
//  std::ostringstream viewer_id;
//  viewer_id << viewer_id_string << viewer_count;
//  debug_viewer->removePointCloud(viewer_id.str(), 0);
//}
//void AddLidarPoint2IcpMap(const PointCloudPtr in_ptr){
//  for(const auto& p:in_ptr->points){
//    icp_map_->points.emplace_back(p.x, p.y, p.z);
//  }
//}
//void Optimize3dFollowing(const PointCloudPtr roi_ptr,
//                         const Eigen::Matrix4d& car_pose,
//                         Eigen::Matrix4d* adjust) {
//  pcl::PointCloud<pcl::PointXYZ>::Ptr icp_input(
//      new pcl::PointCloud<pcl::PointXYZ>);
//  for (const auto& p : roi_ptr->points) {
//    icp_input->points.emplace_back(p.x, p.y, p.z);
//  }
//  icp_.setInputTarget(icp_map_);
//  icp_.setInputSource(icp_input);
//  pcl::PointCloud<pcl::PointXYZ>::Ptr out(new pcl::PointCloud<pcl::PointXYZ>);
//  icp_.align(out, car_pose.cast<float>(), 0);
//  *adjust = icp_.finalTransMatrix().cast<double>();
//}
//void Optimize3d(const vector<PointCloudPtr>& pcds,
//                const vector<vector<vector<double>>>& png_labels,
//                const vector<Eigen::Matrix4d>& car_pose_matrices,
//                vector<Eigen::Matrix4d>* adjust_pose_matrices) {
//  // TODO Split whole set to into groups will speed up convergence?
//  const size_t N = png_labels.size();
//  std::cout << N << " images To be Optimized in PNG level" << std::endl;
//  PointCloudPtr roi_ptr(new PointCloud);
//  PointCloudPtr roi_imu_ptr(new PointCloud);
//  PointCloudPtr roi_world_ptr(new PointCloud);
//  PngLabel2LidarPoints(pcds[0], png_labels[0], roi_ptr);
// 
//  Eigen::Matrix4d lidar_world_matrix = car_pose_matrices[0] * lidar_imu_matrix_;
//  pcl::transformPointCloud(*roi_ptr, *roi_world_ptr, lidar_world_matrix);
//  AddLidarPoint2IcpMap(roi_world_ptr);
//  ShowPointCloud(roi_world_ptr, 255, 255, 255, "map", pcd_viewer_count_++,
//                 pcd_viewer_);
//  pcd_viewer_->spin();
//  // all_cost.push_back(0);
//  adjust_pose_matrices->push_back(car_pose_matrices[0]);
//
//  // pcd_viewer
//  zq_tools::timer_helper::Timer time_node;
//  time_node.name("icp");
//  for (size_t i = 1; i < N; i++) {
//    time_node.start();
//    std::cout << "---- Processing:  " << i << "----- " << std::endl;
//    std::cout << file_names_[i] << std::endl;
//    PngLabel2LidarPoints(pcds[i], png_labels[i], roi_ptr);
//
//    Eigen::Matrix4d adjust = Eigen::Matrix4d::Identity();
//    pcl::transformPointCloud(*roi_ptr, *roi_imu_ptr, lidar_imu_matrix_);
//    Optimize3dFollowing(roi_imu_ptr, car_pose_matrices[i], &adjust);
//
//    pcl::transformPointCloud(*roi_imu_ptr, *roi_world_ptr, adjust);
//    AddLidarPoint2IcpMap(roi_world_ptr);
//    ShowPointCloud(roi_world_ptr, 255, 255, 255, "map", pcd_viewer_count_++,
//                   pcd_viewer_);
//    pcd_viewer_->spin();
//
//    // ShowPointCloud(pcds[i], 255, 255, 255, "roi", pcd_viewer_count_++,
//    //               pcd_viewer_);
//    // ShowPointCloud(roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++,
//    //               pcd_viewer_);
//    ////ShowPointCloud(roi_ptr, colormap_[i * 2 % 256][0] * 255,
//    ////               colormap_[i * 2 % 256][1] * 255,
//    ////               colormap_[i * 2 % 256][2] * 255, "roi",
//    ////               pcd_viewer_count_++, pcd_viewer_);
//    // pcd_viewer_->spin();
//    // RemovePointCloud("roi", --pcd_viewer_count_, pcd_viewer_);
//    // RemovePointCloud("roi", --pcd_viewer_count_, pcd_viewer_);
//
//    // Eigen::Matrix4d adjust = Eigen::Matrix4d::Identity();
//    // double cost = 0;
//    // Optimize3dFollowing(car_x, car_y, points_map, &cost, &adjust);
//
//    // PngLabel2LidarPoints(png_labels[i], car_pose_matrices[i], adjust,
//    //                    &points_map, &car_x, &car_y);
//    // AddLidarPoint2All(car_x, car_y, points_map);
//    //// pcd_viewer
//
//    // all_cost.push_back(cost);
//    adjust_pose_matrices->push_back(adjust);
//    //std::cout << "adjust" << std::endl;
//    //std::cout << adjust << std::endl;
//
//
//    EigenMatrix2Pose(car_pose_matrices[i]);
//    EigenMatrix2Pose(adjust);
//    time_node.stop();
//    std::cout << time_node.toString() << std::endl;
//    // std::cout << cost << std::endl;
//  }
//}
//void SaveMapPcd(const vector<PointCloudPtr>& pcds,
//                const vector<Eigen::Matrix4d>& adjust_pose_matrices) {
//  if(pcds.size() != adjust_pose_matrices.size()){
//    std::cout << "Incorrect datasize : SaveMapPcd " << std::endl;
//  }
//  PointCloudPtr map_ptr(new PointCloud);
//  for(size_t i=0; i<pcds.size(); i++){
//    PointCloudPtr in_ptr(new PointCloud);
//
//    Eigen::Matrix4d lidar_world_matrix =
//        adjust_pose_matrices[i] * lidar_imu_matrix_;
//    pcl::transformPointCloud(*(pcds[i]), *in_ptr, lidar_world_matrix);
//    for(const auto& p:in_ptr->points){
//      map_ptr->push_back(p);
//    }
//  }
//  
//  pcl::io::savePCDFile("/home/host/tmp/map.pcd", *map_ptr, true);
//
//  pcl::PointCloud<pcl::PointXYZI>::Ptr map_xyzi(
//      new pcl::PointCloud<pcl::PointXYZI>);
//  pcl::PointCloud<pcl::PointXYZI>::Ptr map_filtered(
//      new pcl::PointCloud<pcl::PointXYZI>);
//  for(const auto& p:map_ptr->points){
//    pcl::PointXYZI pt;
//    pt.x = p.x;
//    pt.y = p.y;
//    pt.z = p.z;
//    pt.intensity = p.intensity;
//    map_xyzi->points.push_back(pt);
//  }
//
//  pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
//  voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//  voxel_grid_filter.setInputCloud(map_xyzi);
//  voxel_grid_filter.filter(*map_filtered);
//  pcl::io::savePCDFile("/home/host/tmp/map_f.pcd", *map_filtered, true);
//
//}
//void GroundMapGeneration(const std::string& pcd_folder) {
//  vector<PointCloudPtr> pcds;  // load all the pcd which has labels
//  vector<vector<vector<double>>> png_labels;  // image, line, (x0 y0 x1 y1)
//  vector<PointCloudPtr> points;  // the points are related with labels
//  vector<Eigen::Matrix4d> car_pose_matrices;     // related poses
//  vector<Eigen::Matrix4d> adjust_pose_matrices;  // related poses
//  // vector<vector<double>> vec_poses; // x y z roll pitch yaw
//  // vector<vector<double>> adjust_poses; // x y z roll pitch yaw
//
//  // load pcds (check label file)
//  // load png labels
//  int cnt = 0;
//  for (const auto& it : gt_lane_) {
//    cnt++;
//    // if(cnt>46 /*&& cnt < 50*/) {
//    std::cout << it.first << std::endl;
//    file_names_.push_back(it.first);
//    PointCloudPtr in_ptr(new PointCloud);
//    pcl::io::loadPCDFile(pcd_folder + it.first + ".pcd", *in_ptr);
//    pcds.push_back(in_ptr);
//    png_labels.push_back(it.second);
//    Eigen::Matrix4d mat4d;
//    SearchPoseWithTime(car_poses_, std::stod(it.first), &mat4d);
//    car_pose_matrices.push_back(mat4d);
//    // EigenMatrix2Pose(mat4d, &vec_poses);
//    //}
//  }
//
//  //// associate png labels with lidar points
//  // for(size_t iPcd=0; iPcd < pcds.size(); iPcd++){
//  //  MapPngToPcd(pcds[iPcd], png_labels[iPcd], &points);
//  //}
//  // visualization (Wait for bazel update)
//
//  // minimize cost functions;
//  // 1. png level optimization (x y z)
//  // 2. lidar level optimization (roll, pitch, yaw)
//  // 3. shift optimization x y z roll pitch yaw (initial values)
//  // OptimizePng(png_labels, car_pose_matrices, &adjust_pose_matrices);
//  Optimize3d(pcds, png_labels, car_pose_matrices, &adjust_pose_matrices);
//  SaveMapPcd(pcds, adjust_pose_matrices);
//
//  //
//  //
//  //
//  // GPS error calculation
//  // 1. labeled pcds
//  // 2. unlabled pcds in the same rosbags
//  // 3. other rosbags
//
//  //    PointCloudPtr in_ptr(new PointCloud);
//  //    pcl::io::loadPCDFile(in_str, *in_ptr);
//  //    PointCloudPtr filtered_ptr(new PointCloud);
//  //    pcl::PointCloud<pcl::PointXYZ>::Ptr xyz_ptr(
//  //        new pcl::PointCloud<pcl::PointXYZ>);
//  //    for (const auto& p : in_ptr->points) {
//  //      if (std::abs(p.x) < max_distance_ && std::abs(p.y) < max_distance_ &&
//  //          p.z < -1.0) {
//  //        Point pt;
//  //        pt.x = p.x;
//  //        pt.y = p.y;
//  //        pt.z = p.z;
//  //        pt.intensity = p.intensity;
//  //        pt.ring = p.ring;
//  //        filtered_ptr->push_back(pt);
//  //        xyz_ptr->points.emplace_back(p.x, p.y, 0);
//  //      }
//  //    }
//  //    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//  //    kdtree.setInputCloud(xyz_ptr);
//  //    std::vector<int> pointIdxRadiusSearch;
//  //    std::vector<float> pointRadiusSquaredDistance;
//  //
//  //    Eigen::Matrix4d car_pose_matrix = Eigen::Matrix4d::Identity();
//  //    if (!SearchPoseWithTime(car_poses_, std::stod(in_str.substr(59, 20)),
//  //                            &car_pose_matrix)) {
//  //      LOG(ERROR) << "cannot find pose for " << in_str;
//  //      return;
//  //    }
//  //
//  //    for (const auto& vec : gt_lane_[in_str.substr(59, 20)]) {
//  //      if (vec[2] - vec[0] == 0) continue;
//  //      float slope = (vec[3] - vec[1]) * 1.0 / (vec[2] - vec[0]);
//  //      float start_x = 0, end_x = 0, start_y = 0;
//  //      if (vec[0] < vec[2]) {
//  //        start_x = vec[0];
//  //        end_x = vec[2];
//  //        start_y = vec[1];
//  //      } else {
//  //        start_x = vec[2];
//  //        end_x = vec[0];
//  //        start_y = vec[3];
//  //      }
//  //      for (int i = start_x; i <= end_x; i++) {
//  //        const int j = slope * (i - start_x) + start_y;
//  //        const double x = i * resolution_ - max_distance_;
//  //        const double y = j * resolution_ - max_distance_;
//  //        pcl::PointXYZ p(x, y, 0);
//  //        if (kdtree.radiusSearch(p, 0.1, pointIdxRadiusSearch,
//  //                                pointRadiusSquaredDistance) > 0) {
//  //          for (const auto idx : pointIdxRadiusSearch) {
//  //            Eigen::Vector4d vec(filtered_ptr->points[idx].x,
//  //                                filtered_ptr->points[idx].y,
//  //                                filtered_ptr->points[idx].z, 1);
//  //            vec = car_pose_matrix * lidar_imu_matrix_ * vec;
//  //          }
//  //        }
//  //      }
//  //    }
//  //
//  }
//};
//}

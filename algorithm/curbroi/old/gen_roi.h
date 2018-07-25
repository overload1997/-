#pragma once

#include <iostream>
#include "pcl/common/transforms.h"
#include "pcl/filters/approximate_voxel_grid.h"
#include "pcl/filters/voxel_grid.h"
#include "pcl/io/pcd_io.h"
#include "pcl/kdtree/kdtree_flann.h"
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"
#include "s_loop/src/map/semantic_map/adapters/leveldb_adapter.h"
#include "s_loop/src/map/semantic_map/adapters/recordio_adapter.h"
#include "s_loop/src/map/semantic_map/interface.h"
#include "tools/hd_map_dev/tilemap/map_reader.h"

namespace gen_roi {
int pcd_viewer_count_ = 0;
pcl::visualization::PCLVisualizer::Ptr pcd_viewer_(
    new pcl::visualization::PCLVisualizer("Debug Viewer"));
void ShowPointCloud(const pcl::PointCloud<pcl::PointXYZI>::Ptr in_cloud,
                    const int r, const int g, const int b,
                    const std::string &viewer_id_string, const int viewer_count,
                    pcl::visualization::PCLVisualizer::Ptr debug_viewer) {
  pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
      new pcl::PointCloud<pcl::PointXYZRGB>);
  for (const auto &pt : in_cloud->points) {
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

class GenRoi {
  std::vector<std::vector<cv::Point2f>> opencv_polygons_;
  hdmap::MapReader map_reader_;
  double y_flip_ = -1; // 1 or 1

 public:
  explicit GenRoi(const std::string &map_dir, const std::string &label_roi_file,
                  const double y_flip)
      : map_reader_(map_dir), y_flip_(y_flip) {
    std::cout << map_dir << std::endl;
    std::cout << label_roi_file << std::endl;
    semantic_map::Interface::LoadROI<semantic_map::LevelDBAdapter>(
        label_roi_file, &opencv_polygons_);
    std::cout << "polygons : " << opencv_polygons_.size() << std::endl;
  }

  void ShowRoiPng() {
    std::cout << "show png" << std::endl;
    // map_reader_.Visualize();
    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
    for (size_t i = 0; i < 5; i++) {
      double min_x = 1e90;
      double max_x = -1e90;
      double min_y = 1e90;
      double max_y = -1e90;
      for (const auto &p : opencv_polygons_[i]) {
        std::cout << p.x << " " << p.y << " ";
        min_x = std::min(double(p.x), min_x);
        min_y = std::min(double(-1 * p.y), min_y);
        max_x = std::max(double(p.x), max_x);
        max_y = std::max(double(-1 * p.y), max_y);
      }
      std::cout << std::endl;
      std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
                << std::endl;
      cv::Mat mat;
      // map_reader_.Query(min_x, min_y, max_x, max_y, &mat);
      // cv::imshow("image" , mat);
      // cv::waitKey(0);
      pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
          new pcl::PointCloud<pcl::PointXYZI>);
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
    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1,
                      cloud.get());

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
    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1,
                      cloud.get());

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
    int cnt = 0;
    for (const auto &pt : cloud_f->points) {
      if (cnt % 1000 == 0) {
        std::cout << cnt * 1.0 / cloud_f->points.size() << std::endl;
      }
      for (const auto &polygon : polygons) {
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
    ShowPointCloud(cloud_roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++,
                   pcd_viewer_);
    pcd_viewer_->spin();
  }

  void Downsample(const pcl::PointCloud<pcl::PointXYZI>::Ptr cloud,
                  const double filter_size,
                  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f) {
    double cloud_min_x = 1e90;
    double cloud_max_x = -1e90;
    double cloud_min_y = 1e90;
    double cloud_max_y = -1e90;
    double cloud_min_z = 1e90;
    double cloud_max_z = -1e90;
    for(const auto & p:cloud->points){
      cloud_min_x = std::min(double(p.x), cloud_min_x);
      cloud_min_y = std::min(double(p.y), cloud_min_y);
      cloud_min_z = std::min(double(p.z), cloud_min_z);
      cloud_max_x = std::max(double(p.x), cloud_max_x);
      cloud_max_y = std::max(double(p.y), cloud_max_y);
      cloud_max_z = std::max(double(p.z), cloud_max_z);
    }

    int64_t dx =
        static_cast<int64_t>((cloud_max_x - cloud_min_x) / filter_size) + 1;
    int64_t dy =
        static_cast<int64_t>((cloud_max_y - cloud_min_y) / filter_size) + 1;
    int64_t dz =
        static_cast<int64_t>((cloud_max_z - cloud_min_z) / filter_size) + 1;

    int ratio = (dx * dy * dz) /
                static_cast<int64_t>(std::numeric_limits<int32_t>::max());
    std::cout << " ratio " << ratio << std::endl;
    if(ratio < 1){
      std::cout << "Downsample: " << cloud->size() << std::endl;
      pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
      voxel_grid_filter.setLeafSize(filter_size, filter_size, filter_size);
      voxel_grid_filter.setInputCloud(cloud);
      voxel_grid_filter.filter(*cloud_f);
      std::cout << "cloud_f size " << cloud_f->size() << std::endl;
    }else{
      ratio = std::sqrt(ratio)+1;
      std::cout << "split pcd into " << ratio << " x " << ratio
                << "before downnsample" << std::endl;
      for(int i=0; i<ratio; i++){
        for(int j=0; j<ratio; j++){
          double min_x = cloud_min_x + i * (cloud_max_x - cloud_min_x) / ratio;
          double max_x =
              cloud_min_x + (i + 1) * (cloud_max_x - cloud_min_x) / ratio;
          double min_y = cloud_min_y + j * (cloud_max_y - cloud_min_y) / ratio;
          double max_y =
              cloud_min_y + (j + 1) * (cloud_max_y - cloud_min_y) / ratio;
          pcl::PointCloud<pcl::PointXYZI>::Ptr part(
              new pcl::PointCloud<pcl::PointXYZI>);
          pcl::PointCloud<pcl::PointXYZI>::Ptr part_f(
              new pcl::PointCloud<pcl::PointXYZI>);
          for(const auto& p: cloud->points){
            if(p.x > min_x && p.x < max_x && p.y > min_y && p.y< max_y){
              part->push_back(p);
            }
          }
          std::cout << "Downsample: " << part->size() << std::endl;
          pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
          voxel_grid_filter.setLeafSize(filter_size, filter_size, filter_size);
          voxel_grid_filter.setInputCloud(part);
          voxel_grid_filter.filter(*part_f);
          for(const auto& p:part_f->points){
            cloud_f->push_back(p);
          }
          std::cout << "cloud_f size " << cloud_f->size() << std::endl;
        }
      }
    }
  }
  std::vector<std::vector<double>> MedianFilter(
      const std::vector<std::vector<double>> &in_map,
      const std::vector<std::vector<bool>> &in_flag, const int k) {
    const int hk = k / 2;
    const int m = in_map.size();
    const int n = in_map[0].size();
    std::vector<std::vector<double>> res(m, std::vector<double>(n, 0));
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        std::vector<double> vec;
        const int i_start = std::max(0, i - hk);
        const int i_end = std::min(m, i + hk);
        const int j_start = std::max(0, j - hk);
        const int j_end = std::min(n, j + hk);
        for (int ii = i_start; ii < i_end; ii++) {
          for (int jj = j_start; jj < j_end; jj++) {
            if (in_flag[ii][jj]) {
              vec.push_back(in_map[ii][jj]);
            }
          }
        }
        if (!vec.empty()) {
          std::nth_element(vec.begin(), vec.begin() + vec.size() / 2,
                           vec.end());
          res[i][j] = vec[vec.size() / 2];
        }
      }
    }
    return res;
  }
  void EstimateGround(const pcl::PointCloud<pcl::PointXYZI>::Ptr map_ptr,
                      const std::string &out_map_file,
                      const std::string &out_ground_file,
                      const std::string &out_ground_filtered_file,
                      const double ground_filter_size,
                      const double median_filter_size) {
    pcl::PointCloud<pcl::PointXYZ>::Ptr map_xyz_ptr(
        new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PointCloud<pcl::PointXYZ>::Ptr map_ground_ptr(
        new pcl::PointCloud<pcl::PointXYZ>);
    double min_x = std::numeric_limits<double>::max();
    double min_y = std::numeric_limits<double>::max();
    double max_x = -std::numeric_limits<double>::max();
    double max_y = -std::numeric_limits<double>::max();
    for (const auto &p : map_ptr->points) {
      pcl::PointXYZ pt;
      pt.x = p.x;
      pt.y = p.y;  // no flip pcd !!
      pt.z = p.z;
      map_xyz_ptr->push_back(pt);
      min_x = p.x < min_x ? p.x : min_x;
      min_y = y_flip_ * p.y < min_y ?y_flip_ *  p.y : min_y;  // no flip pcd !!

      max_x = p.x > max_x ? p.x : max_x;
      max_y = y_flip_ * p.y > max_y ? y_flip_ * p.y : max_y;  // no  flip pcd !!

      pt.z = 0;
      map_ground_ptr->push_back(pt);
    }
    std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
              << std::endl;
    std::cout << ground_filter_size << std::endl;

    std::cout << "kdtree points : " << map_ground_ptr->size() << std::endl;
    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
    kdtree.setInputCloud(map_ground_ptr);

    std::vector<int> pointIdxRadiusSearch;
    std::vector<float> pointRadiusSquaredDistance;

    const int dim_x = (max_x - min_x) / ground_filter_size + 1;
    const int dim_y = (max_y - min_y) / ground_filter_size + 1;

    std::vector<std::vector<double>> map_sum(dim_x,
                                             std::vector<double>(dim_y, 0));
    std::vector<std::vector<bool>> map(dim_x, std::vector<bool>(dim_y, false));

    int cnt = 0;

    double last_mean_z = 0;
    for (int idx_x = 0; idx_x < dim_x; idx_x++) {
      std::cout << idx_x << " / " << dim_x << " " << cnt << std::endl;
      for (int idx_y = 0; idx_y < dim_y; idx_y++) {
        const double x = idx_x * ground_filter_size + min_x;
        const double y = idx_y * ground_filter_size + min_y;
        for (const auto &polygon : opencv_polygons_) {
          if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) >= 0) {
            pointIdxRadiusSearch.clear();
            pointRadiusSquaredDistance.clear();
            pcl::PointXYZ p(x, y, 0);
            map[idx_x][idx_y] = true;
            map_sum[idx_x][idx_y] = last_mean_z;  // if there is no lidar point
                                                  // in roi region, use values
                                                  // from previous state
            if (kdtree.radiusSearch(p, 1, pointIdxRadiusSearch,
                                    pointRadiusSquaredDistance) > 0) {
              double mean_z = 0;
              for (const auto idx : pointIdxRadiusSearch) {
                mean_z += map_xyz_ptr->points[idx].z;
              }
              last_mean_z = mean_z / pointIdxRadiusSearch.size();
              map_sum[idx_x][idx_y] = last_mean_z;
              cnt++;
            }
            break;
          }
        }
      }
    }

    std::ofstream of;
    of.open(out_ground_file);
    std::cout << out_ground_file << std::endl;

    of << min_x << " " << min_y << " " << ground_filter_size << " " << dim_x
       << " " << dim_y << std::endl;
    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map[i][j]) {
          of << i * dim_y + j << " " << map_sum[i][j] << std::endl;
        }
      }
    }
    of.close();

    pcl::PointCloud<pcl::PointXYZ>::Ptr map_save(
        new pcl::PointCloud<pcl::PointXYZ>);
    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map[i][j]) {
          pcl::PointXYZ p;
          p.x = i * ground_filter_size + min_x;
          p.y = j * ground_filter_size + min_y;
          p.z = map_sum[i][j];
          map_save->push_back(p);
        }
      }
    }
    std::cout << map_save->size() << std::endl;
    pcl::io::savePCDFile(out_map_file, *map_save, true);
    std::cout << out_map_file << std::endl;

    std::vector<std::vector<double>> f_map =
        MedianFilter(map_sum, map,
                     static_cast<int>(median_filter_size / ground_filter_size));
    std::ofstream ofs;
    ofs.open(out_ground_filtered_file);

    ofs << min_x << " " << min_y << " " << ground_filter_size << " " << dim_x
        << " " << dim_y << std::endl;
    for (int i = 0; i < dim_x; i++) {
      for (int j = 0; j < dim_y; j++) {
        if (map[i][j]) {
          ofs << i * dim_y + j << " " << f_map[i][j] << std::endl;
        }
      }
    }
    ofs.close();
  }
  void Run(const std::string &pcd_file, const std::string &pcd_roi_file,
           const std::string &out_map_file, const std::string &out_ground_file,
           const std::string &out_ground_filtered_file,
           const double ground_filter_size, const double median_filter_size) {
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f0(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
        new pcl::PointCloud<pcl::PointXYZI>);

    map_reader_.Query(map_reader_.map_range(), 2, cloud.get());
    //pcl::io::loadPCDFile(pcd_file, *cloud);
    Downsample(cloud, 0.2, cloud_f0);
    for(const auto & p: cloud_f0->points){
      pcl::PointXYZI pt;
      pt.x = p.x;
      pt.y = y_flip_ * p.y;
      pt.z = p.z;
      pt.intensity = p.intensity;
      cloud_f->push_back(p);
    }
    int cnt = 0;
    for (const auto &pt : cloud_f->points) {
      if (cnt % 100000 == 0) {
        LOG(INFO) << cnt * 1.0 / cloud_f->points.size() << std::endl;
      }
      for (const auto &polygon : opencv_polygons_) {
        if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, y_flip_ * pt.y),
                                 false) >= 0) {
          cloud_roi_ptr->push_back(pt);
          break;
        }
      }
      cnt++;
    }

    pcl::io::savePCDFile(pcd_roi_file, *cloud_roi_ptr, true);

    EstimateGround(cloud_roi_ptr, out_map_file, out_ground_file,
                   out_ground_filtered_file, ground_filter_size,
                   median_filter_size);
  }
  void FilterGroundFiles() {
    double ground_min_x, ground_min_y, ground_resolution;
    int ground_dim_x, ground_dim_y;
    std::unordered_map<int, double> ground_map_vec;

    std::ifstream ifs("/home/host/data/1x1_map/ground_1x1_1023_filtered.txt");
    if (!ifs.is_open()) {
      ROS_ERROR("cannot read: file ; Please download it from /media/nas/miao");
      return;
    }
    ifs >> ground_min_x;
    ifs >> ground_min_y;
    ifs >> ground_resolution;
    ifs >> ground_dim_x;
    ifs >> ground_dim_y;
    int key;
    double val;

    while (!ifs.eof()) {
      ifs >> key;
      if (ifs.eof()) {
        break;
      }
      ifs >> val;
      ground_map_vec[key] = val;
    }

    std::ofstream of;
    of.open("/home/host/data/1x1_map/ground_1x1_1119.txt");
    of << ground_min_x << " " << ground_min_y << " " << ground_resolution << " "
       << ground_dim_x << " " << ground_dim_y << std::endl;
    int cnt = 0;
    int cnt_roi = 0;
    int tol = ground_map_vec.size();
    for (const auto it : ground_map_vec) {
      const int j = it.first % ground_dim_y;
      const int i = it.first / ground_dim_y;
      const double x = i * ground_resolution + ground_min_x;
      const double y = j * ground_resolution + ground_min_y;
      if (cnt % 10000 == 0) {
        LOG(INFO) << cnt * 1.0f / tol << " " << 1.0f * cnt_roi / cnt;
      }
      for (const auto &polygon : opencv_polygons_) {
        if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) >= 0) {
          of << i * ground_dim_y + j << " " << it.second << std::endl;
          cnt_roi++;

          break;
        }
      }
      cnt++;
    }

    of.close();
  }
  void ShrinkPolygonToROI(const int map_min_x, const int map_min_y,
                          const int map_max_x, const int map_max_y,
                          std::vector<std::vector<cv::Point2f>> *polygons) {
    for (const auto &vec : opencv_polygons_) {
      if (vec.size() < 3) continue;
      int min_x = vec[0].x;
      int min_y = vec[0].y;
      int max_x = vec[0].x;
      int max_y = vec[0].y;
      for (const auto p : vec) {
        min_x = std::min(int(p.x), min_x);
        min_y = std::min(int(p.y), min_y);
        max_x = std::max(int(p.x), max_x);
        max_x = std::max(int(p.y), max_y);
      }
      int ov_min_x = std::max(int(min_x), map_min_x);
      int ov_min_y = std::max(int(min_y), map_min_y);
      int ov_max_x = std::min(int(max_x), map_max_x);
      int ov_max_y = std::min(int(max_y), map_max_y);
      if (ov_min_x >= ov_max_x || ov_min_y >= ov_max_y) {
        continue;
      }
      polygons->push_back(vec);
    }
  }
  void CheckGround(const std::string& ground_file){
    std::cout << "show png" << std::endl;
    map_reader_.Visualize();
    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
    for (size_t i = 0; i < 5; i++) {
      double min_x = 1e90;
      double max_x = -1e90;
      double min_y = 1e90;
      double max_y = -1e90;
      for (const auto &p : opencv_polygons_[i]) {
        std::cout << p.x << " " << p.y << " ";
        min_x = std::min(double(p.x), min_x);
        min_y = std::min(double(y_flip_ * p.y), min_y);
        max_x = std::max(double(p.x), max_x);
        max_y = std::max(double(y_flip_ * p.y), max_y);
      }
      std::cout << std::endl;
      std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
                << std::endl;
      cv::Mat mat;
      map_reader_.Query(min_x, min_y, max_x, max_y, &mat);
      cv::imshow("image" , mat);
      cv::waitKey(0);
    }
  }
};
} // namespace gen_roi

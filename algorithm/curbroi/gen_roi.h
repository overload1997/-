#pragma once

#include <iostream>
#include "main_util.h"
#include "pcl/common/transforms.h"
#include "pcl/filters/approximate_voxel_grid.h"
#include "pcl/filters/voxel_grid.h"
#include "pcl/io/pcd_io.h"
#include "pcl/kdtree/kdtree_flann.h"
#include "pcl/point_cloud.h"
#include "pcl/point_types.h"
#include "s_loop/src/map/semantic_map/adapters/leveldb_adapter.h"
#include "s_loop/src/map/semantic_map/adapters/recordio_adapter.h"
#include "s_loop/src/map/semantic_map/elements/way_point.h"
#include "s_loop/src/map/semantic_map/interface.h"
#include "tools/hd_map_dev/tilemap/map_reader.h"
#include "s_loop/src/map/semantic_map/tools/find_pcd_utils.h"

namespace curb_roi {
//int pcd_viewer_count_ = 0;
//pcl::visualization::PCLVisualizer::Ptr pcd_viewer_(
//    new pcl::visualization::PCLVisualizer("Debug Viewer"));
//void ShowPointCloud(const pcl::PointCloud<pcl::PointXYZ>::Ptr in_cloud,
//                    const int r, const int g, const int b,
//                    const std::string &viewer_id_string, const int viewer_count,
//                    pcl::visualization::PCLVisualizer::Ptr debug_viewer) {
//  pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
//      new pcl::PointCloud<pcl::PointXYZRGB>);
//  for (const auto &pt : in_cloud->points) {
//    pcl::PointXYZRGB cpt;
//    cpt.x = pt.x;
//    cpt.y = pt.y;
//    cpt.z = pt.z;
//    cpt.r = r;
//    cpt.g = g;
//    cpt.b = b;
//    rgb_cloud->push_back(cpt);
//  }
//  std::ostringstream viewer_id;
//  viewer_id << viewer_id_string << viewer_count;
//  debug_viewer->addPointCloud(rgb_cloud, viewer_id.str(), 0);
//  std::cout << "add points " << rgb_cloud->points.size() << "to viewer"
//            << std::endl;
//}

class GenRoi {
  std::vector<std::vector<cv::Point2f>> opencv_polygons_;
  hdmap::MapReader map_reader_;
  double y_flip_ = -1;  // 1 or 1
  std::string map_pcd_folder_;
  std::string tile_map_path_;

 public:
  explicit GenRoi(const std::string &map_dir, const std::string &label_roi_file,
                  const std::string &label_curb_file, const double y_flip)
      : map_reader_(map_dir), y_flip_(y_flip) {
    std::cout << map_dir << std::endl;
    //std::cout << label_roi_file << std::endl;
    std::cout << label_curb_file << std::endl;
    //semantic_map::Interface::LoadROI<semantic_map::LevelDBAdapter>(
    //    label_roi_file, &opencv_polygons_);
    //std::cout << "polygons : " << opencv_polygons_.size() << std::endl;
    semantic_map::Interface::Load(label_curb_file, true);

    map_pcd_folder_ = map_dir + "/pcds/";
    tile_map_path_ = map_dir ;

    // pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
    //    new pcl::PointCloud<pcl::PointXYZI>);
    // map_reader_.Query(map_reader_.map_range(), 3, cloud.get());
    // for (const auto &p : cloud->points) {
    //  if (p.x > -10 && p.x < 10 && p.y > -10 && p.y < 10) {
    //    std::cout << p.x << " " << p.y << " " << p.z << std::endl;
    //  }
    //}
  }

  void WriteCurbRoi(const std::string &pcd_file,
                    const std::string &pcd_roi_file,
                    const std::string &out_map_file,
                    const std::string &out_ground_file,
                    const double ground_filter_size,
                    const double median_filter_size) {
    // double map_min_x = -10;
    // double map_min_y = -10;
    // double map_max_x = 10;
    // double map_max_y = 10;
    double map_min_x = std::numeric_limits<double>::max();
    double map_min_y = std::numeric_limits<double>::max();
    double map_max_x = std::numeric_limits<double>::lowest();
    double map_max_y = std::numeric_limits<double>::lowest();
    auto curb_gt = semantic_map::Interface::CurbPointRadiusSearch(240, 72, 4000);

    // pcl::PointCloud<pcl::PointXYZ>::Ptr curb_in_world_ptr(
    //    new pcl::PointCloud<pcl::PointXYZ>);
    // for (const auto &c : curb_gt) {
    //  for (const auto &p : c) {
    //    curb_in_world_ptr->push_back(pcl::PointXYZ(p(0), p(1), 0));
    //  }
    //}
    // pcd_viewer_->setBackgroundColor(0, 0, 0);
    // pcd_viewer_->addCoordinateSystem(1.0);
    // pcd_viewer_->initCameraParameters();
    // ShowPointCloud(curb_in_world_ptr, 255, 0, 0, "curb", pcd_viewer_count_++,
    //               pcd_viewer_);
    // pcd_viewer_->spin();

    int curb_all_points_size = 0;
    for (const auto &c : curb_gt) {
      curb_all_points_size += c.size();
      for (const auto &v : c) {
        map_min_x = std::min(map_min_x, v(0));
        map_min_y = std::min(map_min_y, v(1));
        map_max_x = std::max(map_max_x, v(0));
        map_max_y = std::max(map_max_y, v(1));
      }
    }
    std::cout << "curb points " << curb_all_points_size << std::endl;
    std::cout << map_min_x << " " << map_min_y << " " << map_max_x << " "
              << map_max_y << std::endl;
    std::cout << (map_max_x - map_min_x) / 0.2 << " "
              << (map_max_y - map_min_y) / 0.2 << std::endl;

    const int map_dim_x = (map_max_x - map_min_x) / ground_filter_size + 1;
    const int map_dim_y = (map_max_y - map_min_y) / ground_filter_size + 1;

    std::vector<std::vector<double>> ground_height(
        map_dim_x, std::vector<double>(map_dim_y, 0));
    double ground_min_x, ground_min_y;
    int ground_dim_x, ground_dim_y;
    std::vector<std::vector<int>> ground_zone;
    ColorMapRegion(map_min_x, map_min_y, map_dim_x, map_dim_y,
                   ground_filter_size, &ground_zone, &ground_min_x,
                   &ground_min_y, &ground_dim_x, &ground_dim_y);
    AverageGround(ground_min_x, ground_min_y, ground_dim_x, ground_dim_y,
                  ground_filter_size, ground_zone, &ground_height);

    std::vector<std::vector<double>> f_map =
        MedianFilter(ground_height, ground_zone,
                     static_cast<int>(median_filter_size / ground_filter_size));

    pcl::PointCloud<pcl::PointXYZ>::Ptr ground_save(
        new pcl::PointCloud<pcl::PointXYZ>);
    for (int i = 0; i < ground_dim_x; i++) {
      for (int j = 0; j < ground_dim_y; j++) {
        if (ground_zone[i][j] > 0) {
          pcl::PointXYZ p;
          p.x = i * ground_filter_size + ground_min_x;
          p.y = j * ground_filter_size + ground_min_y;
          p.z = f_map[i][j];
          ground_save->push_back(p);
        }
      }
    }
    std::cout << ground_save->size() << std::endl;
    pcl::io::savePCDFile(out_map_file, *ground_save, true);

    std::ofstream ofs;
    ofs.open(out_ground_file);
    std::cout << out_map_file << std::endl;

    ofs << ground_min_x << " " << ground_min_y << " " << ground_filter_size
        << " " << ground_dim_x << " " << ground_dim_y << std::endl;
    for (int i = 0; i < ground_dim_x; i++) {
      for (int j = 0; j < ground_dim_y; j++) {
        if (ground_zone[i][j] > 0) {
          // use int64 will be better
          // int is just enough for 1x1
          ofs << i * ground_dim_y + j << " " << f_map[i][j] << std::endl;
        }
      }
    }
    ofs.close();
  }
  void AverageGround(const double ground_min_x, const double ground_min_y,
                     const int ground_dim_x, const int ground_dim_y,
                     const double ground_resolution,
                     const std::vector<std::vector<int>> &ground_zone,
                     std::vector<std::vector<double>> *ground_height) {
    LOG(INFO) << " AverageGround " << ground_min_x << " " << ground_min_y << " "
              << ground_dim_x << " " << ground_dim_y << " " << ground_resolution
              << std::endl;
    ground_height->resize(ground_dim_x);
    for (auto &vec : (*ground_height)) {
      vec.resize(ground_dim_y);
    }
    std::vector<std::vector<std::vector<double>>> ground_z(
        ground_dim_x,
        std::vector<std::vector<double>>(ground_dim_y, std::vector<double>()));
    // Load PCD folder
    // keep lower 20% point ?
    //std::vector<std::string> pcd_file_names;
    //GetFileNamesInFolder(map_pcd_folder_, ".pcd", &pcd_file_names);
     auto tile_map_result = semantic_map::TileMap::Load(tile_map_path_);
     if (!tile_map_result.ok()) {
       LOG(FATAL) << tile_map_result.status();
     }
    std::unique_ptr<semantic_map::TileMap> tile_map(tile_map_result.ValueOrDie());
    std::vector<Eigen::Vector2d> tile_points;
    for (int i = 0; i < ground_dim_x; i++) {
      for (int j = 0; j < ground_dim_y; j++) {
        if (ground_zone[i][j] > 0) {
          double x = i * ground_resolution+ ground_min_x;
          double y = j * ground_resolution+ ground_min_y;
          tile_points.emplace_back(x, y);
        }
      }
    }
   
    std::vector<std::string> pcd_file_names = semantic_map::FindPCDs(tile_map, tile_points);


    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi(
        new pcl::PointCloud<pcl::PointXYZI>);
    int cnt = 0;
    for (const auto &file_name : pcd_file_names) {
      if (++cnt % 100 == 0) {
        LOG(INFO) << "pcd: " << cnt << " / " << pcd_file_names.size();
      }
      pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
          new pcl::PointCloud<pcl::PointXYZI>);
      //pcl::io::loadPCDFile(map_pcd_folder_ + file_name, *cloud);
      pcl::io::loadPCDFile(file_name, *cloud);

      for (const auto &p : cloud->points) {
        const int i = (p.x - ground_min_x) / ground_resolution;
        const int j = (y_flip_ * p.y - ground_min_y) / ground_resolution;
        if (i >= 0 && j >= 0 && i < ground_dim_x && j < ground_dim_y) {
          if (ground_zone[i][j] > 0) {
            cloud_roi->push_back(p);
            ground_z[i][j].push_back(p.z);
          }
        }
      }
    }
    LOG(INFO) << " ground_roi points " << cloud_roi->size();
    //pcl::io::savePCDFile("/6T/miao/demo_mathilda/ground_roi.pcd", *cloud_roi, true);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_f(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
    voxel_grid_filter.setLeafSize(1, 1, 2);
    voxel_grid_filter.setInputCloud(cloud_roi);
    voxel_grid_filter.filter(*cloud_roi_f);
    pcl::io::savePCDFile("/media/local_6T/map/ROI/ground_roi_f.pcd", *cloud_roi_f,
                         true);

    double last_height = 0;
    for (int i = 0; i < ground_dim_x; i++) {
      for (int j = 0; j < ground_dim_y; j++) {
        if (ground_zone[i][j] > 0) {
          auto &v = ground_z[i][j];
          if (!v.empty()) {
            std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
            (*ground_height)[i][j] = v[v.size() / 2];
            //} else {
            //  (*ground_height)[i][j] = last_height;
          }
        }
      }
    }

    for (int i = 0; i < ground_dim_x; i++) {
      for (int j = 0; j < ground_dim_y; j++) {
        if (ground_zone[i][j] > 0) {
          auto &v = ground_z[i][j];
          if (v.empty()) {
            std::vector<double> vec;
            for (int ii = -10; ii <= 10; ii++) {
              for (int jj = -10; jj <= 10; jj++) {
                if (i + ii >= 0 && i + jj >= 0 && i + ii < ground_dim_x &&
                    j + jj < ground_dim_y) {
                  if (ground_zone[i + ii][j + jj] > 0 &&
                      !ground_z[i + ii][j + jj].empty()) {
                    vec.push_back((*ground_height)[i + ii][j + jj]);
                  }
                }
              }
            }
            if (vec.empty()) {
              (*ground_height)[i][j] = last_height;
            } else {
              std::nth_element(vec.begin(), vec.begin() + vec.size() / 2,
                               vec.end());
              (*ground_height)[i][j] = vec[vec.size() / 2];
              last_height = (*ground_height)[i][j];
            }
          } else {
            last_height = (*ground_height)[i][j];
          }
        }
      }
    }
  }
  std::vector<std::vector<double>> MedianFilter(
      const std::vector<std::vector<double>> &in_map,
      const std::vector<std::vector<int>> &map_zone, const int k) {
    const int hk = k / 2;
    const int m = in_map.size();
    const int n = in_map[0].size();
    std::vector<std::vector<double>> res(m, std::vector<double>(n, 0));
    for (int i = 0; i < m; i++) {
      for (int j = 0; j < n; j++) {
        if (map_zone[i][j] <= 0) {
          continue;
        }
        std::vector<double> vec;
        const int i_start = std::max(0, i - hk);
        const int i_end = std::min(m, i + hk);
        const int j_start = std::max(0, j - hk);
        const int j_end = std::min(n, j + hk);
        for (int ii = i_start; ii < i_end; ii++) {
          for (int jj = j_start; jj < j_end; jj++) {
            if (map_zone[ii][jj] > 0) {
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
      std::cout << "MedianFilter " << i << " / " << m << std::endl;
    }
    return res;
  }
  void ColorMapRegion(const double map_min_x, const double map_min_y,
                      const int map_dim_x, const int map_dim_y,
                      const double map_resolution,
                      std::vector<std::vector<int>> *ground_zone,
                      double *ground_min_x, double *ground_min_y,
                      int *ground_dim_x, int *ground_dim_y) {
    // map_zone  : -1: unknow, 0 curb, 1 roi
    // Step 1: Use curb as boundary
    // Step 2: use (0,0) as seed for floodfill
    //
    // Output drivable area mask, map_min_x, map_min_y, map_max_x, map_max_y

    auto curb_gt = semantic_map::Interface::CurbPointRadiusSearch(240, 72, 4000);

    const std::vector<int> di({0, 0, 0, -1, 1, 1,  1,  -1, -1, 0,  0,  1, 1,
                               2, 2, 2, 2,  2, -1, -1, -2, -2, -2, -2, -2});
    const std::vector<int> dj({0, 1,  -1, 0, 0,  -1, 1,  -1, 1,  2, -2, 2, -2,
                               2, -1, 1,  0, -2, 2,  -2, 2,  -2, 1, -1, 0});

    std::vector<std::vector<int>> map_zone(map_dim_x,
                                           std::vector<int>(map_dim_y, -1));

    int cnt = 0;
    for (const auto &c : curb_gt) {
      cnt++;
      for (const auto &v : c) {
        const int i = (v(0) - map_min_x) / map_resolution;
        const int j = (v(1) - map_min_y) / map_resolution;
        for (size_t k = 0; k < di.size(); k++) {
          if (i + di[k] >= 0 && i + di[k] < map_dim_x && j + dj[k] >= 0 &&
              j + dj[k] < map_dim_y) {
            map_zone[i + di[k]][j + dj[k]] = 0;
          }
        }
      }
      std::cout << "Step 1 : " << cnt << " / " << curb_gt.size() << std::endl;
    }

    const int i0 = (240-1 * map_min_x) / map_resolution;
    const int j0 = (72-1 * map_min_y) / map_resolution;
    std::stack<int> candidate_i;
    std::stack<int> candidate_j;
    candidate_i.push(i0);
    candidate_j.push(j0);
    cnt = 0;
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZ>);
    while (!candidate_i.empty()) {
      if (cnt % 3000000 == 0) {
        std::cout << "Step 2 : " << cnt / 10000 << " / " << candidate_i.size()
                  << std::endl;
        // ShowPointCloud(cloud, 0, 255, 0, "roi", pcd_viewer_count_++,
        //               pcd_viewer_);
        // pcd_viewer_->spin();
        // pcd_viewer_->removePointCloud("roi");
      }
      const int i = candidate_i.top();
      const int j = candidate_j.top();
      candidate_i.pop();
      candidate_j.pop();
      map_zone[i][j] = 1;
      for (int k = 1; k < 5; k++) {
        if (i + di[k] >= 0 && i + di[k] < map_dim_x && j + dj[k] >= 0 &&
            j + dj[k] < map_dim_y) {
          if (map_zone[i + di[k]][j + dj[k]] == -1) {
            candidate_i.push(i + di[k]);
            candidate_j.push(j + dj[k]);
          }
        }
        // if (cnt % 10 == 0) {
        //  const double x = map_min_x + i * map_resolution;
        //  const double y = map_min_y + j * map_resolution;
        //  cloud->push_back(pcl::PointXYZ(x, y, 0));
        //}
      }
      cnt++;
    }

    int min_i = 0, min_j = 0, max_i = map_dim_x, max_j = map_dim_y;
    for (int i = 0; i < map_dim_x; i++) {
      for (int j = 0; j < map_dim_y; j++) {
        if (map_zone[i][j] == 1) {
          min_i = std::min(min_i, i);
          min_j = std::min(min_j, j);
          max_i = std::max(max_i, i);
          max_j = std::max(max_j, j);
          const double x = map_min_x + i * map_resolution;
          const double y = map_min_y + j * map_resolution;
          cloud->push_back(pcl::PointXYZ(x, y, 0));
        }
      }
    }
    pcl::io::savePCDFile("/media/local_6T/map/ROI/map_zone.pcd", *cloud, true);
     //ShowPointCloud(cloud, 0, 255, 0, "roi", pcd_viewer_count_++,
     //pcd_viewer_);
     //pcd_viewer_->spin();

    *ground_dim_x = max_i - min_i + 1;
    *ground_dim_y = max_j - min_j + 1;
    *ground_min_x = min_i * map_resolution + map_min_x;
    *ground_min_y = min_j * map_resolution + map_min_y;
    LOG(INFO) << *ground_min_x << " " << *ground_min_y << " " << *ground_dim_x
              << " " << *ground_dim_y << " " << map_resolution << std::endl;

    ground_zone->resize(*ground_dim_x);
    for (auto &vec : *ground_zone) {
      vec.resize(*ground_dim_y, 0);
    }

    for (int i = 0; i < map_dim_x; i++) {
      for (int j = 0; j < map_dim_y; j++) {
        if (map_zone[i][j] == 1) {
          (*ground_zone)[i - min_i][j - min_j] = 1;
        }
      }
    }
  }
  void ExpandRoi(const int i, const int j,
                 std::vector<std::vector<int>> *map_zone) {
    for (int ii = 0; ii >= -i; ii--) {
      if ((*map_zone)[ii + i][j] < 0 || (*map_zone)[ii + i][j] == 1) {
        break;
      }
      if (std::abs(ii) > 100) {
        break;
      }
      if ((*map_zone)[ii + i][j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    for (int jj = 0; jj >= -j; jj--) {
      if ((*map_zone)[i][jj + j] < 0 || (*map_zone)[i][jj + j] == 1) {
        break;
      }
      if (std::abs(jj) > 100) {
        break;
      }
      if ((*map_zone)[i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    const int m = map_zone->size();
    for (int ii = 0; ii + i < m; ii++) {
      if ((*map_zone)[ii + i][j] < 0 || (*map_zone)[ii + i][j] == 1) {
        break;
      }
      if (std::abs(ii) > 100) {
        break;
      }
      if ((*map_zone)[ii + i][j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    const int n = (*map_zone)[0].size();
    for (int jj = 0; jj + j < n; jj++) {
      if ((*map_zone)[i][jj + j] < 0 || (*map_zone)[i][jj + j] == 1) {
        break;
      }
      if (std::abs(jj) > 100) {
        break;
      }
      if ((*map_zone)[i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j >= 0; ii--, jj--) {
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
      if (ii * ii + jj * jj > 100 * 100) {
        continue;
      }
      if ((*map_zone)[ii + i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j < n; ii++, jj++) {
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
      if (ii * ii + jj * jj > 100 * 100) {
        continue;
      }
      if ((*map_zone)[ii + i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j < n; ii--, jj++) {
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
      if (ii * ii + jj * jj > 100 * 100) {
        continue;
      }
      if ((*map_zone)[ii + i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j >= 0; ii++, jj--) {
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
      if (ii * ii + jj * jj > 100 * 100) {
        continue;
      }
      if ((*map_zone)[ii + i][jj + j] == 3) {
        (*map_zone)[i][j] = 2;
        return;
      }
    }
    (*map_zone)[i][j] = -3;
  }
  void SearchPathPadMap(const int i, const int j,
                        std::vector<std::vector<int>> *map_zone) {
    for (int ii = 0; ii >= -i; ii--) {
      if ((*map_zone)[ii + i][j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][j] < 0 || (*map_zone)[ii + i][j] == 1) {
        break;
      }
    }
    for (int jj = 0; jj >= -j; jj--) {
      if ((*map_zone)[i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[i][jj + j] < 0 || (*map_zone)[i][jj + j] == 1) {
        break;
      }
    }
    const int m = map_zone->size();
    for (int ii = 0; ii + i < m; ii++) {
      if ((*map_zone)[ii + i][j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][j] < 0 || (*map_zone)[ii + i][j] == 1) {
        break;
      }
    }
    const int n = (*map_zone)[0].size();
    for (int jj = 0; jj + j < n; jj++) {
      if ((*map_zone)[i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[i][jj + j] < 0 || (*map_zone)[i][jj + j] == 1) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j >= 0; ii--, jj--) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j < n; ii++, jj++) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j < n; ii--, jj++) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j >= 0; ii++, jj--) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0 || (*map_zone)[ii + i][jj + j] == 1) {
        break;
      }
    }
    (*map_zone)[i][j] = -3;
  }
  void RemoveBuffer(const int i, const int j,
                    std::vector<std::vector<int>> *map_zone) {
    for (int ii = 0; ii >= -i; ii--) {
      if ((*map_zone)[ii + i][j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][j] < 0) {
        break;
      }
    }
    for (int jj = 0; jj >= -j; jj--) {
      if ((*map_zone)[i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[i][jj + j] < 0) {
        break;
      }
    }
    const int m = map_zone->size();
    for (int ii = 0; ii + i < m; ii++) {
      if ((*map_zone)[ii + i][j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][j] < 0) {
        break;
      }
    }
    const int n = (*map_zone)[0].size();
    for (int jj = 0; jj + j < n; jj++) {
      if ((*map_zone)[i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[i][jj + j] < 0) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j >= 0; ii--, jj--) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j < n; ii++, jj++) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i >= 0 && jj + j < n; ii--, jj++) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0) {
        break;
      }
    }
    for (int ii = 0, jj = 0; ii + i < m && jj + j >= 0; ii++, jj--) {
      if ((*map_zone)[ii + i][jj + j] == 3) {
        return;
      }
      if ((*map_zone)[ii + i][jj + j] < 0) {
        break;
      }
    }
    (*map_zone)[i][j] = -3;
  }
  void DownsampleMap(const std::string &out_file) {
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
        new pcl::PointCloud<pcl::PointXYZI>);
    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f0(
        new pcl::PointCloud<pcl::PointXYZI>);
    map_reader_.Query(map_reader_.map_range(), 2, cloud.get());
    Downsample(cloud, 0.16, cloud_f0);
    pcl::io::savePCDFile(out_file, *cloud_f0, true);
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
    for (const auto &p : cloud->points) {
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
    if (ratio < 1) {
      std::cout << "Downsample: " << cloud->size() << std::endl;
      pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
      voxel_grid_filter.setLeafSize(filter_size, filter_size, filter_size);
      voxel_grid_filter.setInputCloud(cloud);
      voxel_grid_filter.filter(*cloud_f);
      std::cout << "cloud_f size " << cloud_f->size() << std::endl;
    } else {
      ratio = std::sqrt(ratio) + 1;
      std::cout << "split pcd into " << ratio << " x " << ratio
                << "before downnsample" << std::endl;
      for (int i = 0; i < ratio; i++) {
        for (int j = 0; j < ratio; j++) {
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
          for (const auto &p : cloud->points) {
            if (p.x > min_x && p.x < max_x && p.y > min_y && p.y < max_y) {
              part->push_back(p);
            }
          }
          std::cout << "Downsample: " << part->size() << std::endl;
          pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
          voxel_grid_filter.setLeafSize(filter_size, filter_size, filter_size);
          voxel_grid_filter.setInputCloud(part);
          voxel_grid_filter.filter(*part_f);
          for (const auto &p : part_f->points) {
            cloud_f->push_back(p);
          }
          std::cout << "cloud_f size " << cloud_f->size() << std::endl;
        }
      }
    }
  }
};
}  // namespace curb_roi
   //// Step 3. assige curb buff(1)
   // for (int i = 0; i < map_dim_x; i++) {
   //  for (int j = 0; j < map_dim_y; j++) {
   //    if (map_zone[i][j] != -2 && map_zone[i][j] != -1 &&
   //        map_zone[i][j] != 3) {
   //      const double x = i * map_resolution + map_min_x;
   //      const double y = j * map_resolution + map_min_y;
   //      auto c2 = semantic_map::Interface::CurbPointRadiusSearch(x, y,
   // 3);
   //      if (c2.empty()) {
   //        const int k = (3 - 1) / map_resolution;
   //        for (int ii = -k; ii <= k; ii++) {
   //          for (int jj = -k; jj <= k; jj++) {
   //            if (ii + i >= 0 && jj + j >= 0 && ii + i < map_dim_x &&
   //                jj + j < map_dim_y) {
   //              map_zone[ii + i][jj + j] = 3;
   //            }
   //          }
   //        }
   //      } else {
   //        auto c1 = semantic_map::Interface::CurbPointRadiusSearch(x,
   // y, 1);
   //        if (!c1.empty()) {
   //          map_zone[i][j] = 1;
   //        }
   //      }
   //    }
   //  }
   //  std::cout << "Step 3 : " << i << " / " << map_dim_x << std::endl;
   //}
   // for (int i = 0; i < map_dim_x; i++) {
   //  for (int j = 0; j < map_dim_y; j++) {
   //    if (map_zone[i][j] == 3) {
   //      map_zone[i][j] = 0;
   //    }
   //  }
   //}
   // for (int i = 0; i < map_dim_x; i++) {
   //  for (int j = 0; j < map_dim_y; j++) {
   //    if (map_zone[i][j] == 3) {
   //      map_zone[i][j] = 0;
   //    }
   //  }
   //}
   //  void SearchPathPadMap(const int i, const int j, const int wi, const int
   //  wj,
   //                        std::vector<std::vector<int>> *map_zone) {
   //    // source roi points, target waypoint
   //    // connecting throught 0 and 1
   //    // if wi wj is not connected, set all path as -2
   //    const int m = map_zone->size();
   //    const int n = (*map_zone)[0].size();
   //    const int dist_threshold = 2 * (std::abs(i - wi) + std::abs(j - wj));
   //    std::vector<std::pair<int, int>> path;
   //    std::stack<std::pair<int, int>> stk;
   //    stk.push(std::make_pair(i, j));
   //    path.push_back(std::make_pair(i, j));
   //    std::stack<int> depths;
   //    depths.push(0);
   //    bool connected = false;
   //    std::set<std::pair<int, int>> visited;
   //    while (!stk.empty()) {
   //      auto &it = stk.top();
   //      stk.pop();
   //      int depth = depths.top();
   //      depths.pop();
   //      if (depth >= dist_threshold) {
   //        continue;
   //      }
   //      for (int ii = -1; ii <= 1 && !connected; ii++) {
   //        for (int jj = -1; jj <= 1 && !connected; jj++) {
   //          if (ii + it.first >= 0 && jj + it.second >= 0 && ii + it.first <
   //          m
   //          &&
   //              jj + it.second < n) {
   //            if (ii + it.first == wi && jj + it.second == wj) {
   //              connected = true;
   //              break;
   //            }
   //            if ((*map_zone)[ii + it.first][jj + it.second] == 3) {
   //              connected = true;
   //              break;
   //            }
   //            if (visited.find(std::make_pair(ii + it.first, jj +
   //            it.second))
   //            !=
   //                visited.end()) {
   //              continue;
   //            }
   //            if ((*map_zone)[ii + it.first][jj + it.second] == 0 ||
   //                (*map_zone)[ii + it.first][jj + it.second] == 2) {
   //              stk.push(std::make_pair(ii + it.first, jj + it.second));
   //              visited.insert(std::make_pair(ii + it.first, jj +
   //              it.second));
   //              depths.push(depth + 1);
   //              if ((*map_zone)[ii + it.first][jj + it.second] == 2) {
   //                path.push_back(std::make_pair(ii + it.first, jj +
   //                it.second));
   //              }
   //            }
   //          }
   //        }
   //      }
   //    }
   //    if (connected) {
   //      // for (const auto &it : path) {
   //      //  (*map_zone)[it.first][it.second] = 3;
   //      //}
   //    } else {
   //      static int remove_cnt = 0;
   //      std::cout << remove_cnt++ << std::endl;
   //      (*map_zone)[i][j] = -3;
   //      // for (const auto &it : path) {
   //      //  (*map_zone)[it.first][it.second] = -2;
   //      //}
   //    }
   //  }
   //// Step 4: make sure roi are within curb
   // for (int i = 0; i < map_dim_x; i++) {
   //  for (int j = 0; j < map_dim_y; j++) {
   //    if (map_zone[i][j] == 2) {
   //      // make sure roi point is closer to waypoints
   //      const double x = i * map_resolution + map_min_x;
   //      const double y = j * map_resolution + map_min_y;
   //      auto curbs = semantic_map::Interface::CurbPointRadiusSearch(x, y,
   //      1);
   //      if (curbs.empty()) {
   //        // no curb nearby; trust human label
   //        continue;
   //      }
   //      double cloest_to_curb = 1e9;
   //      double curb_x = 0;
   //      double curb_y = 0;
   //      for (const auto &c : curb_gt) {
   //        for (const auto &v : c) {
   //          const double d =
   //              (v(0) - x) * (v(0) - x) + (v(1) - y) * (v(1) - y);
   //          if (d < cloest_to_curb) {
   //            cloest_to_curb = d;
   //            curb_x = v(0);
   //            curb_y = v(1);
   //          }
   //        }
   //        auto wp = semantic_map::Interface::NearestWayPoint(x, y);
   //        const double wx = wp->pose.position.x();
   //        const double wy = wp->pose.position.y();
   //        if ((wx - x) * (wx - x) + (wy - y) * (wy - y) >
   //            (wx - curb_x) * (wx - curb_x) + (wy - curb_y) * (wy -
   //            curb_y)) {
   //          map_zone[i][j] = -1;
   //        }
   //      }
   //    }
   //  }
   //  std::cout << "fix roi " << i << std::endl;
   //}
   //
   //    for (int i = 0; i < map_dim_x; i++) {
   //      for (int j = 0; j < map_dim_y; j++) {
   //        if (IsCandidate(map, i, j)) {
   //          ExpandRoi(i, j, &map);
   //        }
   //        // if (map_zone[i][j] == 2) {
   //        //  int k = SearchCurb(map_zone, i, j);
   //        //  }
   //        //}
   //      }
   //      std::cout << "expand roi " << i << std::endl;
   //    }
   //
   // bool IsCandidate(const std::vector<std::vector<int>> &map_zone, const int
   // i,
   //                 const int j) {
   //  if (map_zone[i][j] != 2) {
   //    return false;
   //  }
   //  const int dim_x = map_zone.size();
   //  const int dim_y = map_zone[0].size();
   //  for (int ii = -1; ii <= 1; ii++) {
   //    for (int jj = -1; jj <= 1; jj++) {
   //      if (map_zone[ii][jj] == -1) {
   //        return true;
   //      }
   //    }
   //  }
   //  return false;
   //}
   //    // assume roi and label difference are < 1m, so all roi region
   // are
   //    valid
   //    // Step 5: make sure roi are within curb and expand roi between
   //    labeled roi
   //    // and curb
   //    for (int i = 0; i < map_dim_x; i++) {
   //      for (int j = 0; j < map_dim_y; j++) {
   //        if (map_zone[i][j] == 2) {
   //          ExpandRoi(i, j, &map_zone);
   //        }
   //      }
   //      std::cout << "Step 5 : " << i << " / " << map_dim_x <<
   //    std::endl;
   //    }
   //    //    // Step 6: remove non-roi buffer (if buffer is not
   // connected to
   //    roi
   //    //    for (int i = 0; i < map_dim_x; i++) {
   //    //      for (int j = 0; j < map_dim_y; j++) {
   //    //        if (map_zone[i][j] == 1) {
   //    //          RemoveNonRoiBuffer(i, j, &map_zone);
   //    //        }
   //    //      }
   //    //      std::cout << "Step 6 : " << i << " / " << map_dim_x <<
   //    std::endl;
   //    //    }
   //    //
   // void ExpandRoi(const int i, const int j,
   //               std::vector<std::vector<int>> *map_zone, int depth) {
   //  if(depth > 4){
   //    return;
   //  }
   //  const int m = map_zone->size();
   //  const int n = (*map_zone)[0].size();
   //  std::cout << i << " " << j << " " << m << " " << n << std::endl;
   //  for (int ii = -1; ii <= 1; ii++) {
   //    for (int jj = -1; jj <= 1; jj++) {
   //      if (ii + i >= 0 && jj + j >= 0 && ii + i < m && jj + j < n) {
   //        if ((*map_zone)[ii + i][jj + j] == 0) {
   //          (*map_zone)[ii + i][jj + j] = 2;
   //          ExpandRoi(ii + i, jj + j, map_zone, depth++);
   //        }
   //      }
   //    }
   //  }
   //}
   //  void ExpandRoi(const int i, const int j,
   //                 std::vector<std::vector<int>> *map_zone) {
   //    const int m = map_zone->size();
   //    const int n = (*map_zone)[0].size();
   //
   //    std::stack<std::pair<int, int>> stk;
   //    stk.push(std::make_pair(i, j));
   //    std::stack<int> depths;
   //    depths.push(0);
   //    while (!stk.empty()) {
   //      auto &it = stk.top();
   //      stk.pop();
   //      int depth = depths.top();
   //      depths.pop();
   //      if (depth >= 5) {
   //        continue;
   //      }
   //      for (int ii = -1; ii <= 1; ii++) {
   //        for (int jj = -1; jj <= 1; jj++) {
//          if (ii + it.first >= 0 && jj + it.second >= 0 && ii + it.first < m
//          &&
//              jj + it.second < n) {
//            if ((*map_zone)[ii + it.first][jj + it.second] == 0) {
//              (*map_zone)[ii + it.first][jj + it.second] = 2;
//              stk.push(std::make_pair(ii + it.first, jj + it.second));
//              depths.push(depth + 1);
//            }
//          }
//        }
//      }
//    }
//  }
//  void RemoveNonRoiBuffer(const int i, const int j,
//                          std::vector<std::vector<int>> *map_zone) {
//    const int m = map_zone->size();
//    const int n = (*map_zone)[0].size();
//    for (int ii = -1; ii <= 1; ii++) {
//      for (int jj = -1; jj <= 1; jj++) {
//        if (ii + i >= 0 && jj + j >= 0 && ii + i < m && jj + j < n) {
//          if ((*map_zone)[ii + i][jj + j] == 2) {
//            (*map_zone)[i][j] = 2;
//            return;
//          } else if ((*map_zone)[ii + i][jj + j] <= 0) {
//            continue;
//          } else {
//            RemoveNonRoiBuffer(ii + j, jj + j, map_zone);
//            if ((*map_zone)[ii + i][jj + j] == 2) {
//              (*map_zone)[i][j] = 2;
//            }
//          }
//        }
//      }
//    }
//  }
//  void Run(const std::string &pcd_file, const std::string &pcd_roi_file,
//           const std::string &out_map_file, const std::string
//           &out_ground_file,
//           const std::string &out_ground_filtered_file,
//           const double ground_filter_size, const double median_filter_size) {
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f0(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
//        new pcl::PointCloud<pcl::PointXYZI>);
//
//    map_reader_.Query(map_reader_.map_range(), 2, cloud.get());
//    //pcl::io::loadPCDFile(pcd_file, *cloud);
//    Downsample(cloud, 0.2, cloud_f0);
//    for(const auto & p: cloud_f0->points){
//      pcl::PointXYZI pt;
//      pt.x = p.x;
//      pt.y = y_flip_ * p.y;
//      pt.z = p.z;
//      pt.intensity = p.intensity;
//      cloud_f->push_back(p);
//    }
//    int cnt = 0;
//    for (const auto &pt : cloud_f->points) {
//      if (cnt % 100000 == 0) {
//        LOG(INFO) << cnt * 1.0 / cloud_f->points.size() << std::endl;
//      }
//      for (const auto &polygon : opencv_polygons_) {
//        if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, y_flip_ * pt.y),
//                                 false) >= 0) {
//          cloud_roi_ptr->push_back(pt);
//          break;
//        }
//      }
//      cnt++;
//    }
//
//    pcl::io::savePCDFile(pcd_roi_file, *cloud_roi_ptr, true);
//
//    EstimateGround(cloud_roi_ptr, out_map_file, out_ground_file,
//                   out_ground_filtered_file, ground_filter_size,
//                   median_filter_size);
//  }
//  void ShowCSloop(const int map_min_x, const int map_min_y, const int
//  map_max_x,
//                  const int map_max_y) {
//    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
//    cv::Mat mat;
//    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, &mat);
//    cv::imshow("image", mat);
//    cv::waitKey(0);
//
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1,
//                      cloud.get());
//
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
//    voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//    voxel_grid_filter.setInputCloud(cloud);
//    voxel_grid_filter.filter(*cloud_f);
//
//    pcd_viewer_->setBackgroundColor(0, 0, 0);
//    pcd_viewer_->addCoordinateSystem(1.0);
//    pcd_viewer_->initCameraParameters();
//    ShowPointCloud(cloud_f, 255, 5, 5, "roi", pcd_viewer_count_++,
//    pcd_viewer_);
//    pcd_viewer_->spin();
//  }
//
//  void ShowCSloopRoi(const int map_min_x, const int map_min_y,
//                     const int map_max_x, const int map_max_y) {
//    cv::Mat mat;
//
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    map_reader_.Query(map_min_x, -map_max_y, map_max_x, -map_min_y, 1,
//                      cloud.get());
//
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_f(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    pcl::VoxelGrid<pcl::PointXYZI> voxel_grid_filter;
//    voxel_grid_filter.setLeafSize(0.2, 0.2, 0.2);
//    voxel_grid_filter.setInputCloud(cloud);
//    voxel_grid_filter.filter(*cloud_f);
//
//    std::vector<std::vector<cv::Point2f>> polygons;
//    ShrinkPolygonToROI(map_min_x, map_min_y, map_max_x, map_max_y, &polygons);
//
//    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud_roi_ptr(
//        new pcl::PointCloud<pcl::PointXYZI>);
//    int cnt = 0;
//    for (const auto &pt : cloud_f->points) {
//      if (cnt % 1000 == 0) {
//        std::cout << cnt * 1.0 / cloud_f->points.size() << std::endl;
//      }
//      for (const auto &polygon : polygons) {
//        if (cv::pointPolygonTest(polygon, cv::Point2f(pt.x, -pt.y), false) >=
//            0) {
//          cloud_roi_ptr->push_back(pt);
//          break;
//        }
//      }
//      cnt++;
//    }
//
//    pcd_viewer_->setBackgroundColor(0, 0, 0);
//    pcd_viewer_->addCoordinateSystem(1.0);
//    pcd_viewer_->initCameraParameters();
//    ShowPointCloud(cloud_roi_ptr, 255, 5, 5, "roi", pcd_viewer_count_++,
//                   pcd_viewer_);
//    pcd_viewer_->spin();
//  }
//
//  std::vector<std::vector<double>> MedianFilter(
//      const std::vector<std::vector<double>> &in_map,
//      const std::vector<std::vector<bool>> &in_flag, const int k) {
//    const int hk = k / 2;
//    const int m = in_map.size();
//    const int n = in_map[0].size();
//    std::vector<std::vector<double>> res(m, std::vector<double>(n, 0));
//    for (int i = 0; i < m; i++) {
//      for (int j = 0; j < n; j++) {
//        std::vector<double> vec;
//        const int i_start = std::max(0, i - hk);
//        const int i_end = std::min(m, i + hk);
//        const int j_start = std::max(0, j - hk);
//        const int j_end = std::min(n, j + hk);
//        for (int ii = i_start; ii < i_end; ii++) {
//          for (int jj = j_start; jj < j_end; jj++) {
//            if (in_flag[ii][jj]) {
//              vec.push_back(in_map[ii][jj]);
//            }
//          }
//        }
//        if (!vec.empty()) {
//          std::nth_element(vec.begin(), vec.begin() + vec.size() / 2,
//                           vec.end());
//          res[i][j] = vec[vec.size() / 2];
//        }
//      }
//    }
//    return res;
//  }
//  void EstimateGround(const pcl::PointCloud<pcl::PointXYZI>::Ptr map_ptr,
//                      const std::string &out_map_file,
//                      const std::string &out_ground_file,
//                      const std::string &out_ground_filtered_file,
//                      const double ground_filter_size,
//                      const double median_filter_size) {
//    pcl::PointCloud<pcl::PointXYZ>::Ptr map_xyz_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    pcl::PointCloud<pcl::PointXYZ>::Ptr map_ground_ptr(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    double min_x = std::numeric_limits<double>::max();
//    double min_y = std::numeric_limits<double>::max();
//    double max_x = -std::numeric_limits<double>::max();
//    double max_y = -std::numeric_limits<double>::max();
//    for (const auto &p : map_ptr->points) {
//      pcl::PointXYZ pt;
//      pt.x = p.x;
//      pt.y = p.y;  // no flip pcd !!
//      pt.z = p.z;
//      map_xyz_ptr->push_back(pt);
//      min_x = p.x < min_x ? p.x : min_x;
//      min_y = y_flip_ * p.y < min_y ?y_flip_ *  p.y : min_y;  // no flip pcd
//      !!
//
//      max_x = p.x > max_x ? p.x : max_x;
//      max_y = y_flip_ * p.y > max_y ? y_flip_ * p.y : max_y;  // no  flip pcd
//      !!
//
//      pt.z = 0;
//      map_ground_ptr->push_back(pt);
//    }
//    std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
//              << std::endl;
//    std::cout << ground_filter_size << std::endl;
//
//    std::cout << "kdtree points : " << map_ground_ptr->size() << std::endl;
//    pcl::KdTreeFLANN<pcl::PointXYZ> kdtree;
//    kdtree.setInputCloud(map_ground_ptr);
//
//    std::vector<int> pointIdxRadiusSearch;
//    std::vector<float> pointRadiusSquaredDistance;
//
//    const int dim_x = (max_x - min_x) / ground_filter_size + 1;
//    const int dim_y = (max_y - min_y) / ground_filter_size + 1;
//
//    std::vector<std::vector<double>> map_sum(dim_x,
//                                             std::vector<double>(dim_y, 0));
//    std::vector<std::vector<bool>> map(dim_x, std::vector<bool>(dim_y,
//    false));
//
//    int cnt = 0;
//
//    double last_mean_z = 0;
//    for (int idx_x = 0; idx_x < dim_x; idx_x++) {
//      std::cout << idx_x << " / " << dim_x << " " << cnt << std::endl;
//      for (int idx_y = 0; idx_y < dim_y; idx_y++) {
//        const double x = idx_x * ground_filter_size + min_x;
//        const double y = idx_y * ground_filter_size + min_y;
//        for (const auto &polygon : opencv_polygons_) {
//          if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) >= 0) {
//            pointIdxRadiusSearch.clear();
//            pointRadiusSquaredDistance.clear();
//            pcl::PointXYZ p(x, y, 0);
//            map[idx_x][idx_y] = true;
//            map_sum[idx_x][idx_y] = last_mean_z;  // if there is no lidar
//            point
//                                                  // in roi region, use values
//                                                  // from previous state
//            if (kdtree.radiusSearch(p, 1, pointIdxRadiusSearch,
//                                    pointRadiusSquaredDistance) > 0) {
//              double mean_z = 0;
//              for (const auto idx : pointIdxRadiusSearch) {
//                mean_z += map_xyz_ptr->points[idx].z;
//              }
//              last_mean_z = mean_z / pointIdxRadiusSearch.size();
//              map_sum[idx_x][idx_y] = last_mean_z;
//              cnt++;
//            }
//            break;
//          }
//        }
//      }
//    }
//
//    std::ofstream of;
//    of.open(out_ground_file);
//    std::cout << out_ground_file << std::endl;
//
//    of << min_x << " " << min_y << " " << ground_filter_size << " " << dim_x
//       << " " << dim_y << std::endl;
//    for (int i = 0; i < dim_x; i++) {
//      for (int j = 0; j < dim_y; j++) {
//        if (map[i][j]) {
//          of << i * dim_y + j << " " << map_sum[i][j] << std::endl;
//        }
//      }
//    }
//    of.close();
//
//    pcl::PointCloud<pcl::PointXYZ>::Ptr map_save(
//        new pcl::PointCloud<pcl::PointXYZ>);
//    for (int i = 0; i < dim_x; i++) {
//      for (int j = 0; j < dim_y; j++) {
//        if (map[i][j]) {
//          pcl::PointXYZ p;
//          p.x = i * ground_filter_size + min_x;
//          p.y = j * ground_filter_size + min_y;
//          p.z = map_sum[i][j];
//          map_save->push_back(p);
//        }
//      }
//    }
//    std::cout << map_save->size() << std::endl;
//    pcl::io::savePCDFile(out_map_file, *map_save, true);
//    std::cout << out_map_file << std::endl;
//
//    std::vector<std::vector<double>> f_map =
//        MedianFilter(map_sum, map,
//                     static_cast<int>(median_filter_size /
//                     ground_filter_size));
//    std::ofstream ofs;
//    ofs.open(out_ground_filtered_file);
//
//    ofs << min_x << " " << min_y << " " << ground_filter_size << " " << dim_x
//        << " " << dim_y << std::endl;
//    for (int i = 0; i < dim_x; i++) {
//      for (int j = 0; j < dim_y; j++) {
//        if (map[i][j]) {
//          ofs << i * dim_y + j << " " << f_map[i][j] << std::endl;
//        }
//      }
//    }
//    ofs.close();
//  }
//  void FilterGroundFiles() {
//    double ground_min_x, ground_min_y, ground_resolution;
//    int ground_dim_x, ground_dim_y;
//    std::unordered_map<int, double> ground_map_vec;
//
//    std::ifstream ifs("/home/host/data/1x1_map/ground_1x1_1023_filtered.txt");
//    if (!ifs.is_open()) {
//      ROS_ERROR("cannot read: file ; Please download it from
//      /media/nas/miao");
//      return;
//    }
//    ifs >> ground_min_x;
//    ifs >> ground_min_y;
//    ifs >> ground_resolution;
//    ifs >> ground_dim_x;
//    ifs >> ground_dim_y;
//    int key;
//    double val;
//
//    while (!ifs.eof()) {
//      ifs >> key;
//      if (ifs.eof()) {
//        break;
//      }
//      ifs >> val;
//      ground_map_vec[key] = val;
//    }
//
//    std::ofstream of;
//    of.open("/home/host/data/1x1_map/ground_1x1_1119.txt");
//    of << ground_min_x << " " << ground_min_y << " " << ground_resolution << "
//    "
//       << ground_dim_x << " " << ground_dim_y << std::endl;
//    int cnt = 0;
//    int cnt_roi = 0;
//    int tol = ground_map_vec.size();
//    for (const auto it : ground_map_vec) {
//      const int j = it.first % ground_dim_y;
//      const int i = it.first / ground_dim_y;
//      const double x = i * ground_resolution + ground_min_x;
//      const double y = j * ground_resolution + ground_min_y;
//      if (cnt % 10000 == 0) {
//        LOG(INFO) << cnt * 1.0f / tol << " " << 1.0f * cnt_roi / cnt;
//      }
//      for (const auto &polygon : opencv_polygons_) {
//        if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) >= 0) {
//          of << i * ground_dim_y + j << " " << it.second << std::endl;
//          cnt_roi++;
//
//          break;
//        }
//      }
//      cnt++;
//    }
//
//    of.close();
//  }
//  void ShrinkPolygonToROI(const int map_min_x, const int map_min_y,
//                          const int map_max_x, const int map_max_y,
//                          std::vector<std::vector<cv::Point2f>> *polygons) {
//    for (const auto &vec : opencv_polygons_) {
//      if (vec.size() < 3) continue;
//      int min_x = vec[0].x;
//      int min_y = vec[0].y;
//      int max_x = vec[0].x;
//      int max_y = vec[0].y;
//      for (const auto p : vec) {
//        min_x = std::min(int(p.x), min_x);
//        min_y = std::min(int(p.y), min_y);
//        max_x = std::max(int(p.x), max_x);
//        max_x = std::max(int(p.y), max_y);
//      }
//      int ov_min_x = std::max(int(min_x), map_min_x);
//      int ov_min_y = std::max(int(min_y), map_min_y);
//      int ov_max_x = std::min(int(max_x), map_max_x);
//      int ov_max_y = std::min(int(max_y), map_max_y);
//      if (ov_min_x >= ov_max_x || ov_min_y >= ov_max_y) {
//        continue;
//      }
//      polygons->push_back(vec);
//    }
//  }
//  void CheckGround(const std::string& ground_file){
//    std::cout << "show png" << std::endl;
//    map_reader_.Visualize();
//    std::cout << " ROI polygons: " << opencv_polygons_.size() << std::endl;
//    for (size_t i = 0; i < 5; i++) {
//      double min_x = 1e90;
//      double max_x = -1e90;
//      double min_y = 1e90;
//      double max_y = -1e90;
//      for (const auto &p : opencv_polygons_[i]) {
//        std::cout << p.x << " " << p.y << " ";
//        min_x = std::min(double(p.x), min_x);
//        min_y = std::min(double(y_flip_ * p.y), min_y);
//        max_x = std::max(double(p.x), max_x);
//        max_y = std::max(double(y_flip_ * p.y), max_y);
//      }
//      std::cout << std::endl;
//      std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
//                << std::endl;
//      cv::Mat mat;
//      map_reader_.Query(min_x, min_y, max_x, max_y, &mat);
//      cv::imshow("image" , mat);
//      cv::waitKey(0);
//    }
//  }

// pcl::PointCloud<pcl::PointXYZ>::Ptr curb_in_world_ptr(
//    new pcl::PointCloud<pcl::PointXYZ>);
// curb_in_world_ptr->reserve(curb_all_points_size);
// for (const auto &c : curb_gt) {
//  for (const auto &p : c) {
//    curb_in_world_ptr->push_back(pcl::PointXYZ(p(0), p(1), 0));
//  }
//}
// pcd_viewer_->setBackgroundColor(0, 0, 0);
// pcd_viewer_->addCoordinateSystem(1.0);
// pcd_viewer_->initCameraParameters();
// ShowPointCloud(curb_in_world_ptr, 255, 0, 0, "curb",
// pcd_viewer_count_++,
//               pcd_viewer_);
// pcd_viewer_->spin();

// pcl::PointCloud<pcl::PointXYZ>::Ptr roi_in_world_ptr(
//    new pcl::PointCloud<pcl::PointXYZ>);
// for (const auto &polygon : opencv_polygons_) {
//  // for(const auto &p:polygon){
//  //  roi_in_world_ptr->push_back(pcl::PointXYZ(p.x, p.y, 0));
//  //}
//  float min_x = std::numeric_limits<float>::max();
//  float min_y = std::numeric_limits<float>::max();
//  float max_x = std::numeric_limits<float>::lowest();
//  float max_y = std::numeric_limits<float>::lowest();
//  for (const auto &p : polygon) {
//    min_x = std::min(min_x, p.x);
//    min_y = std::min(min_y, p.y);
//    max_x = std::max(max_x, p.x);
//    max_y = std::max(max_y, p.y);
//  }
//  for (double x = min_x; x < max_x; x += 0.1) {
//    for (double y = min_y; y < max_y; y += 0.1) {
//      if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) >= 0)
//      {
//        roi_in_world_ptr->push_back(pcl::PointXYZ(x, y, 0));
//      }
//    }
//  }
//}
// ShowPointCloud(roi_in_world_ptr, 0, 255, 0, "roi", pcd_viewer_count_++,
//               pcd_viewer_);
// pcd_viewer_->spin();

// for (size_t i = 0; i < 5; i++) {
//  double min_x = 1e90;
//  double max_x = -1e90;
//  double min_y = 1e90;
//  double max_y = -1e90;
//  for (const auto &p : opencv_polygons_[i]) {
//    std::cout << p.x << " " << p.y << " ";
//    min_x = std::min(double(p.x), min_x);
//    min_y = std::min(double(-1 * p.y), min_y);
//    max_x = std::max(double(p.x), max_x);
//    max_y = std::max(double(-1 * p.y), max_y);
//  }
//  std::cout << std::endl;
//  std::cout << min_x << " " << min_y << " " << max_x << " " << max_y
//            << std::endl;
//  cv::Mat mat;
//  // map_reader_.Query(min_x, min_y, max_x, max_y, &mat);
//  // cv::imshow("image" , mat);
//  // cv::waitKey(0);
//  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(
//      new pcl::PointCloud<pcl::PointXYZI>);
//  map_reader_.Query(min_x, min_y, max_x, max_y, cloud.get());

//  pcd_viewer_->setBackgroundColor(0, 0, 0);
//  pcd_viewer_->addCoordinateSystem(1.0);
//  pcd_viewer_->initCameraParameters();
//  ShowPointCloud(cloud, 255, 5, 5, "roi", pcd_viewer_count_++,
//  pcd_viewer_);
//  pcd_viewer_->spin();
//}
//
//   // Step 2: assign curb (-1)
//   for (int i = 0; i < map_dim_x; i++) {
//     for (int j = 0; j < map_dim_y; j++) {
//       if ((*map_zone)[i][j] == 0) {
//         const double x = i * map_resolution + map_min_x;
//         const double y = j * map_resolution + map_min_y;
//         auto c = semantic_map::Interface::CurbPointRadiusSearch(x, y, 0.3);
//         if (!c.empty()) {
//           (*map_zone)[i][j] = -1;
//         }
//       }
//     }
//     std::cout << "Step 2 : " << i << " / " << map_dim_x << std::endl;
//   }
//   // Step 3. assige curb buff(1)
//   for (int i = 0; i < map_dim_x; i++) {
//     for (int j = 0; j < map_dim_y; j++) {
//       if ((*map_zone)[i][j] == 0) {
//         const double x = i * map_resolution + map_min_x;
//         const double y = j * map_resolution + map_min_y;
//         auto c1 = semantic_map::Interface::CurbPointRadiusSearch(x, y, 1);
//         if (!c1.empty()) {
//           (*map_zone)[i][j] = 1;
//         }
//       }
//     }
//     std::cout << "Step 3 : " << i << " / " << map_dim_x << std::endl;
//   }
//   // Step 4: assign roi (2)
//   cnt = 0;
//   for (const auto &polygon : opencv_polygons_) {
//     float min_x = std::numeric_limits<float>::max();
//     float min_y = std::numeric_limits<float>::max();
//     float max_x = std::numeric_limits<float>::lowest();
//     float max_y = std::numeric_limits<float>::lowest();
//     for (const auto &p : polygon) {
//       min_x = std::min(min_x, p.x);
//       min_y = std::min(min_y, p.y);
//       max_x = std::max(max_x, p.x);
//       max_y = std::max(max_y, p.y);
//     }
//     const int min_i = (min_x - map_min_x) / map_resolution;
//     const int min_j = (min_y - map_min_y) / map_resolution;
//     const int max_i = (max_x - map_min_x) / map_resolution;
//     const int max_j = (max_y - map_min_y) / map_resolution;
//     for (int i = min_i; i <= max_i; i++) {
//       for (int j = min_j; j <= max_j; j++) {
//         if (i < 0 || j < 0 || i >= map_dim_x || j >= map_dim_y) {
//           continue;
//         }
//         if ((*map_zone)[i][j] != 0) {
//           continue;
//         }
//         const float x = i * map_resolution + map_min_x;
//         const float y = j * map_resolution + map_min_y;
//         if (cv::pointPolygonTest(polygon, cv::Point2f(x, y), false) > 0) {
//           (*map_zone)[i][j] = 2;
//         }
//       }
//     }
//     cnt++;
//     std::cout << "Step 4 : " << cnt << " / " << opencv_polygons_.size()
//               << std::endl;
//   }
//   // Step 5: make sure roi are within curb
//   for (int i = 0; i < map_dim_x; i++) {
//     for (int j = 0; j < map_dim_y; j++) {
//       if ((*map_zone)[i][j] == 2) {
//         SearchPathPadMap(i, j, map_zone);
//       }
//     }
//     std::cout << "Step 5 : " << i << " / " << map_dim_x << std::endl;
//   }

//   // Step 6:expand roi
//   for (int i = 0; i < map_dim_x; i++) {
//     for (int j = 0; j < map_dim_y; j++) {
//       if ((*map_zone)[i][j] == 0) {
//         ExpandRoi(i, j, map_zone);
//       }
//     }
//     std::cout << "Step 6 : " << i << " / " << map_dim_x << std::endl;
//   }

//   // Step 7: remove non-roi buffer
//   for (int i = 0; i < map_dim_x; i++) {
//     for (int j = 0; j < map_dim_y; j++) {
//       if ((*map_zone)[i][j] == 1) {
//         RemoveBuffer(i, j, map_zone);
//       }
//     }
//     std::cout << "Step 7 : " << i << " / " << map_dim_x << std::endl;
//   }
//   // pcl::PointCloud<pcl::PointXYZRGB>::Ptr rgb_cloud(
//   //    new pcl::PointCloud<pcl::PointXYZRGB>);
//   // for (int i = 0; i < map_dim_x; i++) {
//   //  for (int j = 0; j < map_dim_y; j++) {
//   //    if ((*map_zone)[i][j] == -2) {
//   //      continue;
//   //    }
//   //    const double x = i * map_resolution + map_min_x;
//   //    const double y = j * map_resolution + map_min_y;
//   //    pcl::PointXYZRGB cpt;
//   //    cpt.x = x;
//   //    cpt.y = y;
//   //    cpt.z = 0;
//   //    if ((*map_zone)[i][j] == -1) {
//   //      cpt.r = 255;
//   //      cpt.g = 0;
//   //      cpt.b = 0;
//   //    } else if ((*map_zone)[i][j] == 1) {
//   //      cpt.r = 0;
//   //      cpt.g = 255;
//   //      cpt.b = 0;
//   //    } else if ((*map_zone)[i][j] == 2) {
//   //      cpt.r = 0;
//   //      cpt.g = 0;
//   //      cpt.b = 255;
//   //    } else if ((*map_zone)[i][j] == 0) {
//   //      cpt.r = 255;
//   //      cpt.g = 255;
//   //      cpt.b = 255;
//   //    } else if ((*map_zone)[i][j] == -3) {
//   //      cpt.r = 255;
//   //      cpt.g = 0;
//   //      cpt.b = 255;
//   //    } else if ((*map_zone)[i][j] == 3) {
//   //      cpt.r = 255;
//   //      cpt.g = 255;
//   //      cpt.b = 0;
//   //    }
//   //    rgb_cloud->push_back(cpt);
//   //  }
//   //}
//   // pcd_viewer_->addPointCloud(rgb_cloud, "map", 0);
//   // pcd_viewer_->spin();

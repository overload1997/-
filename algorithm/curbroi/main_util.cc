#include "tools/curbroi/main_util.h"

bool ReadFileN(const std::string &filename, const int n,
               std::vector<std::vector<double>> *out) {
  std::ifstream ifs(filename.c_str());
  if (!ifs.is_open()) {
    std::cout << "Failed to open file: " << filename;
    return false;
  }
  double tmp_d;
  while (!ifs.eof()) {
    std::vector<double> one_vec;
    for (int i = 0; i < n; i++) {
      ifs >> tmp_d;
      if (i == 0 && ifs.eof()) {
        return true;
      }
      one_vec.push_back(tmp_d);
    }
    out->push_back(one_vec);
  }
  return true;
}

bool GetFileNamesInFolder(const std::string &folder, const std::string &ext,
                          std::vector<std::string> *file_names) {
  namespace fs = boost::filesystem;
  if (!fs::exists(folder) || !fs::is_directory(folder)) {
    return false;
  }

  fs::directory_iterator it(folder);
  fs::directory_iterator endit;

  while (it != endit) {
    if (fs::is_regular_file(*it) && it->path().extension() == ext) {
      file_names->push_back(it->path().filename().string());
    }
    ++it;
  }
  std::sort(file_names->begin(), file_names->end());
  return true;
}

bool VectorHomoMatrix(const std::vector<double> &vec, Eigen::Matrix4d *mat4d) {
  Eigen::Quaternion<double> tmp_quat(vec[4], vec[5], vec[6], vec[7]);
  Eigen::Matrix3d tmp_mat3d = tmp_quat.toRotationMatrix();

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      (*mat4d)(i, j) = tmp_mat3d(i, j);
    }
  }
  (*mat4d)(0, 3) = vec[1];
  (*mat4d)(1, 3) = vec[2];
  (*mat4d)(2, 3) = vec[3];
  (*mat4d)(3, 3) = 1;
  return true;
}

bool SearchPoseWithTime(const std::vector<std::vector<double>> &poses,
                        const double query_time, Eigen::Matrix4d *mat4d) {
  int idx = -1;
  double minV = 1;
  for (size_t i = 0; i < poses.size(); i++) {
    if (poses[i][0] - query_time < -0.1) {
      continue;
    }
    if (poses[i][0] - query_time > 0.1) {
      break;
    }
    if (minV > std::fabs(poses[i][0] - query_time)) {
      minV = std::fabs(poses[i][0] - query_time);
      idx = i;
    }
  }
  if (minV < 1) {
    VectorHomoMatrix(poses[idx], mat4d);
    return true;
  }
  return false;
}

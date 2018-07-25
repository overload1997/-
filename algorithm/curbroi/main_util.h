#pragma once
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "Eigen/Dense"
#include "boost/filesystem.hpp"

bool ReadFileN(const std::string &filename, const int n,
               std::vector<std::vector<double>> *maptxt);

bool GetFileNamesInFolder(const std::string &folder, const std::string &ext,
                          std::vector<std::string> *file_names);

bool VectorHomoMatrix(const std::vector<double> &vec, Eigen::Matrix4d *mat4d);

bool SearchPoseWithTime(const std::vector<std::vector<double>> &poses,
                        const double query_time, Eigen::Matrix4d *mat4d);


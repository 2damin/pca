#include <iostream>

#include "datatype.hpp"
#include "pca.hpp"

#include "Eigen/Dense"

using namespace Eigen;

std::vector<float> PCA::getEigenValue(std::shared_ptr<SimpleBBoxes> points) {
  auto eigenvalue = std::vector<float>(4, 0.0);

  auto size = static_cast<int>(points->size());

  if (size == 0)
    return eigenvalue;

  MatrixXf midPoints = MatrixXf::Zero(size, 2);

  // Calculate meanPoint value and midPoints Matrix
  int i = 0;
  float mean_x = 0, mean_y = 0;
  for (auto p : *points) {
    float mid_x = p.x + p.w / 2;
    float mid_y = p.y + p.h / 2;
    midPoints(i, 0) = mid_x;
    midPoints(i, 1) = mid_y;
    mean_x += mid_x;
    mean_y += mid_y;
    ++i;
  }
  mean_x /= size;
  mean_y /= size;

  // make meanPoints matrix
  MatrixXf meanPoints = MatrixXf::Zero(size, 2);
  for (int j = 0; j < size; j++) {
    meanPoints(j, 0) = mean_x;
    meanPoints(j, 1) = mean_y;
  }

  midPoints -= meanPoints;

  auto coMat = midPoints.transpose() * midPoints / (size);

  EigenSolver<MatrixXf> es(coMat);

  auto eivals = coMat.eigenvalues();

  auto eigenv = es.eigenvectors().col(0);

  std::cout << midPoints << std::endl;
  std::cout << "-----" << std::endl;

  std::cout << coMat << std::endl;
  std::cout << eivals << std::endl;
  std::cout << eigenv << std::endl;

  eigenvalue[0] = coMat(0, 0);
  eigenvalue[1] = coMat(0, 1);
  eigenvalue[2] = coMat(1, 0);
  eigenvalue[3] = coMat(1, 1);
  return eigenvalue;
}

bool PCA::getTwoLine(std::shared_ptr<SimpleBBoxes> points) {

  auto size = static_cast<int>(points->size());

  if (size == 0)
    return false;

  MatrixXf midPoints = MatrixXf::Zero(size, 2);

  // Calculate meanPoint value and midPoints Matrix
  int i = 0;
  float mean_x = 0, mean_y = 0;
  for (auto p : *points) {
    float mid_x = p.x + p.w / 2;
    float mid_y = p.y + p.h / 2;
    midPoints(i, 0) = mid_x;
    midPoints(i, 1) = mid_y;
    mean_x += mid_x;
    mean_y += mid_y;
    ++i;
  }
  mean_x /= size;
  mean_y /= size;

  // make meanPoints matrix
  MatrixXf meanPoints = MatrixXf::Zero(size, 2);
  for (int j = 0; j < size; j++) {
    meanPoints(j, 0) = mean_x;
    meanPoints(j, 1) = mean_y;
  }
  midPoints -= meanPoints;

  auto coMat = midPoints.transpose() * midPoints / (size);

  EigenSolver<MatrixXf> es(coMat);

  auto eivals = coMat.eigenvalues();

  auto eigenv = es.eigenvectors().col(0);

  // eigenvalue
  if (eigenv(1, 0).real() > 0.1)
    return true;
  std::cout << eigenv(1, 0).real() << std::endl;

  return false;
}
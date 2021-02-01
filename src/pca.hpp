#ifndef pca_HPP_
#define pca_HPP_

#include <memory>
#include <vector>

class SimpleBBox;

typedef std::vector<SimpleBBox> SimpleBBoxes;

class PCA {
public:
  PCA(){};

  std::vector<float> getEigenValue(std::shared_ptr<SimpleBBoxes> points);

  bool getTwoLine(std::shared_ptr<SimpleBBoxes> points);
};

#endif /* pca_HPP_ */
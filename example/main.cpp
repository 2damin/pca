#include <fstream>
#include <iostream>
#include <sstream>

#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"

#include "pca.hpp"

#include "datatype.hpp"

std::vector<std::string> split(std::string str, char delimiter);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "[ERROR] .EXE testlist" << std::endl;
    return 0;
  }

  std::ifstream txtfile(argv[1]);

  if (!txtfile.is_open()) {
    std::cout << "can not read testlist" << std::endl;
    return 0;
  }

  auto Pca = PCA();
  std::string buffer;
  while (std::getline(txtfile, buffer)) {
    std::ifstream eachfile(buffer);

    auto plain_name = buffer.substr(0, buffer.find_last_of(".txt") - 3);
    auto jpg_name = plain_name + std::string(".jpg");
    auto img = cv::imread(jpg_name);

    std::cout << jpg_name << std::endl;

    if (!eachfile.is_open()) {
      std::cout << "can not read txtfile" << std::endl;
      break;
    }
    auto sboxes = std::make_shared<SimpleBBoxes>();
    std::string eachBuffer;
    while (std::getline(eachfile, eachBuffer)) {
      std::vector<std::string> split_txt = split(eachBuffer, ' ');
      SimpleBBox box;
      box.obj_id = std::stoi(split_txt[0]);
      box.x = std::stof(split_txt[1]) * img.cols;
      box.y = std::stof(split_txt[2]) * img.rows;
      box.w = std::stof(split_txt[3]) * img.cols;
      box.h = std::stof(split_txt[4]) * img.rows;
      sboxes->push_back(box);
      eachBuffer.clear();
    }

    eachfile.close();
    auto eigen = Pca.getEigenValue(sboxes);
    cv::imshow("ocr", img);
    cv::waitKey(0);
    buffer.clear();
  }

  txtfile.close();

  return 0;
}

std::vector<std::string> split(std::string input, char delimiter) {
  std::vector<std::string> answer;
  std::stringstream ss(input);
  std::string temp;

  while (std::getline(ss, temp, delimiter)) {
    answer.push_back(temp);
  }

  return answer;
}
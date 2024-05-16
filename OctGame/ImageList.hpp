#pragma once

#include "octgamedef.hpp"

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class OCT_EXPORTS ImageList {
public:
    int LoadImageFile(std::string filepath, bool isBmp=false);
    int LoadImageFile(std::string filepath, float sx, float sy, bool isBmp=false);
    int LoadRegionImageFile(std::string filepath, int x, int y, int n, bool isBmp=false);
    int LoadRegionImageFile(std::string filepath, float sx, float sy, int x, int y, int n, bool isBmp=false);
    cv::Mat* GetImage(int handle);
private:
    std::vector<cv::Mat> mImageList;
};

#include "ImageManager.hpp"

namespace {
    cv::Mat readImage(string filepath, bool isBmp) {
        cv::Mat img;

        img = cv::imread(filepath, cv::IMREAD_UNCHANGED);
        if(isBmp) {
            cv::cvtColor(img, img, cv::COLOR_BGRA2RGBA);
        }

        return img;
    }

    cv::Mat readImage(string filepath, float sx, float sy, bool isBmp) {
        cv::Mat img = readImage(filepath, isBmp);

        cv::resize(img, img, cv::Size(), sx, sy);

        return img;
    }
}

/// <summary>Load image and add imageList.</summary>
int ImageManager::loadImage(string filepath, bool isBmp) {
    this->imageList.push_back(readImage(filepath, isBmp));
    return this->imageList.size() - 1;
}

int ImageManager::loadImage(string filepath, float sx, float sy, bool isBmp) {
    cv::Mat img = readImage(filepath, sx, sy, isBmp);

    this->imageList.push_back(img);

    return this->imageList.size() - 1;
}

int ImageManager::loadRegionImage(string filepath, int x, int y, int n, bool isBmp) {
    return this->loadRegionImage(filepath, 1, 1, x, y, n, isBmp);
}

int ImageManager::loadRegionImage(string filepath, float sx, float sy, int x, int y, int n, bool isBmp) {
    cv::Mat img = readImage(filepath, sx, sy, isBmp);
    int handle = this->imageList.size();
    int nx = x * sx;
    int ny = y * sy;

    int i = 0;
    for(int r = 0; r * ny < img.rows; r++) {
        for(int c = 0; c * nx < img.cols; c++) {
            this->imageList.push_back(
                    cv::Mat(
                        img,
                        cv::Rect(
                            c * nx,
                            r * ny,
                            c * nx + nx <= img.cols ? nx : img.cols - c * nx,
                            r * ny + ny <= img.rows ? ny : img.rows - r * ny)));
            if(++i >= n) return handle;
        }
    }

    return handle;
}

cv::Mat* ImageManager::getImage(int handle) {
    if(handle >= imageList.size()) {
        return nullptr;
    }

    return &this->imageList[handle];
}

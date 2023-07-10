#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

using namespace std;

class ImageManager {
public:
    int loadImage(string filepath, bool isBmp=false);
    int loadImage(string filepath, float sx, float sy, bool isBmp=false);
    int loadRegionImage(string filepath, int x, int y, int n, bool isBmp=false);
    int loadRegionImage(string filepath, float sx, float sy, int x, int y, int n, bool isBmp=false);
    cv::Mat* getImage(int handle);
private:
    vector<cv::Mat> imageList;
};

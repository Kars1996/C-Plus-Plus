#pragma once

#include <opencv2/opencv.hpp>
#include <string>

class ASCIIConverter {
public:
    ASCIIConverter(int frameWidth = 150);
    
    std::string convertFrameToASCII(const cv::Mat& frame);
    
private:
    static const std::string ASCII_CHARS;
    int frameWidth;
    
    cv::Mat resizeFrame(const cv::Mat& frame);
    cv::Mat convertToGrayscale(const cv::Mat& frame);
    std::string pixelsToASCII(const cv::Mat& grayFrame);
    void showProgress(int current, int total);
};
#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <mutex>
#include <chrono>
#include <windows.h>
#include <semaphore>

using namespace std;
using namespace cv;

Mat captureScreen();
Point matchTemplate(const cv::Mat& scene, const cv::Mat& object, bool& flag, Mat& mask);
Point matchTemplate(const cv::Mat& scene, const cv::Mat& object, bool& flag);
void OpenModeListener(bool& open_mode_program, bool& open_mode_all);
void getMask(const Mat& img, Mat& mask);
void opencv_main(bool& open_mode_program, bool& open_mode_all, bool& open_mode_two, bool& open_mode_three);

void pickUpToTwo(bool& open_mode_program, bool& open_mode_all, bool& open_mode_two);
void pickUpToThree(bool& open_mode_program, bool& open_mode_all, bool& open_mode_three);
void pickBasic(bool& open_mode_program, bool& open_mode_all);
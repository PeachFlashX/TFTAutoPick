#include "part_opencv.h"

Mat img;
Mat r_img;
Mat img_basic = imread("./res/the_icon.png");
Mat img_starToTwo = imread("./res/2.png");
Mat img_starToThree = imread("./res/3.png");

Mat mask_starToTwo;
Mat mask_starToThree;

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
Mat image(screenHeight, screenWidth, CV_8UC4);

HWND hwndDesktop = GetDesktopWindow();
HDC hdcScreen = GetDC(hwndDesktop);
HDC hdcMem = CreateCompatibleDC(hdcScreen);
HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);

Mat captureScreen()
{
    HWND hwndDesktop = GetDesktopWindow();
    HDC hdcScreen = GetDC(hwndDesktop);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);
    SelectObject(hdcMem, hBitmap);

    BitBlt(hdcMem, 0, 0, screenWidth, screenHeight, hdcScreen, 0, 0, SRCCOPY);
    GetBitmapBits(hBitmap, screenHeight * screenWidth * 4, image.data);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwndDesktop, hdcScreen);
    cvtColor(image, r_img, COLOR_BGRA2BGR);
    return r_img;
}

Point matchTemplate(const cv::Mat& scene, const cv::Mat& object, bool& flag, Mat& mask)
{
    Mat result;

    matchTemplate(scene, object, result, cv::TM_SQDIFF_NORMED, mask);

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    matchLoc = minLoc;
    // cout << minVal << endl;
    // cout << "star upper" << endl;
    if (minVal < 0.08f)
        flag = 1;
    else
        flag = 0;

    return matchLoc;
}

Point matchTemplate(const cv::Mat& scene, const cv::Mat& object, bool& flag)
{
    Mat result;

    matchTemplate(scene, object, result, cv::TM_SQDIFF_NORMED);

    double minVal;
    double maxVal;
    Point minLoc;
    Point maxLoc;
    Point matchLoc;
    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    matchLoc = minLoc;
    // cout << minVal << endl;
    // cout << "basic" << endl;
    if (minVal < 0.1)
        flag = 1;
    else
        flag = 0;

    return matchLoc;
}

void OpenModeListener(bool& open_mode_program, bool& open_mode_all)
{
    while (open_mode_program)
    {
        if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
        {
            // open_mode_all = ~open_mode_all;
            if (open_mode_all)
                open_mode_all = 0;
            else
                open_mode_all = 1;
            cout << "basic Change to ";
            if (open_mode_all)
                cout << "on";
            else
                cout << "off";
            cout << endl;
            Sleep(500);
        }
    }
    return;
}

void getMask(const Mat& img, Mat& mask)
{
    cvtColor(img, mask, COLOR_BGR2GRAY);
    threshold(mask, mask, 40, 255, THRESH_BINARY);
}

void opencv_main(bool& open_mode_program, bool& open_mode_all, bool& open_mode_two, bool& open_mode_three)
{
    // opencv code
    Sleep(1000);
    std::cout << "OpenCV version : " << cv::getVersionString() << std::endl;
    cout << "Click [shift] to change basic mode" << endl;

    getMask(img_starToTwo, mask_starToTwo);
    getMask(img_starToThree, mask_starToThree);
    Point reji;
    bool flag = 0;
    cv::Rect rect(0, screenHeight / 5 * 4, screenWidth, screenHeight / 5);

    while (open_mode_program)
    {
        if (open_mode_all)
        {
            img = captureScreen();
            img = img(rect);
            //  imshow("Screenshot", img);
            //  waitKey(0);
            //     img = imread("./the_map_min.png");
            flag = 0;
            reji = matchTemplate(img, img_basic, flag);
            if (flag)
            {
                cout << "basic" << endl;
                SetCursorPos(reji.x + img_basic.size().width / 2, reji.y + img_basic.size().height / 2 + screenHeight / 5 * 4);
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            }

            if (open_mode_two)
            {
                flag = 0;
                reji = matchTemplate(img, img_starToTwo, flag, mask_starToTwo);
                if (flag)
                {
                    cout << "star to two" << endl;
                    SetCursorPos(reji.x + img_basic.size().width / 2, reji.y + img_basic.size().height / 2 + screenHeight / 5 * 4);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
            }

            if (open_mode_three)
            {
                flag = 0;
                reji = matchTemplate(img, img_starToThree, flag, mask_starToThree);
                if (flag)
                {
                    cout << "star to three" << endl;
                    SetCursorPos(reji.x + img_basic.size().width / 2, reji.y + img_basic.size().height / 2 + screenHeight / 5 * 4);
                    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
                }
            }
        }
    }

    // destroyWindow("Screenshot");
    // system("pause");
    return;
}
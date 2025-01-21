#include "part_opencv.h"

Mat img;
Mat r_img;
Mat img_basic = imread("./res/the_icon.png");
Mat img_starToTwo = imread("./res/2.png");
Mat img_starToThree = imread("./res/3.png");

Mat mask_starToTwo;
Mat mask_starToThree;
Mat mask_basic;

int screenWidth = GetSystemMetrics(SM_CXSCREEN);
int screenHeight = GetSystemMetrics(SM_CYSCREEN);
Mat image(screenHeight, screenWidth, CV_8UC4);

HWND hwndDesktop = GetDesktopWindow();
HDC hdcScreen = GetDC(hwndDesktop);
HDC hdcMem = CreateCompatibleDC(hdcScreen);
HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, screenWidth, screenHeight);

binary_semaphore seg_basic(1);
binary_semaphore seg_two(1);
binary_semaphore seg_three(1);
binary_semaphore seg_basic_done(0);
binary_semaphore seg_two_done(0);
binary_semaphore seg_three_done(0);

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
    //cout << minVal;
    //cout << "star upper" << endl;
    if (minVal < 0.09f)
    {
        flag = 1;
    }
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
    //cout << minVal;
    //cout << "basic" << endl;
    if (minVal < 0.1)
    {
        imshow("get", scene);
        waitKey(0);
        flag = 1;
    }
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
            Sleep(200);
        }
        Sleep(50);
    }
    return;
}

void getMask(const Mat& img, Mat& mask)
{
    cvtColor(img, mask, COLOR_BGR2GRAY);
    threshold(mask, mask, 40, 255, THRESH_BINARY);
    //imshow("mask", mask);
    //waitKey(0);
}

void opencv_main(bool& open_mode_program, bool& open_mode_all, bool& open_mode_two, bool& open_mode_three)
{
    // opencv code
    Sleep(1000);
    std::cout << "OpenCV version : " << cv::getVersionString() << std::endl;
    cout << "Click [shift] to change basic mode" << endl;

    getMask(img_starToTwo, mask_starToTwo);
    getMask(img_starToThree, mask_starToThree);
    getMask(img_basic, mask_basic);
    Point reji;
    bool flag = 0;
    cv::Rect rect(0, screenHeight / 5 * 4, screenWidth, screenHeight / 5);

    img = captureScreen();
    img = img(rect);
    thread pick_basic(pickBasic, ref(open_mode_program), ref(open_mode_all));
    thread pick_two(pickUpToTwo, ref(open_mode_program), ref(open_mode_all), ref(open_mode_two));
    thread pick_three(pickUpToThree, ref(open_mode_program), ref(open_mode_all), ref(open_mode_three));
    while (open_mode_program)
    {
        img = captureScreen();
        img = img(rect);

        seg_basic.release();
        seg_two.release();
        seg_three.release();

        seg_basic_done.acquire();
        seg_two_done.acquire();
        seg_three_done.acquire();
    }
    pick_basic.detach();
    pick_two.detach();
    pick_three.detach();


    //  imshow("Screenshot", img);
    //  waitKey(0);
    //     img = imread("./the_map_min.png");

    // destroyWindow("Screenshot");
    // system("pause");
    return;
}

void pickUpToTwo(bool& open_mode_program, bool& open_mode_all,bool& open_mode_two)
{
    bool flag;
    Point reji;
    while (open_mode_program)
    {
        seg_two.acquire();
        if (open_mode_all) 
        {
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
        }
        seg_two_done.release();
    }
}

void pickUpToThree(bool& open_mode_program, bool& open_mode_all,bool& open_mode_three)
{
    bool flag;
    Point reji;
    while (open_mode_program)
    {
        seg_three.acquire();
        if (open_mode_all)
        {
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
        seg_three_done.release();
    }
}

void pickBasic(bool& open_mode_program, bool& open_mode_all)
{
    bool flag;
    Point reji;
    while (open_mode_program)
    {
        seg_basic.acquire();
        if (open_mode_all)
        {
            flag = 0;
            reji = matchTemplate(img, img_basic, flag, mask_basic);
            if (flag)
            {
                cout << "basic" << endl;
                SetCursorPos(reji.x + img_basic.size().width / 2, reji.y + img_basic.size().height / 2 + screenHeight / 5 * 4);
                mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
                mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
            }
        }
        seg_basic_done.release();
    }
}

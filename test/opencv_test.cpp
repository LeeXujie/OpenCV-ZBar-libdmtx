//∂¡»°Õº∆¨≤¢œ‘ æ
#include<iostream> 
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp> 
using namespace cv;
int main()
{
	Mat img = imread("opencv_test.jpg");
	if (!img.isContinuous())
	{
		std::cout << "∂¡»°ÕºœÒ ß∞‹£°" << std::endl;
		getchar();
		return 0;
	}
	namedWindow("≤‚ ‘opencv");
	imshow("≤‚ ‘opencv", img);
	cvWaitKey(0);
	return 0;
}
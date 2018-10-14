#define _CRT_SECURE_NO_WARNINGS

#include <assert.h>
#include <iostream>
#include <Windows.h>
#include <process.h>
#include <string>
#include "dmtx.h"
#include "zbar.h"      
#include "cv.h"  
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace zbar;  //添加zbar名称空间 

char key;
volatile bool flag;
HANDLE HOne;
Mat mat, image;

DmtxTime mdmtxTimeNow(void)
{
	time_t s;
	DmtxTime tNow;

	s = time(NULL);
	if (errno != 0)
		; /* XXX handle error better here */

	tNow.sec = s;
	tNow.usec = 0;

	return tNow;
}

unsigned int __stdcall ThreadFun(PVOID pM)
{
	while (1){
		while (flag)
		{
			//cout << "dmtx thread" << endl;
			if (!mat.data) {
				fprintf(stderr, "read image error\n");
				flag = false;
			}
			
			int width = mat.cols;
			int height = mat.rows;
			int channels = mat.channels();

			DmtxImage* img = dmtxImageCreate(mat.data, width, height, DmtxPack24bppRGB);
			if (!img) {
				fprintf(stderr, "dmtx image create fail\n");
				flag = false;
			}

			DmtxDecode* dec = dmtxDecodeCreate(img, 1);
			if (!dec) {
				fprintf(stderr, "dmtx decode create fail\n");
				flag = false;
			}

			DmtxTime timeout = { mdmtxTimeNow().sec + 1, 0 };
			DmtxRegion* reg = dmtxRegionFindNext(dec, &timeout);
			if (reg != NULL) {
				DmtxMessage* msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
				if (!msg) {
					fprintf(stderr, "dmtx decode matrix region fail\n");
					flag = false; 
				}
				else{
					cout << "条码类型：" << "Datamatrix" << endl;
					std::string str(reinterpret_cast<char*>(msg->output));
					fprintf(stderr, "条码信息: \n%s\n\n", str.c_str());
					dmtxMessageDestroy(&msg);
				}
				dmtxRegionDestroy(&reg);
			}
			/*else
			{
				fprintf(stderr, "dmtx find region fail\n");
				flag = false;
			}*/
			
			dmtxDecodeDestroy(&dec);
			dmtxImageDestroy(&img);
			flag = false;
		}
	}
	return 0;
}

int main(int argc, char* argv[])
{
	ImageScanner scanner;
	cv::VideoCapture inputVideo;
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);

	inputVideo.open(0);
	if (!inputVideo.isOpened())
	{
		cout << "camera open failed!" << endl;
		getchar();
		return 0;
	}

	HOne = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, NULL, 0, NULL);

	while (inputVideo.grab()&&key!=27) {
		inputVideo.retrieve(image);
		if (!flag)
		{
			image.copyTo(mat);
			flag = true;
		}
		Mat imageGray;
		cvtColor(image, imageGray, CV_RGB2GRAY);
		int width = imageGray.cols;
		int height = imageGray.rows;
		uchar *raw = (uchar *)imageGray.data;
		Image imageZbar(width, height, "Y800", raw, width * height);
		scanner.scan(imageZbar); //扫描条码      
		Image::SymbolIterator symbol = imageZbar.symbol_begin();
		/*if (imageZbar.symbol_begin() == imageZbar.symbol_end())
		{
		cout << "查询条码失败，请检查图片！" << endl;
		}*/
		for (; symbol != imageZbar.symbol_end(); ++symbol)
		{
			cout << "条码类型：" << symbol->get_type_name() << endl;
			cout << "条码信息：" << endl << symbol->get_data() << endl << endl;
		}
		cv::imshow("Source Image", image);
		imageZbar.set_data(NULL, 0);
		key = cv::waitKey(10);
	}
	CloseHandle(HOne);
	return 0;
}


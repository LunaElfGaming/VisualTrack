#include <opencv2/opencv.hpp>  
#include <iostream>  

using namespace cv;
using namespace std;

int main()
{
	Mat img = imread("img1.bmp",0);     //��һ֡ͼƬ
	medianBlur(img, img, 3);
	Mat img2;                           //��һ֡ͼƬ�Ĳ�ɫRGB
	cvtColor(img, img2, COLOR_GRAY2BGR);

	Mat nex, nex2;                      //�ڶ�֡ͼƬ�Լ���ɫRGB
	Mat diffImg;                          //����֡
	
	int x, y;
	int w, h;
	x = 249, y = 116, w = 14, h = 26;
	//cin >> x >> y >> w >> h;              //��һ֡�Ŀ�ȡ����
										  
	
	{										//�ڵ�һ֡��ͼ�л���ʾ���Ժ��
		for (int i = x;i < x + w;i++) {
			for (int j = 0;j <= 1;j++) {
				Point p = Point(i, y + h*j - 1);
				circle(img2, p, 0, Scalar(0, 0, 255));
			}
		}
		for (int j = y;j < y + h;j++) {
			for (int i = 0;i <= 1;i++) {
				Point p = Point(i*w + x - 1, j);
				circle(img2, p, 0, Scalar(0, 0, 255));
			}
		}
	}

	
	Mat subimg = img(Rect(x, y, w, h));        //�ֲ�ͼ
	MatND histimg;
	const int channels[1] = { 0 };
	const int histSize[1] = { 256 };
	float subranges[2] = { 0,255 };
	const float *ranges[1] = { subranges };
	calcHist(&subimg, 1, channels, Mat(), histimg, 1, histSize, ranges, true, false);           //�ֲ�ͼ�ĻҶ�ֱ��ͼ
	normalize(histimg, histimg, 0, 1, NORM_MINMAX, -1, Mat());

	nex = imread("img2.bmp", 0);
	medianBlur(nex, nex, 3);
	cvtColor(nex, nex2, COLOR_GRAY2BGR);
	subtract(nex, img, diffImg);                                   //��ȡ����ͼ
	threshold(diffImg, diffImg, 50.0, 255, THRESH_BINARY);          //��ֵ��

	vector<vector<Point>> cont;                   //��ֵ����ı߽�
	vector<Vec4i> hierarchy;
	findContours(diffImg, cont, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	int maxsize = 0, conum=0;
	for (int i = 0;i < cont.size();i++) {
		if (cont[i].size() > maxsize) {
			conum = i;
			maxsize = cont[i].size();
		}
	}

	int nexw, nexh, nexx, nexy;
	nexx = img.cols, nexy = img.rows;
	int maxx = 0, maxy = 0;
	for (int i = 0;i < cont[conum].size();i++) {
		Point p = cont[conum].at(i);
		if (p.x > maxx)
			maxx = p.x;
		if (p.x < nexx)
			nexx = p.x;
		if (p.y > maxy)
			maxy = p.y;
		if (p.y  < nexy)
			nexy = p.y;
	}

	nexw = maxx - nexx;
	nexh = maxy - nexy;

	
	Point center = Point((maxx + nexx) / 2, (maxy + nexy) / 2);

	double min = 1000;
	int mw, mh;
	for (int weight = w*0.9;weight <= w*1.2;weight++) {
		for (int height = h*0.9;height <= h*1.2;height++) {               //������Ϊ����Ѱ�Һ��ʵı߿��С
			int cx, cy;
			cx = center.x - weight / 2;
			cy = center.y - height / 2;
			Mat subnex = nex(Rect(cx, cy, weight, height));             //��ȡ�ֲ�
			MatND histnex;
			calcHist(&subnex, 1, channels, Mat(), histnex, 1, histSize, ranges, true, false);
			normalize(histnex, histnex, 0, 1, NORM_MINMAX, -1, Mat());
			double base = compareHist(histnex, histimg, 2);             //Bhattacharyya��ʽ�Ա�ֱ��ͼ
			//cout << base << endl;
			if (base < min) {
				mw = weight;
				mh = height;
				min = base;
			}
		}
	}

	{													//�ڵڶ���ͼ�����»������
		//mw = w;
		//mh = h;
		int cx = center.x - mw / 2;
		int cy = center.y - mh / 2;
		cout << "�¶�λ�߿�: " << cx << " " << cy << " " << mw << " " << mh << endl;
		cout << "ֱ��ͼ���ƶ�Bhattacharyya ϵ�� " << min << endl;
		//cout << mw << mh << cx << cy;
		for (int i = cx;i < cx + mw;i++) {
			for (int j = 0;j <= 1;j++) {
				Point p = Point(i, cy + mh*j - 1);
				circle(nex2, p, 0, Scalar(0, 0, 255));
			}
		}
		for (int j = cy;j < cy + mh;j++) {
			for (int i = 0;i <= 1;i++) {
				Point p = Point(i*mw + cx - 1, j);
				circle(nex2, p, 0, Scalar(0, 0, 255));
			}
		}
	}


	namedWindow("Test1", CV_WINDOW_AUTOSIZE);
	imshow("Test1", img2);
	imwrite("frame1.bmp", img2);

	namedWindow("Test2", CV_WINDOW_AUTOSIZE);
	imshow("Test2", nex2);
	imwrite("frame2.bmp", nex2);

	namedWindow("Test3", CV_WINDOW_AUTOSIZE);
	imshow("Test3", diffImg);
	imwrite("diff.bmp", diffImg);

	//�ȴ�������������¹رմ˴���  
	waitKey(0);
	return 0;
}
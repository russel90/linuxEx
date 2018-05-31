#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
 
using namespace cv;
using namespace std;
int grayImage();

int main(int argc,char ** argv)
{
	  VideoCapture cap(0);
	    if (!cap.isOpened()) {
		        cerr << "ERROR: Unable to open the camera" << endl;
			    return 0;
			      }
	      grayImage();
	      Mat frame;
	        cout << "Start grabbing, press a key on Live window to terminate" << endl;
		  while(1) {
			      cap >> frame;
			          if (frame.empty()) {
					          cerr << "ERROR: Unable to grab from the camera" << endl;
						          break;
							      }
				      imshow("Live",frame);
				          int key = cv::waitKey(5);
					      key = (key==255) ? -1 : key; //#Solve bug in 3.2.0
					          if (key>=0)
							        break;
						    }
		   
		    cout << "Closing the camera" << endl;
		      cap.release();
		        destroyAllWindows();
			  cout << "bye!" <<endl;
			    return 0;
}

int grayImage(){
		Mat src = imread("pic1.jpg",IMREAD_GRAYSCALE);
		if(src.empty())
		return -1;
		int histSize =256;
		float valueRange[]={0,256};
		const float* ranges[]={valueRange};
		int channels=0;
		int dims=1;

		Mat hist;
		
		calcHist(&src,1,&channels,Mat(),hist,dims,&histSize,ranges);
		Mat histImage(512,512,CV_8U);
		normalize(hist,hist,0,histImage.rows,NORM_MINMAX,CV_32F);
		histImage=Scalar(255);
		int binW=cvRound((double)histImage.cols/histSize);
		int x1,y1,x2,y2;
		for(int i =0;i<histSize;i++){
			x1=i*binW;
			y1=histImage.rows;
			x2=(i+1)*binW;
			y2=histImage.rows-cvRound(hist.at<float>(i));
			rectangle(histImage,Point(x1,y1),Point(x2,y2),Scalar(0),-1);
		}
		imshow("original",src);
		imshow("histImage",histImage);
		return 0;
	}

// http://ghj1001020.tistory.com/604
#include <opencv/cv.h>
#include <opencv/highgui.h>

int main(void) {
    IplImage *frame;    //동영상 프레임
 
 
    CvFont font;
    //cvInitFont(폰트, 폰트이름, 글자 수평스케일, 글자 수직스케일, 기울임, 굵기) : 폰트 초기화
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 1, 1, 0, 5);
 
    //동영상파일 로드
    CvCapture *video = cvCaptureFromFile("/home/russel90/blackbox/testmedia.mov");
 
    cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
    while (1) {
        //cvQueryFrame(동영상) : 동영상으로부터 프레임을 읽고 반환
        //cvQueryFrame = cvGrabFrame함수와 cvRetrieveFrame함수를 합친것
        frame = cvQueryFrame(video);
 
        if (!frame || cvWaitKey(10) >= 0) { break; }
 
        //cvPutText(이미지, 텍스트, 출력위치, 폰트, 텍스트색상) : 텍스트를 이미지에 추가
        cvPutText(frame, "TEST TEXT", cvPoint(100, 100), &font, CV_RGB(255,0,0));
 
        cvShowImage("video", frame);
    }
 
    //메모리에서 동영상 해제
    cvReleaseCapture(&video);
 
    cvReleaseImage(&frame);
    cvDestroyWindow("video");

    return 0;
}

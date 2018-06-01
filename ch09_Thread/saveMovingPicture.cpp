// reference: http://ghj1001020.tistory.com/594
#include <opencv/cv.h>
#include <opencv/highgui.h>
  
int main() {
    IplImage *frame = 0;
    CvVideoWriter *writer = 0;
 
    //cvCaptureFromFile(파일명) : 동영상 파일 불러오기
    CvCapture *capture = cvCaptureFromAVI("~/blackbox/su.avi");
 
    cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
    
    const int fps = 30;    //30프레임
 
    //첫번째 프레임으로부터 크기 구하기
    cvGrabFrame(capture);
    frame = cvRetrieveFrame(capture);
    //cvCreateVideoWriter(경로, 코덱방식, fps, 프레임크기) : 동영상파일 생성
    //CV_FOURCC('D', 'I', 'V', 'X') : MPEG-4 코덱
    writer = cvCreateVideoWriter("~/blackbox/copy.avi", CV_FOURCC('D', 'I', 'V', 'X'), fps, cvGetSize(frame));
    while (1) {
        //cvGrabFrame(동영상) : 하나의 프레임을 잡음
        cvGrabFrame(capture);
        //cvRetrieveFrame(동영상) : 잡은 프레임으로부터 이미지를 구함
        frame = cvRetrieveFrame(capture);
 
        if (!frame || cvWaitKey(10) >= 0) { break; }
 
        
        //cvWriteFrame(동영상, 프레임) : 지정한 동영상에 프레임을 쓴다
        cvWriteFrame(writer, frame);
 
        cvShowImage("video", frame);
    }
 
    //cvReleaseVideoWriter(동영상) : 메모리에서 해제한다
    cvReleaseVideoWriter(&writer);
 
    cvReleaseCapture(&capture);
    cvDestroyWindow("video");
}
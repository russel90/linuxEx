#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <pthread.h>
 
using namespace cv;
using namespace std;
 
VideoCapture vcap;
Mat frame;
 
pthread_mutex_t frameLocker;
pthread_t UpdThread;    
 
void *UpdateFrame(void *arg)
{
    for(;;)
    {
        Mat tempFrame;
        vcap >> tempFrame;
 
        pthread_mutex_lock(&frameLocker);
        frame = tempFrame;
        pthread_mutex_unlock(&frameLocker);
    }
 
    pthread_exit( (void *)0 );
}
 
int main(int, char**) 
{ 
    // webcam video open
    vcap.open(0);
 
    pthread_mutex_init(&frameLocker,NULL);  
    pthread_create(&UpdThread, NULL, UpdateFrame, NULL);
 
    for(;;)
    {
        Mat currentFrame, grayImage;
 
        pthread_mutex_lock(&frameLocker);
        currentFrame = frame;
        pthread_mutex_unlock(&frameLocker);
 
        // currentframe open Y/N
        if(currentFrame.empty())
            continue;

        // set gramyImage
        cvtColor(currentFrame,  grayImage, COLOR_BGR2GRAY);
        imshow("Gray Image", grayImage);
        imshow("Input Image", tempFrame);
 
        //ESC키를 누르면 쓰레드를 강제 종료후.. 프로그램 종료한다. 
        if (waitKey(20) == 27 ) 
        {

            int ret = pthread_cancel( UpdThread );
            int status;
 
            // pthread cancel succcess
            if (ret == 0 )
            {
                //자동 종료    
                ret = pthread_join( UpdThread, (void **)&status );
                if ( ret == 0 )
                    //강제 종료시키면 status로 -1이 리턴된다. 
                    cout << "스레드 종료 성공" << status << endl;
                else
                    cout << "스레드 종료 에러" << ret << endl;
 
                break;
            }
        }
                
    }
 
    return 0;
}

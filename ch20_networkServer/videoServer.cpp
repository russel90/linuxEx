#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <arpa/inet.h>

#include <boost/circular_buffer.hpp>
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Size size;
int fps;
int wait;
int readyCapture = 0;
int loopCnt = 1;

pthread_mutex_t frameLocker;
pthread_t captureManagerThread;
pthread_t serverManagerThread;

// check circular buffer size
boost::circular_buffer<Mat> cb(30*60*10);

void *captureManager(void *arg)
{
    VideoCapture video(0);
    Mat captureFrame;

    cout << "captureManager: Start grabbing" << endl;

    if (!video.isOpened()) {
        cerr << "captureManager: ERROR - Unable to open the camera" << endl;
        video.release();
		pthread_exit((void *)0);
        exit(0);
    }

    size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),(int)video.get(CAP_PROP_FRAME_HEIGHT));
	fprintf(stdout,"captureManager: size.height = %d, size.width = %d\n", size.height, size.width);

    video.set(CAP_PROP_FPS, 30.0);    
    fps = video.get(CAP_PROP_FPS);
    wait = int(1.0 / fps * 1000);

    fprintf(stdout,"captureManager: fps = %d, wait = %d\n", fps, wait);

    while(loopCnt){

       video >> captureFrame;

       // if the frame is empty, break immediately
       if(captureFrame.empty()){
            cerr << "captureManager: ERROR - Unable to grab from the camera" << endl;
            break;
       }else{
		   pthread_mutex_lock(&frameLocker);
		   cb.push_back(captureFrame);
		   pthread_mutex_unlock(&frameLocker);

		   //std::cout << "captureManager : cb.size() - " << cb.size() << '\n';
	   }	

       // if(cvWaitKey(wait)== 27) 
	   //   break;
	   readyCapture = 1;
    }

	std::cout << "captureManager : cb.size() - " << cb.size() << '\n';
    video.release();
	std::cout << "captureManager : video.release()" << std::endl;
    pthread_exit((void *)0);
}

void *serverManager(void *arg)
{
    int socket = *(int *)arg;
    int bytes = 0, imgSize;
    int key;

    while(!readyCapture){
        usleep(5000);
        // std::cout << "serverManager: readyCapture = " << readyCapture << std::endl;
    }

    Mat img;
    img = Mat::zeros(size.height, size.width, CV_8UC3);
    imgSize = img.total() * img.elemSize();
	std::cout << "serverManager: Image Size:" << imgSize << std::endl;

	wait = int(1.0 / fps * 1000);
	std::cout << "serverManager: wait:" << wait << std::endl;

    while(loopCnt){
        if(cb.size() != 0){
            pthread_mutex_lock(&frameLocker);
            img = cb.front();
            cb.pop_front();
            pthread_mutex_unlock(&frameLocker);        

            // send processed image
            if ((bytes = send(socket, img.data, imgSize, 0)) < 0){
              std::cerr << "serverManager: bytes = " << bytes << std::endl;
              loopCnt = 0;
	          break;
            } 
        }
       // if(cvWaitKey(wait)== 27) break;
    }
	
    cb.clear;
    pthread_exit((void *)0);
}

int main(int argc, char** argv)
{
    int results;
    int status;

    //--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //--------------------------------------------------------
    int localSocket, remoteSocket, port = 4097;                               

    struct  sockaddr_in localAddr, remoteAddr;
    pthread_t thread_id;
               
    int addrLen = sizeof(struct sockaddr_in);
       
    if ( (argc > 1) && (strcmp(argv[1],"-h") == 0) ) {
        std::cerr << "usage: ./cv_video_srv [port] [capture device]\n" <<
                     "port           : socket port (4097 default)\n" <<
                     "capture device : (0 default)\n" << std::endl;

        exit(1);
    }

    if(argc == 2) port = atoi(argv[1]);

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    if (localSocket == -1){
        perror("main: socket() call failed!!");
    }    

    localAddr.sin_family = AF_INET;
    localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_port = htons( port );

    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
        perror("main: Can't bind() socket");
        exit(1);
    }
    
    //Listening
    listen(localSocket , 1);
    
    std::cout <<  "Waiting for connections...\n"
              <<  "Server Port:" << port << std::endl;

    //accept connection from an incoming client
    pthread_mutex_init(&frameLocker,NULL);

    while(1){      
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);  
        if (remoteSocket < 0) {
            perror("main: accept failed!");
            exit(1);
        } 

        std::cout << "main: Connection accepted" << std::endl;        
        results = pthread_create(&captureManagerThread, NULL, captureManager, &remoteSocket);
        if(results != 0){
            fprintf(stderr, "main: captureManagerThread creation failed(%d)\n", results);
            return -1;
        }

        results = pthread_create(&serverManagerThread, NULL, serverManager, &remoteSocket);
        if(results < 0){
            fprintf(stderr, "main: serverManagerThread creation failed\n");
            return -1;
        }

	    // if (pthread_join( serverManagerThread, (void **)&status ) != 0 ){
        if (pthread_detach(serverManagerThread) != 0 ){
    		fprintf(stderr, "main: serverManagerThread close error\n");
    	    return -1;
	    }
    
		// captureManagerThread pthread cancel succcess
	    // if (pthread_join(captureManagerThread, (void **)&status ) != 0 ){
         if (pthread_detach(captureManagerThread) != 0 ){
	    	fprintf(stderr, "main: captureManagerThread close error\n");
	        return -1;
    	}

        loopCnt = 1;
	}

	close(remoteSocket);
    close(localSocket);

    return 0;
}

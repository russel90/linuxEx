#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <sys/time.h>

#include <sys/socket.h> 
#include <arpa/inet.h>

#include <iostream>  
#include <boost/circular_buffer.hpp>
#include "opencv2/opencv.hpp"

#define MAX_BUFFER_SIZE 256
#define MAX_FILE_TYPE_SIZE = 3
#define FONT_SCALE 2
#define MAX_STORAGE_USAGE 0.50
#define MAX_SLEEP_TIME 300

using namespace cv;
using namespace std;

enum time_mode {days, hours, mins, secs, msecs, permin, persec};

struct f_size
{
    long blocks;
    long avail; 
};

typedef struct _mountinfo 
{
    FILE *fp;               // 파일 스트림 포인터    
    char devname[80];       // 장치 이름
    char mountdir[80];      // 마운트 디렉토리 이름
    char fstype[12];        // 파일 시스템 타입
    struct f_size size;     // 파일 시스템의 총크기/사용율 
} MOUNTP;

Size size;
int fps = 10;
int wait;
int readyCapture = 0;

pthread_mutex_t frameLocker;
pthread_t captureManagerThread;
pthread_t serverManagerThread;
pthread_t controlManagerThread;

int capDev = 0;
VideoCapture cap(capDev); // open the default camera

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
        return 0;
    }

    size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),(int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);    
    // fps = video.get(CAP_PROP_FPS);
    wait = int(1.0 / fps * 1000);

    fprintf(stdout,"captureManager: size.height = %d, size.width = %d\n", size.height, size.width);

    while(1){

       video >> captureFrame;

       // if the frame is empty, break immediately
       if(captureFrame.empty()){
            cerr << "captureManager: ERROR - Unable to grab from the camera" << endl;
            break;
       }

       pthread_mutex_lock(&frameLocker);
       cb.push_back(captureFrame);
       pthread_mutex_unlock(&frameLocker);

       std::cout << "captureManager : cb.size() - " << cb.size() << '\n';
     
       readyCapture = 1;
       waitKey(wait);
    }

    video.release();
    pthread_exit((void *)0);
}

void *serverManager(void *arg)
{
    int socket = *(int *)arg;
    int bytes = 0, imgSize;
    int key;

    Mat img;
    img = Mat::zeros(size.height, size.width, CV_8UC3);
    imgSize = img.total() * img.elemSize();
  
    // fps = cap.get(CAP_PROP_FPS);
    wait = int(1.0 / fps * 1000);
   
    while(!readyCapture){
        sleep(2);
        std::cout << "serverManager: readyCapture = " << readyCapture << std::endl;
    }

    std::cout << "serverManager: Image Size:" << imgSize << std::endl;

    while(1){

        pthread_mutex_lock(&frameLocker);
        img = cb.front();
        cb.pop_front();
        pthread_mutex_unlock(&frameLocker);        

        // send processed image
        if ((bytes = send(socket, img.data, imgSize, 0)) < 0){
                std::cerr << "serverManager: bytes = " << bytes << std::endl;
                break;
        } 

        waitKey(wait);
    }
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
    listen(localSocket , 5);
    
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
        fprintf(stdout, "main: serverManagerThread Created\n");
    }

    // captureManagerThread pthread cancel succcess
    if (pthread_cancel(captureManagerThread) == 0 ){
        if (pthread_join(captureManagerThread, (void **)&status ) != 0 ){
            fprintf(stderr, "main: captureManagerThread close error\n");
            return -1;
        }
    }          
    
    // serverManagerThread pthread cancel succcess
    if (pthread_cancel(serverManagerThread) == 0 ){
        if (pthread_join( serverManagerThread, (void **)&status ) != 0 ){
            fprintf(stderr, "main: serverManagerThread close error\n");
            return -1;
        }
    }    
        
    close(remoteSocket);
    close(localSocket);

    return 0;
}
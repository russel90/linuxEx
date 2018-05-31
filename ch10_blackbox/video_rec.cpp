#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <iostream>  
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

#define MAX_BUFFER_SIZE 256
#define MAX_FILE_TYPE_SIZE = 3
#define DEBUG
 
using namespace cv;
using namespace std;

char *homeDirName = "/home/russel90/blackbox";
char *logFileName = "Log.txt";

enum time_mode {days, hours, mins, secs};

int initialize(char *workDirName);
bool checkWebCamConnection();
bool createDirectory(char *homeDirName, char *workDirName);
bool removeDirector(char *homeDirName, char *workDirName);

char *getTimeStringFormat(time_mode t_mode);
double getStorageCapacity(); 

bool initlaize(char *workDirName){
    // WebCam Connection
    if(!checkWebCamConnection()){
        cout << "initalize: WebCam Initalize Failure" << endl;
        return false;    
    }
    
    // Check storage capacilty
    // return false if storage capacity less then 30%
    if(getStorageCapacity() < 0.3){
        cout << "initalize: storage capacity less then 30%" << endl;
        return false;
    }

    // Create Driectory
    if(!createDirectory(homeDirName, workDirName)){
        cout << "initalize: Create Directory Failure" << endl;
        return false;
    }
    
    return true;
}

bool checkWebCamConnection()
{
    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "checkWebCamConnection: could not open camera device" << endl;
        return false;
    }

    return true;
}

bool createDirectory(char *homeDirName, char *workDirName)
{
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    sprintf(buffer,"%s/%s", homeDirName, workDirName);
    printf("createDirectory: Full Path = %s\n", buffer);

    if(mkdir(buffer, 0775)== -1 && errno != EEXIST){
        fprintf(stderr, "createDirectory: %s directory create error: %s\n", buffer, strerror(errno));
        return false;
    }

    return true;
}

// Scan Dir delect oldest dir

bool removeDirector(char *homeDirName, char *workDirName)
{
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer,"rm -rf %s%s", homeDirName, workDirName);

    if(system(buffer) != 0){
        fprintf(stderr, "removeDirector: directory delete error: %s\n",buffer);
        return false;        
    }

    return true;
}

char *getTimeStringFormat(time_mode t_mode)
{
	static char buffer[MAX_BUFFER_SIZE];
	struct tm *loc;
	time_t t;

	t = time(&t);
	loc = localtime(&t);

    memset(buffer, 0, sizeof(buffer));
    switch(t_mode){
        case days:
            // per day
            strftime(buffer, sizeof(buffer), "%Y-%m-%d", loc);
            break;

        case hours:
            // per hour
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H", loc);
            break;

        case mins:
            // per min
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M", loc);
            break;

        case secs:
            // per sec
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", loc);
            break;   

        default:
            break;
    }

    return buffer;
}

double getStorageCapacity()
{


    return 1.0;
}

int writeLog(char *msg)
{
	char buffer[MAX_BUFFER_SIZE];
	int fd;

	fd = open( logFileName, O_WRONLY| O_CREAT| O_APPEND, S_IRUSR| S_IWUSR);
    
	if (fd == -1) {
		printf("file open error!!\n");
		return -1;
	}

    memset(buffer, 0, sizeof(buffer));
	sprintf(buffer, "%s\t%s", getTimeStringFormat(secs), msg);
	
	write(fd,buffer,strlen(buffer));

	close(fd);
}

int main()
{
    Mat frame;
    double font_scale = 1;
    char buffer[MAX_BUFFER_SIZE];
    char workDirName[MAX_BUFFER_SIZE];
    char workFileName[MAX_BUFFER_SIZE];
    char *fileType="avi";
    
    // initlaize();
    memset(workDirName, 0, sizeof(workDirName));
    strcpy(workDirName, getTimeStringFormat(days));
    initlaize(workDirName);

    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "웹캠을 열수 없습니다." << endl;
        return -1;
    }
 
    // cvInitFont(폰트, 폰트이름, 글자 수평스케일, 글자 수직스케일, 기울임, 굵기) : 폰트 초기화
    // namedWindow("input", CV_WINDOW_AUTOSIZE);
 
    //웹캠에서 캡쳐되는 이미지 크기를 가져옴   
    Size size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),(int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);
 
    //파일로 동영상을 저장하기 위한 준비  
    VideoWriter outputVideo;
    memset(workFileName, 0, sizeof(workFileName));
    strcpy(workFileName, getTimeStringFormat(mins));

    sprintf(buffer, "%s/%s/%s.%s", homeDirName, workDirName, workFileName, fileType);
    printf("Main - Working File Full Path = %s\n",buffer);

    outputVideo.open(buffer, VideoWriter::fourcc('X', 'V', 'I', 'D'), 30, size, true);
    
    if (!outputVideo.isOpened())
    {
        cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
        return 1;
    }
 
    while (1)
    {
        //웹캡으로부터 한 프레임을 읽어옴  
        video >> frame;
 
        //웹캠에서 캡처되는 속도를 가져옴
        int fps = video.get(CAP_PROP_FPS);
        int wait = int(1.0 / fps * 1000);

        // cvPutText(이미지, 텍스트, 출력위치, 폰트, 텍스트색상) : 텍스트를 이미지에 추가
        putText(frame, getTimeStringFormat(secs), Point(100, 100), FONT_HERSHEY_SIMPLEX, font_scale, Scalar(0,0,0), false);               
        //화면에 영상을 보여줌
        imshow(workFileName, frame);  
        
        //동영상 파일에 한프레임을 저장함.  
        outputVideo << frame;
 
        //ESC키 누르면 종료
        if (waitKey(wait) == 27) break;
    }
 
    return 0;
}

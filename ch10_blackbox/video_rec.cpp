#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include <iostream>  
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

#define MAX_BUFFER_SIZE 256
#define MAX_FILE_TYPE_SIZE = 3
#define DEBUG
 
using namespace cv;
using namespace std;

const char *homeDirName = "/home/russel90/blackbox";
const char *logFileName = "Log.txt";
const char *fileType="avi";

enum time_mode {days, hours, mins, secs};

// Opencv related variable
// VideoCapture vcap;
VideoCapture video(0);
Mat frame;

pthread_mutex_t frameLocker;
pthread_t UpdThread;  

void *UpdateFrame(void *arg)
{

    while(1){
        Mat tempFrame;
        video >> tempFrame;

        pthread_mutex_lock(&frameLocker);
        frame = tempFrame;
        pthread_mutex_unlock(&frameLocker);
    }

    pthread_exit((void *)0);
}

// 파일의 크기를 저장하기 위한 변수
long int total_size = 0;

// 디렉토리 들여쓰기를 위한 디렉토리 depth 레벨 저장용
int  indent = 0;

int initialize(char *workDirName);
bool checkWebCamConnection();
bool createDirectory(char *homeDir, char *workDirName);
bool removeDirector(char *homeDir, char *workDirName);

char *getTimeStringFormat(time_mode t_mode);
double getStorageCapacity(); 

bool initlaize(char *workDirName){
    
    // Setting Inital Variable
    // Home Directory 
    // Working Directory

    // WebCam Connection
    if(!checkWebCamConnection()){
        fprintf(stderr, "initalize: WebCam Initalize Failure\n");
        return false;    
    }
    
    // Check storage capacilty
    // return false if storage capacity less then 30%
    if(getStorageCapacity() < 0.3){
        fprintf(stderr, "initalize: storage capacity less then 30%\n");
        return false;
    }

    // Create Driectory
    if(!createDirectory((char *)homeDirName, workDirName)){
        fprintf(stderr, "initalize: Create Directory Failure\n");
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

bool createDirectory(char *homeDir, char *workDirName)
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

// delect oldest dir
int removeOldDirName(char *path)
{
    struct dirent **namelist;

    int count;
    int i;

    //디렉토리 삭제 부분 구현
    if((count = scandir(path, &namelist, NULL, alphasort)) == -1) 
    {
        fprintf(stderr, "removeOldDirName: %s Directory scan Error: %s\n", path, strerror(errno));
        return -1;
    }
    
    for(i = 0; i < count; i++)
    {
        if((!strcmp(namelist[i]->d_name, ".")) || (!strcmp(namelist[i]->d_name,"..")))
            continue;
        printf("removeOldDirName: Old Director Name: %s\n", namelist[i]->d_name);
        break;
    }

    if(!removeDirector(path, namelist[i]->d_name)){
        fprintf(stderr, "removeOldDirName: removeDirector call Error\n");
        return -1; 
    }

    //scandir함수 내부에서 malloc을 사용하기때문에 free필요
    for(i = 0;i<count;i++)
        free(namelist[i]);

    //namelist에 대한 메모리 해제
    free(namelist);

    return 0;
}

bool removeDirector(char *homeDir, char *workDirName)
{
    char buffer[MAX_BUFFER_SIZE];
    int flags;

    memset(buffer, 0, sizeof(buffer));
    sprintf(buffer,"rm -rf %s/%s", homeDirName, workDirName);
    fprintf(stdin, "removeDirector: path=%s\tdirName=%s\tbuffer=%s\n", homeDirName,workDirName,buffer);

    if((flags = system(buffer)) != 0){
        fprintf(stdin, "removeDirector: path=%s\tdirName=%s\tflags=%d\tbuffer=%s\n", homeDirName, workDirName, flags, buffer);
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

    return 0;
}

int main()
{

    // Mat frame;
    double font_scale = 1;
    char buffer[MAX_BUFFER_SIZE];
    char workDirName[MAX_BUFFER_SIZE];
    char workFileName[MAX_BUFFER_SIZE];
    // char *fileType="avi";
    
    // initlaize();
    memset(workDirName, 0, sizeof(workDirName));
    strcpy(workDirName, getTimeStringFormat(days));
    initlaize(workDirName);

    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    // VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "웹캠을 열수 없습니다." << endl;
        return -1;
    }

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
        // int fps = video.get(CAP_PROP_FPS);
        // int wait = int(1.0 / fps * 1000);

        // cvPutText(이미지, 텍스트, 출력위치, 폰트, 텍스트색상) : 텍스트를 이미지에 추가
        putText(frame, getTimeStringFormat(secs), Point(100, 100), FONT_HERSHEY_SIMPLEX, font_scale, Scalar(0,0,0), false);               
        //동영상 파일에 한프레임을 저장함.  
        outputVideo << frame;
 
        //ESC키 누르면 종료
        if (waitKey(wait) == 27) break;
    }
 
    removeOldDirName((char *)homeDirName);

    printf("main: call exit\n");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <dirent.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>

#include <iostream> 
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

#define MAX_BUFFER_SIZE 256
#define MAX_FILE_TYPE_SIZE = 3
#define FONT_SCALE 2
#define MAX_STORAGE_USAGE 0.50
#define MAX_SLEEP_TIME 60
#define DEBUG

using namespace cv;
using namespace std;

const char *homeDirName = "/home/russel90/blackbox";
const char *logFileName = "Log.txt";
const char *fileType="avi";
const char *MMOUNT = "/proc/mounts";

enum time_mode {days, hours, mins, secs, permin, persec};

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

// Opencv related variable
// VideoCapture vcap;
VideoCapture video(0);
Mat frame;
Size size;

pthread_mutex_t frameLocker;
pthread_t UpdThread, FmThread;  

// 파일의 크기를 저장하기 위한 변수
long int total_size = 0;

// 디렉토리 들여쓰기를 위한 디렉토리 depth 레벨 저장용
int  indent = 0;

bool initialize();
bool checkWebCamConnection();
bool createDirectory(char *homeDir, char *workDirName);
bool removeDirector(char *homeDir, char *workDirName);

char *getTimeStringFormat(time_mode t_mode);
double getStorageUsage(); 

bool initialize(){
    
    // Setting Inital Variable if required
    //웹캠에서 캡쳐되는 이미지 크기를 가져옴   
    size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),(int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);    

    // WebCam Connection
    if(!checkWebCamConnection()){
        fprintf(stderr, "initalize: WebCam Initalize Failure\n");
        return false;    
    }
    
    // Check Storage Capacilty
    // return false if storage capacity less then 30%
    if(getStorageUsage() > MAX_STORAGE_USAGE){
        fprintf(stderr, "initalize: storage capacity less then 30\n");
        return false;
    }

    return true;
}

bool checkWebCamConnection()
{
    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    if (!video.isOpened())
    {
        cout << "checkWebCamConnection: could not open camera device" << endl;
        return false;
    }

    // video.release();
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
    fprintf(stdout, "removeDirector: path=%s\tdirName=%s\tbuffer=%s\n", homeDirName,workDirName,buffer);

    if((flags = system(buffer)) != 0){
        fprintf(stdout, "removeDirector: path=%s\tdirName=%s\tflags=%d\tbuffer=%s\n", homeDirName, workDirName, flags, buffer);
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
            // Time Format: YYYY-MM-DD
            strftime(buffer, sizeof(buffer), "%Y-%m-%d", loc);
            break;

        case hours:
            // Time Format: YYYY-MM-DD-HH
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H", loc);
            break;

        case mins:
            // Time Format: YYYY-MM-DD-HH-MM
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M", loc);
            break;

        case secs:
            // Time Format: YYYY-MM-DD-HH-MM-SS
            strftime(buffer, sizeof(buffer), "%Y-%m-%d-%H-%M-%S", loc);
            break;   

        case permin:
            // Time Format: MM
            strftime(buffer, sizeof(buffer), "%M", loc);
            break;
        case persec:
            // Time Format: MM
            strftime(buffer, sizeof(buffer), "%S", loc);
            break;  
        default:
            break;
    }

    return buffer;
}

double getStorageUsage()
{
    double storgeUsage;

    MOUNTP MP;

    char buf[256];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;

    if(!(MP.fp = fopen(MMOUNT, "r")))
    {
        fprintf(stderr, "getStorageUsage: MOUNT open error");
        exit(1);
    }

    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    while(fgets(buf, 255, MP.fp))
    {
        is_root = 0;
        sscanf(buf, "%s%s%s",MP.devname, MP.mountdir, MP.fstype);
        
        if (strcmp(MP.mountdir,"/") == 0) is_root=1;
        if (stat(MP.devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP.mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP.mountdir, &lstatfs);
                MP.size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP.size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024);
                break;
            }
        }
    }

    storgeUsage = (double )(MP.size.blocks - MP.size.avail)/MP.size.blocks;

    rewind(MP.fp);
    fclose(MP.fp);

    fprintf(stdout,"getStorageUsage: %f\n", storgeUsage);
    return storgeUsage;
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

void *UpdateFrame(void *arg)
{
    char workDirName[MAX_BUFFER_SIZE];    
    char workFileName[MAX_BUFFER_SIZE];
    char buffer[MAX_BUFFER_SIZE];

    Mat tempFrame;
    VideoWriter outputVideo;

    while(1){
        //get working directory
        memset(workDirName, 0, sizeof(workDirName));
        strcpy(workDirName, getTimeStringFormat(hours));
        createDirectory((char *)homeDirName,workDirName);
        
        // get working file name
        memset(workFileName, 0, sizeof(workFileName));
        strcpy(workFileName, getTimeStringFormat(secs));

        sprintf(buffer, "%s/%s/%s.%s", homeDirName, workDirName, workFileName, fileType);
        fflush(stdout);
        fprintf(stdout, "UpdateFrame: Working File Full Path = %s\n",buffer);

        outputVideo.open(buffer, VideoWriter::fourcc('X', 'V', 'I', 'D'), 30, size, true);

        if (!outputVideo.isOpened())
        {
            fprintf(stderr, "UpdateFrame: outputVideo open error\n");;
            exit(0);
        }

        while(atoi(getTimeStringFormat(persec)) != 0)
        {
            video >> tempFrame; 
            // 웹캡으로부터 한 프레임을 읽어옴
            pthread_mutex_lock(&frameLocker);
            frame = tempFrame;
            pthread_mutex_unlock(&frameLocker);

            // cvPutText(이미지, 텍스트, 출력위치, 폰트, 텍스트색상) : 텍스트를 이미지에 추가
            // putText(tempFrame, getTimeStringFormat(secs), Point(100, 100), FONT_HERSHEY_SIMPLEX, FONT_SCALE, Scalar(0,0,0), false);
            putText(tempFrame, getTimeStringFormat(secs), Point(100, 100), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), false);

            // 동영상 파일에 한프레임을 저장함.
            // pthread_mutex_lock(&frameLocker);
            // 화면에 영상을 보여줌
            outputVideo << tempFrame;
            
            // if(atoi(getTimeStringFormat(persec)) == 0) break;
        }
        outputVideo.release();
    }
    pthread_exit((void *)0);
}

void *FileManager(void *arg)
{
    while(1){
        sleep(MAX_SLEEP_TIME);
        while(getStorageUsage() > MAX_STORAGE_USAGE){
            removeOldDirName((char *)homeDirName);
        }
    }
    pthread_exit((void *)0);
}

int main()
{
    initialize();

    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    if (!video.isOpened())
    {
        fprintf(stderr, "웹캠을 열수 없습니다.\n");
        return -1;
    }    

    pthread_mutex_init(&frameLocker,NULL);  
    pthread_create(&UpdThread, NULL, UpdateFrame, NULL);
    fprintf(stdout, "main: UpdThread Created\n");

    pthread_create(&FmThread, NULL, FileManager, NULL);    
    fprintf(stdout, "main: FmThread Created\n");

    sleep(2);

    while(1){
        
        Mat currentFrame;

        pthread_mutex_lock(&frameLocker);
        currentFrame = frame;
        pthread_mutex_unlock(&frameLocker);

        imshow("Inputs", currentFrame);

        // currentframe open Y/N
        if(currentFrame.empty()){
            fprintf(stderr,"main: currentFrame empty\n");
            continue;
        }

        int fps = video.get(CAP_PROP_FPS);
        int wait = int(1.0 / fps * 1000);
    
        if(waitKey(wait) == 27){
            int status;

            // pthread cancel succcess
            if (pthread_cancel(UpdThread) == 0 )
            {
                if (pthread_join( UpdThread, (void **)&status ) != 0 ){
                    fprintf(stderr, "main: UpdThread close error\n");
                    return -1;
                }
            }

            // pthread cancel succcess
            if (pthread_cancel(FmThread) == 0 )
            {
                if (pthread_join( FmThread, (void **)&status ) != 0 ){
                    fprintf(stderr, "main: FmThread close error\n");
                    return -1;
                }
            }
            break;         
        }
    }
    video.release();
    return 0;
}
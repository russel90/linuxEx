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

#include <iostream>  
#include <boost/circular_buffer.hpp>
#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

#define MAX_BUFFER_SIZE 256
#define MAX_FILE_TYPE_SIZE = 3
#define FONT_SCALE 2
#define MAX_STORAGE_USAGE 0.50
#define MAX_SLEEP_TIME 300
#define DEBUG

using namespace cv;
using namespace std;

const char *homeDirName = "/home/russel90/blackbox3";
const char *fileType="avi";
const char *MMOUNT = "/proc/mounts";

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
int fps;
int wait;
int width, hight;

pthread_mutex_t frameLocker;
pthread_t captureManagerThread, recorderManagerThread, fileManagerThread;

// check circular buffer size
boost::circular_buffer<Mat> cb(30*60*10);

bool initialize();
bool checkWebCamConnection();
bool createDirectory(char *homeDir, char *workDirName);
bool removeDirector(char *homeDir, char *workDirName);

char *getTimeStringFormat(time_mode t_mode);
double getStorageUsage(); 

bool initialize(){
    
    // return false if storage capacity less then 30%
    if(getStorageUsage() > MAX_STORAGE_USAGE){
        fprintf(stderr, "initalize: storage capacity less then 30\n");
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
	struct timeval tv;
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
        case msecs:
            t = gettimeofday(&tv, NULL);
            sprintf(buffer,"%04d-%02d-%02d-%02d-%02d-%02d-%03d", loc->tm_year+1900, loc->tm_mon, loc->tm_mday, loc->tm_hour, loc->tm_min, loc->tm_sec,tv.tv_usec/1000);
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

void *captureManager(void *arg)
{
    VideoCapture video(0);
    Mat captureFrame;

    if (!video.isOpened()) {
        cerr << "ERROR: Unable to open the camera" << endl;
        video.release();
        return 0;
    }
    
    cout << "Start grabbing, press a key on Live window to terminate" << endl;

    size = Size((int)video.get(CAP_PROP_FRAME_WIDTH),(int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);    
    fps = video.get(CAP_PROP_FPS);
    wait = int(1.0 / fps * 1000);

    while(1){

       video >> captureFrame;

       // if the frame is empty, break immediately
       if(captureFrame.empty()){
            cerr << "ERROR: Unable to grab from the camera" << endl;
            break;
       }

       pthread_mutex_lock(&frameLocker);
       cb.push_back(captureFrame);
       pthread_mutex_unlock(&frameLocker);

#ifdef DEBUG
       std::cout << "captureManager : cb.size() - " << cb.size() << '\n';
#endif        
       waitKey(wait);
    }
    video.release();
    pthread_exit((void *)0);
}

void *recorderManager(void *arg)
{
    char workDirName[MAX_BUFFER_SIZE];    
    char workFileName[MAX_BUFFER_SIZE];
    char buffer[MAX_BUFFER_SIZE];

    // sleep(2);

    VideoWriter outputVideo;    
    Mat recordFrame;

    while(1){
        //get working directory
        memset(workDirName, 0, sizeof(workDirName));
        strcpy(workDirName, getTimeStringFormat(hours));
        createDirectory((char *)homeDirName,workDirName);
        
        // get working file name
        memset(workFileName, 0, sizeof(workFileName));
        strcpy(workFileName, getTimeStringFormat(secs));

        sprintf(buffer, "%s/%s/%s.%s", homeDirName, workDirName, workFileName, fileType);
        fprintf(stdout, "recorderManager: Working File Full Path = %s\n",buffer);

        fps = 30;
        wait = int(1.0 / fps * 1000);

        outputVideo.open(buffer, VideoWriter::fourcc('X', 'V', 'I', 'D'), fps, size, true);    

        if (!outputVideo.isOpened()){
            fprintf(stderr, "recorderManager: outputVideo open error\n");;
            exit(0);
        }
        
        while(atoi(getTimeStringFormat(persec)) != 0){
            if(!cb.empty()){
                pthread_mutex_lock(&frameLocker);
                recordFrame = cb.front();
				cb.pop_front();
                pthread_mutex_unlock(&frameLocker);

                putText(recordFrame, getTimeStringFormat(msecs), Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0,0,0), false);
                outputVideo << recordFrame;
#ifdef DEBUG
                std::cout << "recorderManager: cb.size() - " << cb.size() << '\n';
#endif
            }
        }
        outputVideo.release();
    }
    pthread_exit((void *)0);
}

void *fileManger(void *arg)
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
    int results;
    initialize();

    pthread_mutex_init(&frameLocker,NULL);  
    results = pthread_create(&captureManagerThread, NULL, captureManager, NULL);
    if(results != 0){
        fprintf(stderr, "main: captureManagerThread creation failed(%d)\n", results);
        return -1;
    }
    fprintf(stdout, "main: captureManagerThread Created\n");
    
    results = pthread_create(&recorderManagerThread, NULL, recorderManager, NULL);
    if(results < 0){
        fprintf(stderr, "main: recorderManagerThread creation failed\n");
        return -1;
    }
    fprintf(stdout, "main: recorderManagerThread Created\n");

    pthread_create(&fileManagerThread, NULL, fileManger, NULL);    
    fprintf(stdout, "main: fileMangerThread Created\n");

    while(1){
        if(waitKey(wait) == 27){
            int status;
            
			// captureManagerThread pthread cancel succcess
            if (pthread_cancel(captureManagerThread) == 0 ){
                if (pthread_join( captureManagerThread, (void **)&status ) != 0 ){
                    fprintf(stderr, "main: captureManagerThread close error\n");
                    return -1;
                }
            }
            
			// recorderManagerThread pthread cancel succcess
            if (pthread_cancel(recorderManagerThread) == 0) {
                if (pthread_join( recorderManagerThread, (void **)&status ) != 0 ){
                    fprintf(stderr, "main: recorderManagerThread close error\n");
                    return -1;
                }
            }            
            
			// fileManagerThread pthread cancel succcess
            if (pthread_cancel(fileManagerThread) == 0 ){
                if (pthread_join( fileManagerThread, (void **)&status ) != 0 ){
                    fprintf(stderr, "main: fileManagerThread close error\n");
                    return -1;
                }
            }
            break;    
        }
  	}
    return 0;
}
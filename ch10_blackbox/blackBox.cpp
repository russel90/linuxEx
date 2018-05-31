#include <iostream>  
#include "opencv2/opencv.hpp"  
#include <time.h>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include <sys/vfs.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <algorithm>
using namespace cv;
using namespace std;

void MakeDirectory(char *full_path);
char* timeToString(struct tm *t);

//////////////////////////////////////////////
const char *MMOUNT = "/proc/mounts";

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

MOUNTP *dfopen()
{
    MOUNTP *MP;

    // /proc/mounts 파일을 연다.
    MP = (MOUNTP *)malloc(sizeof(MOUNTP));
    if(!(MP->fp = fopen(MMOUNT, "r")))
    {
        return NULL;
    }
    else
        return MP;
}

MOUNTP *dfget(MOUNTP *MP)
{
    char buf[256];
    // char *bname;
    // char null[16];
    struct statfs lstatfs;
    struct stat lstat; 
    int is_root = 0;

    // /proc/mounts로 부터 마운트된 파티션의 정보를 얻어온다.
    while(fgets(buf, 255, MP->fp))
    {
        is_root = 0;
        sscanf(buf, "%s%s%s",MP->devname, MP->mountdir, MP->fstype);
        
        if (strcmp(MP->mountdir,"/") == 0) is_root=1;
        if (stat(MP->devname, &lstat) == 0 || is_root)
        {
            if (strstr(buf, MP->mountdir) && S_ISBLK(lstat.st_mode) || is_root)
            {
                // 파일시스템의 총 할당된 크기와 사용량을 구한다.        
                statfs(MP->mountdir, &lstatfs);
                MP->size.blocks = lstatfs.f_blocks * (lstatfs.f_bsize/1024); 
                MP->size.avail  = lstatfs.f_bavail * (lstatfs.f_bsize/1024); 
                return MP;
            }
        }
    }
    rewind(MP->fp);
    return NULL;
}

int dfclose(MOUNTP *MP)
{
    return fclose(MP->fp);
}

int main()
{
	DIR *dir;
    struct dirent *ent;
    char src[255] = "/home/russel90/blackbox/";

	MOUNTP *MP;
	struct tm *t;
	time_t timer;
	
    // 초 단위 시간 설정
    timer = time(NULL);    // 현재 시각을 초 단위로 얻기
	t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
	
    // 자막 관련 설정
    cv::Point myPoint;
    myPoint.x = 10;
    myPoint.y = 40;

    int myFontFace = 2;
    double myFontScale = 1.2;
	
    //웹캡으로 부터 데이터 읽어오기 위해 준비  
    VideoCapture video(0);
    if (!video.isOpened())
    {
        cout << "웹캠을 열수 없습니다." << endl;
        return 1;
    }
 
    Mat frame;
 
    namedWindow("input", 1);
 	 if ((MP=dfopen()) == NULL)
    {
        perror("error");
        return 1;
    }
    //웹캠에서 캡쳐되는 이미지 크기를 가져옴   
    Size size = Size((int)video.get(CAP_PROP_FRAME_WIDTH), (int)video.get(CAP_PROP_FRAME_HEIGHT));
    video.set(CAP_PROP_FPS, 30.0);
    char path[256], spath[256];
    
    sprintf( path, "mkdir ~/blackbox/%04d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour);
    sprintf(spath,"/home/russel90/blackbox/%04d%02d%02d%02d/%s",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour,timeToString(t));
    VideoWriter outputVideo;
    
    const int dir_err = system(path);
    printf("dir Err = %d, Path = %s\n", dir_err, path);
    
    if(dir_err == -1){
        perror("Error: Create Directory Error");
        exit(0);   
    }

    if(t->tm_min==0){
        
        if (-1 == dir_err){
            printf("Error creating directory!n");
            exit(1);
        }

        outputVideo.open(spath, VideoWriter::fourcc('X', 'V', 'I', 'D'),30, size, true);
        
        if (!outputVideo.isOpened()){
            cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
            return 1;
        }
    }else{
        outputVideo.open(spath, VideoWriter::fourcc('X', 'V', 'I', 'D'),30, size, true);
        if (!outputVideo.isOpened()){
            cout << "동영상을 저장하기 위한 초기화 작업 중 에러 발생" << endl;
            return 1;
        }
	}	
		
	while (1)
    {
		timer = time(NULL);    // 현재 시각을 초 단위로 얻기
		t = localtime(&timer); // 초 단위의 시간을 분리하여 구조체에 넣기
        
        //웹캡으로부터 한 프레임을 읽어옴  
        video >> frame;
        CvFont* font = new CvFont;
        cvInitFont(font, CV_FONT_VECTOR0, 1.0f, 1.0f, 0, 1);
        
        static char s[20];
	    char path[256],spath[256];
        sprintf(path,"mkdir ~/BlockBox/%04d%02d%02d%02d",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour);
        sprintf(spath,"/home/pi/BlockBox/%04d%02d%02d%02d/%s",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour,timeToString(t));
	    sprintf(s, "%04d%02d%02d%02d%02d%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        cv::putText( frame, s, myPoint, myFontFace, myFontScale, Scalar(0,0,0) );
 
        //웹캠에서 캡처되는 속도를 가져옴
        int fps = video.get(CAP_PROP_FPS);
        int wait = int(1.0 / fps * 1000);
 
        //화면에 영상을 보여줌
        imshow("input", frame);
		if(((t->tm_sec)==0)){
			//용량부분 
			if(t->tm_min==0){
		        const int dir_err = system(path);
                if (-1 == dir_err){
                    printf("Error creating directory!n");
                    exit(1);
                }
		    }
		
        outputVideo << frame;
		outputVideo.release ();
		if(dfget(MP)){

            if ((dir = opendir (src)) != NULL) { /* 디렉토리를 열 수 있는 경우 */
	            int name[255];
	            int count=0;

            /* 디렉토리 안에 있는 모든 파일&디렉토리 출력 */
            while ((ent = readdir (dir)) != NULL) {
                name[count]=atoi(ent->d_name);
                count ++;
            }

            sort(name,name+count); //[arr, arr+10) default(오름차순)로 정렬	
            for(int i =0;i<count;i++)
            printf("count %d %d\n",count,name[i]);

            float a=MP->size.avail, b=MP->size.blocks;
            float ctt=(a/b)*100;
		    
            printf("%-14s%-20s%10lu	%f	%f %f\n", MP->mountdir, MP->devname,MP->size.blocks,a,b,ctt);
      
            if(ctt<10&&(a!=0 &&b!=0)){
                char path[256];

                sprintf(path,"rm -rf ~/BlockBox/%d",name[6]);
                //인식
                printf("%s",path);
                const int dir_err = system(path);

                if (dir_err = -1)
                {
                    printf("Error creating directory!n");
                    exit(1);
                }
            }
            closedir (dir);
        } else { /* 디렉토리를 열 수 없는 경우 */
            perror ("");
            return EXIT_FAILURE;
        }
    }

		//	outputVideo.open(spath, VideoWriter::fourcc('X', 'V', 'I', 'D'),30, size, true);
		sprintf(spath,"/home/pi/BlockBox/%04d%02d%02d%02d/%s",t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,t->tm_hour,timeToString(t));
		outputVideo.open(spath, VideoWriter::fourcc('X', 'V', 'I', 'D'), 30, size, true);
        //동영상 파일에 한프레임을 저장함.  
	}
    
    outputVideo << frame;
	
    //ESC키 누르면 종료
    if (waitKey(wait) == 27) break;
    }
 
     return 0;
}


char* timeToString(struct tm *t) {
  static char s[20];
  sprintf(s, "%04d%02d%02d%02d%02d%02d.avi",
              t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
              t->tm_hour, t->tm_min, t->tm_sec
          );
  return s;
}

void MakeDirectory(char *full_path)
{
}
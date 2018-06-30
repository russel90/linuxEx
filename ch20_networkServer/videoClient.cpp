#include <sys/socket.h> 
#include <arpa/inet.h>
#include <unistd.h>

#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"

using namespace cv;


int main(int argc, char** argv)
{

    //--------------------------------------------------------
    //networking stuff: socket , connect
    //--------------------------------------------------------
    int         sokt;
    char*       serverIP = "127.0.0.1";
    int         serverPort = 4097;

    if (argc < 3) {
           std::cout << "Usage: cv_video_cli <serverIP: 127.0.0.1> <serverPort:4097> " << std::endl;
    }

    if(argc == 3){
        serverIP   = argv[1];
        serverPort = atoi(argv[2]);
    }

    struct  sockaddr_in serverAddr;
    socklen_t           addrLen = sizeof(struct sockaddr_in);

    if ((sokt = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "main: socket() failed" << std::endl;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(sokt, (sockaddr*)&serverAddr, addrLen) < 0) {
        std::cerr << "main: connect() failed!" << std::endl;
    }

    //----------------------------------------------------------
    //OpenCV Code
    //----------------------------------------------------------

    Mat img;
    img = Mat::zeros(480 , 640, CV_8UC3);

    int imgSize = img.total() * img.elemSize();
    uchar *iptr = img.data;
    int bytes = 0;
    int keyValue;
    int fps = 30;

    //make img continuos
    if ( ! img.isContinuous() ) { 
          img = img.clone();
    }
        
    std::cout << "main: Image Size:" << imgSize << std::endl;

    while (keyValue != 'q') {

        if ((bytes = recv(sokt, iptr, imgSize , MSG_WAITALL)) == -1) {
            std::cerr << "main: recv failed, received bytes = " << bytes << std::endl;
        }
        
        cv::imshow("main: CV Video Client", img); 
        keyValue = cv::waitKey(1.0 / fps * 1000);
    }   

    close(sokt);

    return 0;
}
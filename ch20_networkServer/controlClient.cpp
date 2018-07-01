#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termio.h>

#include <arpa/inet.h>

#include <iostream>  

using namespace std;

int getch(void) {
	int ch;
	struct termios buf, save;

	tcgetattr(0, &save);
	buf = save;
	buf.c_lflag &= ~(ICANON | ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;
	tcsetattr(0, TCSAFLUSH, &buf);
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);
	return ch;
}

int main(int argc, char** argv)
{

    //--------------------------------------------------------
    //networking stuff: socket , connect
    //--------------------------------------------------------
    int         cSocket;
    char*       serverIP = "127.0.0.1";
    int         serverPort = 4098;
    ssize_t     key;
    char         keyValue;

    if (argc < 3) {
           std::cerr << "Usage: cv_video_cli <serverIP:127.0.0.1> <serverPort:4098> " << std::endl;
    }

    if(argc == 3){
        serverIP   = argv[1];
        serverPort = atoi(argv[2]);
    }

    struct  sockaddr_in serverAddr;
    socklen_t           addrLen = sizeof(struct sockaddr_in);

    if ((cSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "main: socket() failed" << std::endl;
    }

    serverAddr.sin_family = PF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(cSocket, (sockaddr*)&serverAddr, addrLen) < 0) {
        std::cerr << "main: connect() failed!" << std::endl;
    }

    while (keyValue != 'q') {
        keyValue =getch();
        if ((key = send(cSocket, &keyValue, sizeof(keyValue) , 0)) == -1) {
            std::cerr << "main: recv failed, received bytes = " << key << std::endl;
        }
        fprintf(stdout, "main: key value = %c\n", keyValue);
    }   

    close(cSocket);

    return 0;
}

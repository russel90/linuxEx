#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <sys/socket.h> 
#include <arpa/inet.h>

#include <iostream>  

using namespace std;

extern "C" {
    int driving(char *key);
    int initialize(void);
    int setDrivingInitialCondition();
    int close(void);
}

pthread_mutex_t frameLocker;
pthread_t contolManagerThread;

void *controlManager(void *arg)
{
    int socket = *(int *)arg;
    int bytes = 0, imgSize;
    ssize_t results; 
    char keyValue;

    // set pi car initial condition
    initialize();
    // Set initial Driving Condition
    setDrivingInitialCondition();

    while(keyValue != 'q'){
        results = recv( socket, (void *)&keyValue, sizeof(keyValue), MSG_WAITALL);
        if(results == -1 ){
            continue;
        }
        driving(&keyValue);
        fprintf(stdout, "controlManager: key value = %c\n", keyValue);
    }
    close();
    pthread_exit((void *)0);
}

int main(int argc, char** argv)
{
    int results;
    int status;

    //--------------------------------------------------------
    //networking stuff: socket, bind, listen
    //--------------------------------------------------------
    int localSocket, remoteSocket, port = 4098;                               

    struct  sockaddr_in localAddr, remoteAddr;
    pthread_t thread_id;
               
    int addrLen = sizeof(struct sockaddr_in);
       
    if ( (argc > 1) && (strcmp(argv[1],"-h") == 0) ) {
        std::cerr << "usage: ./cv_video_srv [port] [capture device]\n" <<
                     "port           : socket port (4098 default)\n" << 
                     std::endl;

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
    listen(localSocket, 1);
    
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
        results = pthread_create(&contolManagerThread, NULL, controlManager, &remoteSocket);
        if(results != 0){
            fprintf(stderr, "main: captureManagerThread creation failed(%d)\n", results);
            return -1;
        }

	    // if (pthread_join( contolManagerThread, (void **)&status ) != 0 ){
        if (pthread_detach( contolManagerThread) != 0 ){
    		fprintf(stderr, "main: serverManagerThread close error\n");
    	    return -1;
	    }
	}

	close(remoteSocket);
    close(localSocket);

    return 0;
}
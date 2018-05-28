#define STRING "/home/pi/linuxEx/ch08_IPC"
#define PROJ_ID 'S'
#define DATA_PATH_ID "/home/pi/linuxEx"
#define DATA_PROJ_ID 'F'
#define SHMSIZE 256
#define SHM_FLAG_SIZE 2
#define READ 1
#define WRITE 0

#define MAX_FILE_NAME_SIZE 1024
#define MAX_ID_SIZE 11

struct pidSt
{
	long msg_type;
	pid_t pid;
};

struct mesData
{
	int flags;
	int read_byte;
	char file_stream[SHMSIZE];
};

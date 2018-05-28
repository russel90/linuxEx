#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int i,j;
	time_t UTCtime;
	struct tm *tm;
	
	char buf[BUFSIZ];
	struct timeval UTCtime_u;
	
	// get current time value based on UTC
	time(&UTCtime);
	printf("time :%u\n", (unsigned)UTCtime); 
	
	// get current time value til microsec
	gettimeofday(&UTCtime_u, NULL); 
	printf("gettimeofday : %ld/%d\n", UTCtime_u.tv_sec, UTCtime_u.tv_usec);
	
	printf("ctime : %s\n", ctime(&UTCtime));
	
	//putenv("TS=PST3PDT"); // 
	//tzset(); // Set Time Zone
	tm = localtime(&UTCtime); // tm = gmtime(&UTCtime);
	printf("asctime : %s\n", asctime(tm));
	
	strftime(buf, sizeof(buf), "%A %m %e %H:%M:%S %Y", tm); // user define
	
	printf("strftime : %s\n", buf); 

	return 0;
}

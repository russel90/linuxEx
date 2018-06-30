#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <termio.h>
#include <signal.h>
#include "pca9685.h"

#define I2C_DEV 				"/dev/i2c-1"
#define CLOCK_FREQ				25000000.0
#define PCA_ADDR				0x40
#define LED_STEP				200

#define PARAM_M90				100	
#define PARAM_0					307
#define PARAM_P90				510

#define ANGLE_M90				0
#define ANGLE_0					1
#define ANGLE_P90				2

#define PAN_SERBO_MIN			530
#define PAN_SERBO_MID			330
#define PAN_SERBO_MAX			110
#define PAN_SERBO_DELTA			((PAN_SERBO_MIN - PAN_SERBO_MAX) / 20)

#define TILT_SERBO_MAX			143
#define TILT_SERBO_MID			275
#define TILT_SERBO_MIN			371
#define TILT_SERBO_DELTA		((TILT_SERBO_MIN - TILT_SERBO_MAX) / 20)

#define STEERING_SERBO_MAX		292
#define STEERING_SERBO_MID		232
#define STEERING_SERBO_MIN		171
#define STEERING_SERBO_DELTA	((STEERING_SERBO_MAX - STEERING_SERBO_MIN) / 20)

#define DCMOTOR_SPEED_MAX		3686
#define DCMOTOR_SPEED_MIN		500
#define DCMOTOR_SPEED_DELTA		((DCMOTOR_SPEED_MAX - DCMOTOR_SPEED_MIN) / 20)
#define FORWARD 				1
#define BACKWARD				-1

#define ESC						27
#define KEY_UP					65
#define KEY_DOWN				66
#define KEY_RIGHT				67
#define KEY_LEFT				68

#define GPIO_17					0
#define GPIO_27					2

int fd;
unsigned char buffer[3] = {0};
int loop;

int reg_read8(unsigned char addr)
{
	int length = 1;
	buffer[0] = addr;

	if(write(fd, buffer,length)!=length)
	{
		printf("Failed to write from the i2c bus\n");
	}
	
	if(read(fd,buffer, length) != length)
	{
		printf("Failed to read from the i2c bus\n");
	}
	//printf("addr[%d] = %d\n", addr, buffer[0]);
	
	return 0;
}

int reg_write8(unsigned char addr, unsigned char data)
{

	int length=2;
	
	buffer[0] = addr;
	buffer[1] = data;

	if(write(fd,buffer,length)!=length)
	{
		printf("Failed to write from the i2c bus\n");
		return -1;
	}
	
	return 0;
}

int reg_read16(unsigned char addr)
{
	unsigned short temp;
	reg_read8(addr);
	temp = 0xff & buffer[0];
	reg_read8(addr+1);
	temp |= (buffer[0] <<8);
	//printf("addr=0x%x, data=%d\n", addr, temp);
	 
	return 0;	
}

int reg_write16(unsigned char addr, unsigned short data)
{
	int length =2;
	reg_write8(addr, (data & 0xff));
	reg_write8(addr+1, (data>>8) & 0xff);
	return 0;
}

int pca9685_restart(void)
{
	int length;
	
	reg_write8(MODE1, 0x00);
	reg_write8(MODE2, 0x04);
	return 0;
}

int pca9685_freq(unsigned int freq)
{
	int length = 2;
	uint8_t pre_val = (CLOCK_FREQ / 4096 / freq) -1; 
	fprintf(stdout, "pca9685_freq: prescale_val = %d\n", pre_val);
	 
	reg_write8(MODE1, 0x10);			// OP : OSC OFF
	reg_write8(PRE_SCALE, pre_val);		// OP : WRITE PRE_SCALE VALUE
	reg_write8(MODE1, 0x80);			// OP : RESTART
	reg_write8(MODE2, 0x04);			// OP : TOTEM POLE 
	return 0;
}
		
int MoveForward(unsigned short speed)
{
	// Set CW
	digitalWrite (0, LOW) ;
	digitalWrite (2, LOW) ;
	
	reg_write16(LED4_ON_L, 0);
	reg_write16(LED5_ON_L, 0);
	reg_write16(LED4_OFF_L, speed);
	reg_write16(LED5_OFF_L, speed);
}

void MoveBackward(unsigned short speed)
{
	// Set CCW
	digitalWrite (0, HIGH) ;
	digitalWrite (2, HIGH) ;
	
	reg_write16(LED4_ON_L, 0);
	reg_write16(LED5_ON_L, 0);
	reg_write16(LED4_OFF_L, speed);
	reg_write16(LED5_OFF_L, speed);
}

void SetSteeringAngle(unsigned short angleValue)
{
	reg_write16(LED7_ON_L, 0);
	reg_write16(LED7_OFF_L, angleValue);
}

int GetDCMortorDirection(void)
{
	if((!digitalRead(0)) && (!digitalRead(2))){
		return FORWARD; 
	}else if(digitalRead(0) && digitalRead(2)){
		return BACKWARD;
	}else{
		return 0;
	}
}

int SetPanServoAngle(unsigned short angleValue)
{
		reg_write16(LED15_ON_L, 0);
		reg_write16(LED15_OFF_L, angleValue);
}

int SetTiltServoAngle(unsigned short angleValue)
{
		reg_write16(LED14_ON_L, 0);
		reg_write16(LED14_OFF_L, angleValue);
}


int pca9685_sleep()
{
    reg_write8(MODE1, 0X10);
    return 0;
}

void Stop()
{
	reg_write16(LED4_ON_L, 0);
	reg_write16(LED5_ON_L, 0);
	reg_write16(LED7_ON_L, 0);
	reg_write16(LED14_ON_L, 0);
	reg_write16(LED15_ON_L, 0);
	reg_write16(LED4_OFF_L, 0);
	reg_write16(LED5_OFF_L, 0);
	reg_write16(LED7_OFF_L, 0);
	reg_write16(LED14_OFF_L, 0);
	reg_write16(LED15_OFF_L, 0);

	pca9685_sleep();
}

// Get pressed Key value without Enter
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

//SIGINT 시그널이 들어오면 호출되는 콜백 함수
void sigHandler(int data)
{
	fprintf(stdout, "sigHandler: servo off\n");

	Stop();
	//RESTART	
	reg_write8(MODE1, 0x80);
	
	//SLEEP
	reg_write8(MODE1, 0x10);
}
git 
int setDrivingInitialCondition()
{
	// Pan Servo Postion: Middle
	reg_write16(LED15_ON_L, 0);
	reg_write16(LED15_OFF_L, PAN_SERBO_MID);
	fprintf(stdout, "setDrivingInitialCondition: (0) 0 Degree\n");
	
	// TILT Servo Postion: Middle
	reg_write16(LED14_ON_L, 0);
	reg_write16(LED14_OFF_L, TILT_SERBO_MID);
	fprintf(stdout, "setDrivingInitialCondition: (0) 0 Degree\n");
	
	// Steering Servo Postion: Middle
	reg_write16(LED7_ON_L, 0);
	reg_write16(LED7_OFF_L, STEERING_SERBO_MID);
	fprintf(stdout, "setDrivingInitialCondition: (0) 0 Degree\n");	
	
	// DC Motor: Set Forward Direction
	MoveForward(DCMOTOR_SPEED_MIN);
	
	return 0;	
}

int draving(char *key)
{
	int i;
	int freq=50;
		
	unsigned short speed = DCMOTOR_SPEED_MIN;
	unsigned short steeringAngleValue = STEERING_SERBO_MID;
	unsigned short panAngleValue = PAN_SERBO_MID;
	unsigned short tiltAngleValue = TILT_SERBO_MID;
	
	// DC Motor Forward Reverse Control 
	wiringPiSetup();
	signal(SIGINT, sigHandler);
	
	// BCM GPIO17
	pinMode (0, OUTPUT) ;
	// BCM GPIO27		
	pinMode (2, OUTPUT) ;		
	
	unsigned short value=2047;
	if((fd=open(I2C_DEV, O_RDWR))<0)
	{
		printf("Failed open i2c-1 bus\n");
		return -1;
	}

	if(ioctl(fd,I2C_SLAVE,PCA_ADDR)<0)
	{
		printf("Failed to acquire bus access and/or talk to slave\n");
		return -1;
	}	
	
    pca9685_restart();
	pca9685_freq(freq);
	
	// Set initial Condition
	setDrivingInitialCondition();
	
	// DC Motor CW Test
	loop = 1;
	while(loop){
		key = getch();
		switch(key){
			case ESC:
				break;
			
			// backward 
			case KEY_DOWN:
				if(GetDCMortorDirection() == FORWARD) speed = DCMOTOR_SPEED_MIN;
				
				speed += DCMOTOR_SPEED_DELTA;
				
				if(speed >= DCMOTOR_SPEED_MAX){
						speed = DCMOTOR_SPEED_MAX;
				}else if(speed <= DCMOTOR_SPEED_MIN){
						speed = DCMOTOR_SPEED_MIN;
				}						
				
				MoveBackward(speed);					
				break;
				
			// forward	
			case KEY_UP:		
				if(GetDCMortorDirection() == BACKWARD) speed = DCMOTOR_SPEED_MIN;
				
				speed += DCMOTOR_SPEED_DELTA;
				
				if(speed >= DCMOTOR_SPEED_MAX){
						speed = DCMOTOR_SPEED_MAX;
				}else if(speed <= DCMOTOR_SPEED_MIN){
						speed = DCMOTOR_SPEED_MIN;
				}
				
				MoveForward(speed);
										
				break;
				
			// left steering
			case KEY_LEFT:
				steeringAngleValue -= STEERING_SERBO_DELTA;
				if(steeringAngleValue >= STEERING_SERBO_MAX){
						steeringAngleValue = STEERING_SERBO_MAX;
				}else if(steeringAngleValue <= STEERING_SERBO_MIN){
						steeringAngleValue = STEERING_SERBO_MIN;
				}
					
				SetSteeringAngle(steeringAngleValue);					 
				break;
			
			// right steering
			case KEY_RIGHT:
				steeringAngleValue += STEERING_SERBO_DELTA;
				if(steeringAngleValue >= STEERING_SERBO_MAX){
						steeringAngleValue = STEERING_SERBO_MAX;
				}else if(steeringAngleValue <= STEERING_SERBO_MIN){
						steeringAngleValue = STEERING_SERBO_MIN;
				}
				SetSteeringAngle(steeringAngleValue);								
			
				break;
			
			// left pan
			case 'a':
				panAngleValue += PAN_SERBO_DELTA;

				// Due to ratating direction, MIN/MAX switched
				if(panAngleValue >= PAN_SERBO_MIN){
						panAngleValue = PAN_SERBO_MIN;
				}else if(panAngleValue <= PAN_SERBO_MAX){
						panAngleValue = PAN_SERBO_MAX;
				}
				SetPanServoAngle(panAngleValue);
			
				break;
			
			// right pan
			case 'd':
				panAngleValue -= PAN_SERBO_DELTA;

				// Due to ratating direction, MIN/MAX switched
				if(panAngleValue >= PAN_SERBO_MIN){
						panAngleValue = PAN_SERBO_MIN;
				}else if(panAngleValue <= PAN_SERBO_MAX){
						panAngleValue = PAN_SERBO_MAX;
				}
				SetPanServoAngle(panAngleValue);
			
				break;
			
			// tilt up
			case 'w':
				tiltAngleValue -= TILT_SERBO_DELTA;

				// Due to ratating direction, MIN/MAX switched
				if(tiltAngleValue >= TILT_SERBO_MIN){
						tiltAngleValue = TILT_SERBO_MIN;
				}else if(tiltAngleValue <= TILT_SERBO_MAX){
						tiltAngleValue = TILT_SERBO_MAX;
				}
				SetTiltServoAngle(tiltAngleValue);										
				break;
			
			// tilt down 	
			case 's':
				tiltAngleValue += TILT_SERBO_DELTA;

				// Due to ratating direction, MIN/MAX switched
				if(tiltAngleValue >= TILT_SERBO_MIN){
						tiltAngleValue = TILT_SERBO_MIN;
				}else if(tiltAngleValue <= TILT_SERBO_MAX){
						tiltAngleValue = TILT_SERBO_MAX;
				}
				SetTiltServoAngle(tiltAngleValue);								
				break;
			
			// speed up
			case '+':
				speed += DCMOTOR_SPEED_DELTA;
				
				if(speed >= DCMOTOR_SPEED_MAX){
						speed = DCMOTOR_SPEED_MAX;
				}else if(speed <= DCMOTOR_SPEED_MIN){
						speed = DCMOTOR_SPEED_MIN;
				}
				break;
			
			// speed down
			case '-':
				speed -= DCMOTOR_SPEED_DELTA;
				
				if(speed >= DCMOTOR_SPEED_MAX){
						speed = DCMOTOR_SPEED_MAX;
				}else if(speed <= DCMOTOR_SPEED_MIN){
						speed = DCMOTOR_SPEED_MIN;
				}					
				break;
			case 'q':
				loop = 0;
				break;
			default:
				break;
		}	
		key = '\0';
	}
	// pause();
	Stop();

	return 0;
}

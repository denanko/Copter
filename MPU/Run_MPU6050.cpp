#include <math.h>
#include <vector>
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <stdint.h>

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#include "MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"

#define ToDeg(x) ((x)*57.2957795131) // *180/pi

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
int16_t g[3];
float gyro[3];
// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector


int16_t accel_x;
int16_t accel_y;
int16_t accel_z;
int16_t gyro_x;
int16_t gyro_y;
int16_t gyro_z;

const char* deviceADDR = " 0x68";
const char* PWR_MGMT_1 = " 0x6B";
const char* ACCEL_X_OUT_H = " 0x3B ";
const char* ACCEL_X_OUT_L = " 0x3C ";
const char* ACCEL_Y_OUT_H = " 0x3D ";
const char* ACCEL_Y_OUT_L = " 0x3E ";
const char* ACCEL_Z_OUT_H = " 0x3F ";
const char* ACCEL_Z_OUT_L = " 0x40 ";
const char* GYRO_X_OUT_H = " 0x43 ";
const char* GYRO_X_OUT_L = " 0x44 ";
const char* GYRO_Y_OUT_H = " 0x45 ";
const char* GYRO_Y_OUT_L = " 0x46 ";
const char* GYRO_Z_OUT_H = " 0x47 ";
const char* GYRO_Z_OUT_L = " 0x48 ";

const char* cmdGet = "i2cget -y 2";
const char* cmdSet = "i2cset -y 2";

/*exec function that runs bash commands in c++ */
string exec(char* cmd) {
	string data;
	FILE * stream;
	const int max_buffer = 256;
	char buffer[max_buffer];
	strcat(cmd," 2>&1");
	stream = popen(cmd, "r");

	if (stream) {
		while (!feof(stream))
			if (fgets(buffer, max_buffer, stream) != NULL) data.append(buffer);
			pclose(stream);
		}
	return data;
}

/*function that performs geti2c */
string get(const char* reg1, const char* reg2){
	char str[100];
	string str2;

	strcpy(str, cmdGet);
	strcat(str, reg1);
	strcat(str, reg2);

	str2 = exec(str);
	return str2;
}

/*function that performs seti2c */
void set(const char* reg1, const char* reg2, int value){
	char str[100];
	string str2;

	strcpy(str, cmdSet);
	strcat(str, reg1);
	strcat(str, reg2);
	strcat(str, to_string(value).c_str());

	str2 = exec(str);
}



int main (void){
#if 0
    int file;
    char filename[40];
    const char *buffer;
    int addr = 0x68;        // The I2C address of the ADC

    sprintf(filename,"/dev/i2c-2");
    if ((file = open(filename,O_RDWR)) < 0) {
        printf("Failed to open the bus.");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }

    if (ioctl(file,I2C_SLAVE,addr) < 0) {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        /* ERROR HANDLING; you can check errno to see what went wrong */
        exit(1);
    }
    
    char buf[100] = {0};
    float data;
    char channel;
    int size = 0x46;

    int adder = 0x3A;

	while(true){
	    
	 
    if (write(file,&adder,1) != 1) {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to write to the i2c bus.\n");
        buffer = strerror(errno);
        printf(buffer);
        printf("\n\n");
    }
    
    if (read(file,buf,size) != size) {
            /* ERROR HANDLING: i2c transaction failed */
            printf("Failed to read from the i2c bus.\n");
            buffer = strerror(errno);
            printf(buffer);
            printf("\n\n");
    } else {
        for (int i = 0x0; i < size; i++)
        {
            cout << hex << (int) buf[i] << "\t";
            if (size % 16 == 15){
                        cout << endl;
            }
        }
        cout << endl;
        cout << endl;
    }
        usleep(1000000);
}
#endif
    
    /*
    set(deviceADDR, PWR_MGMT_1, 0);	//turn on the MPU6050
	while(true){
		accel_x = (stoi(get(deviceADDR, ACCEL_X_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, ACCEL_X_OUT_L), nullptr, 16);
		accel_y = (stoi(get(deviceADDR, ACCEL_Y_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, ACCEL_Y_OUT_L), nullptr, 16);
		accel_z = (stoi(get(deviceADDR, ACCEL_Z_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, ACCEL_Z_OUT_L), nullptr, 16);
		accel_x = accel_x / 16384;
		accel_y = accel_y / 16384;
		accel_z = accel_z / 16384;

		gyro_x = (stoi(get(deviceADDR, GYRO_X_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, GYRO_X_OUT_L), nullptr, 16);
		gyro_y = (stoi(get(deviceADDR, GYRO_Y_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, GYRO_Y_OUT_L), nullptr, 16);
		gyro_z = (stoi(get(deviceADDR, GYRO_Z_OUT_H), nullptr, 16) << 8) + stoi(get(deviceADDR, GYRO_Z_OUT_L), nullptr, 16);
		gyro_x = gyro_x / 131;
		gyro_y = gyro_y / 131;
		gyro_z = gyro_z / 131;

		cout << "X-acc: " << accel_x << " Y-acc: " << accel_y << " Z-acc: " << accel_z << endl;
		cout << "X-gyro: " << gyro_x << " Y-gyro: " << gyro_y << " Z-gyro: " << gyro_z << endl;

        usleep(1000000);
	}
	return 0;
	*/
    
 // initialize device
    printf("Initializing I2C devices...\n");
    mpu.initialize();

    // verify connection
    printf("Testing device connections...\n");
    printf(mpu.testConnection() ? "MPU6050 connection successful\n" : "MPU6050 connection failed\n");

    // load and configure the DMP
    printf("Initializing DMP...\n");
    devStatus = mpu.dmpInitialize();
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // turn on the DMP, now that it's ready
        printf("Enabling DMP...\n");
        mpu.setDMPEnabled(true);

        // enable Arduino interrupt detection
        //Serial.println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
        //attachInterrupt(0, dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        printf("DMP ready!\n");
        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        printf("DMP Initialization failed (code %d)\n", devStatus);
    }
	
/////////////////////////MPU OVER /////////////////////////////////
    while(true){
        long rcthr, rcyaw, rcpit, rcroll,yaw, pitch, roll, gyroYaw, gyroPitch, gyroRoll;  // Variables to store radio in
        do {
            fifoCount = mpu.getFIFOCount();
        }while (fifoCount<42);
        
          if (fifoCount == 1024) {
            // reset so we can continue cleanly
            mpu.resetFIFO();
            printf("FIFO overflow!\n");
        
            // otherwise, check for DMP data ready interrupt
            //(this should happen frequently)
          } else  {
            //read packet from fifo
            mpu.getFIFOBytes(fifoBuffer, packetSize);
        
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        
            //scaling for degrees output
            for (int i=0;i<3;i++){
              ypr[i]*=180/M_PI;
            }
        
            yaw=ypr[0]; pitch=ypr[1];  roll=ypr[2];
        
            mpu.dmpGetGyro(g, fifoBuffer);
        
            //0=gyroX, 1=gyroY, 2=gyroZ
            //swapped to match Yaw,Pitch,Roll
            //Scaled from deg/s to get tr/s
             for (int i=0;i<3;i++){
               gyro[i]   = ToDeg(g[3-i-1]);
            }
            
            gyroYaw     = -gyro[0];   
            gyroPitch   = -gyro[1]; 
            gyroRoll    =  gyro[2];
            
            printf("gyroYaw = %d,\tgyroPitch = %d,\tgyroRoll = %d\n", yaw, pitch, roll);
        }
        usleep(10000);
    }
    return 0;
}
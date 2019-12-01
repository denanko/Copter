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
#include <chrono> 
#include <ctime>
#include <fstream>					// Include this to use file streams


#include <unistd.h>
#include <linux/reboot.h>
#include <sys/reboot.h>

using namespace std;
using namespace std::chrono;

#include "MPU6050_6Axis_MotionApps20.h"
#include "pid.h"

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for SparkFun breakout and InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 mpu;

#include "PruProxy.h"


#define ENABLE_VOLT_MONITOR             TRUE
#define ENABLE_LOGGING                  TRUE
#define DISABLE_MOTORS                  FALSE
#define EXECUTION_TIME_MEASURMENTS      FALSE
#define NUMBER_OF_CYCLES                1000


#define NUMBER_OF_ADC_READS             3

// ******************
// rc functions
// ******************
#define MINCHECK 1050
#define MAXCHECK 1950

//Sensor sensor;
static PruProxy Pru;
 
// Radio min/max values for each stick for my radio (worked out at beginning of article)
#define RC_THR_MIN   1000
#define RC_YAW_MIN   1007
#define RC_YAW_MAX   2000
#define RC_PIT_MIN   1000
#define RC_PIT_MAX   1945
#define RC_ROL_MIN   1003
#define RC_ROL_MAX   2000

// Motor numbers definitions
#define MOTOR_FL   2    // Front left    
#define MOTOR_FR   0    // Front right
#define MOTOR_BL   1    // back left
#define MOTOR_BR   3    // back right


// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
int16_t g[3] = {0, 0, 0};
float gyro[3] = {1, 1, 1};
// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3] = {1, 1, 1};           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

// Condition to power down PCB
bool PowerDownCondition = FALSE;



///////////////////////////////////

// Arduino map function
long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define wrap_180(x) (x < -180 ? x+360 : (x > 180 ? x - 360: x))
#define ToDeg(x) ((x)/32.8)
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

// PID array (6 pids, two for each axis)
PID pids[6];
#define PID_PITCH_RATE 0
#define PID_ROLL_RATE 1
#define PID_PITCH_STAB 2
#define PID_ROLL_STAB 3
#define PID_YAW_RATE 4
#define PID_YAW_STAB 5


///////////////////////////////////////

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() 
{

// PID Configuration

 pids[PID_PITCH_RATE].set_Kpid(0.5,0.003,0.09);
  pids[PID_ROLL_RATE].set_Kpid(0.5,0.003,0.09);
  pids[PID_YAW_RATE].set_Kpid(2.7,0,0);
  
  pids[PID_PITCH_STAB].set_Kpid(6.5,0.1,1.2);
  pids[PID_ROLL_STAB].set_Kpid(6.5,0.1,1.2);
  pids[PID_YAW_STAB].set_Kpid(1,0,0);

/*
  pids[PID_PITCH_RATE].set_Kpid(0.7,1,0);
  pids[PID_ROLL_RATE].set_Kpid(0.7,1,0);
  pids[PID_YAW_RATE].set_Kpid(2.7,1,0);
  
  pids[PID_PITCH_STAB].set_Kpid(4.5,0,0);
  pids[PID_ROLL_STAB].set_Kpid(4.5,0,0);
  pids[PID_YAW_STAB].set_Kpid(10,0,0);
*/
////////////////////////////////// MPU INIT ////////////////////////////////////////

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

    /* Check "power" switch state */
    int valueADC[NUMBER_OF_ADC_READS];
    bool AdcDataInconsistent = FALSE;
    char value_str[7];
    
    ifstream ADC_CH1("/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
    
    for (int i = 0; i < NUMBER_OF_ADC_READS; i++){
        ADC_CH1.read(value_str, 6);
        valueADC[i] = strtol(value_str,NULL,0);
        usleep(100);
    }
    ADC_CH1.close();
    
    if (valueADC[0] < 2500){
        /* Power switch should read High or Low level only */
        for (int i = 0; i < NUMBER_OF_ADC_READS; i++){
            /* Check that all read are the same */
            if (valueADC[i] > 2500){
                AdcDataInconsistent = TRUE;
            }
        }
        if (AdcDataInconsistent == FALSE){
            /* Mark oposite to current switch state for power down condition */
            PowerDownCondition = 1;
        }
        else{
           return; 
        }
    }
    else{
        for (int i = 0; i < NUMBER_OF_ADC_READS; i++){
            if (valueADC[i] < 2500){
                AdcDataInconsistent = TRUE;
            }
        } 
        if (AdcDataInconsistent == FALSE){
            PowerDownCondition = 0;
        }
        else{
           return; 
        }
    }
}





// ******** Main Loop *********
#if ENABLE_LOGGING == TRUE
void loop (ostream& logFile) {
    string logData;
    static long logCnt = 0;
#else
void loop () {
#endif

	
 long rcthr, rcyaw, rcpit, rcroll,yaw, pitch, roll;  // Variables to store radio in
  static float yaw_target = 0; 
  static int coutDelay = 0;
  float gyroYaw = 0;
  float gyroPitch = 0;
  float gyroRoll = 0;
  float pitch_stab_output = 0; 
  float roll_stab_output = 0;
  float yaw_stab_output = 0;
  long pitch_output = 0;  
  long roll_output = 0;  
  long yaw_output = 0;  
  uint16_t channels[4];
  long motor[4] = {0, 0, 0, 0};


    if(Pru.UpdateInput())
    {
    	channels[0] = Pru.Input1;
    	channels[1] = Pru.Input2;
    	channels[2] = Pru.Input3; 
    	channels[3] = Pru.Input4;
    }
    else
    {
    	cout << "Pru failed update" << endl;
    }

  if ((channels[2] >= 0) && (channels[2] < 2100)){
      rcthr = channels[2];
      rcyaw = map(channels[0], RC_YAW_MIN, RC_YAW_MAX, -180, 180);
      rcpit =  -map(channels[3], RC_PIT_MIN, RC_PIT_MAX, -45, 45) - 2;	/* "-2" to compensate some shift somewhere (in remote controler?) */
      rcroll = map(channels[1], RC_ROL_MIN, RC_ROL_MAX, -45, 45);
  }
  else{
      rcthr  = 0;
      rcyaw  = 0;
      rcpit  = 0;
      rcroll = 0; 
  }
   
  //cout<< " rcthr = " << rcthr << "\trcyaw = " << rcyaw << "\trcpitch = " << rcpit <<"\trcroll =  "<< rcroll<<endl;
////////////////////////////////////// MPU START ///////////////////////////////////////

    if (!dmpReady) return;

  // wait for FIFO count > 42 bits
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

//cout<< " ypr-> " << yaw << " " << pitch <<" "<< roll<<endl;

    mpu.dmpGetGyro(g, fifoBuffer);

    //0=gyroX, 1=gyroY, 2=gyroZ
    //swapped to match Yaw,Pitch,Roll
    //Scaled from deg/s to get tr/s
     for (int i=0;i<3;i++){
       gyro[i]   = ToDeg(g[3-i-1]); /* DenAn: scalling is wrong (measurments are in degree/s + range should be considered). Why scalling was needed anyway? */
     }
gyroYaw=gyro[0];   gyroPitch=gyro[1]; gyroRoll=-gyro[2];
//cout<< " Gyroypr-> " << gyroYaw << " " << gyroPitch <<" "<< gyroRoll<<endl;
  }

/////////////////////////// MPU ENDS ///////////////////////////////////


// Do the magic
  if(rcthr > RC_THR_MIN + 50) {  // Throttle raised, turn on stablisation.
    // Stablise PIDS
    pitch_stab_output = pids[PID_PITCH_STAB].update_pid_std((float)rcpit, pitch, 1);    // use float instead of long for pitch, roll, yaw??
    roll_stab_output = pids[PID_ROLL_STAB].update_pid_std((float)rcroll, roll, 1);
    yaw_stab_output = pids[PID_YAW_STAB].update_pid_std(yaw_target, yaw, 1);
  
    // is pilot asking for yaw change - if so feed directly to rate pid (overwriting yaw stab output)
    if(abs(rcyaw ) > 5) {
      yaw_stab_output = rcyaw;
      yaw_target = yaw;   // remember this yaw for when pilot stops
    }
    
    // rate PIDS
    pitch_output =  (long) pids[PID_PITCH_RATE].update_pid_std(pitch_stab_output, gyroPitch, 1);  
    roll_output =  (long) pids[PID_ROLL_RATE].update_pid_std(roll_stab_output, gyroRoll, 1);  
    yaw_output =  (long) pids[PID_YAW_RATE].update_pid_std(yaw_stab_output, gyroYaw, 1);  

    // mix pid outputs and send to the motors.
    motor[MOTOR_FL] = rcthr + roll_output + pitch_output - yaw_output;
    motor[MOTOR_BL] = rcthr + roll_output - pitch_output + yaw_output;
    motor[MOTOR_FR] = rcthr - roll_output + pitch_output + yaw_output;
    motor[MOTOR_BR] = rcthr - roll_output - pitch_output - yaw_output;

	
#if DISABLE_MOTORS == TRUE
    Pru.Output1 = 1000*200;
	Pru.Output2 = 1000*200;
	Pru.Output3 = 1000*200;
	Pru.Output4 = 1000*200;
#else
    Pru.Output1 = motor[MOTOR_BL]*200;
	Pru.Output2 = motor[MOTOR_FL]*200;
	Pru.Output3 = motor[MOTOR_BR]*200;
	Pru.Output4 = motor[MOTOR_FR]*200;
#endif
	
	Pru.UpdateOutput();

  } else {
    // motors off
    motor[MOTOR_FL] = 1000;
    motor[MOTOR_BL] = 1000;
    motor[MOTOR_FR] = 1000;
    motor[MOTOR_BR] = 1000;
   
    Pru.Output1 = motor[MOTOR_BL]*200;
	Pru.Output2 = motor[MOTOR_FL]*200;
	Pru.Output3 = motor[MOTOR_BR]*200;
	Pru.Output4 = motor[MOTOR_FR]*200;
	Pru.UpdateOutput();
	
	yaw_target = yaw;
  }
  
  
//  if (coutDelay > 50){
      /*
      cout << " FL-> " << motor[MOTOR_FL] << ";\tFR-> " << motor[MOTOR_FR] <<";\tBL-> "<< motor[MOTOR_BL] << ";\tBR-> " << motor[MOTOR_BR];
      cout << ";\trcthr-> " << rcthr << ";\tpitch-> " << pitch <<";\troll-> "<< roll << ";\tyaw-> " << yaw << endl;
      cout <<  ";\tpitch_output-> " << pitch_stab_output <<";\troll_output-> "<< roll_stab_output << ";\tyaw_output-> " << yaw_stab_output << endl << endl;
      */
      
      //printf("rcthr-> %4d;\trcpit-> %2d;\trcroll-> %2d\trcyaw-> %3d", rcthr, rcpit, rcroll, rcyaw);
     // printf("\tgPit-> %3d;\tgRoll-> %3d\tgYaw-> %3d\n", gyroPitch, gyroRoll, gyroYaw);
      
     // printf("rcpit-> %2d;\trcroll-> %2d\tpit-> %2d\troll-> %2d", rcpit, rcroll, pitch, roll);
     // printf("\tpit_st-> %3.2f;\troll_st-> %3.2f\tpitch_out-> %3d\troll_out-> %3d\n", pitch_stab_output, roll_stab_output, pitch_output, roll_output);
     
       /* Stabilizer debug */
     // printf("rcpit: %2d;\tpit_int: %3.2f;\tpit_stab: %3.2f;\tpit_gyro: %3.2f;\tpit_out: %2d\n", rcpit,  ypr[1], pitch_stab_output, gyroPitch, pitch_output);
     // printf("rcroll:%2d\troll_int: %3.2f;\troll_stab: %3.2f;\troll_gyro: %3.2f;\troll_out: %2d\n", rcroll, ypr[2], roll_stab_output, gyroRoll, roll_output);
      //printf("rcyaw: %4d\tyaw_target: %3.2f\tyaw_stab_output: %3.2f;\tyaw_output: %4d;\n",rcyaw, yaw_target, yaw_stab_output, yaw_output);
      //printf("yaw: %4d\tpitch: %4d\troll: %4d;\n",yaw, pitch, roll);
       
/*      coutDelay = 0;
  }
  else{
      coutDelay++;
  }*/
  
#if ENABLE_LOGGING == TRUE
  logData  = to_string(logCnt);
  logData += "\t";
  logData += to_string(rcthr);
  logData += "\t";
  logData += to_string(rcpit);  
  logData += "\t";
  logData += to_string(gyroPitch);
  logData += "\t";
  logData += to_string(ypr[1]);
  logData += "\t";
  logData += to_string(pitch_stab_output);
  logData += "\t";
  logData += to_string(pitch_output);  
  logData += "\t";
  logData += to_string(rcroll);  
  logData += "\t";
  logData += to_string(gyroRoll);
  logData += "\t";
  logData += to_string(ypr[2]); 
  logData += "\t";
  logData += to_string(roll_stab_output);  
  logData += "\t";
  logData += to_string(roll_output); 
  logData += "\t";
  logData += to_string(yaw_target);  
  logData += "\t";
  logData += to_string(gyroYaw);  
  logData += "\t";
  logData += to_string(ypr[0]);  
  logData += "\t";
  logData += to_string(yaw_stab_output);  
  logData += "\t";
  logData += to_string(yaw_output);  
  logData += "\t";
  logData += to_string(motor[MOTOR_FR]);
  logData += "\t";
  logData += to_string(motor[MOTOR_FL]);
  logData += "\t";
  logData += to_string(motor[MOTOR_BR]);
  logData += "\t";
  logData += to_string(motor[MOTOR_BL]);
  logData += "\n"; 
  
  logCnt++;
  logFile << logData;
#endif
}


int main(void)
{
    #if ENABLE_VOLT_MONITOR
        int VoltMonCounter = 0;
        char value_str[7];
        long int value_int  = 0;
    #endif
    int LoopCounter = 0;
    
    long int valueADC1[NUMBER_OF_ADC_READS];
    int AdcBufferPointer = 0;
    bool PowerDown = FALSE;
    
    for (int i = 0; i < NUMBER_OF_ADC_READS; i++){
        valueADC1[i] = 2500;
    }
    
    
	cout<<"start"<<endl;	
	
	cout << "setup " << endl;
	//	Pru.DisablePru(); 	// disable pru and keep programming running for debug
	if(Pru.Init())
	{
		cout << "Pru init success." << endl;
	}
	else
	{
		cout << "Pru init failed." << endl;
	}
    	
	setup();
	
	#if ENABLE_VOLT_MONITOR
	    FILE* f0 = fopen("/sys/bus/iio/devices/iio:device0/in_voltage0_raw", "r");
    #endif 
      
#if ENABLE_LOGGING == TRUE
    string location = "Logs//";
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    int titleLenght = 0;
    bool bNewFileCreated = FALSE;

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer,sizeof(buffer),"%Y.%m.%d %H-%M-%S",timeinfo);
    titleLenght = strlen(buffer);
    buffer[titleLenght] = '.';
    buffer[titleLenght + 1] = 't';
    buffer[titleLenght + 2] = 'x';
    buffer[titleLenght + 3] = 't';
    buffer[titleLenght + 4] = '\0';
	
	string title = buffer;
	
	int fileNum = 0;
	while(bNewFileCreated != TRUE){
	    ifstream infile(location + title); 
	    if (infile.good() == TRUE){
	        // If file exist add some number to name 
	        fileNum++;
    	    buffer[titleLenght]     = '_';
    	    sprintf(&buffer[titleLenght + 1], "%d", fileNum);
            titleLenght = strlen(buffer);
    	    buffer[titleLenght]     = '.';
            buffer[titleLenght + 1] = 't';
            buffer[titleLenght + 2] = 'x';
            buffer[titleLenght + 3] = 't';
            buffer[titleLenght + 4] = '\0';
            
            title = buffer;
	    }
	    else{
	        bNewFileCreated = TRUE;
	    }
	}
	
	title = buffer;
	ofstream fout(location + title);
	
	string outString = "";
	outString = "t\trcthr\trcpit\tgyropit\tintpit\tstabpit\tpitout\trcroll\tgyroroll\tintroll\tstabroll\trollout\trcyaw\tgyroyaw\tintyaw\tstabyaw\tyawout\tFR\tFL\tBR\tBL\n";
    fout << outString;
#endif

	
#if EXECUTION_TIME_MEASURMENTS == TRUE	
	// Get starting timepoint 
	auto start = high_resolution_clock::now(); 
	
	while(LoopCounter < NUMBER_OF_CYCLES)
#else
	while(1)	
#endif
	{
	#if ENABLE_LOGGING == TRUE
		loop(fout);
	#else
		loop();
	#endif
	
	#if ENABLE_VOLT_MONITOR
        ifstream f0("/sys/bus/iio/devices/iio:device0/in_voltage0_raw");
        f0.read(value_str, 6);
        value_int = strtol(value_str,NULL,0);
        f0.close();
        
        ifstream f1("/sys/bus/iio/devices/iio:device0/in_voltage1_raw");
        f1.read(value_str, 6);
        valueADC1[AdcBufferPointer] = strtol(value_str,NULL,0);
        f1.close();
        
        if (AdcBufferPointer >= NUMBER_OF_ADC_READS){
            PowerDown = TRUE;
            for (int i = 0; i < NUMBER_OF_ADC_READS; i++){
                if ((PowerDownCondition == 1) && (valueADC1[i] < 2500)){
                    PowerDown = FALSE;
                }
                if ((PowerDownCondition == 0) && (valueADC1[i] > 2500)){
                    PowerDown = FALSE;
                }
            }
            AdcBufferPointer = 0;
        }
        else{
           AdcBufferPointer++; 
        }

        
        if ((value_int < 2900) || (PowerDown == TRUE)){
           fout.close();
           return 0;
        }
	#endif
	
		usleep(10);
#if EXECUTION_TIME_MEASURMENTS == TRUE			
		LoopCounter++;
#endif
	}

#if EXECUTION_TIME_MEASURMENTS == TRUE		
	// Get ending timepoint 
	auto stop = high_resolution_clock::now(); 
    auto duration = duration_cast<microseconds>(stop - start); 
  
    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl; 
#endif
	
	fout.close();
	
	return 0;
}
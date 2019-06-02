//=============================================================================
// File: ReadPWM.p 
// Desc: Read PWM
// Vers: 1.0
//

 // Assemble with:  
 // pasm -b ReadWritePWM.p
 
.origin 0
.entrypoint READ_PWM_INIT

// Address for the eCap (eCap)
#define ECAP_TIMER_ADDR         0x30000
#define ECCTL2_ADDR				0x30028
#define	ECCTL2_VALUE			0x100000		// flag to start time

#define IEP_COUNT_ADDR			0x2e00c
#define	IEP_CFG_ADDR			0x2e000
#define	IEP_CFG_VALUE			0x111

// each channel's data are stored in data, start from 00, each channel use 16 bytes, 
// 00 - 03		// flag
// 04 - 07		// last down time
// 08 - 0b		// current down 
// 0c - 0f		// reserved

// pwm input data store in share memory, start from 0x110
// each up time and pulse width are stored, each channel use 16 bytes
// 00 - 03		// flag
// 04 - 07		// pulse width
// 08 - 0b		// frame width 


// macro for process input, it will count the time for high level, as well as whole frame width
.macro ProcessInput		
.mparam 	dataOffset, bitNum
	QBBS PROCESS_HIGH, R3, bitNum
	
PROCESS_LOW:	
	QBBC PROCESS_END, R2, bitNum	// low to low, nothing change
	// now process high to low,
	MOV R1, dataOffset + 8
	SBBO R4, R1, 0, 4				// write down time to data memeory
	JMP PROCESS_END

PROCESS_HIGH:
	QBBS PROCESS_END, R2, bitNum	// high to high, nothing change
	// write up timer, for low to high
    //WRITE_UP_TIMER:
	MOV R1, dataOffset	+ 4		// load data address in DATA to R1
	LBBO R5, R1, 0, 8			// load last down, current down to R5, R6
	SUB R7, R6, R5				// current up - current down = pulse width
	SUB R8, R4, R5				// current up - last up = frame width
	SBBO R4, R1, 0, 4			// update last up time
	MOV R1, dataOffset + 0x10004
	SBBO R7, R1, 0, 8			// save pulse width and frame width to share data
	JMP PROCESS_END
	
PROCESS_END:
.endm

// R1	temp
// R2	Last frame
// R3	Current Frame
// R4	Current counter
// R5	Last up counter
// R6	Current down  counter
// R7 	down counter
// R8	Frame width counter


// put iep count value into reg
.macro	GetIepCount
.mparam	reg
	// copy the current timer counter
	MOV R18, IEP_COUNT_ADDR	
	LBBO reg, R18, 0, 4	// copy content of ecap r0
.endm

.macro CreatePulse
.mparam		dataOffset, bitNum
	MOV R18, dataOffset 										// R18 = 0x200
	LBBO R14, R18, 0, 0x04	// load data to register			// R14 = 0x200
	MOV R10, dataOffset + 0x10000								// R10 = *0x10200
	LBBO R15, R10, 0, 8		// load pulse width and frame width	// R15 = *0x10200
	SUB R11, R12, R14			// calculate the elpse time and put it to r1	// R11 = R12 - R14 = 
	QBGE	SET_LOW, R15, R11
	
SET_HIGH:
	SET R30, bitNum
	JMP SET_END
	
SET_LOW:
	CLR R30, bitNum
	QBLT	SET_END, R16, R11
	
UPDATE_START_TIME:
	MOV R14, R12
	SET R30, bitNum
	// save data back
	SBBO R14, R18, 0, 0x4		// only save start time and pulse and frame width
	
SET_END:
.endm

// R11	time elpse
// R12	curent time
// R13
// R14	start time
// R15	pulse width
// R16	frame width
// R17
// R18	temp for addrss
// R19	status of output pin


READ_PWM_INIT:
	// start the timer
	MOV R1, ECCTL2_ADDR			// copy ecap config register address
	MOV R2, ECCTL2_VALUE		// copy the same value to current frame Reg
	SBBO R2, R1, 0, 4			// set register of ECCTL2, the timer will start now
	MOV R2, R31					// copy current input to last frame Reg
	MOV R3, R2					// copy the same value to current frame Reg
	// copy the current timer counter
	MOV R1, ECAP_TIMER_ADDR		// copy address of ecap to r1
	LBBO R4, R1, 0, 4			// copy current timer count value

PWM_OUT_INIT:
	// start ecap
	MOV R18, IEP_CFG_ADDR		// copy ecap config register address
	MOV R12, IEP_CFG_VALUE		// copy the same value to current frame Reg
	SBBO R12, R18, 0, 4			// set register of ECCTL2, the timer will start now	
	GetIepCount R12
	MOV R18, 0x00				// save start time to first channel
	SBBO R12, R18, 0, 4
	
READ_PWM_INTERVAL:
	// create snap shot by copy input and timer counter
	MOV R3, R31					// copy current input to current frame Reg
	// copy the current timer counter
	MOV R1, ECAP_TIMER_ADDR		// copy address of ecap to r1
	LBBO R4, R1, 0, 4		    // copy content of ecap r0
	// Read inputs
	ProcessInput	0x100, 7	// 1st channel			//PRU0_R31_7	P9-25
	ProcessInput	0x110, 14	// 2nd channel			//PRU0_R31_14	P8-16
	ProcessInput	0x120, 15	// 3rd channel			//PRU0_R31_15	P8-15
	ProcessInput	0x130, 16	// 4th channel			//PRU0_R31_16	P9-24

PWM_OUT_INTERVAL:
	// copy the current timer counter
	GetIepCount R12
	// Create outputs
	CreatePulse 	0x200, 4	//	1st channel			PRU0_R30_7		P9-42
	CreatePulse 	0x210, 6	//	2nd channel			PRU0_R30_5		P9-52
	CreatePulse 	0x220, 14	//	3rd channel			PRU0_R30_3		P9-12
	CreatePulse 	0x230, 15	//	4th channel			PRU0_R30_1		P9-13

UPDATE_LAST:		// update last input frame 
	MOV R2, R3
	JMP READ_PWM_INTERVAL
	HALT
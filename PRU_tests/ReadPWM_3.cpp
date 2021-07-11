// read pwm
// the ReadPWM.bin will read pwm input from R31 
// the firmware will put pluse width value, frame width in pru share memory.
// the first channel value will be put into 
// 0x10100 in pru, which is 0x4a310100
// each channel has 16 bytes, 
// 0x4a310100  	1st channel
// 0x4a310110	2nd channel
// 0x4a310120	3rd channel,
// ...
// 0x4a3101E0	15th channel
// for each channel, 
// the first 4 bytes are flag, reserved for future use
// the seconds 4 bytes are pluse width, it is timer count number, timer run at 200mhz, 1us is 200.
// the third 4 byts are the frame width, it is timer count number
// the fourth 4 bytes are reserved for future use
//
// Compile with:  
// gcc -o ReadPWM ReadPWM.c -lprussdrv  

/******************************************************************************
* Include Files                                                               *
******************************************************************************/

// Standard header files
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>


/******************************************************************************
* Explicit External Declarations                                              *
******************************************************************************/

/******************************************************************************
* Local Macro Declarations                                                    *
******************************************************************************/

#define PRU_NUM 	 0

#define OFFSET_SHAREDRAM 2048		//equivalent with 0x00002000

#define PRUSS_MAX_IRAM_SIZE	0x2000

/******************************************************************************
* Local Typedef Declarations                                                  *
******************************************************************************/


/******************************************************************************
* Local Function Declarations                                                 *
******************************************************************************/

/******************************************************************************
* Local Variable Definitions                                                  *
******************************************************************************/


/******************************************************************************
* Intertupt Service Routines                                                  *
******************************************************************************/


/******************************************************************************
* Global Variable Definitions                                                 *
******************************************************************************/

static int mem_fd;
static void *ddrMem, *sharedMem;

/******************************************************************************
* Global Function Definitions                                                 *
******************************************************************************/



// write a unsigned long to memory
bool WriteUInt32(unsigned long addr, unsigned long data)
{
	/* Initialize example */
	/* map the shared memory */
	void * pMem = mmap(0, 0x2000, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, addr);
	if (pMem == NULL) 
	{
		printf("Failed to map the device (%s)\n", strerror(errno));
		close(mem_fd);
		return false;
	}
	*(unsigned long *)(pMem) = data;
	munmap(pMem, 0x2000);	
	return true;
}

// Reset Pru0,the image won't run
bool ResetPru0()
{
	printf("Reset Pru0\n");
	return WriteUInt32(0x4a322000, 0x10a);
}


// Run image on Pru0
bool RunPru0()
{
	return WriteUInt32(0x4a322000, 0xa);
}

// load image file
bool LoadImage(unsigned long addr, char * filename)
{
	FILE *fPtr;
	
	// Open an File from the hard drive
	fPtr = fopen(filename, "rb");
	if (fPtr == NULL) {
		printf("Image File %s open failed\n", filename);
	} else {
		printf("Image File %s open passed\n", filename);
	}
	// Read file size
	fseek(fPtr, 0, SEEK_END);
	// read file
	unsigned char fileDataArray[PRUSS_MAX_IRAM_SIZE];
	int fileSize = 0;		
	fileSize = ftell(fPtr);

	if (fileSize == 0) {
		printf("File read failed.. Closing program\n");
		fclose(fPtr);
		return -1;
	}

	fseek(fPtr, 0, SEEK_SET);

	if (fileSize !=
		fread((unsigned char *) fileDataArray, 1, fileSize, fPtr)) {
		printf("WARNING: File Size mismatch\n");
	}	
	fclose(fPtr);
	/* Initialize example */
	/* map the shared memory */
	void * pMem = mmap(0, 0x2000, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, addr);
	if (pMem == NULL) {
		printf("Failed to map the device (%s)\n", strerror(errno));
		close(mem_fd);
		return false;
	}
	char * p = (char*)pMem;
	for(int i = 0; i < fileSize; i ++)
	{
		*(p + i) = fileDataArray[i];
	}
	munmap(pMem, 0x2000);
	
	return true;
}

// load Image file to Pru0, the image will not run until ResetPru0 is called
bool LoadImageToPru0(char * filename)
{
	if(!ResetPru0())
	{
		printf("fail to reset pru0 \n");
		return false;
	}
		printf("Loading image\n");	
	return LoadImage(0x4a334000, filename);	// 0x4a334000 is address of instruction in Pru0
		printf("Loading image\n");	
		
	return true;
}


int main (void)
{
	long count = 0;
	
	/* open the device */	
	// map the device and init the varible before use it
	mem_fd = open("/dev/mem", O_RDWR);
	if (mem_fd < 0) {
		printf("Failed to open /dev/mem (%s)\n", strerror(errno));
		return false;
	}
	printf("Loading image\n");	
	LoadImageToPru0("ReadWritePWM.bin");
	printf("Image loaded\n");
	/* map the shared memory */
	sharedMem = mmap(0, 0x400, PROT_WRITE | PROT_READ, MAP_SHARED, mem_fd, 0x4a310000);
	if (sharedMem == NULL) {
		printf("Failed to map the device (%s)\n", strerror(errno));
		close(mem_fd);
		return false;
	}
	printf("Starting PRU\n");	
	RunPru0();
	printf("PRU0 is running\n");	
	printf("sharedMem =  %d\n", *(unsigned long *)sharedMem);	
	
	/* Init outputs */
	*(unsigned long *)(sharedMem + 0x200) = 1000 * 200;
	*(unsigned long *)(sharedMem + 0x210) = 1000 * 200;
	*(unsigned long *)(sharedMem + 0x220) = 1000 * 200;
	*(unsigned long *)(sharedMem + 0x230) = 1000 * 200;
	
	*(unsigned long *)(sharedMem + 0x204) = 5000 * 200;
	*(unsigned long *)(sharedMem + 0x214) = 5000 * 200;
	*(unsigned long *)(sharedMem + 0x224) = 5000 * 200;
	*(unsigned long *)(sharedMem + 0x234) = 5000 * 200;
	
	while(1)
	{
		int i = 0;
		int j = 0;
		unsigned long  rate = 0;
		unsigned long   r1[8], r2[8];
		unsigned long   r3[8];
		for(j =0; j < 8; j++)
		{
			r3[j] = 0;
		}
		for( i =0; i < 7; i++)
		{
			//unsigned long n = (*(unsigned long *)(sharedMem + 114 + i * 0x10));
			r1[i] 	= (*(unsigned long *)(sharedMem + 0x104 + i * 0x10));
			r2[i]  	= (*(unsigned long *)(sharedMem + 0x108 + i * 0x10));
			rate 	= r1[i];
			r3[i] 	= r3[i] + rate/20;
		}
		usleep(10000);
		for(j = 0; j < 4; j++){
		//	*(unsigned long *)(sharedMem + 0x200 + j * 0x10) = r1[0];	// One stick controls all motors
			*(unsigned long *)(sharedMem + 0x200 + j * 0x10) = r1[j];	// Independent control for each motor
		}
		
		count++;
		if (count % 10 == 0){
			printf("%3d ", count);
			for(j = 0; j < 4; j++){
				printf("%8d ", (r1[j]/200));
			}	
			printf("\r\n");
		}
	}
    munmap(ddrMem, 0x2000);
    close(mem_fd);
	
    return(0);
}

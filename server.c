#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
#include <bcm2835.h>
#include "clientserver.h"
//#include "demonize.h"

/* #define RPI_GPIO_P1_21 RPI_GPIO_P1_21;
#define RPI_GPIO_P1_19 RPI_GPIO_P1_19;
#define RPI_GPIO_P1_23 RPI_GPIO_P1_23;
 */
 
#define LC72131_N1  0b00010100
#define LC72131_N2  0b10010100
#define DI_PIN RPI_GPIO_P1_19 
#define CL_PIN RPI_GPIO_P1_21
#define CE_PIN RPI_GPIO_P1_23
#define hex2dec(c) ( c-( ((c)<='9') ? '0' : (c>='a') ? 'a'-10 : 'A'-10) ) 
 
int interval;
char* substring(const char* str, size_t begin, size_t len){ 
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len)) 
    return 0; 

  return strndup(str + begin, len); 
} 


void BusStrob(void){
		bcm2835_gpio_write(CL_PIN, HIGH);
		usleep(interval);
		bcm2835_gpio_write(CL_PIN, LOW);
		usleep(interval);
}

void strrev(char *p){
  char *q = p;
  while(q && *q) ++q;
  for(--q; p < q; ++p, --q)
    *p = *p ^ *q,
    *q = *p ^ *q,
    *p = *p ^ *q;
}

void LC72131_Send ( unsigned int ch ){ 
        int i; 
		 for (i = 0; i < 8; i++) {
			if((ch & (0x80 >> i))>0){
				printf("1");
				usleep(interval);
				bcm2835_gpio_write(DI_PIN, HIGH);
			}else{
				bcm2835_gpio_write(DI_PIN, LOW);
				printf("0");
			}
			BusStrob();
		}

	printf("\n");
}

void initGPIO(){
	if (!bcm2835_init()){
		return 1;
	}
	bcm2835_gpio_fsel(CL_PIN, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(DI_PIN, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(CE_PIN, BCM2835_GPIO_FSEL_OUTP); 
}


void LC72131_SendN1(){
	printf("SendN1 \n");

	bcm2835_gpio_write(CE_PIN, LOW);
	LC72131_Send(LC72131_N1);
	usleep(interval);
	bcm2835_gpio_write(CE_PIN, HIGH);
}

void LC72131_SendN2(){
	printf("SendN2 \n");
	bcm2835_gpio_write(CE_PIN, LOW);
	LC72131_Send(LC72131_N2);
	usleep(interval);
	bcm2835_gpio_write(CE_PIN, HIGH);
}

void LC72131_Init(){
	LC72131_SendN2();
	printf("n2 1 ");
	LC72131_Send(0b11110110);
	printf("n2 2 ");
	LC72131_Send(0b11101100);
	printf("n2 3 ");
	LC72131_Send(0b01000101);
}

int reverseBits(int x){
   return (x * 0x0202020202ULL & 0x010884422010ULL) % 1023;
}

void LC72131_setFreq(float f){
	unsigned int out;
	char res[5]; /* two bytes of hex = 4 characters, plus NULL terminator */
	int freq = (((f + 10.7)/0.0025)/2)/10; 
	printf(" freq = %i \n", freq);

	sprintf(&res[0], "%04x", freq);
	strrev(res);
	
	unsigned char resF;
	char sh[100];
	char *stop;
	unsigned int test;
		sprintf(sh,"%s", substring(res, 0, 2));
		test = strtol(reverseBits(sh),&stop,16);
	
		printf("Test: %d \n",reverseBits(test));
		sprintf(sh,"%s", substring(res, 2, 2));
		test = strtol(sh,&stop,16);
		printf("Test: %d \n",reverseBits(test));

	
	LC72131_SendN1();
	printf("n1 1 \n");
	LC72131_Send(substring(res, 0, 2));
	
	LC72131_Send(substring(res, 0, 2));
	printf("n1 3 ");
	LC72131_Send(0b01010101);
}


int main(int argc, char **argv) {
	
	interval = 100;
	initGPIO();
	
	bcm2835_gpio_write(CL_PIN, HIGH);
	bcm2835_gpio_write(DI_PIN, HIGH);
	bcm2835_gpio_write(CE_PIN, HIGH);
	usleep(interval);
	bcm2835_gpio_write(CL_PIN, LOW);
	bcm2835_gpio_write(DI_PIN, LOW);
	bcm2835_gpio_write(CE_PIN, LOW);

	
	LC72131_Init();
	usleep(interval);
	printf("setFreq \n");

	LC72131_setFreq(90);
//	LC72131_setFreq(90);
	
	bcm2835_gpio_write(CL_PIN, LOW);
	bcm2835_gpio_write(DI_PIN, LOW);
	bcm2835_gpio_write(CE_PIN, LOW);
	
}


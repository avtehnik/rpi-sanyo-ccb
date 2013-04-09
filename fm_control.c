#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <linux/kd.h>
#include <sys/ioctl.h>
#include <bcm2835.h>
#include "clientserver.h"
#include "demonize.h"

#define LC72131_N1  0b00010100
#define LC72131_N2  0b10010100

#define LC72131_N2_1_poweroff 0b0000000

#define LC72131_N2_1 0b11110110
#define LC72131_N2_2 0b00000000
#define LC72131_N2_3 0b00000000

#define DI_PIN RPI_GPIO_P1_21 
#define CL_PIN RPI_GPIO_P1_19
#define CE_PIN RPI_GPIO_P1_23

#define HiBYTE(w) (uint8_t)((w >> 8) & 0x00ff)
#define LoBYTE(w) (uint8_t)((w >> 0) & 0x00ff)
 
int interval;

void BusStrob(void){
		bcm2835_gpio_write(CL_PIN, HIGH);
		usleep(interval);
		bcm2835_gpio_write(CL_PIN, LOW);
		usleep(interval);
}

void LC72131_Send ( unsigned int ch ){ 
        int i; 
		 for (i = 0; i < 8; i++) {
			if((ch & (0x80 >> i))>0){
				/*printf("1");*/
				usleep(interval);
				bcm2835_gpio_write(DI_PIN, HIGH);
			}else{
				bcm2835_gpio_write(DI_PIN, LOW);
				/*printf("0");*/
			}
			BusStrob();
		}
	/*printf("\n");*/
}

void initGPIO(){
	bcm2835_init();
	bcm2835_gpio_fsel(CL_PIN, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(DI_PIN, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(CE_PIN, BCM2835_GPIO_FSEL_OUTP); 

	bcm2835_gpio_write(CL_PIN, LOW);
	bcm2835_gpio_write(DI_PIN, LOW);
	bcm2835_gpio_write(CE_PIN, LOW);
}

void LC72131_SendN1(){
	bcm2835_gpio_write(CE_PIN, LOW);
	LC72131_Send(LC72131_N1);
	usleep(interval);
	bcm2835_gpio_write(CE_PIN, HIGH);
}

void LC72131_SendN2(){
	bcm2835_gpio_write(CE_PIN, LOW);
	LC72131_Send(LC72131_N2);
	usleep(interval);
	bcm2835_gpio_write(CE_PIN, HIGH);
}

void LC72131_Init(){
	LC72131_SendN2();
	LC72131_Send(LC72131_N2_1);
	LC72131_Send(LC72131_N2_2);
	LC72131_Send(LC72131_N2_3);
}
 
void LC72131_PowerOff(){
	LC72131_SendN2();
	LC72131_Send(LC72131_N2_1_poweroff);
	LC72131_Send(LC72131_N2_2);
	LC72131_Send(LC72131_N2_3);
}
 
unsigned char reverse_byte(unsigned char x){
    static const unsigned char table[] = {
        0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
        0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
        0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
        0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
        0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
        0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
        0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
        0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
        0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
        0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
        0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
        0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
        0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
        0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
        0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
        0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
        0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
        0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
        0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
        0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
        0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
        0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
        0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
        0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
        0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
        0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
        0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
        0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
        0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
        0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
        0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
        0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
    };
    return table[x];
}

void LC72131_setFreq(double f){
	uint16_t freq = (((f + 10.7)/0.005)/2)/10; 
	printf("f = %4.2f, decimal = %i, PLL fref = 50khz \n",f, freq);
	LC72131_SendN1();
	LC72131_Send(reverse_byte(LoBYTE(freq)));
	LC72131_Send(reverse_byte(HiBYTE(freq)));
	LC72131_Send(0b01001000);
	bcm2835_gpio_write(CL_PIN, LOW);
	bcm2835_gpio_write(DI_PIN, LOW);
	bcm2835_gpio_write(CE_PIN, LOW);
}


int main(int argc, char **argv) {

	/*
		for debug bus you can change the delay betwen bits send by changing interval value;
		default 100;
	*/
	interval = 100;
	initGPIO();
	LC72131_Init();

	double frequency = 0;
	double fPart0 = 0;
	double fPart1 = 0;

    struct sockaddr_in si_local, si_remote;
    int s, port;
    size_t slen;
    char buf[BUFLEN];

    slen = sizeof (si_remote);
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port number>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    port = atoi(argv[1]);
	
    if (port < 1024) {
        fprintf(stderr, "Usage: %s <port number>  <time delay>  \n", argv[0]);
        fprintf(stderr, "\twhere <port number> shall be > 1023\n");
        exit(EXIT_FAILURE);
    }

/*   demonize(argv[0]); */
    demonize(argv[0]);

    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset((char *) &si_local, 0, sizeof (si_local));
    si_local.sin_family = AF_INET;
    si_local.sin_port = htons(port);
    si_local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(s, (const struct sockaddr *) &si_local, sizeof (si_local)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buf, 0, sizeof (char) *BUFLEN);
        if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_remote, &slen) == -1) {
            perror("recvfrom()");
            exit(EXIT_FAILURE);
        }
		
		if(buf[0]==0){
			LC72131_PowerOff();
			LC72131_setFreq(0.0);
		}else{
			LC72131_Init();
			fPart0 = buf[0];
			fPart1 = buf[1]/10.0f;
			frequency  = fPart0+fPart1;
			if(frequency>90 || frequency<109){
				LC72131_setFreq(frequency);
			}
		}
	}
    close(s);
    exit(EXIT_SUCCESS);
}


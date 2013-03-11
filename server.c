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

/* #define RPI_GPIO_P1_21 RPI_GPIO_P1_21;
#define RPI_GPIO_P1_19 RPI_GPIO_P1_19;
#define RPI_GPIO_P1_23 RPI_GPIO_P1_23;
 */
int interval;

// ia?aaa?a ii oeia MW
void MWSend ( unsigned char Data ){ 
   unsigned char i;
   for ( i = 0; i < 8; i++ ){
        if ( Data & 0x01 ){
			bcm2835_gpio_write(RPI_GPIO_P1_23,HIGH);
			fprintf(stderr, "1");
        }else{
			bcm2835_gpio_write(RPI_GPIO_P1_23, LOW);
        fprintf(stderr, "0");
		}
        Data = Data >> 1;
		usleep(interval);
		bcm2835_gpio_write(RPI_GPIO_P1_19, HIGH);
//		bcm2835_gpio_write(RPI_GPIO_P1_23, LOW);
		usleep(interval);
        bcm2835_gpio_write(RPI_GPIO_P1_19, LOW);
		usleep(interval);
      }
	return;
}

void InitLC72131(){
//	InitMW();
// ia?aaa?a aa?ana
// 40 - ?anoioa
// 41 - onoaiiaee
	bcm2835_gpio_write(RPI_GPIO_P1_21, LOW);
	MWSend(41);
	fprintf(stderr, " ");
	bcm2835_gpio_write(RPI_GPIO_P1_21, HIGH);
	usleep(interval);
//5-0,4-0 ioieou ii iienaie?
	MWSend(128); //aee BO4 (+B i?i)
	fprintf(stderr, " ");
// 8-0,7-0,6-0,13-0
// 10  00  000 0   27/03 - 10110010=0xB2
	MWSend(0xB2);
	fprintf(stderr, " ");
// 12-0,11-1,10-0,9-1,3-0
// 000  1    0    1   00   00010100=20
	MWSend(0);
	fprintf(stderr, " ");
	bcm2835_gpio_write(RPI_GPIO_P1_21, LOW);
	return;	
	}

// iano?ieea ?anoiou LC72131
void DLL_Freq(unsigned int FFreq	,unsigned char StepFreq){
	unsigned char P0,P1,RDD;
	unsigned int FreqT;
// ia?an?ao ?anoiou iia i?eaiiee (f+10,7)/0,025/2=(f+10,7)/0,05
// eee (f+10,7)/0,005/2=(f+10,7)/0,01 a caaeneiinoe io aeaiaciia

	if (StepFreq==1){
		FreqT=FFreq+1070; //i?eaiiee
//		FreqT=FFreq;	//ia?aaao?ee
		// aey iaoaai - fref=5kHz eaa?o 4,5IAo
		// 2     3 1
		// 10100 0 10 10100010=128+32+2=162
		RDD=0xA2;//aey 4,5IAo
//		RDD=0xAA;//aey 7,2IAo
	}else{
		FreqT=FFreq/5+214;	//i?eaiiee
//		FreqT=FFreq/5;	//ia?aaao?ee
		// aey aey ao??oeneiai - fref=25kHz eaa?o 4,5IAo
		// 2     3 1
		// 00100 0 10  00100010=32+2=34
		RDD=0x32;//aey 4,5IAo
//		RDD=0x3A;//aey 7,2IAo		
	}
	P1=FreqT/256;
	P0=FreqT-P1*256;

// ia?aaa?a a PLL
	bcm2835_gpio_write(RPI_GPIO_P1_21, LOW);
	MWSend(40); // aa?an
	fprintf(stderr, " ");
	bcm2835_gpio_write(RPI_GPIO_P1_21, HIGH);
	usleep(interval);
	MWSend(P0);
	fprintf(stderr, " ");
	MWSend(P1);
	fprintf(stderr, " ");
	MWSend(RDD);
	fprintf(stderr, " ");
	bcm2835_gpio_write(RPI_GPIO_P1_21, LOW);
return;
}




int main(int argc, char **argv) {
unsigned int FFreq;
unsigned char StepFreq;
    struct sockaddr_in si_local, si_remote;
    int s, port;
    size_t slen;
    char buf[BUFLEN];


	if (!bcm2835_init()){
		return 1;
	}

	bcm2835_gpio_fsel(RPI_GPIO_P1_19, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(RPI_GPIO_P1_21, BCM2835_GPIO_FSEL_OUTP); 
	bcm2835_gpio_fsel(RPI_GPIO_P1_23, BCM2835_GPIO_FSEL_OUTP); 
	
    slen = sizeof (si_remote);
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port number>  <interval> \n", argv[0]);
        exit(EXIT_FAILURE);
    }

    interval = atoi(argv[2]);
    port = atoi(argv[1]);
	InitLC72131();
	
    if (port < 1024) {
        fprintf(stderr, "Usage: %s <port number>  <time delay>  \n", argv[0]);
        fprintf(stderr, "\twhere <port number> shall be > 1023\n");
        exit(EXIT_FAILURE);
    }

/*   demonize(argv[0]); */
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
        fprintf(stderr, " f = %i %i \n" ,buf[1], buf[2] );
			FFreq = (buf[1]*100)+buf[2];
			if(FFreq<=7600)StepFreq=1;
			if(FFreq>=8800)StepFreq=5;
			//if(FFreq>=11870)FFreq=6395;// íåêðàñèâî, íî ÊÎÐÎÒÊÎ					
//			if(FFreq>=10800)FFreq=6395;// íåêðàñèâî, íî ÊÎÐÎÒÊÎ
			FFreq=FFreq-StepFreq;
	        fprintf(stderr, "init \n");
			InitLC72131();
	        fprintf(stderr, "FFreq: %i step: %i \n", FFreq , StepFreq);
			DLL_Freq(FFreq,StepFreq);
        }

    close(s);
    exit(EXIT_SUCCESS);
}

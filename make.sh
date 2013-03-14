gcc -pedantic -Wall -g -O2   -c -o demonize.o demonize.c
gcc -pedantic -Wall -g -O2   -c -o fm_control.o fm_control.c 
gcc -o fm_control fm_control.o demonize.o -lrt -lbcm2835

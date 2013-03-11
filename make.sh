gcc -pedantic -Wall -g -O2   -c -o demonize.o demonize.c

gcc -pedantic -Wall -g -O2   -c -o server.o server.c 

gcc -o fm_server server.o demonize.o -lrt -lbcm2835

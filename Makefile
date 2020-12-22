# Makefile for TCP project

all: Measure sender

tcp-server: Measure.c
	gcc -o Measure Measure.c

tcp-client: tcp-sender.c
	gcc -o sender sender.c

clean:
	rm -f *.o Measure sender

runs:
	./Measure

runc:
	./sender

runs-strace:
	strace -f ./Measure

runc-strace:
	strace -f ./sender

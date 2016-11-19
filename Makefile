all: packet_sender.c packet_receiver.c mm.o
	gcc -o packet_sender packet_sender.c mm.o
	gcc -o packet_receiver packet_receiver.c mm.o

mm.o: mm.c mm.h
	gcc -o mm.o -c mm.c

clean:
	rm -f *.o packet_receiver packet_sender

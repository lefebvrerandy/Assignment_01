# =======================================================
#                  		CNTR A01
# =======================================================
# FINAL BINARY Target
./bin/chat-server : ./obj/A01.o ./obj/server.o ./obj/client.o
	cc -g ./obj/A01.o ./obj/server.o -o ./bin/A01 -lpthread
#
# =======================================================
#                     Dependencies
# =======================================================                     
./obj/A01.o : ./src/main.c ./src/shared.c ./inc/shared.h
	cc -g -c ./src/client.c -o ./obj/client.o
	cc -g -c ./src/server.c -o ./obj/server.o

#
# =======================================================
# Other targets
# =======================================================                     
all : ./bin/A01

clean:
	rm -f ./bin/*
	rm -f ./obj/*.o
	rm -f ./inc/*.h~
	rm -f ./src/*.c~




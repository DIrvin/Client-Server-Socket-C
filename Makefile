#Beginning of Makefile
EXECUTABLE = server client
CFLAGS = -Wall
CC = gcc
CID = A

#What needs to be built to make all the files and dependencies
all: $(EXECUTABLE)


#Create the main executable
server:
	$(CC) -o server server.c
client:
	$(CC) -o client client.c

clean:
	-rm -f server client

runclient: client
	./client $(CID)

runserver: server
	./server

tarball:
	-rm -f $(EXECUTABLE)
	(cd; tar czf Irvin_Derek_FinalProject.tar.z csc60)

# End Of Makefile

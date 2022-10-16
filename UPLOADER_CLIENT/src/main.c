#include <stdio.h>
#include <winsock.h>

#pragma comment(lib, "WSOCK32.LIB")

void main(int argc, char** argv) {
	WSADATA wsaData;
	SOCKADDR_IN target;
	SOCKET s;
	FILE* filePtr;
	void* fileData;
	long int fileSize;
	long int filenameSize;

	if(argc < 2) {
		printf("You need to supply a server to connect! Should be listening on port 7419!\n");
		return;
	}

	if(argc < 3) {
		printf("You need to supply a filename to send over!\n");
		return;
	}

	filenameSize = strlen(argv[2]);

	printf("Connecting to %s; Sending %s\n", argv[1], argv[2]);

	WSAStartup(MAKEWORD(1, 1), &wsaData);

	target.sin_family = AF_INET;
	target.sin_port = htons(7419);
	target.sin_addr.s_addr = inet_addr(argv[1]);

	s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if(s == INVALID_SOCKET) {
		printf("Failed to connect! Invalid Socket.\n");
		return;
	}

	if(connect(s, (SOCKADDR*) &target, sizeof(target)) == SOCKET_ERROR) {
		printf("Failed to connect! Socket Error.\n");
		return;
	}

	filePtr = fopen(argv[2], "rb");

	fseek(filePtr, 0, SEEK_END);
	fileSize = ftell(filePtr);

	fseek(filePtr, 0, SEEK_SET);

	fileData = malloc(fileSize);

	fread(fileData, 1, fileSize, filePtr);

	printf("file[2]: %02x\n", ((char*) fileData)[2]);
	printf("file[3]: %02x\n", ((char*) fileData)[3]);
	printf("file[4]: %02x\n", ((char*) fileData)[4]);
	printf("file[5]: %02x\n", ((char*) fileData)[5]);
	printf("file[6]: %02x\n", ((char*) fileData)[6]);

	printf("Size: %d; Filename Size: %d; sizeof(long int): %d\n", fileSize, filenameSize, sizeof(long int));

	send(s, &fileSize, sizeof(long int), 0);
	send(s, &filenameSize, sizeof(long int), 0);
	send(s, argv[2], filenameSize * sizeof(char), 0);
	send(s, fileData, fileSize, 0);

	shutdown(s, 2);
	closesocket(s);

	free(fileData);
}
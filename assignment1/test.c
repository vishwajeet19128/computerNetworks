#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
int main()
{
	int pid = getpid();
	char *path;
	snprintf(path, 20, "cat /proc/%d/stat", pid);
	printf("%s\n", path);
	int r=system(path);
}

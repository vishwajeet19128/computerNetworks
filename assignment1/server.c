#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

struct data
{
    int pid;
    char pName[50];
    int pTime;
};

void listFiles(const char *path, int *ithNum, struct data arr[]);
void sort(struct data arr[], int len);
void solve(int n, char sendFile[]);

void* func(void *nSocket)
{
	struct data arr[2000];
	int ithNum;
	int newSocket = *(int*)nSocket;
	free(nSocket);

	/*-----------------recieving n-----------------*/

	char *buffer = malloc(sizeof(int));
	int valread = recv(newSocket, buffer, 1024, 0);
	if(valread < 0)
	{
		printf("recv failed");
		exit(EXIT_FAILURE);
	}
	int n = atoi(buffer);
	printf("%d\n", n);

	/*---finding n processes with most cpu usage---*/

    char sendFile[512] = {0};
	solve(n, sendFile);

	/*---------------sending file----------------*/

	if(send(newSocket , sendFile, strlen(sendFile) , 0) < 0)
	{
		printf("send failed");
		exit(EXIT_FAILURE);
	}
    printf("message sent\n");

    /*----------recieving final data------------*/

    char buffer1[1024] = {0};
	int valread1 = recv(newSocket, buffer1, 1024, 0);
	if(valread1 < 0)
	{
		printf("recv failed");
		exit(EXIT_FAILURE);
	}
	printf("%s\n", buffer1);

	/*-----------------closing socket-----------*/

    close(newSocket);
    pthread_exit(NULL);
} 


/*-------------------main--------------------------------*/

int main(int argc, char const *argv[])
{
	struct sockaddr_in address;
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(serverSocket == -1)
	{
		printf("socket failed");
		exit(EXIT_FAILURE);
	}
    char HOST[10];
    strcpy(HOST, argv[1]);
    int PORT = atoi(argv[2]);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(HOST);
	address.sin_port = htons(PORT);

	if(bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		printf("bind faile");
		exit(EXIT_FAILURE);
	}

	if(listen(serverSocket, 3))
	{
		printf("listen failed");
		exit(EXIT_FAILURE);
	}

	while(1)
	{
		printf("Waiting for incoming connections.....\n");
		int clientLen = sizeof(struct sockaddr_in);
		int newSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t *)&clientLen);
		if(newSocket < 0)
		{
			perror("accept failed");
			exit(EXIT_FAILURE);
		}
		printf("connection established\n");
		pthread_t ptid;
		int *pClient = malloc(sizeof(int));
		*pClient = newSocket;

		pthread_create(&ptid, NULL, &func, pClient);

	    sleep(1);
	}

}

/*---------------------processes seraching--------------*/

void solve(int n, char sendFile[])
{
    char *path = "/proc";
    int ithNum = 0;
    struct data arr[2000];
    // printf("a\n");
    listFiles(path, &ithNum, arr);

    int len = ithNum;
    sort(arr, len);
    
    FILE *fptr;
    fptr = fopen("file.txt","w");
    if(fptr == NULL)
    {
        printf("error opening file\n");
        return;
    }

    for (int i = 0; i < n; ++i)
    {
        fprintf(fptr, "%d ", arr[i].pid);
        fprintf(fptr, "%s ", arr[i].pName);
        fprintf(fptr, "%d\n", arr[i].pTime);
    }
    fclose(fptr);
    fptr = fopen("file.txt", "r");
    char temp[50];
    while(fgets(temp, 50, fptr))
    {
        strcat(sendFile, temp);
    }

}

void sort(struct data arr[], int len)
{
    for (int i = 0; i < len; ++i)
    {
        for (int j = i; j < len; ++j)
        {
            if(arr[i].pTime < arr[j].pTime)
            {
                struct data temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

void listFiles(const char *path, int *ithNum, struct data arr[])
{
    struct dirent *dp;
    int f = 0;
    char *delim = " ";

    DIR *dir = opendir(path);
    if (!dir) 
    {    
        printf("unable to open directory");
        return;
    }
    
    while ((dp = readdir(dir)) != NULL)
    {
        if(strcmp(dp->d_name, "1") == 0)
        {
            f = 1;
        }
        if(f)
        {
            char fpath[30];
            snprintf(fpath, 30, "/proc/%s/stat", dp->d_name);

            int fd = open(fpath, O_RDONLY);
            if(fd == -1)
            {
                printf("could not open file\n");
                return;
            }

            char c[512];
            char *lastToken;
            int ind = 0;
            arr[*ithNum].pTime = 0;

            while(read(fd, c, 512))
            {
                lastToken = strtok(c, delim);
                
                while(lastToken != NULL)
                {
                    if(ind == 0)
                    {
                        arr[*ithNum].pid = atoi(lastToken);
                    }
                    else if(ind == 1)
                    {
                        snprintf(arr[*ithNum].pName, 50, lastToken);
                    }
                    else if(ind == 13 || ind == 14)
                    {
                        arr[*ithNum].pTime += atoi(lastToken);
                    }
                    ind++;
                    if(ind >= 17)
                    {
                        break;
                    }
                    lastToken = strtok(NULL, " ");
                }
                if(ind >= 17)
                {
                    break;
                }
            }
            close(fd);
            (*ithNum)++;
        }
    }
    closedir(dir);
}
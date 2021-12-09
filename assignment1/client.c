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
void solve(char sendBack[]);

int main(int argc, char const *argv[])
{
	struct sockaddr_in serverAddress;

	int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket == -1)
	{
		printf("\n Socket creation error \n");
        return -1;
	}
	printf("socket created\n");

    char HOST[10];
    strcpy(HOST, argv[1]);
    int PORT = atoi(argv[2]);

	serverAddress.sin_addr.s_addr = inet_addr(HOST);
	serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    /*--------------------connect------------------------------*/
    if(connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
    	printf("\nConnection Failed \n");
        return -1;
    }
    printf("Successfully connected\n");

    /*--------------------sending no of process------------------------*/

    printf("enter the message(no of processes) : ");
    char sendToServer[10];
    fgets(sendToServer, 10, stdin);

    send(clientSocket, sendToServer, sizeof(sendToServer), 0);
    printf("message sent from client\n");

    /*-----------------------recieving----------------------*/

    char buffer[1024] = {0};
    int valread = recv(clientSocket, buffer, 1024, 0);
    FILE *fptr1;
    fptr1 = fopen("file1.txt","w");
    if(fptr1 == NULL)
    {
        printf("error opening file\n");
        exit(EXIT_FAILURE); 
    }
    fprintf(fptr1, "%s", buffer);
    fclose(fptr1);
    printf("%s\n",buffer );

    /*--------------------sending back-----------------------*/

    char sendBack[512] = {0};
    solve(sendBack);
    send(clientSocket, sendBack, sizeof(sendBack), 0);

    sleep(10);

    /*---------------------- close ---------------------*/

    close(clientSocket);
    shutdown(clientSocket, 0);
    shutdown(clientSocket, 1);
    shutdown(clientSocket, 2);

	return 0;
}

/*---------------------processes seraching--------------*/

void solve(char sendBack[])
{
    char *path = "/proc";
    int ithNum = 0;
    struct data arr[2000];
    listFiles(path, &ithNum, arr);
    sort(arr, ithNum);
    snprintf(sendBack, 50, "%d %s %d\n", arr[0].pid, arr[0].pName, arr[0].pTime);
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
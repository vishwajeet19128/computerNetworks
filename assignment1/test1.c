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

int main()
{
    int n = 5;
    char sendFile[512] = {0};
    solve(n, sendFile);
    printf("%s\n", sendFile);

    return 0;
}

void solve(int n, char sendFile[])
{
    char *path = "/proc";
    int ithNum = 0;
    struct data arr[2000];
    // printf("a\n");
    listFiles(path, &ithNum, arr);

    int len = ithNum;
    // printf("%d\n", len);

    // for (int i = 0; i < len; ++i)
    // {
    //     printf("%d\t%s\t%d\n", arr[i].pid, arr[i].pName, arr[i].pTime);
    // }

    sort(arr, len);

    // printf("\n\n");
    // for (int i = 0; i < len; ++i)
    // {
    //     printf("%d\t%s\t%d\n", arr[i].pid, arr[i].pName, arr[i].pTime);
    // }

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
        // printf("%s\n", temp);
        strcat(sendFile, temp);
    }
    printf("%s\n", sendFile);
    fclose(fptr);
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
    // *ithNum = 0;
    char *delim = " ";

    DIR *dir = opendir(path);
    if (!dir) 
    {    
        printf("unable to open directory");
        return;
    }
    // printf("directory opened successfully\n");
    
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
            // printf("file opened successfully pid %s\n", dp->d_name);

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
                        // arr[*ithNum].pName = lastToken;
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
            // printf("%d\t%s\t%d\n", arr[*ithNum].pid, arr[*ithNum].pName, arr[*ithNum].pTime);
            (*ithNum)++;
        }
    }
    closedir(dir);
}
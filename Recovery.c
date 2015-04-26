#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<string.h>
#include<pthread.h>
#include<Recovery.h>
#include "LogRecord.h"
#include "WriteFile.h"
#include "ReadFile.h"

int start_recovery()
{
	pthread_mutex_lock(&lock);
	printf("Here in Recovery 1\n");
	int field1;
	char field2[MAX];
	char field3[MAX];
	char field4[MAX];
	FILE* fd = NULL;
        long sz;
	int count=0;
        fd = fopen("journal.txt", "rb");
        sz = fsize(fd);
        if (sz > 0)
        {
		printf("Here in Recovery 2\n");
                char buf[MAX];
                char tmp[MAX];
                char *token;
                fseek(fd, sz, SEEK_SET);
                while (fgetsr(buf, sizeof(buf), fd) != NULL)
                {
			printf("Here in Recovery 3\n");
                        if(!strcmp(buf, "RECOVERY END"))
                        {
				printf("Here in Recovery 4\n");
                                printf("Log Entry Encountered : %s\n", buf);
                                strcpy(tmp,buf);
                                token = strtok(tmp,",");
                                while(isspace(*token)) token++; //Remove leading whitespaces
                                field1 = atoi(token);
	
				token = strtok(NULL, ",");
                                while(isspace(*token)) token++; //Remove leading whitespaces
				strcpy(field2, token);
			
				token = strtok(NULL, ",");
                                while(isspace(*token)) token++; //Remove leading whitespaces
                                strcpy(field3, token);
			
				token = strtok(NULL, ",");
                                while(isspace(*token)) token++; //Remove leading whitespaces
                                strcpy(field4, token);
			
				if(!strcmp(field4, "pending"))
				{
					count++;
					printf("Running Recovery for %s\n", field3);
					all_or_nothing_get(field2);	
				}
			}
			else
				break;
		}
	}
	pthread_mutex_unlock(&lock);
	return count;
}
	

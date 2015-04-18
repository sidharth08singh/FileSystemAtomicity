#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include "ReadFile.h"
#include "WriteFile.h"
#include "LogRecord.h"

void all_or_nothing_get(char file_to_read[])
{
	char read_cmd[MAX];
	struct FileSector obj1;
	int ret = 1;

	find_sectors(file_to_read, &obj1);
	compute_md5sum(&obj1);
	pthread_mutex_lock(&lock);
	if(!strcmp(obj1.cs1, obj1.cs2))
	{
		printf("************************************************************************\n");
		sprintf(read_cmd, "cat %s", obj1.fs1);
		ret = system(read_cmd);
		printf("************************************************************************\n");
		if(ret != 0)
		{
			printf("Failure Reading from Sector\n");
		}
		else
		{
			printf("Read Successful\n");
		}
	}	
	else
	{
		printf("************************************************************************\n");
		sprintf(read_cmd, "cat %s", obj1.fs3);
		ret = system(read_cmd);
		printf("************************************************************************\n");
		if(ret != 0)
		{
			printf("Failure Reading from Sector\n");
		}
		else
		{
			printf("Read Successful\n");
		}
	}
	pthread_mutex_unlock(&lock);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include "WriteFile.h"
#include "LogRecord.h"

//pthread_mutex_t lock; 

void all_or_nothing_put(char data[], char file_to_write[], int tid)
{
	char data_to_write[MAX];
	strcpy(data_to_write, data);
	struct FileSector obj1;
	find_sectors(file_to_write, &obj1); //Find File Sectors
	check_and_repair(&obj1);
	printf("Update...Check And Repair Passed\n\n");
	printf("Update...Writing to Sectors Now\n\n");
	write_and_commit(data_to_write, &obj1, tid);
	return;
}

void write_and_commit(char data[], struct FileSector *obj1, int tid)
{
	char write_cmd[MAX];
	int ret=1;
	//Write to Sector 1
	pthread_mutex_lock(&lock);
	sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs1);
	//printf("## Write Cmd Sector 1 : %s ##\n", write_cmd);
	ret = system(write_cmd);
	if(ret != 0)
	{
		printf("Write to Sector 1 failed");
	}
	else
	{
		//Write to Sector 2
		ret=1;
		sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs2);
		//printf("## Write Cmd Sector 2 : %s ##\n", write_cmd);
		ret = system(write_cmd);
		if(ret != 0 )
		{
			printf("Write to Sector 2 failed");
		}
		else
		{
			ret=1;
			//Commit Point
			add_log_record_entry("write", "commit", obj1->fs1, tid);
			//Write to Sector 3
			sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs3);
			//printf("## Write Cmd Sector 3 : %s\n ##", write_cmd);
			ret = system(write_cmd);
			if(ret != 0)
			{
				printf("Write to Sector 3 failed");
			}
			else
			{
				printf("Update...Write Operation Successful on all Sectors\n\n");
				add_log_record_entry("write", "end", obj1->fs1, tid);
			}
		}
	}
	pthread_mutex_unlock(&lock);
}


void find_sectors(char filename[], struct FileSector *obj1)
{
        strcpy(obj1->fs1, filename);
        strcpy(obj1->fs2, filename);
        strcpy(obj1->fs3, filename);

        //strcat(obj1->fs1, ".txt");
        //strcat(obj1->fs2, "_sector2.txt");
        //strcat(obj1->fs3, "_sector3.txt");
		
	strcat(obj1->fs2, "_sector2");
	strcat(obj1->fs3, "_sector3");
	
}

void check_and_repair(struct FileSector *obj1)
{
	char system_cmd[MAX];
	int ret = 1;
	//Calculate Checksums for the three sectors
	match: compute_md5sum(obj1);
	printf("==========================================================================\n");
	printf("Sector 1 : %s\nSector 2 : %s\nSector 3 : %s\n",obj1->cs1, obj1->cs2, obj1->cs3);
	printf("==========================================================================\n");

	pthread_mutex_lock(&lock);
	if(strcmp(obj1->cs1, obj1->cs2) == 0 && strcmp(obj1->cs2, obj1->cs3) == 0 ) 
	{
		pthread_mutex_unlock(&lock);	
		return;	
	}
	
	if(strcmp(obj1->cs1, obj1->cs2) == 0) 
	{
		//Copy contents of sector1 to sector3	
		sprintf(system_cmd, "cp -p %s %s", obj1->fs1, obj1->fs3);
		printf("2 : %s\n", system_cmd); 
		ret = system(system_cmd);
		if(ret != 0)
			printf("copy operation failed\n");
		else
			pthread_mutex_unlock(&lock);
			return;
	}

	if(strcmp(obj1->cs2, obj1->cs3) == 0)
	{
		//Copy contents of sector2 to sector1
		sprintf(system_cmd, "cp -p %s %s", obj1->fs2, obj1->fs1);
		printf("2 : %s\n", system_cmd);
		ret = system(system_cmd);
		if(ret != 0)
			printf("copy operation failed\n"); 
		else
			pthread_mutex_unlock(&lock);
			return;
	}

	//Copy contents of sector1 to sector2
	sprintf(system_cmd, "cp -p %s %s", obj1->fs1, obj1->fs2);
	ret = system(system_cmd);
	if(ret != 0) 
		printf("copy operation failed\n");

	//Copy contents of sector1 to sector3
	sprintf(system_cmd, "cp -p %s %s", obj1->fs1, obj1->fs3);
	ret = system(system_cmd);
	if(ret != 0) 
		printf("copy operation failed\n");
	
	pthread_mutex_unlock(&lock);
	goto match;
}

void compute_md5sum(struct FileSector *obj1)
{
	char command[MAX];
	char checksum[MAX] = ""; 
	snprintf(command, sizeof(command), "md5sum %s", obj1->fs1);
	FILE *ls1 = popen(command, "r");
	fgets(checksum, sizeof(checksum), ls1);
	pclose(ls1);
	char *temp1 = strtok(checksum, " ");
	strcpy(obj1->cs1, temp1);

	snprintf(command, sizeof(command), "md5sum %s", obj1->fs2);
	FILE *ls2 = popen(command, "r");
	fgets(checksum, sizeof(checksum), ls2);
	pclose(ls2);
	char *temp2= strtok(checksum, " ");
	strcpy(obj1->cs2, temp2);

	snprintf(command, sizeof(command), "md5sum %s", obj1->fs3);
	FILE *ls3 = popen(command, "r");
	fgets(checksum, sizeof(checksum), ls3);
	pclose(ls3);
	char *temp3 = strtok(checksum, " ");
	strcpy(obj1->cs3, temp3);
}

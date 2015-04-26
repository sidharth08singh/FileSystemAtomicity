#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include "ReadFile.h"
#include "WriteFile.h"
#include "LogRecord.h"

void manual_conflict_resolution(struct FileSector *obj1, char contents[], int conflict_choice);

void all_or_nothing_get(char file_to_read[])
{
	char read_cmd[MAX];
	struct FileSector obj1;
	int ret = 1;
	int conflict_choice=0;
	char temp[MAX];
	char contents_s1[MAX];
	char contents_s2[MAX];
	char contents_s3[MAX];

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
		pthread_mutex_unlock(&lock);
	}	
	else if (!strcmp(obj1.cs2, obj1.cs3))
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
		pthread_mutex_unlock(&lock);
	}
	else
	{
		printf("An error might have occured during last write operation\n");
		printf("Please resolve the conflict manually from among the following options : \n");

		struct last_transaction lt;
	        lt = search_log_record_entry(file_to_read);

		if(!strcmp(lt.status, "pending"))
		{
			printf("-------------------------------------------------------------------\n");
			printf("\n1)\n");
			sprintf(read_cmd, "cat %s", obj1.fs1);
			FILE *ls1 = popen(read_cmd, "r");
        		while(fgets(temp, sizeof(temp), ls1) != NULL)
			{
				strcat(contents_s1, temp);
			}
        		pclose(ls1);
			printf("SECTOR 1\n%s", contents_s1);
			
			printf("-------------------------------------------------------------------\n");

			memset(temp, 0, MAX);

			printf("\n2)\n");
			sprintf(read_cmd, "cat %s", obj1.fs2);
			ls1 = popen(read_cmd, "r");
                	while(fgets(temp, sizeof(temp), ls1) != NULL)
			{
				strcat(contents_s2, temp);
			}
                	pclose(ls1);
			printf("SECTOR 2\n%s", contents_s2);

                	printf("-------------------------------------------------------------------\n");
		
			memset(temp, 0, MAX);

                	printf("\n3)\n");
			sprintf(read_cmd, "cat %s", obj1.fs3);
			ls1 = popen(read_cmd, "r");
                	while(fgets(temp, sizeof(temp), ls1) != NULL)
			{
				strcat(contents_s3, temp);
			}
                	pclose(ls1);
			printf("SECTOR 3\n%s", contents_s3);

                	printf("-------------------------------------------------------------------\n");
			
			while(1)
			{
				printf("Please choose one version of the file - Enter 1, 2 or 3\n");
				scanf("%d", &conflict_choice);
				if(conflict_choice <1  || conflict_choice >3)
				{
					printf("Invalid choice. Pick Again. Enter 1, 2 or 3\n");
				}
				else
				{
					if(conflict_choice == 1)
					{
						manual_conflict_resolution(&obj1, contents_s1, conflict_choice);
					}
					else if(conflict_choice == 2)
					{
						manual_conflict_resolution(&obj1, contents_s2, conflict_choice);
					}
					else if(conflict_choice == 3)
					{
						manual_conflict_resolution(&obj1, contents_s3, conflict_choice);
					}
					break;
				}
			}

			printf("Here!!!\n");
			pthread_mutex_unlock(&lock);
			//Add an entry to Journal Log. 
			add_log_record_entry(lt.operation, "commit", lt.fname, lt.tid);
			add_log_record_entry(lt.operation,"end", lt.fname, lt.tid);
		}
	}
}

void manual_conflict_resolution(struct FileSector *obj1, char contents[], int conflict_choice)
{
	printf("Inside Manual Conflict Resolution\n");
	printf("Contents to be written %s\n", contents);			
	int ret;
	char conflict_cmd[MAX];
	if(conflict_choice == 1)
	{
		printf("Here 1\n");
		sprintf(conflict_cmd, "echo '%s' > %s", contents, obj1->fs2);
		ret = system(conflict_cmd);
		memset(conflict_cmd, 0, MAX);
		sprintf(conflict_cmd, "echo '%s' > %s", contents, obj1->fs3);
                ret = system(conflict_cmd);
	}
	else if(conflict_choice == 2)
	{
		printf("Here 2\n");
		sprintf(conflict_cmd, "echo -e  \'%s\' > %s", contents, obj1->fs1);
		printf("Here 2 Cmd 1: %s", conflict_cmd);
                ret = system(conflict_cmd);
		memset(conflict_cmd, 0, MAX);
		sprintf(conflict_cmd, "echo '%s' > %s", contents, obj1->fs3);
                ret = system(conflict_cmd);
	}
	else if(conflict_choice == 3)
	{
		printf("Here 3\n");
		sprintf(conflict_cmd, "echo '%s' > %s", contents, obj1->fs1);
                ret = system(conflict_cmd);
		memset(conflict_cmd, 0, MAX);
		sprintf(conflict_cmd, "echo '%s' > %s", contents, obj1->fs2);
                ret = system(conflict_cmd);
	}
}
		
	

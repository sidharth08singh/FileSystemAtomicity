#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include "CreateFile.h"
#include "DeleteFile.h"
#include "WriteFile.h"
#include "LogRecord.h"

// Locks not needed here because touch command only modifies timestamp of a file that already exists. 

void all_or_nothing_delete(char filename[], int tid)
{
	
	char sector1[MAX], sector2[MAX], sector3[MAX];
	char create_cmd[MAX];
	int res1, res2, res3;

	strcpy(sector1, filename);
	strcpy(sector2, filename);
	strcpy(sector3, filename);
	strcat(sector2, "_sector2");
	strcat(sector3, "_sector3");
	
	printf("Update...All Three Logical File Sectors will be deleted:\n%s\n%s\n%s\n", sector1, sector2, sector3);
	sprintf(create_cmd, "rm -rf %s", sector1);
	res1 = system(create_cmd);
	if(res1 != 0 )
	{
		printf("Error while deleting file\n");
		return;
	}
	else
	{
		sprintf(create_cmd, "rm -rf %s", sector2);
		res2 = system(create_cmd);
		if(res2 != 0 )
		{
			printf("Error while deleting file\n");
			return;
		}
		else
		{
			sprintf(create_cmd, "rm -rf %s", sector3);
			res3 = system(create_cmd);
			if(res3 != 0)
			{
				printf("Error while deleting file\n");		
				return;
			}
			else
			{
				//commit
				add_log_record_entry("delete", "commit", filename, tid);
				add_log_record_entry("delete", "end", filename, tid);
			}
		}
	}
}	

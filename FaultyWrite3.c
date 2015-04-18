#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "WriteFile.h"
#include "LogRecord.h"
#include "FaultyWrite2.h"

void faulty_put2(char data[], char file_to_write[], int tid)
{
	struct FileSector obj1;
	find_sectors(file_to_write, &obj1); //Find File Sectors
	check_and_repair(&obj1);
	printf("## Check And Repair Passed ##\n");
	printf("## Writing to Sectors Now ##\n");
	faulty_write_abort_after_commit(data, &obj1, tid);
	return;
}

void faulty_write_abort_after_commit(char data[], struct FileSector *obj1, int tid)
{
	char write_cmd[MAX];
	int ret=1;
	//Write to Sector 1
	sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs1);
	printf("## Write Cmd Sector 1 : %s ##\n", write_cmd);
	ret = system(write_cmd);
	if(ret != 0)
	{
		printf("Write to Sector 1 failed\n");
	}
	else
	{
		//Write to Sector 2
		sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs2);
		printf("## Write Cmd Sector 2 : %s ##\n", write_cmd);
		ret = system(write_cmd);
		if(ret != 0)
		{
			printf("Write to Sector 2 failed\n");
		}
		else
		{
			//Commit
			add_log_record_entry("write", "commit", obj1->fs1, tid);
			//Abort before Writing to Sector 3
			add_log_record_entry("write", "abort", obj1->fs1, tid);
		}
	}
}

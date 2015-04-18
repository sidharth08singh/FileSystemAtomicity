#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "WriteFile.h"
#include "LogRecord.h"
#include "FaultyWrite1.h"

void faulty_put1(char data[], char file_to_write[], int tid)
{
	struct FileSector obj1;
	find_sectors(file_to_write, &obj1); //Find File Sectors
	check_and_repair(&obj1);
	printf("## Check And Repair Passed ##\n");
	printf("## Writing to Sectors Now ##\n");
	faulty_write_abort_before_commit(data, &obj1, tid);
	return;
}

void faulty_write_abort_before_commit(char data[], struct FileSector *obj1, int tid)
{
	char write_cmd[MAX];
	int ret=1;
	//Write to Sector 1
	sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs1);
	printf("## Write Cmd Sector 1 : %s ##\n", write_cmd);
	ret = system(write_cmd);
	if(ret != 0)
	{
		printf("Write to Sector 1 failed");
	}
	else
	{
		//Abort after writing to only one sector
		add_log_record_entry("write", "abort", obj1->fs1, tid);
	}
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "WriteFile.h"
#include "LogRecord.h"
#include "FaultyWrite3.h"

void faulty_put3(char data[], char file_to_write[], int tid)
{
	struct FileSector obj1;
	find_sectors(file_to_write, &obj1); //Find File Sectors
	check_and_repair(&obj1);
	printf("Update...Check And Repair Passed\n\n");
	printf("Update...Writing to Sectors Now\n\n");
	faulty_write_crash_before_commit(data, &obj1, tid);
	return;
}

void faulty_write_crash_before_commit(char data[], struct FileSector *obj1, int tid)
{
	char write_cmd[MAX];
	int ret=1;
	int corrupt_marker_end;

	sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs1);
	ret = system(write_cmd);
	if(ret != 0)
	{
		printf("Write to Sector 1 failed\n");
	}
	else
	{
		// Write to Sector 2
		// Simulate crash by writing incorrect data to Sector 2 and returning before writing to Sector 3. Make no entry in 
		// the Log Record. This operation results in the three sectors being in {new, bad, old} state. The log record for 
		// this 'tid' has only one entry in pending state and no commit/abort. 

		// corrupting data before writing to Sector 2 
		//Truncate the actual data by half 
        	corrupt_marker_end = strlen(data)/2;
        	data[corrupt_marker_end] = '\0';

		// Write corrupted data to Sector 1
		sprintf(write_cmd, "echo \"%s\" >> %s", data, obj1->fs2);
		ret = system(write_cmd);
		if(ret != 0)
		{
			printf("Write to Sector 2 failed\n");
		}
		else
		{
			printf("\nUpdate...System Crashed while Write in Progress\n\n");
			printf("Update...Sector 1 : New Data, Sector 2 : Damaged, Sector 3 : Old Data\n");
		}
	}
}

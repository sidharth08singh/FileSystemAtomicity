#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include "CreateFile.h"
#include "WriteFile.h"
#include "ReadFile.h"
#include "LogRecord.h"
#include "FaultyWrite1.h"
#include "FaultyWrite2.h"
#include "FaultyWrite3.h"
#include "Recovery.h"

char data[MAX];

enum {CREATE, LIST, READ, WRITE, FWRITE1, FWRITE2, MULWRITE, BEGINRECOVERY, ENDRECOVERY,DELETE};

struct arg_thrd
{
	char arg1[MAX];
	char arg2[MAX];
	char arg3[MAX];
	char arg4[MAX];
};

void list_all_files(char path[])
{
	char list_cmd[MAX];
	char list_output[MAX];
	sprintf(list_cmd, "ls -ltrh %s", path);
	FILE *l = popen(list_cmd, "r");
	while(fgets(list_output, sizeof(list_output), l) != 0) 
	{
		if(strstr(list_output, "_sector") == NULL)
		{
			printf("%s\n", list_output);
		}
	}
	fclose(l);
}

int isFileExist(char filename[], char path[])
{
	char list_cmd[MAX];
	char file_sector1[MAX];
	char file_sector2[MAX];
	char file_sector3[MAX];
	int check1 = 1;  

	memset(file_sector1, 0, MAX);
	memset(file_sector2, 0, MAX);
	memset(file_sector3, 0, MAX);
	memset(list_cmd, 0, MAX); 
	
	struct FileSector obj1;
	find_sectors(filename, &obj1);

	strcat(file_sector1, path);
	strcat(file_sector2, path);
	strcat(file_sector3, path);

	strcat(file_sector1, obj1.fs1);
	strcat(file_sector2, obj1.fs2);
	strcat(file_sector3, obj1.fs3);

	// Check if all three sectors are present
	printf("\n\nUpdate...Checking if all sectors exist\n\n");
        sprintf(list_cmd, "ls -ltrh %s", file_sector1);
	check1 = system(list_cmd);
	if(check1 == 0)
	{
		memset(list_cmd, 0, MAX); 
		check1 = 1;
		sprintf(list_cmd, "ls -ltrh %s", file_sector2);
		check1 = system(list_cmd);
		if(check1 == 0)
		{
			memset(list_cmd, 0, MAX);
			check1 = 1;
			sprintf(list_cmd, "ls -ltrh %s", file_sector3);
			check1 = system(list_cmd);
			if(check1 == 0)
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}
	else
		return 0;
}

void read_input(char data[])
{
	memset(data, 0, MAX);
	printf("Terminate writing using !wq\n");
	char line[150];
	while(1)
	{
		fgets(line, MAX, stdin);
		if(!strncmp(line, "!wq", 3))
			break;
		strcat(data,line);
	}
	
	size_t ln = strlen(data) - 1;
	if(data[ln] == '\n')
        	data[ln] = '\0';

}

void read_filename(char filename[], char path[], int option)
{
	int filecheck;
	int count=0;
	memset(filename, 0, MAX);
	while(1)
	{
		printf("Enter Filename : ");
		fgets(filename, MAX, stdin);
		//strtok(filename, "\n"); Not thread safe to use strtok to remove newline from a fgets input. 
		size_t ln = strlen(filename) - 1;
		if(filename[ln] == '\n')
			filename[ln] = '\0';
		if(strlen(filename) > 1)
			break;
		else
		{
			if(count != 0)
				printf("Filename can't be an empty string. Please enter again!\n");
		}
		count = 1;
	}

	// Check if file exists in the Dummy Filesystem. 
	if(option != 0)
	{
		filecheck = isFileExist(filename, path);
		if(filecheck == 0)
		{
			printf("Warning : %s does not exist in the Dummy Filesystem. Please enter again!\n", filename);
			memset(filename,0,MAX);
			read_filename(filename, path, option);
		}
	}

	if(option == 0) 
	{
		filecheck = isFileExist(filename, path);
		if(filecheck == 1)
		{
			printf("Warning : %s already exists in the Dummy Filesystem. Please choose another name\n", filename);
			memset(filename, 0, MAX);
			read_filename(filename, path, option);
		}
	}
}

void *multithreaded_write(void *arguments)
{
	struct arg_thrd *arg_var = (struct arg_thrd *)arguments;
	struct operation op_mt; 

	printf("Transaction ID for this operation is: %d\n", op_mt.tid);

	// Lock the Critical Section : Data to be written comprises critical section. 
	pthread_mutex_lock(&lock);
	op_mt.tid = 0;
        create_tid(&op_mt);
	system("clear");
	printf("For %s, Enter the Data to be Written to File:\n", arg_var->arg4);
	read_input(data);
       	add_log_record_entry("write", "pending", arg_var->arg3, op_mt.tid);
	pthread_mutex_unlock(&lock); // Release the lock

	all_or_nothing_put(data, arg_var->arg3, op_mt.tid);
	return NULL;
}	

void *multithreaded_read(void *arguments)
{
	struct arg_thrd *arg_var = (struct arg_thrd *)arguments;
	all_or_nothing_get(arg_var->arg3);
	return NULL;
}

int main()
{
	printf("==============================================\n");
	printf("CSE 536: Advanced Operating System Project\n");
	printf("==============================================\n");

	int ch, check; 
	int num_threads;
	char filename[MAX];
	char filefullpath[MAX];
	char homepath[MAX];
	char path[MAX];
	char sys_cmd[MAX];
	
	// Initialize Dummy Filesystem Path Variable. 
	strcpy(homepath, getenv("HOME"));
	strcpy(path, homepath);
	strcat(path, "/FileSystemAtomicity/dummy_filesystem/");
	printf("Dummy FileSystem Path is: %s\n", path);

	// Check if Dummy Filesystem Path Exists, else create. 
	check = 1;
	memset(sys_cmd, 0, MAX);
	sprintf(sys_cmd, "ls -ltrh %s", path);
	printf("\n\n--------------------------------------------------------\n\n");
	printf("\n\nUpdate...Checking If Dummy File System Exists\n\n");
	check=system(sys_cmd);
	printf("\n\n--------------------------------------------------------\n\n");
	
	if(check != 0)
	{
		// Create the Dummy FileSystem. 
		check = 1;
		memset(sys_cmd, 0, MAX);
		sprintf(sys_cmd, "mkdir %s", path);
		check = system(sys_cmd);
		if(check != 0) 
		{
			printf("Exiting...Dummy Filesystem could not be created. Please check permissions or create manually - %s\n", path);
			exit(1);
		}
		else
		{
			printf("Update...Dummy Filesystem Created\n");
		}
	}
	else
	{
		printf("\n\nCheck Complete...Dummy File System Exists. Proceeding Now...\n\n");
	}

	while(1)
	{
		printf("\n-----------------------------------------------\n");
		printf("\nOperations Supported - \n");
		printf("\n-----------------------------------------------\n");
        	printf("\n1. Create a New File\n2. List All Files\n3. Read from File\n4. Write to File\n5. Faulty Write 1 (Abort before commit)\n6. Faulty Write 2 (Abort after commit)\n7. Multithreaded Write (Two threads write to the same file simultaneously)\n8. MultiThreaded Write ('n' threads write to the same file simultaneously)\n9. Multithreaded Write & Read(Two threads write to and read from the same file simultaneously)\n10. Recovery after system crash during Write\n11. Sytem Recovery : Check Journal Log, Identify Conflicts, Ask user to Resolve\n12. Delete a File\n13. Exit\n");
		printf("\nPick one option:");
		scanf("%d", &ch);
		printf("\n");
		struct operation op1;
		switch(ch)
		{
			case 1: 
			{
				printf("==============================================\n");
				printf("Create New File\n");
				printf("==============================================\n");
			
				op1.tid=0;  //Unset any pre-existing operation ID. 
				create_tid(&op1);

				read_filename(filename, path, CREATE);
				printf("\n");
				strcpy(filefullpath, path);
				strcat(filefullpath, filename);
				add_log_record_entry("create", "pending", filefullpath, op1.tid);
				all_or_nothing_create(filefullpath, op1.tid);	
				break;
			}

			case 2:
			{
				printf("Operation Selected: LIST FILES, Dummy FileSystem Path- %s\n", path);
				printf("==================================================================================================\n");
				printf("\n");

				list_all_files(path);
				printf("==================================================================================================\n");
				break; 
			}

			case 3: 
			{
				printf("Operation Selected: READ\n");
				printf("==================================================================================================\n");
				printf("Files present in Dummy FileSystem :\n");

				list_all_files(path);
				printf("==================================================================================================\n");

				read_filename(filename, path, READ);
				printf("Reading File: %s", filename);
				printf("\n");

				strcpy(filefullpath, path);
				strcat(filefullpath, filename);
				all_or_nothing_get(filefullpath);
				break;
			}

			case 4: 
			{
				printf("Operation Selected : WRITE\n");
				printf("==================================================================================================\n");
				printf("Files present in Dummy FileSystem :\n");

				
				list_all_files(path);
				printf("==================================================================================================\n");
			
				op1.tid=0; 
				create_tid(&op1);

				read_filename(filename, path, WRITE);
				printf("\n");
				strcpy(filefullpath, path);
				strcat(filefullpath, filename);

				printf("Enter the Data to be written to file:\n");
				read_input(data);
				printf("\n");

				add_log_record_entry("write", "pending", filefullpath, op1.tid);
				all_or_nothing_put(data, filefullpath, op1.tid);
				break;
			}

			case 5:
			{
				printf("Operation Selected : FAULTY WRITE 1 : Abort Before Commit\n");
				printf("==================================================================================================\n");
				printf("\n\"Note: In this testcase, data is written only to sector1 " 
					"of the file and the program aborts. The next read operation "
					"will ignore this write operation because the operation didn't " 
					"\'commit\'.\"\n\n");

				printf("Files present in Dummy FileSystem :\n");
				list_all_files(path);
				printf("==================================================================================================\n");

				op1.tid=0;
				create_tid(&op1);
				
				read_filename(filename, path, FWRITE1);
				printf("\n");
				strcpy(filefullpath, path);
				strcat(filefullpath, filename);

				printf("Enter the Data to be written to file:\n");
				read_input(data);
				printf("\n");

				add_log_record_entry("write", "pending", filefullpath, op1.tid);
				faulty_put1(data, filefullpath, op1.tid);
				break;
			}

			case 6: 
			{
				printf("Operation Selected : FAULTY WRITE 2: Abort After Commit\n");
				printf("==================================================================================================\n");
				printf("\n\"Note: In this testcase, data is written on sector1 and sector2 "
					"before the program aborts. Write operation \'commits\' but doesnot "
					"\'end\'. Next read operation picks the value of this write because "
					"of commit.\"\n\n");

				printf("Files present in Dummy FileSystem:\n");
				list_all_files(path);
				printf("==================================================================================================\n");

				op1.tid=0;
				create_tid(&op1);

				read_filename(filename, path, FWRITE2);
				printf("\n");
				strcpy(filefullpath, path);
				strcat(filefullpath, filename);

				printf("Enter the Data to be written to file:\n");
				read_input(data);
				printf("\n");

				add_log_record_entry("write", "pending", filefullpath, op1.tid);
				faulty_put2(data, filefullpath, op1.tid);
				break;
			}

			case 7:
			{
				printf("Operation Selected : MULTITHREADED WRITE (TWO THREADS WRITE TO SAME FILE SIMULTANEOUSLY)\n");
				printf("==================================================================================================\n");
				printf("Files present in Dummy FileSystem:\n");

				list_all_files(path);
				printf("==================================================================================================\n");
				printf("\n");

				read_filename(filename, path, MULWRITE);
				strcpy(filefullpath, path);
				strcat(filefullpath, filename);

				pthread_t thrd1, thrd2;
				int thret1, thret2;
				thret1 = 1;
				thret2 = 1;
				struct arg_thrd args1, args2; 

				strcpy(args1.arg4, "thread1");
				strcpy(args1.arg1, filename);
				strcpy(args1.arg2, path);
				strcpy(args1.arg3, filefullpath);

				strcpy(args2.arg4, "thread2");
				strcpy(args2.arg1, filename);
				strcpy(args2.arg2, path);
				strcpy(args2.arg3, filefullpath);
				
				thret1 = pthread_create(&thrd1, NULL, multithreaded_write, (void *)&args1);
				thret2 = pthread_create(&thrd2, NULL, multithreaded_write, (void *)&args2);
				//check if threads are created successfully

				pthread_join(thrd1, NULL);
				pthread_join(thrd2, NULL);
				printf("Printing %d, %d\n", thret1, thret2);
				break;
			}

			case 8 : 
			{
				printf("Operation Selected : MULTITHREADED WRITE ('n' THREADS WRITE TO SAME FILE SIMULTANEOUSLY)\n");
				printf("==================================================================================================\n");

                                printf("Files present in Dummy FileSystem:\n");
                                list_all_files(path);
                                printf("==================================================================================================\n");
                                printf("\n");

                                read_filename(filename, path, MULWRITE);
                                strcpy(filefullpath, path);
                                strcat(filefullpath, filename);
			
				int i, thret;
				thret = 1; 
				printf("Enter the number of parallel threads you want to spawn - ");
				scanf("%d", &num_threads);
				printf("\n");
				
				struct arg_thrd args[num_threads];
				//struct arg_thread *args = malloc(sizeof(struct *arg_thread)*num_threads);

				for(i=0; i<num_threads; i++)
				{
                                	strcpy(args[i].arg4, "thread1");
                               		strcpy(args[i].arg1, filename);
                                	strcpy(args[i].arg2, path);
                                	strcpy(args[i].arg3, filefullpath);
				}
				
				pthread_t *thrd = malloc(sizeof(pthread_t)*num_threads);
				for(i=0; i<num_threads; i++)
				{
					thret = pthread_create(&thrd[i], NULL, multithreaded_write, (void *)&args[i]);
				}
				
				for(i=0; i<num_threads; i++)
				{
					pthread_join(thrd[i], NULL);
				}
				free(thrd);
				break;
			}

			case 9 : 
			{
				printf("OPERATION SELECTED: MULTITHREADED WRITE TO AND READ FROM SAME FILE\n");
				printf("==================================================================================================\n");
                                printf("Files present in Dummy FileSystem:\n");
                                list_all_files(path);
                                printf("==================================================================================================\n");
                                printf("\n");

                                read_filename(filename, path, MULWRITE);
                                strcpy(filefullpath, path);
                                strcat(filefullpath, filename);

				pthread_t thrd1, thrd2;
                                int thret1, thret2;
                                thret1 = 1;
                                thret2 = 1;
                                struct arg_thrd args1, args2;

                                strcpy(args1.arg4, "thread1");
                                strcpy(args1.arg1, filename);
                                strcpy(args1.arg2, path);
                                strcpy(args1.arg3, filefullpath);

                                strcpy(args2.arg4, "thread2");
                                strcpy(args2.arg1, filename);
                                strcpy(args2.arg2, path);
                                strcpy(args2.arg3, filefullpath);

                                thret1 = pthread_create(&thrd1, NULL, multithreaded_write, (void *)&args1);
                                thret2 = pthread_create(&thrd2, NULL, multithreaded_read, (void *)&args2);
                                //check if threads are created successfully

                                pthread_join(thrd1, NULL);
                                pthread_join(thrd2, NULL);
                                printf("Printing %d, %d\n", thret1, thret2);
				break;
			}

			case 10 : 
			{
				printf("OPERATION SELECTED : RECOVERY FROM SYSTEM CRASH DURING WRITE\n");
				printf("==================================================================================================\n");
                                printf("Files present in Dummy FileSystem:\n");

                                list_all_files(path);
                                printf("==================================================================================================\n");
                                printf("\n");

				create_tid(&op1);
                                read_filename(filename, path, MULWRITE);
                                strcpy(filefullpath, path);
                                strcat(filefullpath, filename);
				
				printf("Enter the Data to be written to file:\n");
                                read_input(data);
                                printf("\n");
				
				add_log_record_entry("write", "pending", filefullpath, op1.tid);
				faulty_put3(data, filefullpath, op1.tid);
				break;
			}
			
			case 11 : 
			{
				int recovered_files;
				recovered_files = 0; 
				system("clear");
				printf("OPERATION SELECTED : System Recovery\n");
				add_recovery_entry(BEGINRECOVERY);
				recovered_files = start_recovery();
				add_recovery_entry(ENDRECOVERY);
				if(recovered_files == 0)
					printf("Recovery Finished. No Files To Recover\n");
				else
					printf("Recovery Procedure Has Successfully Finished\n\nTotal Number of Files Recovered : %d", recovered_files);
				break;
			}

			case 12 : 
			{
				printf("==============================================\n");
                                printf("Delete a File\n");
                                printf("==============================================\n");

                                op1.tid=0;  //Unset any pre-existing operation ID. 
                                create_tid(&op1);

                                read_filename(filename, path, DELETE);
                                printf("\n");
                                strcpy(filefullpath, path);
                                strcat(filefullpath, filename);
                                add_log_record_entry("delete", "pending", filefullpath, op1.tid);
                                all_or_nothing_delete(filefullpath, op1.tid);
                                break;
			}

			default:
			{
				if (ch == 13)
				{
					exit(1);
				}
				else
				{
					printf("Incorrect Choice. Pick Again\n");
				}
			}
		}
	}
	return 0;
}

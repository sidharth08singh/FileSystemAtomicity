#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<pthread.h>
#include "LogRecord.h"
#include "WriteFile.h"
#include "ReadFile.h"

//pthread_mutex_t lock;

void add_log_record_entry(char operation[], char state[], char filename[], int tid)
{
	pthread_mutex_lock(&lock);
	FILE *fd = NULL;
	fd = fopen("journal.txt", "a");
	fprintf(fd, "%d, %s, %s, %s\n", tid, operation, filename, state);
	fclose(fd);
	pthread_mutex_unlock(&lock);
}

void create_tid(struct operation *op1)
{
	//create 10 digit transaction id
	pthread_mutex_lock(&lock);
	time_t t;
	srand((unsigned) time(&t));
	op1->tid = rand() % 40000;	
	pthread_mutex_unlock(&lock);
}

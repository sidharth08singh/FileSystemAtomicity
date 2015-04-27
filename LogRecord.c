#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<limits.h>
#include<time.h>
#include<string.h>
#include<pthread.h>
#include "LogRecord.h"
#include "WriteFile.h"
#include "ReadFile.h"

pthread_mutex_t lock1;

void add_log_record_entry(char operation[], char state[], char filename[], int tid)
{
	pthread_mutex_lock(&lock1);
	FILE *fd = NULL;
	fd = fopen("journal.txt", "a");
	fprintf(fd, "%d, %s, %s, %s\n", tid, operation, filename, state);
	fclose(fd);
	pthread_mutex_unlock(&lock1);
}

void add_recovery_entry(int state)
{
	pthread_mutex_lock(&lock1);
 	time_t rawtime;
	struct tm * timeinfo;
  	time ( &rawtime );
  	timeinfo = localtime ( &rawtime );
	
	FILE *fd = NULL;
	fd = fopen("journal.txt", "a");
	if(state == 7)
	{
		fprintf(fd, "RECOVERY START, %s", asctime (timeinfo));
	}
	else if(state == 8)
	{
		fprintf(fd, "RECOVERY END, %s", asctime (timeinfo));
	}
	fclose(fd);
	pthread_mutex_unlock(&lock1);
}

//Returns Last TID for a filename in the Journal Log. 
struct last_transaction search_log_record_entry(char filename[])
{
	struct last_transaction lt;
	FILE* fd = NULL;
  	long sz;
	fd = fopen("journal.txt", "rb");
  	sz = fsize(fd);
  	if (sz > 0)
  	{
    		char buf[MAX];
    		char tmp[MAX];
    		char *token;
    		fseek(fd, sz, SEEK_SET);
    		while (fgetsr(buf, sizeof(buf), fd) != NULL)
		{
			if(strstr(buf, filename) != NULL)
        		{
				printf("%s\n", buf);
                		strcpy(tmp,buf);
                		token = strtok(tmp,",");
                		while(isspace(*token)) token++; //Remove leading whitespaces
                		printf("!%s\n", token);
				lt.tid = atoi(token);

                		token = strtok(NULL,",");
                		while(isspace(*token)) token++; //Remove leading whitespaces
                		printf("!%s\n", token); 
				strcpy(lt.operation, token);

                		token = strtok(NULL,",");
                		while(isspace(*token)) token++; //Remove leading whitespaces
                		printf("!%s\n", token);
				strcpy(lt.fname, token);

                		token = strtok(NULL,",");
                		while(isspace(*token)) token++; //Remove leading whitespaces
                		printf("!%s\n", token);
				size_t ln = strlen(token) - 1;
			        if(token[ln] == '\n')
                			token[ln] = '\0';
				strcpy(lt.status, token);

                		break;
        		}
    		}
  	}
  	fclose(fd);
  	return lt ;
}

/* File must be open with 'b' in the mode parameter to fopen() */
/* Set file position to size of file before reading last line of file */
char* fgetsr(char* buf, int n, FILE* binaryStream)
{
	long fpos;
  	int cpos;
  	int first = 1;

  	if (n <= 1 || (fpos = ftell(binaryStream)) == -1 || fpos == 0)
    		return NULL;

  	cpos = n - 1;
  	buf[cpos] = '\0';

  	for (;;)
  	{
    		int c;
    		if (fseek(binaryStream, --fpos, SEEK_SET) != 0 || (c = fgetc(binaryStream)) == EOF)
      			return NULL;

    		if (c == '\n' && first == 0) /* accept at most one '\n' */
      			break;
    		first = 0;

    		if (c != '\r') /* ignore DOS/Windows '\r' */
    		{
      			unsigned char ch = c;
      			if (cpos == 0)
      			{
        			memmove(buf + 1, buf, n - 2);
				++cpos;
      			}
      			memcpy(buf + --cpos, &ch, 1);
    		}

    		if (fpos == 0)
    		{
      			fseek(binaryStream, 0, SEEK_SET);
      			break;
    		}
  	}
	memmove(buf, buf + cpos, n - cpos);
  	return buf;
}

long fsize(FILE* binaryStream)
{
	long ofs, ofs2;
  	int result;

  	if (fseek(binaryStream, 0, SEEK_SET) != 0 || fgetc(binaryStream) == EOF)
    		return 0;
  	ofs = 1;

  	while ((result = fseek(binaryStream, ofs, SEEK_SET)) == 0 && (result = (fgetc(binaryStream) == EOF)) == 0 && ofs <= LONG_MAX / 4 + 1)
    		ofs *= 2;

  	/* If the last seek failed, back up to the last successfully seekable offset */
  	if (result != 0)
   		ofs /= 2;

  	for (ofs2 = ofs / 2; ofs2 != 0; ofs2 /= 2)
    		if (fseek(binaryStream, ofs + ofs2, SEEK_SET) == 0 && fgetc(binaryStream) != EOF)
      			ofs += ofs2;

  	/* Return -1 for files longer than LONG_MAX */
  	if (ofs == LONG_MAX)
    		return -1;
	return ofs + 1;
}
	
void create_tid(struct operation *op1)
{
	//create 10 digit transaction id
	pthread_mutex_lock(&lock1);
	time_t t;
	srand((unsigned) time(&t));
	op1->tid = rand() % 40000;	
	pthread_mutex_unlock(&lock1);
}
